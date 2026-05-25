# Interface Prototype

## 1. Purpose

このドキュメントは、IRCサーバ実装における担当者間Interfaceを定義する。

現在の内容は `includes/*.hpp` の公開インターフェイスに合わせる。
実装側でAPIを変更した場合は、該当ヘッダとこのドキュメントを同時に更新する。

全体設計・責務分割は `docs/design.md` に記載する。

---

## 2. Dependency Policy

本プロジェクトでは、担当者間の依存を最小化する。

- Network / IO層は、IRCコマンドの意味を知らない。
- Protocol / Command層は、Network / IO層の内部実装を知らない。
- Command層は送信処理を直接行わない。
- Command層はコマンド処理結果を `CommandResult` として返す。
- Serverは `CommandResult` を受け取り、対象fdのsend bufferへ積む。
- `ServerState` は `Client` と `Channel` の辞書を管理する。
- `Channel` は `Client` を所有しない。`Client*` を参照するだけ。
- `Client` の生成・削除は `ServerState` が担当する。

---

## 3. Processing Flow

基本的な処理フローは以下。

```text
Server
  poll() でfdイベントを受け取る
    ↓
Connection / recv buffer
  complete line を取り出す
    ↓
Parser
  line を Message に変換する
    ↓
CommandDispatcher
  Message と ServerState を使ってコマンド処理する
    ↓
CommandDispatcher
  CommandResult を返す
    ↓
Server
  CommandResult 内の t_reply を対象fdのsend bufferへ積む
    ↓
Connection / send buffer
  POLLOUTで送信する
```

---

## 4. Common Interface Types

### 4.1 `t_reply`

送信対象fdと送信文字列を表す。

```cpp
struct t_reply
{
    int         fd;
    std::string reply;
};
```

#### 役割

- Command層が「誰に何を送るか」を表現する。
- 実際の `send()` は行わない。
- Serverがこの情報を見てsend bufferへ積む。

---

### 4.2 `CommandResult`

1つのIRCコマンド処理結果を表す。

```cpp
class CommandResult
{
public:
    CommandResult();
    CommandResult(const CommandResult& src);
    ~CommandResult();

    CommandResult& operator=(const CommandResult& src);

    void addReply(int fd, const std::string& reply);
    const std::vector<t_reply>& getReplies() const;
    void requestToDisconnect();
    bool shouldDisconnect() const;
};
```

#### 公開関数

| 関数プロトタイプ | 戻り値 | 利用者 | 内容 |
| --- | ---: | --- | --- |
| `void addReply(int fd, const std::string& reply);` | `void` | Command層 | 送信対象fdと返信文字列を追加する |
| `const std::vector<t_reply>& getReplies() const;` | `const std::vector<t_reply>&` | Server | 返信一覧を取得する |
| `void requestToDisconnect();` | `void` | Command層 | コマンド処理後の切断を要求する |
| `bool shouldDisconnect() const;` | `bool` | Server | 切断要求があるか確認する |

#### 役割

- コマンド処理後に送るべきメッセージを保持する。
- 必要に応じて切断要求を表す。
- Command層からServerへ返される。
- Command層が `Connection` や `Server` を直接触らないための境界になる。

---

## 5. B: Protocol / Command Interface

BはIRCメッセージの解析、コマンド振り分け、返信生成を担当する。

BはNetwork / IOクラスに依存しない。

---

### 5.1 `Message`

IRCの1行を解析した結果を表す。

```cpp
class Message
{
public:
    Message();
    Message(const Message& src);
    Message(const std::string& command,
            const std::vector<std::string>& params);
    ~Message();

    Message& operator=(const Message& src);

    const std::string& getCommand() const;
    const std::vector<std::string>& getParams() const;
    size_t getParamCount() const;
    const std::string& getSingleParam(size_t index) const;
    bool hasParam(size_t index) const;
};
```

#### 公開関数

