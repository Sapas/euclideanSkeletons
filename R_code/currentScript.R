source("R_code/analysis_functions/arrayJobCombiner.R")
source("R_code/analysis_functions/edge_analysis.R")
source("R_code/analysis_functions/plotter.R")
source("R_code/analysis_functions/size_analysis.R")
source("R_code/analysis_functions/steiner_size_analysis.R")
source("R_code/analysis_functions/time_analysis.R")
source("R_code/analysis_functions/infeasible_analysis.R")


uniformData <- combineArrayResults("largeRunUniformDist", 1, 990)
#normalData <- combineArrayResults("largeRunNormalDist", 1, 990)

# "Hack" for convex times, should be extremely small so don't feel bad about this
# uniformData$find_convex = 0
# normalData$find_convex = 0


# Remove nas, got seg fault for uniform data
if(nrow(uniformData) != nrow(uniformData[!is.na(uniformData$total), ])){
  print(paste0("Removing ", nrow(uniformData) - nrow(uniformData[!is.na(uniformData$total), ]), " rows from uniform data due to nas"))
  uniformData <- uniformData[!is.na(uniformData$total), ]
}
# if(nrow(normalData) != nrow(normalData[!is.na(normalData$total), ])){
#   print(paste0("Removing ", nrow(normalData) - nrow(normalData[!is.na(normalData$total), ]), " rows from normal data due to nas"))
#   normalData <- normalData[!is.na(normalData$total), ]
# }

infeasibility_analysis(50, 2500, 25, 1, 50, "uniform")
size_analysis(50, 2500, 25, 1, 50, "uniform")
solution_time(uniformData, "uniform", 100, 2500, 200, 1, 50)
solution_time(uniformData, "uniform", 100, 2500, 100, 1, 50)
solution_time(uniformData, "uniform", 50, 2500, 25, 1, 50)
solution_time(uniformData, "uniform", 50, 500, 25, 1, 50)
edge_analysis(50, 2500, 25, 1, 50, "uniform")
edge_analysis(50, 500, 25, 1, 50, "uniform")
steiner_size(50, 500, 25, 1, 50, "uniform")

steiner_size(100, 2500, 150, 1, 50, "uniform")
steiner_size(100, 2500, 200, 1, 50, "uniform")
steiner_size(100, 2500, 100, 1, 50, "uniform")




# solution_time(normalData, "normal", 50, 500, 25, 1, 50)
# solution_time(uniformData, "uniform", 50, 500, 25, 1, 50)
# solution_time(normalData, "normal", 100, 2500, 200, 1, 50)
# solution_time(uniformData, "uniform", 100, 2500, 200, 1, 50)
# solution_time(normalData, "normal", 50, 2500, 25, 1, 50)
# solution_time(uniformData, "uniform", 50, 2500, 25, 1, 50)
# 
# edge_analysis(50, 500, 25, 1, 50, "normal")
# edge_analysis(50, 500, 25, 1, 50, "uniform")
# edge_analysis(100, 2500, 200, 1, 50, "normal")
# edge_analysis(100, 2500, 200, 1, 50, "uniform")
# edge_analysis(50, 2500, 25, 1, 50, "normal")
# edge_analysis(50, 2500, 25, 1, 50, "uniform")
# 
# infeasibility_analysis(50, 500, 25, 1, 50, "normal")
# infeasibility_analysis(50, 500, 25, 1, 50, "uniform")
# infeasibility_analysis(50, 2500, 25, 1, 50, "normal")
# infeasibility_analysis(50, 2500, 25, 1, 50, "uniform")
# 
# size_analysis(50, 500, 25, 1, 50, "normal")
# size_analysis(50, 500, 25, 1, 50, "uniform")
# size_analysis(50, 2500, 25, 1, 50, "normal")
# size_analysis(50, 2500, 25, 1, 50, "uniform")
# 
# edge_analysis_full(50, 50, 25, 3, 3, 's', FALSE, "normal")
#edges <- read.table("data/polygon/normal_n275_s11_edges.txt", header = TRUE)
#edgesInSolution <- edges[edges_in_solution(275, 11, "normal"), ]
# for(i in seq(50, 300, 25)){
#   for(s in 1:50){
#     plot_polygon(paste0("normal_n", i, "_s", s))
#   }
# }
# plot_polygon("uniform_n50_s1")
# plot_polygon("uniform_n1250_s37")
# plot_polygon("uniform_n1800_s3")
# plot_polygon("uniform_n1825_s40")
# plot_polygon("uniform_n2275_s6")

# polygon <- read.table(paste0("data/polygon/uniform_n200_s15_polygon.txt"), header = TRUE)

