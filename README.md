# TermChat

TermChat is a small TCP-based terminal chat application written in C.  
It was built as a practice project while learning low-level socket programming, multithreading, and terminal-based UI development in Linux.

The project consists of:

- A TCP server
- Multiple terminal clients
- Real-time message broadcasting between connected users
- A simple terminal UI built with `ncurses`

The goal of this project was not to build a production-ready chat platform, but to understand how networking applications work internally using raw sockets and threads in C.

---

## Features

- TCP socket communication
- Multi-client support
- Real-time messaging
- Terminal-based chat interface
- Lightweight and minimal design
- Linux-based development environment

---

## Demo

Below is a local demonstration of the server and two connected clients communicating in real time.

<img width="1918" height="938" alt="TermChat Demo" src="https://github.com/user-attachments/assets/7816fa56-d466-47a5-801c-07debd9f8295" />


---

## Technologies Used

- C Language
- POSIX Sockets
- pthreads
- ncurses
- CMake
- Linux / Ubuntu

---

## Project Structure

```text
termChat/
│
├── Server/
│   ├── main.c
│   ├── CMakeLists.txt
│   └── cmake-build-debug/
│       └── Server
│
├── Client/
│   ├── main.c
│   ├── CMakeLists.txt
│   └── cmake-build-debug/
│       └── Client
│
└── README.md
```

## Dependencies

This project uses standard Linux networking libraries along with pthread and ncurses.

Main headers used:

```
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <ncurses.h>
```

### Install Required Packages (Ubuntu/Debian)

```
sudo apt update
sudo apt install build-essential cmake libncurses5-dev libncursesw5-dev
```

## Build Instructions

This project was developed using CLion and uses separate CMakeLists.txt files for both the server and client.

**Build Server**

```
cd Server

cmake -B cmake-build-debug
cmake --build cmake-build-debug
```

**Build Client**

```
cd Client

cmake -B cmake-build-debug
cmake --build cmake-build-debug
```

## Running the Application

**Start the Server**

```
./cmake-build-debug/Server
```

**Example output:**

```
Server Active on 127.0.0.1:4444
```

## Start the Client

Open another terminal and run:

```
./cmake-build-debug/Client
```

You can run multiple clients simultaneously to test real-time messaging.

---

## Notes
- This is a learning/practice project.
- The application is intentionally simple.
- Security, authentication, encryption, scalability, and production-grade networking concerns are not implemented.
- The project mainly focuses on understanding:
      - TCP sockets
      - Client-server communication
      - Thread handling
      - Terminal UI programming

## About This Project

This project was partially developed with the assistance of AI/LLM tools during the learning and experimentation process.
You can think of it as a mix of hands-on systems programming practice and “vibe coding” experimentation while learning socket programming in C.
The primary objective was educational rather than production deployment.
