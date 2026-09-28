# Task Overview

Goal was to build one small C program for two different systems and study how the build environment affects the result.

- **HOST:** x86-64 PC (Linux under WSL2)
- **TARGET:** Raspberry Pi 5 (aarch64, stock OS image)

The program (`main.c`, using `stdio.h`, `time.h`, `sys/utsname.h`) prints a formatted message with the hostname, current time, OS name/release and hardware platform. If a file name is passed as an argument, the message is appended to that file, with a warning if the file already exists.

The task covered these builds, each checked with `readelf`, `ldd`, `size` and `strings`:

1. native build on HOST;
2. cross-compiled build for TARGET (built on HOST, copied to TARGET);
3. native build on TARGET (after installing `build-essential`);
4. static build (`-static`) for TARGET;
5. final analysis of cross-development problems.

Results and comparisons are in `Report.md`.

## Scripts

| Script | What it does |
|---|---|
| `build_host.sh` | Native build on HOST: plain `gcc main.c -o app_host`. |
| `build_target.sh` | Cross-compiles on HOST with `aarch64-linux-gnu-gcc -Wall -Wextra -O2 -mcpu=cortex-a76` → `app_target`. |
| `build_universal_host.sh` | Native build that detects the machine with `uname -m` and names the output `app_host` (x86_64), `app_target` (aarch64) or `app_unknown`. Uses `gcc -Wall -Wextra -O2` without `-mcpu`. Created so the same script works after `git clone` on both machines. |
| `build_static_target.sh` | Same as `build_target.sh` plus `-static` → `app_static_target` (no shared libraries needed). |

## Usage

```bash
./build_universal_host.sh   # build on the current machine
./app_host                  # or ./app_target; prints the message to the console
./app_host report.txt       # appends the message to report.txt
```