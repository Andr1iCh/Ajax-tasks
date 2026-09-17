#!/usr/bin/env bash

CONFIG_PATH="$HOME/projects/.git_myconfig"


main(){

startup_security "$@"
source "$CONFIG_PATH"
confVar_security

check_target_dir "$1"
case "$?" in
1|3)
	if [[ $? -eq 1 ]];then
        echo "Directory $1 is created"
        fi
	mkdir -p "$1"

	clean_name="${1%/}"
	dir_name="${clean_name##*/}"	
	echo "# $dir_name" > "$clean_name/README.md"
	echo "README.md created in $1"

	git -C "$1" init -b "$USER_BRANCH"
	git -C "$1" config --local user.name "$USER_NAME"
	git -C "$1" config --local user.email "$USER_EMAIL"
	git -C "$1" config --local init.defaultBranch "$USER_BRANCH"
	
	if [[ -n "$2" ]];then
	git -C "$1" remote add origin "$2"
	fi
	;;
2)	
	if [[ -z "$2" ]]; then
	echo "To set remote, use the [remote_url] argument"
	exit 0
	else
	echo "Adding remote origin to existing repository"
	git -C "$1" remote add origin "$2"
	fi
	;;
	
esac

verify_cred "$@"
#rm -rf ./.git
}

verify_cred(){
echo -e "\nParameters set as:"
echo "DIR=	$1"
echo "NAME=	$(git -C "$1" config --local user.name)"
echo "EMAIL=	$(git -C "$1" config --local user.email)"
echo "BRANCH=	$(git -C "$1" config --local init.defaultBranch)"

if git -C "$1" remote get-url origin >/dev/null 2>&1; then
echo "REMOTE=	$(git -C "$1" remote get-url origin)"
else
echo "REMOTE=	NOT ASSIGNED"
fi

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
echo "Wrong argument value!"
exit 1
fi

if [[ ! -f "$CONFIG_PATH" ]]; then
echo "Config file does not exist!"
ask_confirmation
while [ $? -eq 3 ]; do
ask_confirmation
done
fi

if [[ $# -eq 0 || "$1" == "-h" || "$1" == "--help" ]]; then
show_help
exit 0
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


show_help(){
cat << EOF
USAGE:
${0##*/} <dir_name> [remote_url]
${0##*/} -h | --help

DESCRIPTION:
Initializes a local Git repository with personal configurations 
and optionally links it to a remote.

ARGUMENTS:
<dir_name>	the name of the directory which will be created and/or initialised with Git
[remote_url]	remote repository URL 

MODES:
0 params	help
1 params	create folder, git init, create README.md
2 params	execute initialization and link remote repository

EXAMPLES:
${0##*/} embedded_project
${0##*/} driver_lib git@github.com:username/driver_lib.git
EOF
}

check_target_dir(){

if [[ ! -e "$1" ]]; then
echo "Directory does not exist"
return 1
elif [[ -d "$1" ]];then
	if git -C "$1" rev-parse --is-inside-work-tree >/dev/null 2>&1; then
	echo "Repository already initialized"
	return 2
	elif [[ -z "$(ls -A "$1")" ]];then
	echo "Directory exists" 
	return 3
	else
	echo "Directory is not empty!"
	exit 6
	fi
	
else
echo "A file with this name already exists!"
exit 5
fi

}
main "$@"
