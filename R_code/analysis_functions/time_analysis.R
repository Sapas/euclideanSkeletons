library(dplyr)
library(pracma)
library(tidyverse)
# First of all, want a a function which compares time taken to find a solution, prune vs not prune
# Write a function to find the solver time taken
avg_solver_time <- function(n, s_start, s_end, endFile){
	time <- 0
	for(s in s_start:s_end){
		con <- file(paste0("data/STP_output/random_n",n,"_s",s,"_output_STP_format_", endFile, ".txt"), "r")
		# Read file until get to solution
		while(!strcmp(readLines(con, n = 1), "SECTION Run")){}
		# Read an extra one
		readLines(con, n = 1)
		# Care about this one
		time <- time + as.numeric(gsub("Time", "", readLines(con, n = 1)))
		close(con)
	}
	time <- time / (s_end - s_start + 1)
	return(time)		
}

# Another function to extract time taken for a specified file name
extract_solver_time <- function(filename){
  con <- file(filename, "r")
  while(!strcmp(readLines(con, n = 1), "SECTION Run")){}
  # Read an extra one
  readLines(con, n = 1)
  # Care about this one
  time <- as.numeric(gsub("Time", "", readLines(con, n = 1)))
  close(con)
  return(time)
}

solution_time <- function(n_start, n_end, n_interval, s_start, s_end){
	
	# First create the name of the file where the times should be stored
	times <- read.table(paste0("data/exec_times/run_n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end,".txt"), header = TRUE)
	times_heur <- times[times$heur == TRUE,]
	times <- times[times$heur == FALSE,]
	sol_times <- c()
	sol_times_prunned <- c()
	heur_sol_times <- c()
	heur_sol_times_prunned <- c()
	for(n in seq(n_start, n_end, n_interval)){		
		avg_times <- colMeans(times[times$n == n, c("find_convex", "find_edges", "STP_intersection", "polygon_file", "STP_file")])
		avg_prune_time <- mean(times[times$n == n, "prune_edges"])
		avg_heur_times <- colMeans(times_heur[times_heur$n == n, c("find_convex", "find_edges", "STP_intersection", "polygon_file", "STP_file")])
		avg_heur_prune_time <- mean(times_heur[times_heur$n == n, "prune_edges"])

		# Now need to read all the solver times
		sol_times <- c(sol_times, sum(avg_times) + avg_solver_time(n, s_start, s_end, "all"))
		sol_times_prunned <- c(sol_times_prunned, sum(avg_times) + avg_prune_time + avg_solver_time(n, s_start, s_end, "pruned"))
		heur_sol_times <- c(heur_sol_times, sum(avg_heur_times) + avg_solver_time(n, s_start, s_end, "1-2-3"))
		heur_sol_times_prunned <- c(heur_sol_times_prunned, sum(avg_heur_times) + avg_heur_prune_time + avg_solver_time(n, s_start, s_end, "1-2-3_pruned"))	
	}
	
	x <- seq(n_start,n_end,n_interval)
	png(paste0("data/analysis_plots/n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end,"_solve_times.png"), width = 512, height = 384)
	plot(x, sol_times, 
			col = "chartreuse4", 
			type = "b",
			ylab = "Time (seconds)",
			xlab = "Polygon size",
			#main = "Pruned vs Unpruned solution time",
			ylim = c(0,max(sol_times_prunned,sol_times)*1.1),
			xaxt = "n",
			pch = 21)
	points(x, sol_times_prunned, col = "blue", type = "b", pch = 22)
	points(x, heur_sol_times, col = "purple3", type = "b", pch = 23)
	points(x, heur_sol_times_prunned, col = "red", type = "b", pch = 24)
	legend("topleft", c("Unpruned", "Pruned", "Restricted-type", "Restricted-type pruned"), col = c("chartreuse4", "blue", "purple3", "red"), pch = c(21, 22, 23, 24))
	axis(1, at = x)
	out <- dev.off()
    
    # Also plot times as percentage of the longest option (i.e. unprunned full solution)
    sol_times_prunned_proportion <- sol_times_prunned / sol_times * 100
    heur_sol_times_proportion <- heur_sol_times / sol_times * 100
    heur_sol_times_prunned_proportion <- heur_sol_times_prunned / sol_times * 100
    
    png(paste0("data/analysis_plots/n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end,"_solve_times_percentage.png"), width = 512, height = 384)
    plot(x, sol_times_prunned_proportion,
    col = "blue",
    type = "b",
    ylab = "Percentage time",
    xlab = "Polygon size",
    #main = "Percentage time taken relative to unprunned solution",
    ylim = c(0,100),
    xaxt = "n",
	pch = 22)
    points(x, heur_sol_times_proportion, col = "purple3", type = "b", pch = 23)
    points(x, heur_sol_times_prunned_proportion, col = "red", type = "b", pch = 24)
    legend("topleft", c("Pruned", "Restricted-type", "Restricted-type pruned"), col = c("blue", "purple3", "red"), pch = c(22, 23, 24))
    axis(1, at = x)
    out <- dev.off()
    
    # Adding an option where what is shown is percentage improvement over slowest, and percentage increase over fastest
    # Will want statistical measures, so have to do it per entry
    # Try something different
    timeData <- data.frame(matrix(ncol = 5, nrow = 0))
    row <- 1
    for(n in seq(n_start, n_end, n_interval)){
      for(s in s_start:s_end){
        # Extract the required times
        timeStandard <- sum(times[times$n == n & times$seed == s, c("find_convex", "find_edges", "STP_intersection", "polygon_file", "STP_file")]) + 
          extract_solver_time(paste0("data/STP_output/random_n",n,"_s",s,"_output_STP_format_all.txt"))
        
        timePruned <- sum(times[times$n == n & times$seed == s, c("find_convex", "find_edges", "prune_edges", "STP_intersection", "polygon_file", "STP_file")]) + 
          extract_solver_time(paste0("data/STP_output/random_n",n,"_s",s,"_output_STP_format_pruned.txt"))
        
        timeHeur <- sum(times_heur[times_heur$n == n & times_heur$seed == s, c("find_convex", "find_edges", "STP_intersection", "polygon_file", "STP_file")]) +
          extract_solver_time(paste0("data/STP_output/random_n",n,"_s",s,"_output_STP_format_1-2-3.txt"))
        
        timeHeurPruned <- sum(times_heur[times_heur$n == n & times_heur$seed == s, c("find_convex", "find_edges", "prune_edges", "STP_intersection", "polygon_file", "STP_file")]) +
          extract_solver_time(paste0("data/STP_output/random_n",n,"_s",s,"_output_STP_format_1-2-3_pruned.txt"))
        
        best <- min(timeStandard, timePruned, timeHeur, timeHeurPruned)
        worst <- max(timeStandard, timePruned, timeHeur, timeHeurPruned)
        
        timeData[row, ] <- c(n, s, timeStandard, "Unpruned", "time")
        timeData[row + 1, ] <- c(n, s, timePruned, "Pruned", "time")
        timeData[row + 2, ] <- c(n, s, timeHeur, "Restricted-type", "time")
        timeData[row + 3, ] <- c(n, s, timeHeurPruned, "Restricted-type Pruned", "time")
        
        timeData[row + 4, ] <- c(n, s, 100 * (timeStandard - best)/best, "Unpruned", "BestComp")
        timeData[row + 5, ] <- c(n, s, 100 * (timePruned - best)/best, "Pruned", "BestComp")
        timeData[row + 6, ] <- c(n, s, 100 * (timeHeur - best)/best, "Restricted-type", "BestComp")
        timeData[row + 7, ] <- c(n, s, 100 * (timeHeurPruned - best)/best, "Restricted-type Pruned", "BestComp")
        
        timeData[row + 8, ] <- c(n, s, 100 * (worst - timeStandard) / worst, "Unpruned", "WorstComp")
        timeData[row + 9, ] <- c(n, s, 100 * (worst - timePruned) / worst, "Pruned", "WorstComp")
        timeData[row + 10, ] <- c(n, s, 100 * (worst - timeHeur) / worst, "Restricted-type", "WorstComp")
        timeData[row + 11, ] <- c(n, s, 100 * (worst - timeHeurPruned) / worst, "Restricted-type Pruned", "WorstComp")
        
        row <- row + 12
      }
    }
    colnames(timeData) <- c("n", "seed", "time", "Algorithm", "valType")
    timeData <- rbind(rbind(rbind(timeData[timeData$Algorithm == "Unpruned", ], timeData[timeData$Algorithm == "Pruned", ]), timeData[timeData$Algorithm == "Restricted-type", ]), timeData[timeData$Algorithm == "Restricted-type Pruned", ])
    timeData$time <- as.numeric(timeData$time)
    
    subsetData <- timeData[timeData$valType == "BestComp", ]
    ggplot(subsetData, aes(x=n, y=time, fill=Algorithm)) +
      geom_boxplot() + 
      scale_fill_grey(name = "Algorithm", labels = c("Unpruned", "Pruned", "Restricted-type", "Restricted-type Pruned")) +
      labs(x = "Polygon size", y = "Percentage increase over best algorithm")
    ggsave(paste0("data/analysis_plots/n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end,"_solve_times_box_plot_bestComp.png"))
    
    subsetData <- timeData[timeData$valType == "WorstComp", ]
    ggplot(subsetData, aes(x=n, y=time, fill=Algorithm)) +
      geom_boxplot() + 
      scale_fill_grey(name = "Algorithm", labels = c("Unpruned", "Pruned", "Restricted-type", "Restricted-type Pruned")) +
      labs(x = "Polygon size", y = "Percentage improvement over worst algorithm")
    ggsave(paste0("data/analysis_plots/n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end,"_solve_times_box_plot_worstComp.png"))
    
    return(timeData)
}

data <- solution_time(10, 50, 10, 1, 50)
