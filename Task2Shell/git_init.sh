#!/usr/bin/env bash

CONFIG_PATH="$HOME/projects/.git_myconfig"


main(){

startup_security "$@"
source "$CONFIG_PATH"

confVar_security

git init -b "$USER_BRANCH"
git config --local user.name "$USER_NAME"
git config --local user.email "$USER_EMAIL"
git config --local init.defaultBranch "$USER_BRANCH"
#git remote add origin "$USER_REMOTE"

verify_cred
#rm -rf ./.git
}

verify_cred(){
echo -e "\nParameters set as:"
echo "NAME   = $(git config --local user.name)"
echo "EMAIL  = $(git config --local user.email)"
echo "BRANCH = $(git config --local init.defaultBranch)"
#echo "REMOTE = $(git remote get-url origin)"
echo "==================="
}

ask_confirmation(){
read -p "Create file?(y/n): " answer

case "$answer" in
y|Y|yes|Yes|YES)
	init_gitConfig
	;;
n|N|no|No|NO)
	exit 0
	;;
*)
	return 3
	;;
esac
}

init_gitConfig(){
echo -e "\n===.git_myconfig creation manager ==="
mkdir -p "$(dirname "$CONFIG_PATH")"
touch "$CONFIG_PATH"

read -p "USER_NAME= " USER_NAME
read -p "USER_EMAIL= " USER_EMAIL
read -p "USER_BRANCH= " USER_BRANCH

echo "USER_NAME=\"$USER_NAME\"" >> "$CONFIG_PATH"
echo "USER_EMAIL=\"$USER_EMAIL\"" >> "$CONFIG_PATH"
echo "USER_BRANCH=\"$USER_BRANCH\"" >> "$CONFIG_PATH"
}

startup_security(){
if [ $# -gt 2 ]; then
echo "Wrong argument value"
exit 1
fi

if git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
echo "Repository is already initialized!"
exit 2
fi

if [[ ! -f "$CONFIG_PATH" ]]; then
echo "Config file does not exist"
ask_confirmation
while [ $? -eq 3 ]; do
ask_confirmation
done
fi
}

confVar_security(){
if [[ -z "$USER_NAME" ]]; then
        echo "USER_NAME is empty!"
        exit 4
elif [[ -z "$USER_EMAIL" ]]; then
        echo "USER_EMAIL is empty!"
        exit 4
elif [[ -z "$USER_BRANCH" ]]; then
        echo "USER_BRANCH is empty!"
        exit 4
fi

}

main "$@"
