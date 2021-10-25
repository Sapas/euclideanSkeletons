# Function to have a look at proportions of infeasibility

size_analysis <- function(n_start, n_end, n_interval, s_start, s_end, type){
	proportions <- c()
	proportions_full <- c()
	solutions_ratio <- c()	
	proportionData <- data.frame(matrix(ncol = 3, nrow = 0))
	proportionDataFull <- data.frame(matrix(ncol = 3, nrow = 0))
	row <- 0
	countImproved <- 0
	for(n in seq(n_start, n_end, n_interval)){
		proportion <- 0
		proportion_full <- 0
		solution_ratio <- 0
		missed <- 0
		count <- 0
		for(seed in s_start:s_end){
		  if(!file.exists(paste0("data/polygon/", type, "_n",n,"_s",seed,"_polygon.txt")) |
		     !file.exists(paste0("data/STP_output/", type, "_n",n,"_s",seed,"_output_STP_format_pruned.txt")) |
		     !file.exists(paste0("data/STP_output/", type, "_n",n,"_s",seed,"_output_STP_format_1-2-3.txt"))){
		    next
		  }
		  count <- count + 1
		  row <- row + 1
			# Get the number of convex vertices
			polygon <- read.table(paste0("data/polygon/", type, "_n",n,"_s",seed,"_polygon.txt"), header = TRUE)
			n_convex <- nrow(polygon %>% filter(convex))
			# Read solution file
			con <- file(paste0("data/STP_output/", type, "_n",n,"_s",seed,"_output_STP_format_pruned.txt"), "r")
			# Read file until get to solution, count lines to skip
			while(!strcmp(readLines(con, n = 1), "SECTION FinalSolution")){}
			# Read extra line (tells number of vertices)
			sol_size <- as.numeric(gsub("Vertices", "", readLines(con, n = 1))) - n_convex
			close(con)
			
			# Ratio to convex vertices
			proportion <- proportion + sol_size / n_convex
			# Ratio to polygon size
			proportion_full <- proportion_full + sol_size / n
			
			proportionData[row, ] <- c(n, s, sol_size / n_convex)
			proportionDataFull[row, ] <- c(n, s, sol_size / n)
			


			# Now read "heuristic" solution size
			# Read solution file
			con <- file(paste0("data/STP_output/", type, "_n",n,"_s",seed,"_output_STP_format_1-2-3.txt"), "r")
			# Read file until get to solution, count lines to skip
			while(!strcmp(readLines(con, n = 1), "SECTION FinalSolution")){}
			# Read extra line (tells number of vertices)
			heur_size <- as.numeric(gsub("Vertices", "", readLines(con, n = 1))) - n_convex
			if(heur_size < 0){
				print(paste0("Heuristic for n ", n, " seed ", seed, " didn't find solution!"))
				missed <- missed + 1
			}else{
				# Ratio solution to heuristic
			  # if(heur_size > sol_size){
			  #   countImproved <- countImproved + 1
			  #   print(paste0("For n ", n, " seed ", seed, ", heuristic ", heur_size, " vs sol ", sol_size, ", diff ", heur_size - sol_size, ", current count ", countImproved))
			  # }
				solution_ratio <- solution_ratio + heur_size / sol_size	
			}
			close(con)			
			
		}
		proportion <- proportion / count
		proportions <- c(proportions,proportion)
		
		proportion_full <- proportion_full / count
		proportions_full <- c(proportions_full,proportion_full)
		
		solution_ratio <- solution_ratio / (count - missed)
		solutions_ratio <- c(solutions_ratio,solution_ratio)
	}
	# print("Proportions Full")
	# print(proportions_full)
	# print("Proportions Convex")
	# print(proportions)
	
	colnames(proportionData) <- c("n", "seed", "proportion")
	proportionData$proportion <- as.numeric(proportionData$proportion)
	proportionData$n <- factor(proportionData$n, levels=unique(proportionData$n))

	colnames(proportionDataFull) <- c("n", "seed", "proportion")
	proportionDataFull$proportion <- as.numeric(proportionDataFull$proportion)
	proportionDataFull$n <- factor(proportionDataFull$n, levels=unique(proportionDataFull$n))
	
	interval <- seq(n_start, n_end, round((n_end - n_start) / 6))
	if(tail(interval, 1) != n_end){interval[[length(interval)]] <- n_end}
	#print(interval)
	# interval <- c(50, 450, 875, 1275, 1675, 2100, 2500)
	interval <- as.factor(interval)
	
	ggplot(proportionData, aes(x=n, y=proportion)) +
	  geom_boxplot(fill = "grey") + 
	  labs(x = "Polygon size", y = "Ratio of output skeleton size to \nnumber of convex vertices in input polygon") + 
	  scale_x_discrete(breaks = interval, labels = interval) + 
	  theme(plot.margin = unit(c(1,1,1,1), "cm")) +
	  ggsave(paste0("data/analysis_plots/", type, "_n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end,"_optimal_solution_convex_ratio_box_plot.png"), width = 20, height = 15, units = "cm")
	
	ggplot(proportionDataFull, aes(x=n, y=proportion)) +
	  geom_boxplot(fill = "grey") + 
	  labs(x = "Polygon size", y = "Ratio of output skeleton size to \nnumber of vertices in input polygon") + 
	  scale_x_discrete(breaks = interval, labels = interval) + 
	  theme(plot.margin = unit(c(1,1,1,1), "cm")) +
	  ggsave(paste0("data/analysis_plots/", type, "_n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end,"_optimal_solution_size_ratio_box_plot.png"), width = 20, height = 15, units = "cm")
	
	
	x <- seq(n_start,n_end,n_interval)
	png(paste0("data/analysis_plots/", type, "_n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end,"_optimal_solution_convex_ratio.png"), width = 512, height = 384)
	par(mar=c(5, 6, 4, 2) + 0.1)
	plot(x, proportions, 
			col = "chartreuse4", 
			type = "b",
			ylab = "Ratio of output skeleton size to \nnumber of convex vertices in input polygon",
			xlab = "Polygon size",
			#main = "Ratio solution size vs number of convex vertices",
			ylim = c(min(proportions)*0.9,max(proportions)*1.1),
			xaxt = "n")
	axis(1, at = x)
	out <- dev.off()
	
	png(paste0("data/analysis_plots/", type, "_n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end,"_optimal_solution_size_ratio.png"), width = 512, height = 384)
	par(mar=c(5, 6, 4, 2) + 0.1)
	plot(x, proportions_full, 
			col = "chartreuse4", 
			type = "b",
			ylab = "Ratio of output skeleton size to \nnumber of vertices in input polygon",
			xlab = "Polygon size",
			#main = "Ratio solution size vs number of vertices",
			ylim = c(min(proportions_full)*0.9,max(proportions_full)*1.1),
			xaxt = "n")
	axis(1, at = x)
	out <- dev.off()
	# print(solutions_ratio)
	png(paste0("data/analysis_plots/", type, "_n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end,"_solutions_ratio.png"), width = 512, height = 384)
	plot(x, solutions_ratio, 
			col = "chartreuse4", 
			type = "b",
			ylab = "Ratio",
			xlab = "Polygon size",
			#main = "Ratio solution size vs heuristic solution size",
			ylim = c(min(solutions_ratio)*0.9,max(solutions_ratio)*1.1),
			xaxt = "n")
	axis(1, at = x)
	out <- dev.off()
}
