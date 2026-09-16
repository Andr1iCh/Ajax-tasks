#!/usr/bin/env bash

source ./.git_myconfig

main(){

REMOTE_URL=$1

git init -b "$USER_BRANCH"
git config --local user.name "$USER_NAME"
git config --local user.email "$USER_EMAIL"
git config --local init.defaultBranch "$USER_BRANCH"

git remote add origin "$REMOTE_URL"

verify_cred
rm -rf ./.git
}

verify_cred(){
echo -e "\nParameters set as:"
echo "NAME   = $(git config --local user.name)"
echo "EMAIL  = $(git config --local user.email)"
echo "BRANCH = $(git config --local init.defaultBranch)"
echo "REMOTE = $(git remote get-url origin)"
echo "==================="
}

main "$@"
