#!/usr/bin/env sh

rsync -av -e "ssh -p 2222" "/Users/reza/PantherX/Projects/px-accounts-service" root@127.0.0.1:/root/projects/ --exclude ".git" --exclude ".idea" --exclude "cmake-build-debug"

ssh root@127.0.0.1 -p 2222 "cd projects/px-accounts-service/build && cmake .. && make && cd tests && ./tests"