| 関数プロトタイプ | 戻り値 | 利用者 | 内容 |
| --- | ---: | --- | --- |
| `const std::string& getCommand() const;` | `const std::string&` | `CommandDispatcher` | command名を取得する |
| `const std::vector<std::string>& getParams() const;` | `const std::vector<std::string>&` | `CommandDispatcher` | parameter一覧を取得する |
| `size_t getParamCount() const;` | `size_t` | `CommandDispatcher` | parameter数を取得する |
| `const std::string& getSingleParam(size_t index) const;` | `const std::string&` | `CommandDispatcher` | 指定indexのparameterを取得する |
| `bool hasParam(size_t index) const;` | `bool` | `CommandDispatcher` | 指定indexのparameterが存在するか確認する |

#### 補足

`trailing` は `_params` の最後の要素として扱う。

例:

```text
PRIVMSG #room :hello world
```

```text
command = "PRIVMSG"
params  = ["#room", "hello world"]
```

---

### 5.2 `Parser`

IRCの1行文字列を `Message` に変換する。

```cpp
class Parser
{
public:
    Parser();
    Parser(const Parser& src);
    ~Parser();

    Parser& operator=(const Parser& src);

    static Message parse(const std::string& line);
};
```

#### 公開関数

| 関数プロトタイプ | 戻り値 | 利用者 | 内容 |
| --- | ---: | --- | --- |
| `static Message parse(const std::string& line);` | `Message` | Server | 1行文字列を `Message` へ変換する |

#### 補足

`Parser` はrecv bufferを扱わない。
recv bufferからcomplete lineを切り出すのはNetwork / IO層の責務。

---

### 5.3 `CommandDispatcher`

IRC commandを実行し、処理結果を `CommandResult` として返す。

```cpp
class CommandDispatcher
{
public:
    CommandDispatcher();
    CommandDispatcher(const CommandDispatcher& src);
    ~CommandDispatcher();

    CommandDispatcher& operator=(const CommandDispatcher& src);

    CommandResult dispatch(int fd, const Message& msg, ServerState& state);
};
```

#### 公開関数

| 関数プロトタイプ | 戻り値 | 利用者 | 内容 |
| --- | ---: | --- | --- |
| `CommandResult dispatch(int fd, const Message& msg, ServerState& state);` | `CommandResult` | Server | IRC commandを実行し、結果を返す |

#### private handler

以下のhandlerは `CommandDispatcher` 内部で利用する。

- `handlePass(int fd, const Message& msg, ServerState& state)`
- `handleNick(int fd, const Message& msg, ServerState& state)`
- `handleUser(int fd, const Message& msg, ServerState& state)`
- `handleJoin(int fd, const Message& msg, ServerState& state)`
- `handlePrivMsg(int fd, const Message& msg, ServerState& state)`
- `handleKick(int fd, const Message& msg, ServerState& state)`
- `handleInvite(int fd, const Message& msg, ServerState& state)`
- `handleTopic(int fd, const Message& msg, ServerState& state)`
- `handleMode(int fd, const Message& msg, ServerState& state)`

#### やらないこと

- `send()`
- `queueSend()`
- `Connection` 操作
- `Poller` 操作
- `Server` 操作

#### やること

- `Message` のcommandを見る。
- `ServerState` から `Client` / `Channel` を取得する。
- 必要に応じて状態を更新する。
- `ReplyBuilder` で返信文字列を作る。
- `CommandResult` に送信対象と文字列を詰める。

---

### 5.4 `ReplyBuilder`

IRC返信文字列を生成する。

