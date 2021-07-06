# Load g++ module
module load gcc/10.2.0
module load cplex/12.8
module load cmake/3.18.4



module load cmake/3.18.4
# Load eigen
module load eigen/3.3.8
# By this stage, everything should be working
# Go to cpp_folder and compile code
cd ../cpp_code
make lin