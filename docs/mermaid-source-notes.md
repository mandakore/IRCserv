# README Mermaid Diagram Source Notes

この資料は README に載せる Mermaid 図を作るための情報整理である。
実装の根拠はリリース用ソースである `Makefile`、`includes/`、`src/` とし、`docs/design.md` との差分は実装時の方針変更として扱う。

## 1. リリース対象の実装単位

`Makefile` は以下を `ircserv` としてビルドする。

- エントリポイント: `src/main.cpp`
- Network / IO: `Server`
- Protocol / Command: `Message`, `Parser`, `CommandDispatcher`, `CommandResult`, `ReplyBuilder`
- Client / State: `Client`, `ServerState`
- Channel: `Channel`, `ChannelModes`
- コマンドハンドラ: `PASS`, `NICK`, `USER`, `JOIN`, `PRIVMSG`, `KICK`, `INVITE`, `TOPIC`, `MODE`, `PING`, `CAP`

設計書では `Connection`, `Poller`, `ConnectionManager` の分離候補があるが、現在のリリース実装では未分離であり、`Server` がそれらの責務を内包している。

## 2. 推奨図一覧

README 用には以下の粒度が適切。

| 図 | Mermaid 種別 | 目的 |
| --- | --- | --- |
| 全体構成図 | `flowchart` または `graph` | 主要クラスのレイヤと依存方向を示す |
| I/O 処理シーケンス | `sequenceDiagram` | `poll()` から受信、コマンド処理、送信までを示す |
| クライアント登録フロー | `flowchart` | `PASS` / `NICK` / `USER` による登録完了条件を示す |
| チャンネル参加フロー | `flowchart` | `JOIN` の検証、作成、operator 付与、通知を示す |
| メッセージ配送フロー | `flowchart` | `PRIVMSG` の個人宛とチャンネル宛の分岐を示す |
| チャンネル操作フロー | `flowchart` | `KICK` / `INVITE` / `TOPIC` / `MODE` の共通権限チェックを示す |
| 状態保持関係図 | `classDiagram` または `erDiagram` | `ServerState`, `Client`, `Channel`, `ChannelModes` の参照関係を示す |

## 3. 全体構成図の要素

### ノード

- `main`
  - 引数 `<port> <password>` を検証する。
  - シグナルハンドラを設定する。
  - `Server` を生成して `ircLoop()` を開始する。
- `Server`
  - listen socket の作成、`poll()` ループ、accept、recv、send、切断処理を担当する。
  - `_pollfds`, `_recvBuffers`, `_sendBuffers`, `_state` を保持する。
- `Parser`
  - 受信した 1 行を `Message` に変換する。
  - command は大文字化する。
  - prefix と trailing parameter に対応する。
- `Message`
  - command と params を保持する。
- `CommandDispatcher`
  - command 名を判定し、各 `_handleXxx()` に分岐する。
  - 未登録クライアントには登録前許可コマンド以外で `451` を返す。
- `CommandResult`
  - `fd -> reply` のリストと切断要求を保持する。
- `ReplyBuilder`
  - numeric reply と broadcast 用メッセージ文字列を生成する。
- `ServerState`
  - `_clientsByFd`, `_clientsByNick`, `_channels` を集中管理する。
  - password 照合、nick 更新、channel 作成/削除、client 削除時の参照掃除を担当する。
- `Client`
  - fd、nick、username、realname、PASS 成功状態、登録完了状態を保持する。
- `Channel`
  - channel name、topic、members、operators、invited、modes を保持する。
- `ChannelModes`
  - `+i`, `+t`, `+k`, `+l` の状態と mode 引数を保持する。

### 依存方向

- `main --> Server`
- `Server --> ServerState`
- `Server --> Parser`
- `Server --> CommandDispatcher`
- `Server --> CommandResult`
- `Parser --> Message`
- `CommandDispatcher --> ServerState`
- `CommandDispatcher --> Message`
- `CommandDispatcher --> CommandResult`
- `CommandDispatcher --> ReplyBuilder`
- `CommandDispatcher --> Client`
- `CommandDispatcher --> Channel`
- `CommandDispatcher --> ChannelModes`
- `ServerState --> Client`
- `ServerState --> Channel`
- `Channel --> Client`
- `Channel --> ChannelModes`

