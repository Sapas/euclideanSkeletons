# This is the setup script which should be run on a new ubuntu machine
# Should only need to run this once

# Update sudo
sudo apt update
# Install g++
sudo apt install g++
# Install R
sudo apt-get install r-base
# Install required R packages
sudo apt install build-essential
sudo apt-get install -y libxml2-dev libcurl4-openssl-dev libssl-dev
sudo -i R
install.packages("libcurl4-openssl-dev")
install.packages("tidyverse")
install.packages("dplyr")
install.packages("pracma")
q()
n
# Install cplex with academic initiative. Included the installer, but might need to get it again
# https://www.ibm.com/support/pages/installation-ibm-ilog-cplex-optimization-studio-linux-platforms
# https://feed.moopt.com/cplex-optimization-studio-for-students-and-academics/






# Possible fixes (i.e. required installations)

# cplex installation with academic initiative: 
# https://my15.digitalexperience.ibm.com/b73a5759-c6a6-4033-ab6b-d9d4f9a6d65b/dxsites/151914d1-03d2-48fe-97d9-d21166848e65/technology/data-science

# installing and running solver (stay_nerd): instructions can be found here: 
# https://homepage.univie.ac.at/ivana.ljubic/research/staynerd/StayNerd.html





# installing g++ compiler in linux:
# https://linuxconfig.org/how-to-install-g-the-c-compiler-on-ubuntu-18-04-bionic-beaver-linux

# installing r:
# https://www.r-bloggers.com/download-and-install-r-in-ubuntu/

# might need to install r packages from terminal:
# https://linuxize.com/post/how-to-install-r-on-ubuntu-18-04/

# will need this command in that case:
# sudo apt-get install -y libxml2-dev libcurl4-openssl-dev libssl-dev