```cpp
class ReplyBuilder
{
public:
    ReplyBuilder();
    ReplyBuilder(const ReplyBuilder& src);
    ~ReplyBuilder();

    ReplyBuilder& operator=(const ReplyBuilder& src);

    static std::string numeric(const std::string& target,
                               const std::string& num,
                               const std::string& msg);
    static std::string numeric(const Client& client,
                               const std::string& num,
                               const std::string& msg);

    static std::string join(const Client& client, const std::string& channel);
    static std::string privMsg(const Client& from,
                               const std::string& to,
                               const std::string& msg);

    static std::string kick(const Client& opUser, const Client& target,
                            const std::string& channel,
                            const std::string& reason);
    static std::string invite(const Client& opUser,
                              const Client& target,
                              const std::string& channel);
    static std::string topic(const Client& opUser,
                             const std::string& channel,
                             const std::string& topic);
    static std::string mode(const Client& opUser,
                            const std::string& channel,
                            const std::string& modeStr);
};
```

#### 公開関数

| 関数プロトタイプ | 戻り値 | 利用者 | 内容 |
| --- | ---: | --- | --- |
| `static std::string numeric(const std::string& target, const std::string& num, const std::string& msg);` | `std::string` | `CommandDispatcher` | target文字列を指定してnumeric replyを生成する |
| `static std::string numeric(const Client& client, const std::string& num, const std::string& msg);` | `std::string` | `CommandDispatcher` | Clientを指定してnumeric replyを生成する |
| `static std::string join(const Client& client, const std::string& channel);` | `std::string` | `CommandDispatcher` | JOIN通知を生成する |
| `static std::string privMsg(const Client& from, const std::string& to, const std::string& msg);` | `std::string` | `CommandDispatcher` | PRIVMSG配送文を生成する |
| `static std::string kick(const Client& opUser, const Client& target, const std::string& channel, const std::string& reason);` | `std::string` | `CommandDispatcher` | KICK通知を生成する |
| `static std::string invite(const Client& opUser, const Client& target, const std::string& channel);` | `std::string` | `CommandDispatcher` | INVITE通知を生成する |
| `static std::string topic(const Client& opUser, const std::string& channel, const std::string& topic);` | `std::string` | `CommandDispatcher` | TOPIC通知を生成する |
| `static std::string mode(const Client& opUser, const std::string& channel, const std::string& modeStr);` | `std::string` | `CommandDispatcher` | MODE通知を生成する |

---

## 6. C1: Client / ServerState Interface

C1はClientの状態、Clientの生死、サーバ全体の辞書を管理する。

---

### 6.1 `Client`

接続中のクライアント状態を表す。

```cpp
class Client
{
public:
    Client();
    Client(const Client& src);
    Client(int socketFd);
    ~Client();

    Client& operator=(const Client& src);

    int getSocketFd() const;
    const std::string& getUserName() const;
    const std::string& getRealName() const;
    const std::string& getNickName() const;
    bool isPassAccepted() const;
    bool isRegistered() const;
    void setUserName(const std::string& name);
    void setRealName(const std::string& name);
    void setNickByServerState(const std::string& name);
    void acceptPassword();
    bool tryRegister();
};
```

#### 公開関数

| 関数プロトタイプ | 戻り値 | 利用者 | 内容 |
| --- | ---: | --- | --- |
| `int getSocketFd() const;` | `int` | B / C2 | Clientに対応するfdを取得する |
| `const std::string& getUserName() const;` | `const std::string&` | B | usernameを取得する |
| `const std::string& getRealName() const;` | `const std::string&` | B | realnameを取得する |
| `const std::string& getNickName() const;` | `const std::string&` | B / C2 | nicknameを取得する |
| `bool isPassAccepted() const;` | `bool` | B | PASS済みか確認する |
| `bool isRegistered() const;` | `bool` | B | 登録完了済みか確認する |
| `void setUserName(const std::string& name);` | `void` | B | usernameを設定する |
| `void setRealName(const std::string& name);` | `void` | B | realnameを設定する |
| `void setNickByServerState(const std::string& name);` | `void` | `ServerState` | nicknameを設定する |
| `void acceptPassword();` | `void` | B | PASS成功状態にする |
| `bool tryRegister();` | `bool` | B | 登録条件が揃っていれば登録済みにする |

