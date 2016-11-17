#!/bin/bash

# Get arguments
if [ $# -ne 3 ]; then
    echo "Usage: ./measure.sh <teamname> <testsize> hdd|ssd"
    exit 1
fi
TEAM=$1
TESTSIZE=$2
DEVTYPE=$3
if [ $DEVTYPE != "hdd" ] && [ $DEVTYPE != "ssd" ]; then
    echo "Last argument must be 'hdd' or 'ssd'"
    exit 1
fi

# Constants
SERVER_LIST="http://cs.colgate.edu/~aaron/cosc301/f16/lab10_servers.txt"
CLOUDLAB_USER="agember"
SSH_OPTS="-i cloudlab_rsa"
WORKQUEUE="workqueue"

# Get server list
CLOUDLAB_HOST=`curl -s $SERVER_LIST | shuf -n 1`
echo "Selected server $CLOUDLAB_HOST"

# Prepare to transfer files
ssh $SSH_OPTS $CLOUDLAB_USER@$CLOUDLAB_HOST \
        "mkdir -p ~/uploads/$TEAM; cp ~/Makefile ~/uploads/$TEAM"

# Transfer files
scp $SSH_OPTS diskperf.c $CLOUDLAB_USER@$CLOUDLAB_HOST:~/uploads/$TEAM/

# Compile and run measurement
ssh $SSH_OPTS $CLOUDLAB_USER@$CLOUDLAB_HOST "~/client.sh $TEAM $TESTSIZE $DEVTYPE"
