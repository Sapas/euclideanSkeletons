# Run this script to solve a set of randomly generated polygon skeleton problems
# Script will cycle through the problems and solve them all, as well as run analysis on them
# To run this script, navigate to parent folder (i.e. euclideanSkeletons/) and run:
# bash randomPolygonsCommands.sh

# NOTE: please make sure you run the ubuntu set up script to make sure everything is as should be

# ATTENTION: Only change the values under this statement

n_start=10
n_end=10
n_interval=10
seed_min=1
seed_max=50
print_info="true"
run_heuristic="true"
create_graphs=0

# NO CHANGES PAST THIS POINT

# Get time
start=`date +%s`

# First get skeletons, start by compiling (note need to install g++, if not command should appear on screen)
g++ cpp_code/skeleton.cpp -o cpp_code/skeleton

# Now can call exec, create/read polygons and find skeleton edges
./cpp_code/skeleton $n_start $n_end $n_interval $seed_min $seed_max $run_heuristic $print_info

# Finally, cycle through outputs and call solver
for ((n = $n_start; n <= $n_end; n = n + $n_interval)); do
    for ((s = $seed_min; s <= $seed_max; s++)); do
        ./steiner_solver/staynerd data/STP_input/random_n${n}_s${s}_STP_format_1-2.txt 100000 1 data/STP_output/random_n${n}_s${s}_output_STP_format_1-2.txt
		./steiner_solver/staynerd data/STP_input/random_n${n}_s${s}_STP_format_1-2-3.txt 100000 1 data/STP_output/random_n${n}_s${s}_output_STP_format_1-2-3.txt 
		./steiner_solver/staynerd data/STP_input/random_n${n}_s${s}_STP_format_1-2-3_pruned.txt 100000 1 data/STP_output/random_n${n}_s${s}_output_STP_format_1-2-3_pruned.txt 
		./steiner_solver/staynerd data/STP_input/random_n${n}_s${s}_STP_format_all.txt 100000 1 data/STP_output/random_n${n}_s${s}_output_STP_format_all.txt 
		./steiner_solver/staynerd data/STP_input/random_n${n}_s${s}_STP_format_pruned.txt 100000 1 data/STP_output/random_n${n}_s${s}_output_STP_format_pruned.txt 
    done
done

Rscript R_code/randomPolygonsAnalysis.R $n_start $n_end $n_interval $seed_min $seed_max $create_graphs


end=`date +%s`
echo "Total execution time: $((end-start)) seconds"
