# This program is developed for Ubuntu (18.04)
## Program need a database (Dump20210521.sql), therefore
mysql-server installation is need. Build file installs mysql, configures it and creates the database. 
To execute build, open a terminal and run the following instructions:

Set in Capture_App folder
`cd SiVis/Code/Capture_App`

Change file permissions
chmod +x build.sh

Execute file
./build

## Once database is running, you can run CaptureCE programm
Set in Capture_App folder
cd SiVis/Code/Capture_App

Change file permissions
chmod +x CapturaCE

Execute file
./CapturaCE

## Considerations
192.168.1.102

the images will save in /media/fs_compartida


