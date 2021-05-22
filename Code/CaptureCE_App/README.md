# This program is developed for Ubuntu (18.04)
The CaptureCE application is runnig on Desktop Computer, this (1) recieves images from SiViS, (2) sets up the experiments and (3) stores the images.
It communicates with another application through TCP/IP protocol, this other app is located in xxxxxxxxxx, it runs on SiViS and has to be install on Raspberry Pi.

## Database
The CaptureCE application needs a database (Dump20210521.sql), therefore
mysql-server installation is required. The "Build.sh file" installs the mysql-server, configures it and creates the "Dump20210521.sql" database. 
To execute build.sh, open a terminal and run the following instructions:

Set in Capture_App folder
`cd SiVis/Code/Capture_App`

Change file permissions
`chmod +x build.sh`

Execute file
`./build`

## Once database is running
CaptureCE programm can run

Set in Capture_App folder
`cd SiVis/Code/Capture_App`

Change file permissions
`chmod +x CapturaCE`

Execute file
`./CapturaCE`

## Considerations
The Desktop Computer and SiViS has to be on the same network (via router or directly via ethernet cable). 
The Desktop Computer IP has to be: 
192.168.1.102

The images will save in /media/fs_compartida


