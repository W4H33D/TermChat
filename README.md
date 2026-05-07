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

![TermChat Demo](./assets/termchat-demo.png)

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
│   └── cmake-build-debug
│
├── Client/
│   ├── main.c
│   ├── CMakeLists.txt
│   └── cmake-build-debug
│
└── README.md
