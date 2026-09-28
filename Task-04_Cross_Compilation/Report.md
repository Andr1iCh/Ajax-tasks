# Task 2 – HOST Build Report

**Build:** native `gcc` on HOST (x86-64, Linux/WSL2), default dynamic linking.

## Program output

```
========================================
 Hostname:  DESKTOP-TN9CF4K
 Time:      2026-09-28 18:15:48
 OS:        Linux 6.18.33.2-microsoft-standard-WSL2
 Hardware:  x86_64
========================================
```

## Binutils results

| Tool | Key data |
|---|---|
| `readelf -h` | ELF64, little endian, System V ABI, type DYN (PIE), machine x86-64, entry `0x11c0`, 14 program / 31 section headers |
| `readelf -d` | NEEDED: `libc.so.6` (only dependency) |
| `ldd` | `linux-vdso.so.1` (kernel-provided), `libc.so.6` → `/usr/lib/x86_64-linux-gnu/`, loader `/lib64/ld-linux-x86-64.so.2` |
| `size` | text 3490 B (code + read-only data), data 688 B (`.data`, `.got`, `.dynamic`), bss 48 B (zero-initialized), total 4226 B |
| `strings` | `Hostname:`, `Time:`, `OS:`, `Hardware:` – format strings in `.rodata` |

## Conclusions

The native build works, and the program's output agrees with the tools: `Hardware: x86_64` matches the x86-64 ELF header and the `x86_64-linux-gnu` libc path, and `OS: Linux` matches the System V ABI and glibc loader. Hostname, time and the WSL2 kernel release are runtime values obtained via `uname()`/`time()`, so they are not visible in the binary itself. The four labels found by `strings` correspond to the format strings in the source.

# Task 3 – Cross-Compiled TARGET Build: Comparison Report
In the build script for the TARGET, I decided to try enabling optimization and explicitly specifying the processor model
## Build commands

| | HOST build (Task 2) | TARGET build (Task 3) |
|---|---|---|
| Compiler | `gcc` (native x86-64) | `aarch64-linux-gnu-gcc` (cross) |
| Command | `gcc main.c -o app_host` | `aarch64-linux-gnu-gcc -Wall -Wextra -O2 -mcpu=cortex-a76 main.c -o app_target` |
| Flags | none (defaults, `-O0`) | `-Wall -Wextra` (warnings), `-O2` (optimization), `-mcpu=cortex-a76` (CPU target) |

The binary was built on HOST, copied to TARGET (Raspberry Pi 5, BCM2712 / Cortex-A76) and run there.

## Program output

| | HOST | TARGET |
|---|---|---|
| Hostname | DESKTOP-TN9CF4K | arpinet |
| OS | Linux 6.18.33.2-microsoft-standard-WSL2 | Linux 6.18.50+rpt-rpi-2712 |
| Hardware | x86_64 | aarch64 |

## Binutils comparison

| Tool / field | HOST `app_host` | TARGET `app_target` |
|---|---|---|
| `readelf -h` Machine | AMD x86-64 | AArch64 |
| Class / Data / OS-ABI / Type | ELF64 / little endian / System V / DYN (PIE) | same |
| Entry point | `0x11c0` | `0xcc0` |
| Program / section headers | 14 / 31 | 10 / 29 |
| Section headers offset | 14520 | 69152 |
| File size | not measured | 70 KB |
| `NEEDED` | `libc.so.6` | `libc.so.6`, `ld-linux-aarch64.so.1` |
| Interpreter (from `ldd`) | `/lib64/ld-linux-x86-64.so.2` | `/lib/ld-linux-aarch64.so.1` |
| libc path | `/usr/lib/x86_64-linux-gnu/libc.so.6` | `/lib/aarch64-linux-gnu/libc.so.6` |
| `size` text / data / bss | 3490 / 688 / 48 | 3558 / 760 / 8 |
| `size` total | 4226 (0x1082) | 4326 (0x10e6) |
| `strings` | 4 labels found | same 4 labels |

## Explaining the differences

- **Architecture:** `Machine`, entry point, loader name and library directories differ because they are architecture-specific. The generic parts like ELF64, little endian, System V, PIE are identical.
- **`-O2`, `-Wall`, `-Wextra`:** the warning flags only affect compile-time diagnostics, not the binary. `-O2` may simplify calls such as `fprintf` with constant strings, but for this small, I/O-bound program the effect on size and speed is negligible.
- **`-mcpu=cortex-a76`:** lets GCC use Cortex-A76 instructions (ARMv8.2-A) and its scheduling model. The gain here is negligible, and the trade-off is portability: the binary may not run on older cores such as the Cortex-A72 (Raspberry Pi 4). The HOST build used the generic x86-64 baseline. The ELF header flags stay `0x0`, so this restriction is not recorded in the file.
- **File size and headers:** the 70 KB file (section headers at offset 69152) versus a few KB on HOST is most likely due to AArch64 aligning segments to 64 KB pages instead of 4 KB. Sections themselves are the same size (about 4 KB total). Fewer program headers (10 vs 14) reflect a slightly different segment layout by the AArch64 linker.
- **`size`:** text is 68 B larger and data 72 B larger, while bss shrank from 48 to 8 B. This is most likely because on AArch64 `stdout`/`stderr` are reached through GOT entries (data) instead of copy-relocated variables (bss) as on x86-64.
- **Extra `NEEDED: ld-linux-aarch64.so.1`:** most likely added by the linker because the `libc.so` linker script marks the loader as `AS_NEEDED`. It is harmless: the same file is already used as the interpreter.

## Conclusions

The program built for TARGET runs correctly and its output agrees with the tools: `Hardware: aarch64` matches `Machine: AArch64`, and `OS: Linux` with the kernel `rpi-2712` describes the actual board. Generic ELF properties are the same as on HOST, while architecture-specific ones (machine type, entry point, loader path, libc location) differ, and code size is nearly identical. The optimization flags change little for such a small program.
