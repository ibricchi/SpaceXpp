#!/bin/bash

# Script for installing all necessary dependencies
# Make sure to reboot the machine after running this script to apply the changes to /etc/profile

# Install dependencies
sudo apt-get update
sudo apt-get -y upgrade
yes | sudo apt install \
    make \
    gcc \
    sqlite3 \
    jq

# Install Golang
wget https://dl.google.com/go/go1.16.linux-amd64.tar.gz
sudo tar -C /usr/local -xzf go1.16.linux-amd64.tar.gz
echo 'export PATH=$PATH:/usr/local/go/bin' | sudo tee -a /etc/profile

# Setup daily db backup
echo '0 03 * * * root /home/ubuntu/SpaceXpp/command/server/db_backup.sh' | sudo tee -a /etc/crontab
