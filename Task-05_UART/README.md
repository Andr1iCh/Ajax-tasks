# UART Echo


## Features


## Build

Build the application using GNU Make.

**Native build:**

```bash
make
```

**Cross-compilation for ARM64:**

```bash
make CROSS_COMPILE=aarch64-linux-gnu-
```

The executable is generated at `bin/echo_app`.

**Clean build artifacts:**

```bash
make clean
```

## Deployment

Deployment uses `scp` to transfer the executable to the target device over SSH.

Create a `local.mk` file in the project root:

```makefile
TARGET_HOST = arpinet
TARGET_USER = arpi
DEPLOY_DIR = /home/arpi
```

Then build and deploy:

```bash
make CROSS_COMPILE=aarch64-linux-gnu- deploy
```


## Usage


## Documentation

See [Report.md](Report.md) for hardware configuration, UART testing, implementation details, and test results.