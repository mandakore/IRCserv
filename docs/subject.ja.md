C++98標準規格を使用してIRCサーバーを開発する必要があります。

IRCクライアントを開発してはいけません。
サーバー間通信を実装してはいけません。

作成した実行ファイルは、以下の形式で実行してください。
./ircserv port password

ポート：IRCサーバーがIRCクライアントからの接続を受け付ける際に使用するポート番号です。
password: 接続パスワード。このサーバーに接続しようとするすべてのIRCクライアントで必要になります。

課題文や評価項目では `poll()` が言及されていますが、`select()`、`kqueue()`、`epoll()` などの同等機能を使っても構いません。

---

* サーバーは、複数のクライアントを同時に処理できなければならず、処理が停止（ハング）してはいけません。
* `fork()` の使用は禁止されています。すべてのI/O操作はノンブロッキングで行う必要があります。
* これらすべての操作（read、write、listen など）を処理するために使用できる `poll()`（または同等機能）は1つだけです。

ノンブロッキングなファイルディスクリプタを使用する必要があるため、`poll()`（または同等機能）を使わなくても `read/recv` や `write/send` を呼び出した際に、サーバー自体はブロックされない可能性があります。

しかし、その方法ではシステムリソースをより多く消費してしまいます。

そのため、`poll()`（または同等機能）を使用せずに、いずれかのファイルディスクリプタに対して `read/recv` または `write/send` を行った場合、評価は0点になります。

* IRCクライアントは複数存在します。その中から1つをリファレンスクライアント（基準となるクライアント）として選ばなければなりません。評価時には、そのリファレンスクライアントが使用されます。
* あなたのリファレンスクライアントは、エラーなくサーバーへ接続できなければなりません。

* クライアントとサーバー間の通信は、TCP/IP（IPv4 または IPv6）を用いて行わなければなりません。

* あなたのサーバーでリファレンスクライアントを使用した際の挙動は、公式IRCサーバーで使用した場合と同様でなければなりません。
* ただし、実装が必要なのは以下の機能のみです。
aaaaaaaaaaaaaaaaaaaaaaaaaa
* リファレンスクライアントを用いて、認証、ニックネーム設定、ユーザー名設定、チャンネル参加、プライベートメッセージの送受信ができなければなりません。
* あるクライアントからチャンネルへ送信されたすべてのメッセージは、そのチャンネルに参加している他のすべてのクライアントへ転送されなければなりません。
* オペレーター（管理者）ユーザーと通常ユーザーを実装しなければなりません。
* 次に、チャンネルオペレーター専用のコマンドを実装しなければなりません。

  * `KICK` — クライアントをチャンネルから追放する
  * `INVITE` — クライアントをチャンネルへ招待する
  * `TOPIC` — チャンネルのトピックを変更または表示する
  * `MODE` — チャンネルのモードを変更する

    * `i` : 招待制チャンネルの設定 / 解除
    * `t` : `TOPIC` コマンドをチャンネルオペレーターのみに制限する設定 / 解除
    * `k` : チャンネルキー（パスワード）の設定 / 解除
    * `o` : チャンネルオペレーター権限を付与 / 剥奪する
    * `l` : チャンネルのユーザー数制限を設定 / 解除する

* もちろん、クリーンなコードを書くことも求められます。

MacOS は `write()` の実装が他の Unix 系OSと異なるため、`fcntl()` の使用が許可されています。

他の Unix 系OS と似た挙動を得るために、ファイルディスクリプタはノンブロッキングモードで使用しなければなりません。

ただし、`fcntl()` の使用は以下の形式に限り許可されています。

```c
fcntl(fd, F_SETFL, O_NONBLOCK);
```

これ以外のフラグを使用することは禁止されています。


---

## ソケット・ネットワーク系

### `socket`

通信に使う「ソケット」を作る関数。
TCPサーバー/クライアントの開始地点。

```cpp
int fd = socket(AF_INET, SOCK_STREAM, 0);
```

* `AF_INET` : IPv4
* `SOCK_STREAM` : TCP通信

---

### `close`

ファイルやソケットを閉じる。

```cpp
close(fd);
```

---

### `setsockopt`

ソケットの設定を変更する。

よく使う:

```cpp
int yes = 1;
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
```

→ ポート再利用を許可。

---

### `getsockname`