#### 注意

`Client::setNickByServerState()` は `ServerState` から呼ぶための関数。
nick変更は必ず `ServerState::updateNickname()` を通す。

---

### 6.2 `ServerState`

サーバ全体の `Client` / `Channel` 辞書とpasswordを管理する。

```cpp
class ServerState
{
public:
    ServerState();
    ServerState(const ServerState& src);
    ServerState(const std::string& password);
    ~ServerState();

    ServerState& operator=(const ServerState& src);

    bool addNewClient(int socketFd);
    bool removeClient(int socketFd);
    bool updateNickname(Client& client, const std::string& nick);
    bool isNicknameExist(const std::string& nick) const;
    Client* getClientByFd(int socketFd) const;
    Client* getClientByNick(const std::string& nick) const;

    Channel* ensureChannel(const std::string& name);
    bool removeChannelIfEmpty(const std::string& name);
    Channel* findChannel(const std::string& name) const;

    bool isCorrectPassword(const std::string& pass) const;
};
```

#### 公開関数

| 関数プロトタイプ | 戻り値 | 利用者 | 内容 |
| --- | ---: | --- | --- |
| `bool addNewClient(int socketFd);` | `bool` | Server | 新規接続時に `Client` を作成する |
| `bool removeClient(int socketFd);` | `bool` | Server / B | 切断時に `Client` を削除する |
| `bool updateNickname(Client& client, const std::string& nick);` | `bool` | B | Clientのnicknameとnickname辞書を同時に更新する |
| `bool isNicknameExist(const std::string& nick) const;` | `bool` | B | nickname重複を確認する |
| `Client* getClientByFd(int socketFd) const;` | `Client*` | B | fdからClientを取得する |
| `Client* getClientByNick(const std::string& nick) const;` | `Client*` | B | nicknameからClientを取得する |
| `Channel* ensureChannel(const std::string& name);` | `Channel*` | B | Channelを取得する。なければ作成する |
| `bool removeChannelIfEmpty(const std::string& name);` | `bool` | B / Server | 空Channelを削除する |
| `Channel* findChannel(const std::string& name) const;` | `Channel*` | B | Channelを取得する。なければNULL |
| `bool isCorrectPassword(const std::string& pass) const;` | `bool` | B | サーバpasswordと一致するか確認する |

#### 削除ルール

`removeClient(int socketFd)` はClientを削除する前に、全Channelから該当Clientへの参照を除去する必要がある。

削除対象:

- Channel member
- Channel operator
- invited list

その後、空になったChannelを削除し、`fd -> Client` と `nick -> Client` の辞書を更新する。

---

## 7. C2: Channel Interface

C2はChannel内部状態とChannelModesを管理する。

---

### 7.1 `Channel`

IRC channelの参加者、operator、招待リスト、topic、modeを管理する。

```cpp
class Channel
{
public:
    Channel();
    Channel(const Channel& src);
    Channel(const std::string& name);
    ~Channel();

    Channel& operator=(const Channel& src);

    const std::string& getChannelName() const;
    const std::string& getChannelTopic() const;
    std::vector<Client*> getMembers() const;
    std::string getChannelModes() const;

    void setChannelTopic(const std::string& topic);
    bool applyMode(char opt, bool flag, const std::string& val);
    bool addMember(Client* target);
    bool addOperator(Client* target);
    bool addInvitedMember(Client* target);

    bool removeMember(Client* target);
    bool removeOperator(Client* target);
    bool removeInvitedMember(Client* target);

    bool isChannelMember(Client* target) const;
    bool isOperator(Client* target) const;
    bool isInvitedMember(Client* target) const;
};
```

#### 公開関数

