#!/usr/bin/env bash

PREFIX="${PREFIX:-/usr/local}"
MAKEFLAGS="${MAKEFLAGS:--j $(nproc)}"

log(){
    msg="$1"; shift
    _color_bold_yellow='\e[1;33m'
    _color_reset='\e[0m'
    echo "${_color_bold_yellow}${msg}${_color_reset}"
}

logGreen(){
    msg="$1"; shift
    _color_bold_yellow='\e[1;32m'
    _color_reset='\e[0m'
    echo "${_color_bold_yellow}${msg}${_color_reset}"
}

logRed(){
    msg="$1"; shift
    _color_bold_yellow='\e[1;31m'
    _color_reset='\e[0m'
    echo "${_color_bold_yellow}${msg}${_color_reset}"
}

install_basics(){
  sudo apt-get install gcc
  sudo apt-get install cmake
  sudo apt-get install libncurses5-dev libncursesw5-dev
  sudo apt-get install git
}

enable_SSH(){
  sudo systemctl enable ssh
  sudo systemctl start ssh
}

install_userland(){
  cd
  log "Downloading userland..."
  git clone https://github.com/raspberrypi/userland
  logGreen "Installing userland..."
  cd userland/
  ./buildme
}

install_essencials(){
  sudo apt-get install build-essential cmake unzip pkg-config

  sudo apt-get install libjpeg-dev libpng-dev libtiff-dev
  sudo apt-get install libavcodec-dev libavformat-dev libswscale-dev libv4l-dev
  sudo apt-get install libxvidcore-dev libx264-dev

  sudo apt-get install libgtk-3-dev

  sudo apt-get install libcanberra-gtk*

  sudo apt-get install libatlas-base-dev gfortran
}

install_python(){
  sudo apt-get install python3-dev
  # For installing OpenCV into python environment
  #sudo apt install virtualenv
  pip install numpy
}

enable_camera()
{
  if grep "start_x=" /boot/config.txt
  then
    echo "Camera is enabled"
    local old=$(grep -i "start_x=" /boot/config.txt)
    local new="start_x=1"
    echo "$old -> $new"
    sudo sed -i "s/"$old"/"$new"/g" /boot/config.txt
  else
    echo "Camera has been enabled"
    sudo sh -c "echo \"start_x=1\" >> /boot/config.txt"
  fi
}

disable_camera_led(){
  if grep "disable_camera_led=" /boot/config.txt
  then
    local old=$(grep -i "disable_camera_led=" /boot/config.txt)
    local new="disable_camera_led=1"
    echo "$old -> $new"
    sudo sed -i "s/"$old"/"$new"/g" /boot/config.txt
  else
    echo "Camera led has been disabled"
    sudo sh -c "echo \"disable_camera_led=1\" >> /boot/config.txt"
  fi
}

set_GPU_memory(){
  if grep "gpu_mem=" /boot/config.txt
  then
    local old=$(grep -i "gpu_mem=" /boot/config.txt)
    local new="gpu_mem=$1"
    echo "$old -> $new"
    sudo sed -i "s/"$old"/"$new"/g" /boot/config.txt
  else
    echo "Set gpu_mem=$1"
    sudo sh -c "echo \"gpu_mem=$1\" >> /boot/config.txt"
  fi
}

free_space(){
  sudo apt-get purge wolfram-engine
  sudo apt-get purge libreoffice*
  sudo apt-get clean
  sudo apt-get autoremove
}

change_swap(){
  #Needs 3GB RAM, so increase swap, when finish opencv intallation may be 100MB again
  local old=$(grep -i "CONF_SWAPSIZE" /etc/dphys-swapfile)

  local new="CONF_SWAPSIZE="$1""
  echo "$old -> $new"
  sudo sed -i "s/"$old"/"$new"/g" /etc/dphys-swapfile

  sudo /etc/init.d/dphys-swapfile stop
  sudo /etc/init.d/dphys-swapfile start
}

install_Celegans_smartLight(){
  #log "Downloading SiViS_App..."
  cd
  #git clone https://github.com/JCPuchalt/c-elegans_smartLight.git
  #logRed "Installing SiViS_App..."
  cd SiViS_App
  cmake .
  make ${MAKEFLAGS}
  cd
}

