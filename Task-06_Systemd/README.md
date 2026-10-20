# UART Echo — TASK06

UART echo application managed by systemd: boot startup, restart on failure,
and graceful shutdown through SIGTERM. Swaps ASCII letter case at 115200 8N1.

## Configuration

Create a private `local.mk` and replace the placeholders:

```makefile
TARGET_HOST = <target-ip>
TARGET_USER = <target-user>
DEPLOY_DIR = /home/<target-user>
UART_DEVICE = /dev/ttyAMA0
INSTALL_BIN_DIR = /usr/local/bin
```

SSH access and sudo permissions are required. Keep `local.mk` out of Git.
The target user needs UART access through the `dialout` group.

## Build and install — host

```bash
make clean
make CROSS_COMPILE=aarch64-linux-gnu- LDFLAGS=-static install
```

Static linking avoids the target glibc version mismatch.
Installation copies the binary and unit, writes `/etc/uart-echo.conf`, reloads
systemd, and enables boot startup. It does not start the service immediately.

## Manage — target

```bash
sudo systemctl restart uart-echo.service
systemctl status uart-echo.service
sudo systemctl stop uart-echo.service
```

Expect `active (running)` after startup. Send `aBc\r\n` from the host UART
terminal and expect `AbC\r\n`. Reboot and check status to verify boot startup.

Logs are stored in `~/logs/` for the service user:

```bash
ls -lt ~/logs/
journalctl -u uart-echo.service -b --no-pager -n 20
```

## Uninstall — host

```bash
make uninstall
```

Stops and disables the service, removes installed and transferred files,
and preserves application logs. If a custom `INSTALL_BIN_DIR` was used,
pass the same value to `uninstall`.
