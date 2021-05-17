library(dplyr)
library(pracma)
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
}
