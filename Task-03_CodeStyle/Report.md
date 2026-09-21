## **Initial formatting check results**

Both clang-format 17 and clang-format 22 failed on the unmodified files. All 8 source and header files contained formatting violations.

Clang-format 17 reported 6943 issues.
Clang-format 22 reported 6941 issues.

The difference of 2 issues between versions 17 and 22 proves that different clang-format versions apply parsing and line-breaking rules slightly differently, even when using the exact same configuration file. None of the raw source files matched the Linux kernel code style configuration out of the box.

## **Impact of clang-format version on formatting**

Applying clang-format 17 reformatted all 8 source and header files, enforcing Linux kernel indentation, tabs, and line breaks across the codebase. This produced massive git code churn: 5955 insertions and 5457 deletions across 13 files.

Applying clang-format 22 directly after version 17 did not leave the repository clean. It altered line breaks and alignment in 3 source files, adding 7 insertions and 11 deletions to the git statistics.

Result shows that even with the exact same .clang-format file, different major versions of the tool interpret wrapping and alignment rules differently. In git, this generates phantom diffs and pollutes commit history.

## **Chromium config comparison**
Comparing the dumped Chromium configurations between clang-format 17 and 22 resulted in 116 diff lines.
Key changes mainly include converting flat options into nested blocks, finer alignment sub-rules, modern macro support, and renamed legacy keys 