| 関数プロトタイプ | 戻り値 | 利用者 | 内容 |
| --- | ---: | --- | --- |
| `const std::string& getChannelName() const;` | `const std::string&` | B / C1 | channel名を取得する |
| `const std::string& getChannelTopic() const;` | `const std::string&` | B | topicを取得する |
| `std::vector<Client*> getMembers() const;` | `std::vector<Client*>` | B | member一覧を取得する |
| `std::string getChannelModes() const;` | `std::string` | B | 現在のmode文字列を取得する |
| `void setChannelTopic(const std::string& topic);` | `void` | B | topicを設定する |
| `bool applyMode(char opt, bool flag, const std::string& val);` | `bool` | B | mode変更を適用する |
| `bool addMember(Client* target);` | `bool` | B | memberを追加する |
| `bool addOperator(Client* target);` | `bool` | B | operator権限を付与する |
| `bool addInvitedMember(Client* target);` | `bool` | B | 招待リストに追加する |
| `bool removeMember(Client* target);` | `bool` | B / C1 | memberを削除する |
| `bool removeOperator(Client* target);` | `bool` | B / C1 | operator権限を剥奪する |
| `bool removeInvitedMember(Client* target);` | `bool` | B / C1 | 招待状態を解除する |
| `bool isChannelMember(Client* target) const;` | `bool` | B | Clientが参加済みか確認する |
| `bool isOperator(Client* target) const;` | `bool` | B | Clientがchannel operatorか確認する |
| `bool isInvitedMember(Client* target) const;` | `bool` | B | 招待済みか確認する |

#### 補足

`Channel` は `Client` を所有しない。
`Channel` は `Client*` を保持するだけで、`delete` はしない。

#### JOIN時のoperator初期化

新規作成されたChannelに最初のClientが参加する場合、参加処理は `addMember()` に加えて `addOperator()` も行う。

この判定は `CommandDispatcher` 側で行う。

---

### 7.2 `ChannelModes`

Channel modeの状態を管理する。

```cpp
class ChannelModes
{
public:
    ChannelModes();
    ChannelModes(const ChannelModes& src);
    ~ChannelModes();

    ChannelModes& operator=(const ChannelModes& src);

    bool isInviteOnly() const;
    bool isTopicRestricted() const;
    bool isMemberLimited() const;
    bool isChannelProtected() const;
    int getMemberLimit() const;
    bool checkChannelPassword(const std::string& input) const;

    void setInviteOnly();
    void unsetInviteOnly();
    void setTopicRestricted();
    void unsetTopicRestricted();
    void setMemberLimit(int limit);
    void unsetMemberLimit();
    void setChannelProtected(const std::string& password);
    void unsetChannelProtected();
};
```

#### 公開関数

| 関数プロトタイプ | 戻り値 | 利用者 | 内容 |
| --- | ---: | --- | --- |
| `bool isInviteOnly() const;` | `bool` | B / C2 | `+i` 状態を取得する |
| `bool isTopicRestricted() const;` | `bool` | B / C2 | `+t` 状態を取得する |
| `bool isMemberLimited() const;` | `bool` | B / C2 | `+l` 状態を取得する |
| `bool isChannelProtected() const;` | `bool` | B / C2 | `+k` 状態を取得する |
| `int getMemberLimit() const;` | `int` | B / C2 | user limitを取得する |
| `bool checkChannelPassword(const std::string& input) const;` | `bool` | B / C2 | channel keyが一致するか確認する |
| `void setInviteOnly();` | `void` | C2 | `+i` を設定する |
| `void unsetInviteOnly();` | `void` | C2 | `-i` を設定する |
| `void setTopicRestricted();` | `void` | C2 | `+t` を設定する |
| `void unsetTopicRestricted();` | `void` | C2 | `-t` を設定する |
| `void setMemberLimit(int limit);` | `void` | C2 | `+l` を設定する |
| `void unsetMemberLimit();` | `void` | C2 | `-l` を設定する |
| `void setChannelProtected(const std::string& password);` | `void` | C2 | `+k` を設定する |
| `void unsetChannelProtected();` | `void` | C2 | `-k` を設定する |