ソケットに割り当てられたアドレス情報を取得。

```cpp
getsockname(fd, ...);
```

「実際にどのポートが使われているか」など確認できる。

---

### `getprotobyname`

プロトコル名から情報取得。

```cpp
getprotobyname("tcp");
```

現在はあまり使わない。

---

### `gethostbyname`

ホスト名からIP取得。

```cpp
gethostbyname("google.com");
```

古いAPI。今は `getaddrinfo` 推奨。

---

### `getaddrinfo`

ホスト名やポート番号をアドレス構造体に変換。

```cpp
getaddrinfo("localhost", "8080", ...);
```

IPv4/IPv6両対応。

---

### `freeaddrinfo`

`getaddrinfo` が確保したメモリを解放。

```cpp
freeaddrinfo(res);
```

---

### `bind`

ソケットにIP/ポートを割り当てる。

```cpp
bind(fd, ...);
```

サーバーで必須。

---

### `connect`

サーバーへ接続。

```cpp
connect(fd, ...);
```

クライアント側で使う。

---

### `listen`

接続待ち状態にする。

```cpp
listen(fd, SOMAXCONN);
```

---

### `accept`

クライアント接続を受け入れる。

```cpp
int client = accept(fd, ...);
```

新しいクライアント用ソケットが返る。

---

## バイト順変換

ネットワークではビッグエンディアンを使う。

---

### `htons`

short型をネットワークバイト順へ。

```cpp
addr.sin_port = htons(8080);
```

Host TO Network Short

---

### `htonl`

long型を変換。

```cpp
htonl(x);
```

---

### `ntohs`

ネットワーク→ホストへ変換。

```cpp
ntohs(port);
```

---

### `ntohl`

long版。

---

## IPアドレス変換

### `inet_addr`

文字列IP → 数値IP。

```cpp
inet_addr("127.0.0.1");
```

古め。

---

### `inet_ntoa`

数値IP → 文字列IP。

```cpp
inet_ntoa(addr.sin_addr);
```

---

### `inet_ntop`

新しい版。IPv6対応。

```cpp
inet_ntop(AF_INET, ...);
```

---

## データ送受信

### `send`

データ送信。

```cpp
send(fd, buf, len, 0);
```

TCP通信でよく使う。

---

### `recv`

データ受信。

```cpp
recv(fd, buf, size, 0);
```

0が返ると接続終了。

---

# シグナル系

## `signal`

シグナルハンドラ設定。

```cpp
signal(SIGINT, handler);
```

Ctrl+C対策など。

---

## `sigaction`

より高機能な signal。

```cpp
sigaction(SIGINT, &sa, NULL);
```

ft_ircならこっち推奨。

---

## `sigemptyset`

シグナル集合を空にする。

```cpp
sigemptyset(&set);
```

---

## `sigfillset`

全部のシグナルを追加。

```cpp
sigfillset(&set);
```

---

## `sigaddset`

集合へシグナル追加。

```cpp
sigaddset(&set, SIGINT);
```

---

## `sigdelset`

集合から削除。

```cpp
sigdelset(&set, SIGINT);
```

---

## `sigismember`

集合に含まれるか確認。

```cpp
sigismember(&set, SIGINT);
```

---

# ファイル操作系

## `lseek`

ファイル位置を移動。

```cpp
lseek(fd, 0, SEEK_SET);
```

* `SEEK_SET` : 先頭基準
* `SEEK_CUR` : 現在位置
* `SEEK_END` : 末尾

---

## `fstat`

ファイル情報取得。

```cpp
struct stat st;
fstat(fd, &st);
```

サイズなど確認可能。

---

## `fcntl`

ファイル/ソケット設定変更。

よく使う:

```cpp
fcntl(fd, F_SETFL, O_NONBLOCK);
```

→ ノンブロッキング化。

ft_ircで超重要。

---

# 多重化I/O

## `poll`

複数ソケットを同時監視。

```cpp
poll(fds, nfds, timeout);
```

* 読み込み可能
* 接続
* 切断

などをまとめて監視できる。

ft_irc の中心機能。

---

# ft_ircで特に重要なもの

特に頻繁に使うのは:

* `socket`
* `bind`
* `listen`
* `accept`
* `send`
* `recv`
* `poll`
* `fcntl`
* `close`
* `setsockopt`

この辺。


