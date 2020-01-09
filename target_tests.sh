#!/usr/bin/env bash

# REQUIRED PACKAGES:
# guix package -i cmake make pkg-config gcc-toolchain util-linux capnproto yaml-cpp python pybind11 nng

TARGET_PATH="/home/panther/projects/px-accounts-service"
SERVER_ADDRESS="panther@127.0.0.1"

# temp commands
#ssh $SERVER_ADDRESS -p 2222 'for pid in $(ps aux | grep px-secret | grep -v grep | awk '"'"'{ print $2}'"'"'); do echo "killing $pid"; done;';
#exit 0;

# run secret service
#ssh $SERVER_ADDRESS -p 2222 'screen -X -S secret kill'
#ssh $SERVER_ADDRESS -p 2222 'screen -d -S secret -m px-secret-service; sleep 1;'

# Uncomment for re-build
#ssh $SERVER_ADDRESS -p 2222 "rm -rf $TARGET_PATH;"

ssh $SERVER_ADDRESS -p 2222 "mkdir -p $TARGET_PATH"

rsync -av -e "ssh -p 2222" --exclude ".git" --exclude "cmake-build-debug" --exclude "docs/html" --exclude ".idea" "." "$SERVER_ADDRESS:$TARGET_PATH/"

CMD='export GUILE_LOAD_PATH=$GUILE_LOAD_PATH:.'
CMD="$CMD;
     cd $TARGET_PATH;
     mkdir -p build && cd build ;
     cmake .. &&
        make
        && echo '#####################################################################################'
        && echo 'DONE'
        && cd tests
        && ./tests -r console -d yes --order lex \"Account Writer Tests\";
#        && ./tests -d yes;
#        && ./tests -d yes \"Account Management Tasks\";
#        && ./secret_tester;
#     ./src/px-accounts-service;
#     ./src/px-accounts-service -p 123;
#     cd tests/ &&
#     ./tests;
#     ./tests 'Account Management Tasks';
#     cd src/ &&
#     ./px-accounts-service ;
     "

ssh $SERVER_ADDRESS -p 2222 $CMD