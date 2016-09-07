#!/bin/bash 

# install updated version of cmake
sudo apt-get install --assume-yes software-properties-common
sudo add-apt-repository -y ppa:george-edison55/cmake-3.x
sudo apt-get update --assume-yes
sudo apt-get install --assume-yes cmake
cmake --version  # This allows us to confirm which version of cmake has been installed.
  
# install dependencies for ui
sudo apt-get install --assume-yes libfltk1.3 fluid
