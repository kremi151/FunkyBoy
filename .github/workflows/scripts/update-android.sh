#!/bin/bash

set -euo pipefail;
set +x

PARENT_REPO="kremi151/FunkyBoy"

mkdir -p ~/.ssh

mkdir tmp_android_ws
cd tmp_android_ws

echo "$FB_ANDROID_DEPLOY_KEY" > ~/.ssh/id_rsa
chmod 600 ~/.ssh/id_rsa
git clone git@github.com:kremi151/FunkyBoyAndroid.git

cd FunkyBoyAndroid
FBA_WORKSPACE=$(pwd)

git submodule update --init --recursive

cd app/src/main/cpp/funkyboy
OLD_COMMIT_HASH=$(git rev-parse HEAD)
cd $FBA_WORKSPACE

GIT_UPDATE_OUTPUT=$(git submodule update --recursive --remote)

if ! echo "$GIT_UPDATE_OUTPUT" | grep -q "checked out"; then
	echo "Nothing changed, exiting..."
	exit 0
fi

cd app/src/main/cpp/funkyboy
NEW_COMMIT_HASH=$(git rev-parse HEAD)

# Prepare commit message
echo "Update FunkyBoy core" > /tmp/fb_git_log.txt
echo "" >> /tmp/fb_git_log.txt
git log --pretty="format:%h by %an: %s" ${OLD_COMMIT_HASH}..${NEW_COMMIT_HASH} >> /tmp/fb_git_log.txt
sed -i -E "s|#([[:digit:]]+)|${PARENT_REPO}#\1|" /tmp/fb_git_log.txt

cd $FBA_WORKSPACE

git add app/src/main/cpp/funkyboy
git config --local user.email "$(git log --format='%ae' HEAD^!)"
git config --local user.name "CI"
git commit -F /tmp/fb_git_log.txt
git push

cd ../..
rm -rf tmp_android_ws
