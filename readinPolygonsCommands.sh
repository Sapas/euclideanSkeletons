# Run this script to solve a skeleton generating problem. 
# This script is to solve and plot an instance inputted as a txt file
# The instance file should be placed in data/input_polygon
# To run this script, navigate to parent folder (i.e. euclideanSkeletons/) and run:
# bash readinPolygonsCommands.sh

# NOTE: please make sure you run the ubuntu set up script to make sure everything is as should be


# ATTENTION: Only change the name of the polygon file (excluding the .txt)
polygon_name="spike"
print_info="true"
# NO CHANGES PAST THIS POINT

# Get time
start=`date +%s`

# First get skeletons, start by compiling (note need to install g++, if not command should appear on screen)
g++ cpp_code/skeleton.cpp -o cpp_code/skeleton

# Now can call exec, create/read polygons and find skeleton edges
./cpp_code/skeleton $polygon_name $print_info


#./steiner_solver/staynerd data/STP_input/${polygon_name}_STP_format_1-2.txt 100 1 data/STP_output/${polygon_name}_output_STP_format_1-2.txt 
#./steiner_solver/staynerd data/STP_input/${polygon_name}_STP_format_1-2-3.txt 100 1 data/STP_output/${polygon_name}_output_STP_format_1-2-3.txt 
#./steiner_solver/staynerd data/STP_input/${polygon_name}_STP_format_1-2-3.txt 100 1 data/STP_output/${polygon_name}_output_STP_format_1-2-3_pruned.txt 
#./steiner_solver/staynerd data/STP_input/${polygon_name}_STP_format_all.txt 100 1 data/STP_output/${polygon_name}_output_STP_format_all.txt 
#./steiner_solver/staynerd data/STP_input/${polygon_name}_STP_format_pruned.txt 100 1 data/STP_output/${polygon_name}_output_STP_format_pruned.txt 


Rscript R_code/readinPolygonsAnalysis.R $polygon_name

end=`date +%s`
echo "Total execution time: $((end-start)) seconds"
