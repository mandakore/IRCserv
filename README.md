*This project has been created as part of the 42 curriculum by atashiro, sohyamaz.*

# Description
## Architecture

```mermaid
flowchart LR
    main[main.cpp] --> Server

    subgraph Network
        Server[Server]
    end

    subgraph Protocol
        Parser[Parser]
        Message[Message]
        Dispatcher[CommandDispatcher]
        Result[CommandResult]
        Reply[ReplyBuilder]
    end

    subgraph State
        ServerState[ServerState]
        Client[Client]
        Channel[Channel]
        ChannelModes[ChannelModes]
    end

    Server --> Parser
    Parser --> Message
    Message --> Dispatcher
    Dispatcher --> ServerState
    Dispatcher --> Reply
    Dispatcher --> Result
    Result --> Server

    Server --> ServerState
    ServerState --> Client
    ServerState --> Channel
    Channel --> Client
    Channel --> ChannelModes
```

## Request / Response Flow

```mermaid
sequenceDiagram
    participant Client as IRC Client
    participant Server as Server
    participant Parser as Parser
    participant Dispatcher as CommandDispatcher
    participant State as ServerState

    Client->>Server: send IRC message
    Server->>Server: poll / recv / build complete line
    Server->>Parser: parse(line)
    Parser-->>Server: Message
    Server->>Dispatcher: dispatch(fd, message, state)
    Dispatcher->>State: read / update server state
    Dispatcher-->>Server: CommandResult
    Server->>Server: queue replies to send buffer
    Server-->>Client: send replies by POLLOUT
```

## State Ownership

```mermaid
classDiagram
    Server *-- ServerState : owns
    ServerState *-- Client : owns
    ServerState *-- Channel : owns
    Channel *-- ChannelModes : owns
    Channel o-- Client : references
    CommandResult *-- t_reply : contains

    class Server {
        pollfds
        recvBuffers
        sendBuffers
    }

    class ServerState {
        clientsByFd
        clientsByNick
        channels
    }

    class Client {
        fd
        nick
        username
        registered
    }

    class Channel {
        members
        operators
        invited
        topic
    }

    class ChannelModes {
        +i
        +t
        +k
        +l
    }

    class t_reply {
        fd
        reply
    }
```

## About IRC

* IRC stands for Internet Relay Chat. It is a text-based chat protocol created in 1988.
* IRC allows users to communicate over the Internet through servers and clients.
* One important characteristic of IRC is that clients may disconnect at any time.
* Based on this idea, the server does not rely on a persistent database to store client data. It only manages the current state of clients, channels, and messages.

## About I/O Multiplexing

* This server uses I/O multiplexing to handle multiple clients at the same time.
* All socket I/O operations are performed in non-blocking mode.
* The server uses a single `poll()` loop to monitor client connections and process read/write events.

# Instructions

**Build and start the server**

```bash
make
./ircserv <portNumber> <serverPassword>
```

**Connect to the server using nc**

```bash
nc -C <hostName> <portNumber>
```

**Connect to the server using irssi**

```bash
irssi
/connect <hostName> <portNumber> <serverPass> <yourNickName>
```

# Resources
# Sites
[RFC 1459](https://datatracker.ietf.org/doc/html/rfc1459)

[簡単なエコーサーバを作成してみた](https://qiita.com/gu-chi/items/1e2ba4e19902f9e39b5e)

[I/O多重化を施したサーバを作成してみた](https://qiita.com/gu-chi/items/243fa63e17617bb9ef77)

[ノンブロッキングなファイルディスクリプタを用いて「I/O多重化」を施したエコーサーバーを作成してみた。](https://qiita.com/gu-chi/items/57d9ba6d6e797dfc8967)

[IRSSI](https://github.com/irssi/irssi)

## Books
- Michel J. Donahoo/Kenneth L. Calvert TCP/IP ソケットプログラミング C言語編 2003

## AI USAGE
We used AI
- To translate README we written in Japanese
- To write Mermaid format Architecture figure.
- Assist to understand new knowledge or functions from sites and books.

