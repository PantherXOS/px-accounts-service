#!/usr/bin/env bash

TARGET_PATH="/root/tmp/px-accounts-service"

ssh root@127.0.0.1 -p 2222 "#rm -rf $TARGET_PATH; mkdir -p $TARGET_PATH"

rsync -av -e "ssh -p 2222" --exclude ".git" --exclude "cmake-build-debug" --exclude ".idea" "." "root@127.0.0.1:$TARGET_PATH"

CMD='export GUILE_LOAD_PATH=$GUILE_LOAD_PATH:.'
CMD="$CMD;
     cd $TARGET_PATH;
     mkdir -p build && cd build ;
     cmake ..;
     make &&
     echo '#####################################################################################' &&
     ./src/px-accounts-service -p 123;
#     cd tests/ &&
#     ./tests;
#     ./tests 'Account Management Tasks';

#     cd src/ &&
#     ./px-accounts-service ;
     "

ssh root@127.0.0.1 -p 2222 $CMD