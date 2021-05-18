# This file should be run in order to plot the results of finding the euclidian skeleton
source("R_code/analysis_functions/plotter.R")

args = commandArgs(trailingOnly=TRUE)

filename <- args[1]
plot_types(filename)
#plot_solution(filename)
#plot_heuristic_solution(filename)
