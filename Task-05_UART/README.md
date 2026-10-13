# UART Echo

A C application for Raspberry Pi that receives UART messages ending in `\r\n`,
swaps ASCII letter case, and sends the result back with `\r\n`.

## Features

- 115200 baud, 8N1, no flow control; device selected as a command-line argument.
- Timestamped logs of original messages and UART errors in `logs/`.
- Graceful shutdown on `SIGINT` and `SIGTERM`.
- Maximum message payload: 255 bytes.

## Build

```bash
make                                      # Native build
make clean                                # Removes build/, bin/, and logs/
make CROSS_COMPILE=aarch64-linux-gnu-      # ARM64 cross-build
```

The executable is `bin/echo_app`. Clean before switching architectures and
preserve any required logs before cleaning.

## Deployment

Create `local.mk` in the project root:

```makefile
TARGET_HOST = arpinet
TARGET_USER = arpi
DEPLOY_DIR = /home/arpi
```

With SSH access configured and the destination directory available:

```bash
make CROSS_COMPILE=aarch64-linux-gnu- deploy
```

This copies the executable to `/home/arpi/echo_app` using `scp`.

## Usage

Enable the target UART and grant the user access to its device, then run:

```bash
./bin/echo_app /dev/ttyAMA0
```

After deployment, run `/home/arpi/echo_app /dev/ttyAMA0` instead.
Send `aBc\r\n` to receive `AbC\r\n`. Stop with `Ctrl+C` or `SIGTERM`.
Logs are created in `logs/` under the current working directory:

```text
uart_2026-10-13_17-45-48.log
[2026-10-13 17:45:52] RX: aBc
```

Remove the TX-RX loopback jumper for normal host communication. Keep `logs/`
and `local.mk` out of Git. Format C sources using the project's `.clang-format`.

See [Report.md](Report.md) for implementation details, test results, and limitations.
