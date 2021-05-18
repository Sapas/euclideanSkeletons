# This file should be run in order to plot the results of finding the euclidian skeleton
source("R_code/analysis_functions/plotter.R")
source("R_code/analysis_functions/time_analysis.R")
source("R_code/analysis_functions/edge_analysis.R")
source("R_code/analysis_functions/infeasible_analysis.R")
source("R_code/analysis_functions/size_analysis.R")

args = commandArgs(trailingOnly=TRUE)

# These specify which files to look at (when random generated)
# If want to plot your own special scenario, write the filename
n_start <- as.numeric(args[1])
n_end <- as.numeric(args[2])
n_interval <- as.numeric(args[3])
s_start <- as.numeric(args[4])
s_end <- as.numeric(args[5])
disp <- as.numeric(args[6])

if(disp > 0.5){
    for(n in seq(n_start, n_end, n_interval)){
        for(s in s_start:s_end){
            filename = paste0("random_n",n,"_s",s)
            print(filename)
            plot_types(filename)
            plot_solution(filename)
            plot_heuristic_solution(filename)
        }
    }
}

#solution_time(n_start, n_end, n_interval, s_start, s_end)
#edge_analysis(n_start, n_end, n_interval, s_start, s_end)
#infeasibility_analysis(n_start, n_end, n_interval, s_start, s_end)
#size_analysis(n_start, n_end, n_interval, s_start, s_end)


