# Task-05: UART ECHO Report

Date: 13 October 2026.

## Objective and environment

Implement a UART ECHO application in C that receives CRLF-terminated messages,
swaps ASCII letter case, returns the converted message, and logs incoming data.

| Item | Configuration |
| --- | --- |
| Target | Raspberry Pi, hostname `arpinet`, user `arpi` |
| Project directory | `~/Task-05_UART` |
| UART device | `/dev/ttyAMA0` on the target's 40-pin header |
| Serial configuration | 115200 baud, 8 data bits, no parity, 1 stop bit; no flow control |
| Build/deployment tools | GCC, GNU Make, SSH/scp; ARM64 prefix `aarch64-linux-gnu-` |

## Implementation

| Module | Responsibility |
| --- | --- |
| `main.c` | Command-line argument, application loop, logging, signals, cleanup |
| `serial.c` / `serial.h` | UART setup through `termios`, nonblocking I/O, port closure |
| `protocol.c` / `protocol.h` | CRLF framing and ASCII case conversion |

The UART is opened with `O_RDWR | O_NOCTTY | O_NONBLOCK`. `termios` sets 115200
8N1 and disables canonical input, terminal echo, CR/LF conversion, and software
and RTS/CTS flow control. Setup failures close the descriptor while preserving
`errno` for error reporting.

`LineBuffer` retains bytes between reads. A message is complete only when `\n`
immediately follows `\r`, even when the two bytes arrive separately. CRLF is
removed from the payload and appended to the response:

```text
Request:  aBc XYZ 123!\r\n
Response: AbC xyz 123!\r\n
```

Only `a`-`z` and `A`-`Z` change. The 256-byte buffer permits a maximum complete
payload of 255 bytes because the trailing CR also occupies buffer space.
Overlong input is logged and terminates the application with an error.

### Logging and error handling

At startup, the application creates `logs/` relative to its working directory
and opens `uart_YYYY-MM-DD_HH-MM-SS.log`. Original messages are logged before
conversion using local-time timestamps and flushed after each message:

```text
[2026-10-13 17:45:52] RX: aBc
```

UART errors are printed to stderr and written to the log before termination.
A missing device produced exit code `1` and this actual log entry:

```text
[2026-10-13 17:53:48] ERROR: /dev/uart-does-not-exist: No such file or directory
```

If the log itself cannot be opened or written, the error is reported on screen.

### Shutdown

`sigaction` handlers for `SIGINT` and `SIGTERM` set a `volatile sig_atomic_t`
flag. UART operations check it and use `poll()` with a 100 ms timeout.
Nonblocking writes handle partial transfers and `EAGAIN`, allowing shutdown
when the other endpoint stops reading.

The UART and log are closed after leaving the loop. Exit codes are `0` for
successful signal-requested shutdown, `1` for errors, and `2` for an incorrect
number of arguments. Incomplete input is discarded on shutdown.

## Build and deployment

The Makefile supports `all`, `clean`, `deploy`, and `CROSS_COMPILE`. It discovers
`src/*.c` automatically and tracks header dependencies with `-MMD -MP`.
Deployment uses `scp` and settings from `local.mk`.

```bash
make clean
make CROSS_COMPILE=aarch64-linux-gnu-
make deploy CROSS_COMPILE=aarch64-linux-gnu-
```

Cross-compilation, deployment, and execution on the target were confirmed by
the developer. Cleanup also removes project logs, so required test evidence
must be preserved beforehand.

## Verification

### Target checks

The UART was tested with a TX-RX jumper under the ordinary `arpi` account,
confirming application access. CRLF reception, conversion, and logging worked.
After response transmission was enabled, loopback produced repeated `aBc` / `AbC`
messages: each response became the next request.

`Ctrl+C` printed `Shutdown requested` and returned `0`. A nonexistent UART
printed and logged an error and returned `1`. A separate terminal-program
loopback transcript was not supplied.

### Software checks

The final sources, including the corrected setup cleanup and RTS/CTS settings,
compiled without warnings:

```bash
gcc -Iinc -std=c11 -Wall -Wextra -Wpedantic -O2 \
    src/main.c src/serial.c src/protocol.c -o echo_app
```

Additional checks used a POSIX pseudo-terminal (PTY):

| Check | Result |
| --- | --- |
| Missing device argument | Exit `2` |
| Nonexistent device | Exit `1`; error printed and logged |
| `/dev/null` supplied as UART | Setup error logged; exit `1` |
| 115200 8N1 with inherited RTS/CTS enabled | Settings applied; RTS/CTS disabled |
| Incomplete CRLF | No response |
| CR and LF received separately | Correct converted response with CRLF |
| Empty and consecutive messages | Correct separate responses |
| 255-byte payload | Full converted response |
| `SIGTERM` while awaiting input | Exit `0` |
| Original payload and shutdown logging | Expected records present |
| 256-byte payload | Capacity error; exit `1` |
| `SIGINT` with UART output backpressure | Exit `0`; no hang |
| `SIGTERM` with UART output backpressure | Exit `0`; no hang |

All 13 checks passed. PTY tests verify application behavior and `termios`
settings; they do not measure physical UART voltage or wire-level baud rate.

## Limitations and task status

- Independent HOST-TARGET UART exchange was not verified because the USB-UART adapter was faulty.
- Exact UART boot-configuration commands and a terminal-program loopback transcript were not supplied.
- Payloads are limited to 255 bytes; case conversion applies to ASCII letters only.
- Timestamps have one-second resolution; launches within the same second append to the same log file.
- Logs store payload bytes directly: an embedded LF can create a line without a timestamp. The shutdown information record also has no timestamp.
- Permission-denied behavior uses the `open()` error path but was not independently tested.
- Automatic startup through systemd/init is optional and is not included in the reviewed implementation.