## 4. I/O 処理シーケンス

### 受信から返信登録まで

1. `main` が `Server(port, password)` を生成する。
2. `Server::setupSocket()` が listen socket を作成する。
3. `Server::ircLoop()` が単一箇所で `poll()` を呼ぶ。
4. listen fd に `POLLIN` が来た場合、`Server::acceptNewClient()` を呼ぶ。
5. accept 済み client fd を `_pollfds` に追加する。
6. `_recvBuffers[fd]` と `_sendBuffers[fd]` を初期化する。
7. `ServerState::addNewClient(fd)` で `Client` を作成する。
8. client fd に `POLLIN` が来た場合、`Server::receiveData(fd)` を呼ぶ。
9. `recv()` 結果が `EAGAIN`, `EWOULDBLOCK`, `EINTR` の場合は切断しない。
10. `recv()` 結果が `0` または致命的エラーの場合は `disconnectClient(fd)` へ進む。
11. 受信データを `_recvBuffers[fd]` に追加する。
12. `\n` 単位で complete line を取り出し、末尾 `\r` があれば削除する。
13. `Server::processMessage(fd, line)` を呼ぶ。
14. `Parser::parse(line)` が `Message` を返す。
15. command が空なら何もしない。
16. `CommandDispatcher::dispatch(fd, msg, _state)` が `CommandResult` を返す。
17. `Server` が `CommandResult::getReplies()` を走査する。
18. 各 reply を `queueResponse(reply.fd, reply.reply)` で `_sendBuffers` に積む。
19. 対象 fd の `pollfd.events` に `POLLOUT` を追加する。
20. `CommandResult::shouldDisconnect()` が true なら `disconnectClient(fd)` を呼ぶ。

### 送信

1. client fd に `POLLOUT` が来た場合、`Server::sendData(fd)` を呼ぶ。
2. `_sendBuffers[fd]` が空なら `POLLOUT` を解除する。
3. buffer があれば `send()` する。
4. 送信済みバイト数だけ `_sendBuffers[fd]` から削除する。
5. buffer が空になったら `POLLOUT` を解除する。
6. `send()` が `EAGAIN`, `EWOULDBLOCK`, `EINTR` の場合は切断しない。
7. その他の送信エラーは `disconnectClient(fd)` へ進む。

## 5. クライアント登録フロー

登録完了条件は `Client::tryRegister()` に集約されている。

### 状態

- 初期状態
  - `passAccepted = false`
  - `nickname = ""`
  - `username = ""`
  - `registered = false`
- 登録完了状態
  - `passAccepted == true`
  - `nickname != ""`
  - `username != ""`
  - `registered == true`

### PASS

1. `PASS` の引数がなければ `461`。
2. すでに登録済みなら `462`。
3. password が不一致なら `464`。
4. 一致したら `Client::acceptPassword()`。
5. `tryRegister()` が成功したら `001`, `002`, `003`, `004` を返す。

### NICK

1. nick 引数がなければ `431`。
2. nick 形式が不正なら `432`。
3. 他 Client が使用中なら `433`。
4. `ServerState::updateNickname(client, nick)` で nick 辞書と `Client` を同時更新する。
5. `tryRegister()` が成功したら `001`, `002`, `003`, `004` を返す。

### USER

1. 引数が 4 未満なら `461`。
2. すでに登録済みなら `462`。
3. `username` と `realname` を設定する。
4. `tryRegister()` が成功したら `001`, `002`, `003`, `004` を返す。

### 登録前に許可される command

- `PASS`
- `NICK`
- `USER`
- `CAP`
- invalid command

それ以外は `CommandDispatcher::dispatch()` の入口で `451` を返す。

## 6. チャンネル参加フロー: JOIN

`JOIN` は複数 channel と複数 key を comma-separated で受け取る。

1. 引数がなければ `461`。
2. channel list を `,` で分割する。
3. key list があれば `,` で分割する。
4. 各 channel について `_joinSingleChannel()` を実行する。
5. channel 名が不正なら `403`。
6. `ServerState::findChannel(channel)` で既存 channel を探す。
7. 存在しなければ `ServerState::ensureChannel(channel)` で作成し、新規 channel として扱う。
8. すでに member なら何もしない。
9. 既存 channel の場合、以下を検証する。
   - `+i`: invited に含まれなければ `473`
   - `+k`: key 不一致なら `475`
   - `+l`: member 数が limit 以上なら `471`