install_opencv(){
  cd
  # For installing OpenCV into python environment
  #mkdir python-environments
  #cd python-environments
  #virtualenv -p python3 env
  #source env/bin/activate
  #cd

  log "Downloading OpenCV..."
  git clone https://github.com/opencv/opencv.git
  #cd opencv
  #git checkout $cvVersion
  #cd ..
  cd
  
  log "Downloading OpenCV_contrib..."
  git clone https://github.com/opencv/opencv_contrib.git
  #cd opencv_contrib
  #git checkout $cvVersion
  #cd ..

  cd opencv
  mkdir build
  cd build

  cmake -D CMAKE_BUILD_TYPE=RELEASE \
      -D CMAKE_INSTALL_PREFIX=/usr/local \
      -D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib/modules \
      -D ENABLE_NEON=ON \
      -D ENABLE_VFPV3=ON \
      -D BUILD_TESTS=OFF \
      -D OPENCV_ENABLE_NONFREE=ON \
      -D INSTALL_PYTHON_EXAMPLES=OFF \
      -D BUILD_EXAMPLES=OFF ..

  make ${MAKEFLAGS}
  sudo make install
  sudo ldconfig
  cd
}

reInit(){
logRed "Please reboot. After reboot, system will be ready."
logRed "For launching programm, type on a terminal:"
log ">> cd SiViS_App"
log ">> sudo ./c-elegans"
logRed "Do you want to restart? yes or no."
local keepLoop=true;
while $keepLoop
do
  read INPUT_STRING
  case $INPUT_STRING in
    "yes")
      echo "Rebooting..."
      sudo reboot
      keepLoop=false
      ;;
    "no")
      echo "Not reboot!"
      keepLoop=false
      ;;
    *)
      echo "Do you want to restart? yes or no."
      ;;
  esac
 done
}

ask_for_free_space(){
  local keepLoop=true;
  while $keepLoop
  do
    read INPUT_STRING
    case $INPUT_STRING in
      "yes")
        echo "Freeing up some space..."
        free_space
        keepLoop=false
        ;;
      "no")
        echo "Not free!"
        keepLoop=false
        ;;
      *)
        echo "Free space? yes or no."
        ;;
    esac
   done
}

ask_for_opencv_installation(){
  local keepLoop=true;
  while $keepLoop
  do
    read INPUT_STRING
    case $INPUT_STRING in
      "yes")
        echo "Installing OpenCV..."
        install_opencv
        keepLoop=false
        ;;
      "no")
        echo "No OpenCV installation!"
        keepLoop=false
        ;;
      *)
        echo "Do you want to install OpenCV? yes or no."
        ;;
    esac
   done
}

ask_for_Celegans_smartLight_installation(){
  local keepLoop=true;
  while $keepLoop
  do
    read INPUT_STRING
    case $INPUT_STRING in 
      "yes")
        echo "Installing SiViS_App..."
        install_Celegans_smartLight
        keepLoop=false
        ;;
      "no")
        echo "No SiViS_App installation!"
        keepLoop=false
        ;;
      *)
        echo "Do you want to install SiViS_App? yes or no."
        ;;
    esac
   done
}

ask_for_userland_installation(){
  local keepLoop=true;
  while $keepLoop
  do
    read INPUT_STRING
    case $INPUT_STRING in
      "yes")
        echo "Installing userland..."
        install_userland
        keepLoop=false
        ;;
      "no")
        echo "No userland installation!"
        keepLoop=false
        ;;
      *)
        echo "Do you want to install userland? yes or no."
        ;;
    esac
   done
}


main(){
  log "Updating and upgrading sistem..."
  sudo apt-get update && sudo apt-get upgrade
  cd

  logGreen "Installing basics (Compiler, git...)..."
  install_basics

  logGreen "Enabling SSH..." # Interfacing options -> Enable SSH
  enable_SSH

  logGreen "Enabling Camera..."
  enable_camera

  logGreen "Disabling camera led..."
  disable_camera_led

  logRed "Do you want to install userland (camera driver)? yes or no."
  ask_for_userland_installation

  logRed "Make sure there is enough free space (more than 8GB). It is needed "
  logRed "at least a 16GB memory card, for OpenCV 4 on Raspberry Pi 3."
  logRed "If there is not space enough, try to free."
  logRed "Do you want to free some space? yes or no"
  ask_for_free_space

  logGreen "Installing essencials and python..."
  install_essencials
  install_python

  logGreen "Increasing swap..."
  change_swap "2048"

  logRed "OpenCV is required. Maybe you want to install it later or you have already installed."
  logRed "Do you want to install opencv? It will take about 10 hours. yes or no."
  ask_for_opencv_installation

  logGreen "Decreasing swap..."
  change_swap "100"

  logGreen "Setting GPU memory to 256MB..."
  set_GPU_memory "256"

  # Installing Celegans_smartLight...
  logRed "Do you want to install SiViS_App? yes or no."
  ask_for_Celegans_smartLight_installation

  # Reboot system
  reInit

}

main
