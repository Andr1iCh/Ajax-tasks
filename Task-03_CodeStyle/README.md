### Task Overview

This project investigates the impact of different `clang-format` versions on C/C++ code formatting. It focuses on analyzing how version discrepancies affect Git diff statistics and evaluates the project's transition between different style configurations.

### Scripts Description

* **`check_format.sh`**: Verifies if the source code in the target directory complies with the `.clang-format` rules by running a dry-run check with both `clang-format-17` and `clang-format-22`.

* **`make_format_17.sh`**: Automatically formats all `.c` and `.h` source files in-place using `clang-format-17`.

* **`make_format_22.sh`**: Automatically formats all `.c` and `.h` source files in-place using `clang-format-22`.
