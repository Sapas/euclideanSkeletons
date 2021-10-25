# Here are a set of commands I keep on calling to use scp (transferring between the cluster or the cloud and my machine)
# It is important I do not forget them so I am writing them here

# First deal with the cloud, where most things should already be working
# Connecting to the cloud, go to Documents/GitHubRepositories/ and call
ssh -i /Users/nandresthio/Documents/GitHubRepositories/access/euclideanSkeletonsKey.pem ubuntu@45.113.234.177
# This should not even prompt a password. Neat!

# Now to transfer files, as an example to transfer the stay_nerd certificate
# Go to euclideanSkeletons/steiner_solver/ and call
scp -i /Users/nandresthio/Documents/GitHubRepositories/access/euclideanSkeletonsKey.pem staynerd.license ubuntu@45.113.234.177:/home/ubuntu/euclideanSkeletons/steiner_solver/
# Example, pass in bash script
scp -i /Users/nandresthio/Documents/GitHubRepositories/access/euclideanSkeletonsKey.pem randomPolygonsCommands.sh ubuntu@45.113.234.177:/home/ubuntu/euclideanSkeletons/

# To retrieve data, for example all STP output files, go to euclideanSkeletons/data/ and call
scp -i /Users/nandresthio/Documents/GitHubRepositories/access/euclideanSkeletonsKey.pem -r ubuntu@45.113.234.177:/home/ubuntu/euclideanSkeletons/data/STP_output/ .
# Same for exec times
scp -i /Users/nandresthio/Documents/GitHubRepositories/access/euclideanSkeletonsKey.pem -r ubuntu@45.113.234.177:/home/ubuntu/euclideanSkeletons/data/exec_times/ .



# Now starting on cluster commands
# First of all want to log into the cluster
ssh nandres@spartan.hpc.unimelb.edu.au
# Should be prompted by password
# Next will want to go to my project folder
cd /data/gpfs/projects/punim1489/euclideanSkeletons/

# Navigate to euclideanSkeletons/cpp_code/
# Now transfer all files 
scp -r *.cpp *.h nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/euclideanSkeletons/cpp_code/ 
# Should be prompted for password

# Transfer all data in data folder, should only want to do this once
# Navigate to euclideanSkeletons/data/ and call
scp -r . nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/euclideanSkeletons/data/
# Actually might not be a bad idea just transferring all data back
# Be careful to navigate to euclideanSkeletons/ (i.e. parent folder) and call
scp -r nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/euclideanSkeletons/data/ .

# Doing something a bit more subtle, three commands to transfer the required files from three different folders
# Navigate to euclideanSkeletons/data/clusterResults/ and call
scp nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/euclideanSkeletons/data/clusterResults/largeRunUniform* .
# Navigate to euclideanSkeletons/data/polygon/ and call
scp nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/euclideanSkeletons/data/polygon/uniform* .
# Navigate to euclideanSkeletons/data/STP_input/ and call
scp nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/euclideanSkeletons/data/STP_input/uniform* .
# Navigate to euclideanSkeletons/data/STP_output/ and call
scp nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/euclideanSkeletons/data/STP_output/uniform* .

# Transfer required steiner_solver files
# Navigate to euclideanSkeletons/steiner_solver/ and call
scp make_cplex_shared.sh staynerd staynerd.license nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/euclideanSkeletons/steiner_solver/ 

# Transfer run sh files
# Navigate to euclideanSkeletons/ and call
scp randomPolygonsCommands.sh readinPolygonsCommands.sh nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/euclideanSkeletons/ 

# Transfer cluster run and setup files
# Navigate to euclideanSkeletons/setup/ and call (note it is the head folder, things are wired to run this way)
scp *.sh nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/euclideanSkeletons/





