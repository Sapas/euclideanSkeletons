# Function to have a look at proportions of infeasibility

infeasibility_analysis <- function(n_start, n_end, n_interval, s_start, s_end, type){
	
	percent_1_2 <- c()
	percent_1_2_3 <- c()
	
	for(n in seq(n_start, n_end, n_interval)){
		infeasible_1_2 <- 0
		infeasible_1_2_3 <- 0
		count_1_2 <- 0
		count_1_2_3 <- 0
		for(seed in s_start:s_end){
		  if(file.exists(paste0("data/STP_output/", type, "_n",n,"_s",seed,"_output_STP_format_1-2.txt"))){
		    count_1_2 <- count_1_2 + 1
  			# First look at 1-2, check proportion 
  			con <- file(paste0("data/STP_output/", type, "_n",n,"_s",seed,"_output_STP_format_1-2.txt"), "r")
  			while(!strcmp(readLines(con, n = 1), "SECTION Solutions")){}
  			# If now got END, no solutions, infeasible
  			if(strcmp(readLines(con, n = 1), "END")){
  				infeasible_1_2 <- infeasible_1_2 + 1
  			}
  			close(con)
		  }
  		if(file.exists(paste0("data/STP_output/", type, "_n",n,"_s",seed,"_output_STP_format_1-2-3.txt"))){
  		  count_1_2_3 <- count_1_2_3 + 1
  			# Repeat for 1-2-3
  			con <- file(paste0("data/STP_output/", type, "_n",n,"_s",seed,"_output_STP_format_1-2-3.txt"), "r")
  			while(!strcmp(readLines(con, n = 1), "SECTION Solutions")){}
  			# If now got END, no solutions, infeasible
  			if(strcmp(readLines(con, n = 1), "END")){
  				infeasible_1_2_3 <- infeasible_1_2_3 + 1
  			}
  			close(con)	
  		  }
		}
		infeasible_1_2 <- 100 - infeasible_1_2 / count_1_2 * 100
		infeasible_1_2_3 <- 100 - infeasible_1_2_3 / count_1_2_3  * 100	
		percent_1_2 <- c(percent_1_2, infeasible_1_2)
		percent_1_2_3 <- c(percent_1_2_3, infeasible_1_2_3)	
	}
	
	x <- seq(n_start,n_end,n_interval)
	png(paste0("data/analysis_plots/", type, "_n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end,"_infeasibility_percentage.png"), width = 512, height = 384)
	plot(x, percent_1_2_3, 
			col = "chartreuse4", 
			type = "b",
			ylab = "Proportion feasible (%)",
			xlab = "Polygon size",
			#main = "Heuristic approach feasibility",
			ylim = c(0,110),
			xaxt = "n",
			pch = 21)
	points(x, percent_1_2, col = "blue", type = "b", pch = 22)
	legend("bottomleft", c("Types 1, 2 & 3", "Types 1 & 2"), col = c("chartreuse4", "blue"), pch = c(21,22))
	axis(1, at = x)
	out <- dev.off()

	
	
}
