# Task 2 – Native (HOST) Build Report

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
