#!/bin/bash

sudo apt-get install mysql-server
#sudo mysql_secure_installation
#sudo mysql -u root -p

# Database restore
sudo mysql -u root -e "create database BiopolisDB;"
sudo mysql -u root BiopolisDB < ./Dump20210521.sql

# Root password configure
sudo mysql -u root -e "ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'celegans';"
sudo mysql -u root -pcelegans -e "FLUSH PRIVILEGES;"

# First start
chmod +x CapturaCE
./CapturaCE
