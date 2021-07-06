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

# Transfer required steiner_solver files
# Navigate to euclideanSkeletons/steiner_solver/ and call
scp make_cplex_shared.sh staynerd staynerd.license nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/euclideanSkeletons/steiner_solver/ 

# Transfer run sh files
# Navigate to euclideanSkeletons/ and call
scp randomPolygonsCommands.sh readinPolygonsCommands.sh nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/euclideanSkeletons/ 



# Transfer cluster run and setup files
# Navigate to euclideanSkeletons/setup/ and call
scp clusterRun.sh nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/euclideanSkeletons/setup/




# Transfer new run scripts
# Navigate to bifiEBBO/data/runScripts/
# Specify function, this is just an example
scp modelPerformanceSetup.txt nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/bifiEBBO/data/runScripts/
# To transfer all scripts
scp *.txt nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/bifiEBBO/data/runScripts/
# Again, this should prompt a password

# Might want to transfer clusterRun files 
# Navigate to bifiEBBO/setup/
scp clusterRun.sh nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/bifiEBBO/setup/
scp longClusterRun.sh nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/bifiEBBO/setup/
# Again, both should prompt a password

# After doing all of this, normally should want to run some experiments.
# First of all will want to recompile code if something changed
# Navigate to bifiEBBO/cpp_code/ in the cluster
# Will need to load the required modules
module load gcccore/10.2.0
module load cmake/3.18.4
module load eigen/3.3.8
# Now can make executable
make lin

# Everything should be ready to run the preliminary run to make sure all is well.
# Navigate to the bifiEBBO/setup/ in the cluster
# Then run the set of commands (this file should only have 5 array jobs or so, something small)
# Submit the job
sbatch clusterRun.sh

# To check status, can do 
squeue -u nandres

# Also keep in mind you can look at the output of the code by calling the following (edit file name)
cat slurm-26411012_1.out

# Once all seems to be working, call the long run
sbatch clusterRunLong.sh

# Make sure to remove all the output files when you are sure you do not need them
rm slurm*

# Finally, will probably want to move the resulting data back to the machine. 
# To do so, navigate to bifiEBBO/data/ and call
scp -r nandres@spartan.hpc.unimelb.edu.au:/data/gpfs/projects/punim1489/bifiEBBO/data/clusterResults/ .
# Should be prompted by password