10. `Channel::addMember(client)` で member に追加する。
11. 新規 channel の場合、最初の参加者を `Channel::addOperator(client)` で operator にする。
12. `JOIN` メッセージを channel 全員へ broadcast する。
13. 参加者へ topic numeric を返す。
   - topic なし: `331`
   - topic あり: `332`
14. 参加者へ names numeric を返す。
   - `353`
   - `366`

## 7. メッセージ配送フロー: PRIVMSG

`PRIVMSG` は複数 target を comma-separated で受け取る。

1. target 引数がなければ `411`。
2. message 引数がない、または空なら `412`。
3. target list を `,` で分割する。
4. target が `#` で始まる場合は channel 宛として処理する。
5. channel が存在しなければ `403`。
6. 送信者が channel member でなければ `404`。
7. `ReplyBuilder::privMsg()` でメッセージを作り、送信者以外の channel members へ broadcast する。
8. target が `#` で始まらない場合は nick 宛として処理する。
9. nick が存在しなければ `401`。
10. 対象 client の fd に reply を追加する。

## 8. チャンネル操作フロー

以下の操作は `CommandDispatcher` が権限判定と返信生成を行い、状態変更は `Channel` / `ChannelModes` に反映する。

### KICK

1. 引数が 2 未満なら `461`。
2. channel が存在しなければ `403`。
3. target nick を `,` で分割する。
4. 各 target について以下を検証する。
5. target nick が空なら `411`。
6. target client が存在しなければ `401`。
7. 実行者が channel member でなければ `442`。
8. 実行者が operator でなければ `482`。
9. target が channel member でなければ `441`。
10. `KICK` メッセージを channel 全員へ broadcast する。
11. `Channel::removeClient(target)` で member/operator/invited から削除する。
12. 空 channel になれば `ServerState::removeChannelIfEmpty()` で削除する。

### INVITE

1. 引数が 2 未満なら `461`。
2. channel が存在しなければ `403`。
3. 実行者が channel member でなければ `442`。
4. 実行者が operator でなければ `482`。
5. target nick が存在しなければ `401`。
6. target がすでに channel member なら `443`。
7. `Channel::addInvitedMember(target)` で invited に追加する。
8. 実行者へ `341` を返す。
9. target へ `INVITE` メッセージを送る。

### TOPIC

1. 引数がなければ `461`。
2. channel が存在しなければ `403`。
3. 実行者が channel member でなければ `442`。
4. topic 引数がなければ現在 topic を返す。
   - topic なし: `331`
   - topic あり: `332`
5. `+t` が有効で実行者が operator でなければ `482`。
6. `Channel::setChannelTopic()` で topic を更新する。
7. `TOPIC` メッセージを channel 全員へ broadcast する。

### MODE

1. 引数がなければ `461`。
2. channel が存在しなければ `403`。
3. mode 引数がなければ現在 mode を `324` で返す。
4. 実行者が channel member でなければ `442`。
5. 実行者が operator でなければ `482`。
6. mode string を左から順に処理する。
7. `+i` / `-i`: invite-only を設定/解除する。
8. `+t` / `-t`: topic restricted を設定/解除する。
9. `+k <key>`: channel key を設定する。
10. `-k`: channel key を解除する。
11. `+l <limit>`: limit が正の整数なら member limit を設定する。
12. `-l`: member limit を解除する。
13. `+o <nick>` / `-o <nick>`:
    - nick が存在しなければ `401`
    - target が channel member でなければ `441`
    - member なら operator を付与/剥奪する。
14. 未知 mode 文字は `472`。
15. 実際に変更された mode があれば `MODE` メッセージを channel 全員へ broadcast する。

## 9. 接続終了と状態掃除

`Server::disconnectClient(fd)` が接続終了の入口。

1. fd を `close()` する。
2. `_recvBuffers` から fd を削除する。
3. `_sendBuffers` から fd を削除する。
4. `ServerState::removeClient(fd)` を呼ぶ。
5. `_pollfds` から fd を削除する。

`ServerState::removeClient(fd)` は以下を行う。

