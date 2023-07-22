## cihttpd - Aaron Urrea
    Course: COMP-429 Computer Networks
    Professor - Kevin Scrivnor

## Table of Contents

- [Overview](#overview)
- [Files](#files)
  - [tftp.h](#tftph)
  - [ciftpd.c](#ciftpdc)
- [Usage](#usage)
- [Requirements](#requirements)
- [RFC 2616 Implementation](#rfc-2616-implementation)
- [Debugging](#debugging)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

---

## Overview

For program 3, we will write a simple HTTP server that implements a select few request types and responses. Once implemented, your server should be able to serve HTTP requests from a modern browser. We will implement part of HTTP/1.1 as described in RFC 2616.

## Files

### tftp.h

This header file contains the function prototypes and necessary includes for `ciftpd.c`.

### ciftpd.c

The C source file `ciftpd.c` contains the implementation of the `cihttpd` server. It includes functions to handle incoming HTTP requests, send data to clients, receive data from clients, and handle errors. The server uses UDP sockets to communicate with clients.

## Usage

To use `cihttpd`, follow these steps:

1. Compile the `ciftpd.c` source file:

   gcc ciftpd.c -o cihttpd

2. Run the cihttpd server:

    ./cihttpd

3. The server will listen on port 69 by default. You can navigate to http://localhost/index.html in your browser to test the server and monitor the network traffic using your browser's Network Inspection tool.

## Requirements

- Linux Operating System (tested on Linux)
- GCC compiler (tested with GCC)

## RFC 2616 Implementation

The server implements parts of RFC 2616, specifically:

    The server listens on TCP port 80 and waits for an HTTP request.
    The server handles GET and HEAD requests appropriately, returning a 501 Not Implemented response if necessary.
    Upon receiving a GET/HEAD request, the server returns:
        200 OK along with the requested file (assuming it exists in www/).
        404 Not Found with a custom 404 HTML page to your liking.

## Debugging

The project provides some debugging features:

    Network traffic can be debugged using Wireshark from the Linux system by monitoring the loopback interface (l0) with an HTTP filter.
    Debugging with gdb is supported, with considerations for handling child processes created by forking.

## License

This project is licensed under the MIT License.
