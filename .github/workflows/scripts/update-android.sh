#!/bin/bash

set -euo pipefail;

mkdir -p ~/.ssh

mkdir tmp_android_ws
cd tmp_android_ws

set +x
echo "$FB_ANDROID_DEPLOY_KEY" > ~/.ssh/id_rsa
chmod 600 ~/.ssh/id_rsa
set -x
git clone git@github.com:kremi151/FunkyBoyAndroid.git

cd FunkyBoyAndroid
git submodule update --init --recursive
GIT_UPDATE_OUTPUT=$(git submodule update --recursive --remote)

if ! echo "$GIT_UPDATE_OUTPUT" | grep -q "checked out"; then
	echo "Nothing changed, exiting..."
	exit 0
fi

git add app/src/main/cpp/funkyboy
git config --local user.email "$(git log --format='%ae' HEAD^!)"
git config --local user.name "CI"
git commit -m "Update FunkyBoy" -m "Placeholder to include commit messages"
git push

cd ../..
rm -rf tmp_android_ws
