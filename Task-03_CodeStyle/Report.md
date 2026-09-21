## **Initial formatting check results**

Both clang-format 17 and clang-format 22 failed on the unmodified files. All 8 source and header files contained formatting violations.

Clang-format 17 reported 6943 issues.
Clang-format 22 reported 6941 issues.

The difference of 2 issues between versions 17 and 22 proves that different clang-format versions apply parsing and line-breaking rules slightly differently, even when using the exact same configuration file. None of the raw source files matched the Linux kernel code style configuration out of the box.
