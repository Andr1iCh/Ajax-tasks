# git_init.sh

Bash script for initializing local Git repositories with predefined user configurations and optional remote linking.

## Configuration

Settings (`USER_NAME`, `USER_EMAIL`, `USER_BRANCH`) are loaded from `~/.git_myconfig`. If the file does not exist, the script prompts to create it interactively.

## Usage

```bash
./git_init.sh                     # Show help (prompts for setup if config is missing)
./git_init.sh <dir>               # Create directory, init Git repo, set local configs, create README.md
./git_init.sh <dir> <remote_url>  # Same as above + link remote origin (or attach remote to an existing repo)
