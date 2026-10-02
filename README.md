# IRC

*This project has been created as part of the 42 curriculum by lguerbig, pchalmin and tle-floc.*

## Description

This project is an IRC (Internet Relay Chat) server written in C++ 98. IRC is a text-based communication protocol that provides real-time messaging, either public or private : users can exchange direct messages and join group channels, while clients connect to servers to take part in them.

This project implements the server side only. It does not include an IRC client, nor server-to-server communication.

### Features

- Multiple simultaneous clients, handled without ever blocking.
- Authentication with a password, nickname and username setup.
- Channels : joining and message forwarding to every member.
- Private messages between users.
- Regular users and channel operators.
- Operator-specific commands :
	- `KICK` : eject a client from a channel.
	- `INVITE` : invite a client to a channel.
	- `TOPIC` : view or change the channel topic.
	- `MODE` : change the channel's mode :
		- `i` : set/remove invite-only.
		- `t` : set/remove the restriction of `TOPIC` to operators.
		- `k` : set/remove the channel key (password).
		- `o` : give/take operator privilege.
		- `l` : set/remove the user limit.
	<!-- TODO : complete with other features implement (not ask in the subject) -->
	<!-- TODO : complete with bot features -->
	<!-- TODO : complete with bonus feature "Handle file transfer" -->

### Technical overview

- Non-blocking I/O : all file descriptors are non-blocking and no forking is used.
- Single event loop : a single `poll()` call handles every operation (listening, reading and writing).
- TCP/IP : communication over TCP/IP (v4 or v6).
- Partial data handling : incoming packets are aggregated per client so that commands split across several reads (with `Ctrl + D`) are correctly rebuilt (e.g. `JO` + `IN` + ` #my_` + `channel\n`).
- Reference client : the server is designed to behave like an official IRC server when used with HexChat.

### Allowed external functions

`socket`, `close`, `setsockopt`, `getsockname`, `getprotobyname`, `gethostbyname`, `getaddrinfo`, `freeaddrinfo`, `bind`, `connect`, `listen`, `accept`, `htons`, `htonl`, `ntohs`, `ntohl`, `inet_addr`, `inet_ntoa`, `send`, `recv`, `signal`, `sigaction`, `lseek`, `fstat`, `fcntl`, `poll` (or equivalent).

### Commands

<!-- TODO : commands with arguments and argument explication-->

### Bot commands

<!-- TODO : commands with arguments and argument explication-->

## Tools

| Tool | Version |
|------|---------|
| clang++ | 12 |
| valgrind | 3.18.1 |
| Make | any |

## Project architecture

```
project-root
├── irc
│   └── [project files...]
├── unit_tests
│   └── [unit test files...]
├── .gitignore
├── en.subject.pdf
├── flake.lock
├── flake.nix
└── README.md
```

For this project, we have created our own [tester](unit_tests/README.md).

## Instructions

### Nix

To avoid compatibility issues, you can use the Nix terminal :
```bash
nix develop
```

### Project

All commands are executed in the irc directory.

#### Makefile

Use the provided `Makefile` to compile the project and manage it:

| Command | Description |
|--------|-------------|
| `make` / `make all` | Compile the project |
| `make bonus` | Compile the project and the bot |
| `make clean` | Remove object files |
| `make fclean` | Remove object files and executables |
| `make re` | Clean and recompile the project |

#### Run program

To launch the server :
```bash
./ircserv <port> <password>
```

- `port`: the port on which the server listens for incoming IRC connections.
- `password`: the connection password, required by any client trying to connect.

To connect to the server :
```bash
netcat 127.0.0.1 <port>
```

To connect the bot to the server :
```bash
./weatherBot <port> <password>
```

## Resources

[Norm RFC 2812](https://datatracker.ietf.org/doc/html/rfc2812)
