# Embedded Serial Communication System (C)

A modular C library simulating serial communication over UART, CAN, and MODBUS protocols with CRC-based error detection and built-in diagnostics.

## Features
- Multi-protocol support: UART, CAN, ModBus
- CRC-16 (CCITT) frame validation
- Frame create / send / receive / validate pipeline
- Built-in diagnostic reporting with timestamps
- Simulated physical layer (can be adapted to real `/dev/tty*` / `/dev/can*`)

## Build & Run
```bash
make
./comm_system
```

## Project Structure
```
src/
  main.c    — demo: opens UART, CAN, MODBUS channels; sends/receives frames
  comm.c    — channel management, frame helpers, CRC, diagnostics
  comm.h    — public API
Makefile
README.md
```

## Concepts Demonstrated
- Serial protocols: UART, CAN bus, MODBUS RTU
- CRC-16 error detection
- Frame-based communication design
- Diagnostic logging and error codes
- Layered hardware abstraction
