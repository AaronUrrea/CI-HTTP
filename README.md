## CI-HTTP - Aaron Urrea
 - Course: COMP-429 Computer Networks
 - Professor: Kevin Scrivnor

## Table of Contents

- [Overview](#overview)
- [Reading](#reading)
  - [Other Resources](#other-resources)
- [Implementation](#implementation)
  - [RFC 2616](#rfc-2616)
  - [Response Headers](#response-headers)
- [Testing](#testing)
- [Debugging](#debugging)
  - [Wireshark/Browser](#wiresharkbrowser)
  - [gdb](#gdb)
  - [Network Speed](#network-speed)

## Overview

For program 3, we will write a simple HTTP server that implements a select few request types and responses. Once implemented, your server should be able to serve HTTP requests from a modern browser. We will implement part of HTTP/1.1 as described in RFC 2616.

## Reading

- Chapter 4.* from the Stevens book
- Chapter 5.* from the Stevens book
- [RFC 2616: Hypertext Transfer Protocol — HTTP/1.1](https://www.w3.org/Protocols/rfc2616/rfc2616.html)
  - Header
  - Requests
  - Responses
  - GET method
  - HEAD method

and probably some easier reading:

- General
  - [Overview of HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP/Overview)
  - [HTTP Message Format](https://developer.mozilla.org/en-US/docs/Web/HTTP/Messages)
- Requests
  - [HEAD request](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/HEAD)
  - [GET request](https://developer.mozilla.org/en-US/docs/Web/HTTP/Methods/GET)
- Responses
  - [200 response](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/200)
  - [404 response](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/404)
  - [Content-length header](https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Content-Length)
  - [Last-Modified header](https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Last-Modified)
  - [Server header](https://developer.mozilla.org/en-US/docs/Web/HTTP/Headers/Server)

### Other Resources

- [21st Century C](https://nostarch.com/21stcenturyc)
- [The Linux Programming Interface](http://man7.org/tlpi/)

  - Socket chapters, File I/O

## Implementation

### RFC 2616

We are implementing some of RFC 2616, but not all of it.

- The server must listen on tcp port 80 and wait for an HTTP request
- The server must handle GET and HEAD requests appropriately, otherwise returning a 501 Not Implemented response
- Upon receiving a GET/HEAD request, the server should return
  - 200 OK along with the requested file (assuming it exists in www/)
  - 404 Not Found with a custom 404 HTML page to your liking

### Response Headers

The server is responsible for including the following header information in the response:

- `server` set to `cihttpd`
- `Content-Length`
- `Last-Modified` in the correctly formatted timestamp

## Testing

To test your program, use a browser and navigate to [http://localhost/index.html](http://localhost/index.html). Use your browser’s Network Inspection to monitor the traffic.

## Debugging

### Wireshark/Browser

To debug network traffic, launch Wireshark from the Linux system and monitor the l0 interface (loopback). A simple http filter will suffice, there isn’t much traffic here to begin with.

### gdb

Debugging with gdb has a small challenge, we are forking. Usually gdb follows the parent process after a fork, but we can change it by using `set follow-fork-mode child` or by attaching to the child PID after execution has started with `gdb -p [PID]`. This can help you find bugs when the child process has a segmentation fault, which can easily go unnoticed without using tools.

### Network Speed

We are doing networking with essentially 0ms lag. If something is taking longer than one second, it probably isn’t working. Or, you’re using an example file that is just way too big.