1. fd から `Client*` を取得する。
2. 全 channel について `Channel::removeClient(client)` を呼ぶ。
3. 空になった channel 名を一時リストに積む。
4. 空 channel を `_channels` から削除して `delete` する。
5. nick があれば `_clientsByNick` から削除する。
6. `_clientsByFd` から削除する。
7. `Client` を `delete` する。

## 10. 状態保持関係

Mermaid の `classDiagram` では以下の関係を表すとよい。

- `Server` has `ServerState`
- `Server` has many `pollfd`
- `Server` has `_recvBuffers: map<int,string>`
- `Server` has `_sendBuffers: map<int,string>`
- `ServerState` owns many `Client`
- `ServerState` owns many `Channel`
- `ServerState` indexes `Client` by fd and nick
- `ServerState` indexes `Channel` by channel name
- `Channel` references many `Client` as members
- `Channel` references many `Client` as operators
- `Channel` references many `Client` as invited
- `Channel` owns one `ChannelModes`
- `CommandResult` has many `t_reply`
- `t_reply` contains `fd` and `reply`

所有関係の注意:

- `ServerState` は `Client*` と `Channel*` を `new/delete` する所有者である。
- `Channel` の `Client*` は所有しない参照である。
- そのため client 削除時には `ServerState::removeClient()` 経由で全 channel から参照を掃除する必要がある。

## 11. 実装上の設計差分

`docs/design.md` から Mermaid 図へ反映する際の注意点。

- `Connection`, `Poller`, `ConnectionManager` は現在の実装クラスとしては存在しない。
- 受信 buffer と送信 buffer は `Server` の `map<int, string>` にある。
- `poll()` を呼ぶ場所は `Server::ircLoop()` の 1 箇所。
- `POLLOUT` は実装済みで、送信待ち buffer がある fd にだけ追加される。
- partial write は `_sendBuffers[fd].erase(0, bytesSent)` で対応している。
- `recv()` / `send()` の `EAGAIN`, `EWOULDBLOCK`, `EINTR` は切断扱いにしない。
- non-blocking 設定は macOS では `fcntl(... O_NONBLOCK)`、Linux では `recv/send` に `MSG_DONTWAIT` を使う実装になっている。
- complete line の切り出しは `\n` 単位で、直前の `\r` を削除する。
- `QUIT` コマンドハンドラは存在しない。TCP 切断時の cleanup は実装済み。

## 12. コマンドと主な返信コード

| Command | 成功時の主な返信/配送 | 主なエラー |
| --- | --- | --- |
| `PASS` | 登録完了時 `001`-`004` | `461`, `462`, `464` |
| `NICK` | 登録完了時 `001`-`004` | `431`, `432`, `433` |
| `USER` | 登録完了時 `001`-`004` | `461`, `462` |
| `JOIN` | `JOIN` broadcast, `331`/`332`, `353`, `366` | `403`, `471`, `473`, `475` |
| `PRIVMSG` | 個人 fd または channel members へ配送 | `401`, `403`, `404`, `411`, `412` |
| `KICK` | `KICK` broadcast | `401`, `403`, `411`, `441`, `442`, `461`, `482` |
| `INVITE` | 実行者へ `341`, 対象へ `INVITE` | `401`, `403`, `442`, `443`, `461`, `482` |
| `TOPIC` | `331`/`332` または `TOPIC` broadcast | `403`, `442`, `461`, `482` |
| `MODE` | `324` または `MODE` broadcast | `401`, `403`, `441`, `442`, `461`, `472`, `482` |
| `PING` | `PONG ircServ :<token>` | なし |
| `CAP LS` | `CAP <target> LS :` | なし |
| `CAP REQ` | `CAP <target> NAK :<requested>` | なし |

## 13. 図化時の推奨スコープ

README の図では詳細な numeric message 文面までは描かず、以下に絞ると読みやすい。

- `Server` が I/O 境界、`CommandDispatcher` がプロトコル境界、`ServerState` が状態境界であること。
- コマンド処理は `CommandResult` を返し、送信は `Server` が担当すること。
- `Channel` は `members/operators/invited/modes` を持つこと。
- operator は `Client` ではなく `Channel` 側で管理されること。
- client 削除時に `ServerState` が全 channel から参照を掃除すること。