#### 補足

外部から直接 `ChannelModes` を取得する公開関数は現行 `Channel.hpp` にはない。
Command層からは原則として `Channel::applyMode()` と `Channel::getChannelModes()` を通して操作する。

---

## 8. Important Rules

### 8.1 Command層はNetwork / IO層に依存しない

`CommandDispatcher` は以下をincludeしない。

- `Server.hpp`
- `Connection.hpp`
- `Poller.hpp`
- `ConnectionManager.hpp`

Command層は `CommandResult` を返すだけにする。

---

### 8.2 Server applies CommandResult

Serverは `CommandResult` を受け取って送信処理を行う。

```cpp
CommandResult result = dispatcher.dispatch(fd, msg, state);
applyCommandResult(result);
```

`applyCommandResult()` は、`result.getReplies()` を見て各fdのsend bufferへ積む。

---

### 8.3 Nick update

nick変更は必ず `ServerState::updateNickname()` を通す。

NG:

```cpp
client.setNickByServerState(newNick);
```

OK:

```cpp
state.updateNickname(client, newNick);
```

---

### 8.4 Client ownership

Clientの生成・削除は `ServerState` が担当する。

- `ServerState::addNewClient(socketFd)` で作成する。
- `ServerState::removeClient(socketFd)` で削除する。
- `Channel` は `Client*` を参照するだけ。
- `Channel` は `Client*` を `delete` しない。

---

### 8.5 Channel operator

operator権限は `Client` ではなく `Channel` が管理する。

NG:

```cpp
client.setOperator(true);
```

OK:

```cpp
channel.addOperator(&client);
```

理由:

1人のClientは複数Channelに参加でき、Channelごとにoperator権限が異なるため。

---

### 8.6 POLLOUT

送信すべきデータがあるfdだけ `POLLOUT` を有効にする。

- send bufferが空でない場合、`POLLOUT` を有効にする。
- send bufferが空になった場合、`POLLOUT` を無効にする。
- 常時 `POLLOUT` を監視し続けない。

---

### 8.7 Client removal cleanup

`ServerState::removeClient(socketFd)` はClientを削除する前に、全Channelから該当Clientへの参照を除去する。

削除対象:

- member
- operator
- invited list

`Channel` は `Client*` を所有しないため、Client削除後にChannel内へ `Client*` を残してはいけない。

---

### 8.8 TOPIC / MODE read operations

`TOPIC #channel` と `MODE #channel` は状態変更ではなく照会として扱う。

- `TOPIC #channel` はtopicがあればnumeric reply、なければno topicのnumeric replyを返す。
- `MODE #channel` は `Channel::getChannelModes()` で現在のmode文字列を作り、numeric replyを返す。

---

## 9. Open Questions

以下は実装しながら確定する。

### 9.1 `Poller` を分離するか

現時点では `includes/*.hpp` に `Poller` はない。

まずは `Server` 内で管理してもよい。
肥大化した場合に `Poller` へ分離する。

---

### 9.2 `ConnectionManager` を分離するか

現時点では `includes/*.hpp` に `ConnectionManager` はない。

まずは `Server` がfdごとの接続状態を直接管理してもよい。
send buffer管理が膨らむ場合、`ConnectionManager` へ分離する。

---

### 9.3 `ClientRegistry` を分離するか

現時点では `includes/*.hpp` に `ClientRegistry` はない。

まずは `ServerState` が `fd -> Client` と `nick -> Client` を直接持つ。
肥大化した場合に `ClientRegistry` へ分離する。

---

### 9.4 `ChannelService` を分離するか

現時点では `includes/*.hpp` に `ChannelService` はない。

まずは `CommandDispatcher` が `Channel` / `ChannelModes` を直接操作する。
肥大化した場合に `ChannelService` へ分離する。
