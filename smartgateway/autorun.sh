#!/bin/sh
NODE_HOME="/opt/node-v5.1.0-linux-armv7l"
NODE_PATH=$NODE_HOME/lib/node_modules

export PATH=$PATH:$NODE_HOME/bin 

sleep 60
cd /home/lewis/portus/SmartGateway
ln -sf /opt/node-v5.1.0-linux-armv7l/bin/pangolin ./pangolin
node ./pangolin client -r 45.32.248.21:10000 -l 8360 > /var/log/pangolin &
/home/lewis/portus/SmartGateway/SmartGateway > /var/log/SmartGateway & 
