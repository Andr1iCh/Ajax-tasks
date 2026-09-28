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

# Task 4 – Native Build on TARGET: Comparison Report

For this step it was necessary to create a new, modified universal build script, `build_universal_host.sh`, which replaces the separate HOST and cross-compile scripts. 

## Program output

```
========================================
 Hostname:  arpinet
 Time:      2026-09-28 22:50:44
 OS:        Linux 6.18.50+rpt-rpi-2712
 Hardware:  aarch64
========================================
```

The output matches Task 3 apart from the time: same hostname, kernel and architecture.

## Binutils comparison

| Tool / field | Task 3: cross-built on HOST | Task 4: native on TARGET |
|---|---|---|
| `readelf -h` Machine | AArch64 | AArch64 |
| Class / Data / OS-ABI / Type | ELF64 / little endian / System V / DYN (PIE) | same |
| Entry point | `0xcc0` | `0xbc0` |
| Program / section headers | 10 / 29 | 10 / 29 |
| Section headers offset | 69152 | 69104 |
| `NEEDED` | `libc.so.6`, `ld-linux-aarch64.so.1` | `libc.so.6` |
| Interpreter (`ldd`) | `/lib/ld-linux-aarch64.so.1` | `/lib/ld-linux-aarch64.so.1` |
| libc path | `/lib/aarch64-linux-gnu/libc.so.6` | `/lib/aarch64-linux-gnu/libc.so.6` |
| `size` text | 3558 | 3295 (−263) |
| `size` data | 760 | 712 (−48) |
| `size` bss | 8 | 8 |
| `size` total | 4326 (0x10e6) | 4015 (0xfaf), −311 |
| `strings` | 4 labels | same 4 labels |

## Explaining the differences

- **Identical properties:** architecture, ELF class, ABI, PIE, header counts, interpreter and libc location are the same. Both binaries target the same platform, so nothing about the runtime environment changed.
- **`NEEDED`:** the extra `ld-linux-aarch64.so.1` from the cross build is gone. Both builds use the same source, so the difference comes from the toolchain: the native gcc/binutils on TARGET and the cross toolchain on HOST are different packages and possibly different versions, with different linker defaults. This supports the earlier assumption that the extra entry was a linker artifact and not a real dependency.
- **Smaller text (−263 B) and data (−48 B):** the code is the same, so the difference most likely comes from the different compiler and linker versions and from the flags used in `build_universal_host.sh` (`-O2` and `-mcpu=cortex-a76` were set explicitly in Task 3). The size change is small (under 8%) and has no functional effect.
- **Entry point and header offset:** these shift slightly (`0xcc0` → `0xbc0`, 69152 → 69104) because the section layout changed with the code size. The file remains about 70 KB, which is consistent with the 64 KB segment alignment on AArch64 seen in Task 3.

## Conclusions

Building natively on TARGET produces a working binary with the same platform-level properties as the cross-compiled one: the architecture, loader and libc dependency are identical, and the program output reports the same hardware and kernel. The only visible differences are a cleaner dependency list (just `libc.so.6`) and a slightly smaller code size, both caused by the different toolchain rather than by the source or the target.

# Task 5 – Static Linking (`-static`): Comparison Report

The build script from Task 3 was copied and modified to link the application statically (`-static`). The binary `app_static_target` was built and analysed on TARGET and is compared with the dynamically linked native TARGET build from Task 4.

## Program output

```
========================================
 Hostname:  arpinet
 Time:      2026-09-28 23:22:25
 OS:        Linux 6.18.50+rpt-rpi-2712
 Hardware:  aarch64
========================================
```

The output is identical to the dynamic build (same hostname, kernel and architecture), so static linking does not change the program's behaviour.

## Binutils comparison

| Tool / field | Task 4: dynamic (native TARGET) | Task 5: static (`-static`) |
|---|---|---|
| `readelf -h` Machine | AArch64 | AArch64 |
| Type | DYN (PIE) | EXEC (fixed address) |
| OS/ABI | UNIX – System V | UNIX – GNU |
| Entry point | `0xbc0` | `0x4009c0` |
| Program / section headers | 10 / 29 | 7 / 25 |
| Section headers offset | 69104 | 848008 |
| `NEEDED` | `libc.so.6` | none (no dynamic section) |
| `ldd` | vdso, `libc.so.6`, loader | `not a dynamic executable` |
| `size` text | 3295 | 676853 |
| `size` data | 712 | 23372 |
| `size` bss | 8 | 22096 |
| `size` total | 4015 (0xfaf) | 722321 (0xb0591) |
| `strings` | 4 labels | same 4 labels |

## Explaining the differences

- **Size (~180× larger):** the static binary contains the parts of glibc that the program uses (stdio, time and locale handling, startup code, memory management) instead of loading them from `libc.so.6` at run time. `text` grows from about 3 KB to about 660 KB, and the file is roughly 830 KB (section headers at offset 848008 plus 25 × 64 bytes).
- **Data and bss:** the larger `data` (23 KB) and `bss` (22 KB) hold glibc's own tables and internal state (locale, I/O, allocator, startup structures), which previously lived in the shared library.
- **No dynamic linking:** there is no interpreter, no `.dynamic` section and no `NEEDED` entries, which is why `readelf -d` shows nothing and `ldd` reports `not a dynamic executable`. The program headers drop from 10 to 7 because the interpreter and dynamic segments are gone.
- **EXEC instead of PIE:** a plain `-static` build produces a non-relocatable executable loaded at a fixed address (entry `0x4009c0`, i.e. near `0x400000`) instead of a position-independent one. The binary's own address is therefore not randomized by ASLR.
- **OS/ABI GNU:** static glibc uses GNU-specific features (such as IFUNC, used to select optimized routines like `memcpy` at start-up), and the linker marks the file as UNIX – GNU instead of System V.
- **`strings`:** the four format strings are the same in both builds, because they come from the program's own source.

## Conclusions

Static linking trades size for independence. The static binary is about 180 times larger than the dynamic one, but it depends only on the kernel and the CPU architecture: it needs no `libc.so.6` or `ld-linux-aarch64.so.1` on the machine where it runs, and it behaves exactly the same. This removes the main risk found in the cross-development tasks, a missing or incompatible glibc on the target system.
