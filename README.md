# CACHE 🧠  
*A Lightweight In-Memory Tree-Based Database in C*

## Overview
CACHE is a minimalist, socket-based, in-memory database system written in pure C. It uses a custom hierarchical **tree structure** (with Nodes and Leaves) to store key-value data and supports basic database-like commands over a TCP connection.

This was developed as an exercise in low-level systems programming, combining **tree data structures** and **socket programming** to build a client-server application without third-party libraries.

## Features

- 🗂️ `CREATE` — Create folders/nodes in a tree (like directories)
- 📝 `INSERT` — Insert key-value pairs under a specific node
- 🔍 `SELECT` — Retrieve values by key and path
- 🌲 `PRINT` — Visualize the entire tree structure (with indentation)
- 🔌 Telnet-compatible socket server
- ⚙️ Custom memory management (manual allocation/zeroing)
- 📦 No external dependencies — 100% pure C

## Technologies Used

- C
- POSIX sockets (TCP/IP)
- Telnet for client testing
- Manual memory & pointer handling

## How It Works

- The **tree** starts with a root node.
- Clients connect via socket and issue commands like:
  ```bash
  CREATE /users
  INSERT /users name John
  SELECT /users name
  PRINT

Foder Structure:
    ├── cache22.h        # Core server & client definitions
    ├── server.c         # Main server logic
    ├── tree.c           # Tree data structure logic
    ├── tree.h           # Tree data structure declarations
    └── Makefile         # Build instructions


How to Run: (commands)
  . build : make all
  . start the server : ./cache
  . connect to server via port(server) : telnet localhost server_port

Sample Commands:
  CREATE /data
  INSERT /data key1 value1
  SELECT /data key1
  PRINT
  
Future Enhancements:
✅ Command error handling
🔐 Authentication support
🗃️ Persistent storage (serialize tree to file)
⚡ Hash table-based lookup
📚 Command history and better CLI parsing


 
 Huge thanks to Jonas Birch — his tutorials that laid the conceptual foundation that made this possible. 
