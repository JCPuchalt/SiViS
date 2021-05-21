#!/bin/bash

sudo apt-get install mysql-server
#sudo mysql_secure_installation

#sudo mysql -u root -p
sudo mysql -e "create database BiopolisDB;"
sudo mysql -u root -p BiopolisDB < Dump20210521.sql

sudo mysql -u root -e "ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY 'celegans';"
sudo mysql -u root -p celegans -e "FLUSH PRIVILEGES;"

chmod +x CapturaCE
./CapturaCE
#sudo mysqladmin -u root -h localhost password 'rootbiopolis'

#cd 
#git clone https://github.com/JCPuchalt/SiViS.git
#cd CapturaCE_App
#sudo mysql -u root -p BiopolisDB < Dump20210520.sql
