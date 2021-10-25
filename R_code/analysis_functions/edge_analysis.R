library(tidyverse)
library(dplyr)

edges_in_solution <- function(n, seed, type){
	polygon <- read.table(paste0("data/polygon/", type, "_n",n,"_s",seed,"_polygon.txt"), header = TRUE)
	edges <- read.table(paste0("data/polygon/", type, "_n",n,"_s",seed,"_edges.txt"), header = TRUE)
	n_convex <- nrow(polygon %>% filter(convex))
	# Read solution file
	#con <- file(paste0("data/STP_output/", type, "_n",n,"_s",seed,"_output_STP_format_pruned.txt"), "r")
	con <- file(paste0("data/STP_output/", type, "_n",n,"_s",seed,"_output_STP_format_all.txt"), "r")
	# Read file until get to solution, count lines to skip
	while(!strcmp(readLines(con, n = 1), "SECTION FinalSolution")){}
	# Read extra line (tells number of vertices)
	n <- as.numeric(gsub("Vertices", "", readLines(con, n = 1)))
	# Read n lines, turn into ints
	solution <- as.numeric(gsub("V", "", readLines(con, n = n))) - n_convex
	# Change back to edge id, remove convex points
	solution <- solution[solution > 0]
	close(con)
	return(solution)
}


edge_analysis_full <- function(n_start, n_end, n_interval, s_start, s_end, analysis_type, absolute, type){
	edge_all <- data.frame(size = integer(), type1 = integer(), type2 = integer(), type3 = integer(), type4 = integer())
	
	# Need to cycle through all edge files of same size to get averages
	for(n in seq(n_start, n_end, n_interval)){
		types <- c(0,0,0,0)
		count <- 0
		for(s in s_start:s_end){
		  if(!file.exists(paste0("data/polygon/", type, "_n",n,"_s",s,"_edges.txt")) |
		     !file.exists(paste0("data/STP_output/", type, "_n",n,"_s",s,"_output_STP_format_pruned.txt"))){
		    print(paste0("Skipping file data/polygon/", type, "_n",n,"_s",s,"_edges.txt as it does not exist!"))
		    next
		  }
		  count <- count + 1
			edges <- read.table(paste0("data/polygon/", type, "_n",n,"_s",s,"_edges.txt"), header = TRUE)
			if(analysis_type == 'p'){
				edges <- edges[edges$intersect_prune == TRUE,]
			}
			if(analysis_type == 'f'){
				edges <- edges[edges$intersect_prune == FALSE,]
			}
			if(analysis_type == 's'){
				edges <- edges[edges_in_solution(n, s, type),]
			}
			size <- nrow(edges)
			if(absolute){
				types <- types + c(nrow(edges[edges$type == "Type 1",]), 
									nrow(edges[edges$type == "Type 2",]), 
									nrow(edges[edges$type == "Type 3",]), 
									nrow(edges[edges$type == "Type 4",]))

			}else{
			  # if(nrow(edges[edges$type == "Type 1",])/size + 
			  #    nrow(edges[edges$type == "Type 2",])/size +
			  #    nrow(edges[edges$type == "Type 3",])/size + 
			  #    nrow(edges[edges$type == "Type 4",])/size > 1.0001){
			  #   print(paste0("Something weird going on with ", type, " n ", n, " s ", s, ", got ", size, " total edges, then ", nrow(edges[edges$type == "Type 1",]), " type 1, ",
			  #                nrow(edges[edges$type == "Type 2",]), " type 2, ", 
			  #                nrow(edges[edges$type == "Type 3",]), " type 3, ", 
			  #                nrow(edges[edges$type == "Type 4",]), " type 4! Val is ", nrow(edges[edges$type == "Type 1",])/size, " ", nrow(edges[edges$type == "Type 2",])/size, " ",
			  #                nrow(edges[edges$type == "Type 3",])/size, " ", nrow(edges[edges$type == "Type 4",])/size, " ", nrow(edges[edges$type == "Type 1",])/size + 
			  #                  nrow(edges[edges$type == "Type 2",])/size +
			  #                  nrow(edges[edges$type == "Type 3",])/size + 
			  #                  nrow(edges[edges$type == "Type 4",])/size))
			  #   #print(edges)
			  #   
			  # }
				types <- types + c(nrow(edges[edges$type == "Type 1",])/size, 
									nrow(edges[edges$type == "Type 2",])/size, 
									nrow(edges[edges$type == "Type 3",])/size, 
									nrow(edges[edges$type == "Type 4",])/size)
			}
		}
		# Now get average
		if(absolute){
			#types <- types / (s_end - s_start + 1)
		  types <- types / count
		}else{
		  types <- types / count * 100
			#types <- types / (s_end - s_start + 1) * 100
		}
		# And add to the data frame!
		edge_all[nrow(edge_all)+1,] = c(n,types)
	}
	temp <- edge_all %>% gather(key = "Type", value = "value", 2:5) %>% group_by(size, Type) %>% summarise(prop = mean(value))
	x <- seq(n_start, n_end, round((n_end - n_start) / 6))
	if(tail(x, 1) != n_end){x[[length(x)]] <- n_end}
	p <- ggplot() +
  			geom_bar(aes(size, prop, group = size, fill = Type), temp, stat = "identity") +
  			#scale_fill_discrete(name = "Edge type", labels = c("Type 1", "Type 2", "Type 3", "Type 4")) + 
  			scale_fill_grey(name = "Edge type", labels = c("Type 1", "Type 2", "Type 3", "Type 4")) + 
  			xlab("Polygon size") +
  			scale_x_discrete(limits = x)
  	if(absolute){
  		p <- p + ylab("Number of edges per type")
  	}else{
  		p <- p + ylab("Proportion of edge types (%)")
  	}
  	filename <- paste0("data/analysis_plots/", type, "_n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end, "_edge")
  	if(absolute){
  		filename <- paste0(filename,"_numbers")
  	}else{
  		filename <- paste0(filename, "_proportions")
  	}
	if(analysis_type == 'a'){
		#if(absolute){
		#	p <- p + ggtitle("Numbers of edge types pre-pruning")
		#}else{
		#	p <- p + ggtitle("Proportion of edge types pre-pruning")	
		#}
		ggsave(paste0(filename,"_prepruning.png"), width = 16, height = 9, units = "cm")		
	}else if(analysis_type == 'p'){
		#if(absolute){
		#	p <- p + ggtitle("Numbers of edge types pruned")
		#}else{
		#	p <- p + ggtitle("Proportion of edge types pruned")	
		#}
		ggsave(paste0(filename,"_pruned.png"), width = 16, height = 9, units = "cm")
	}else if(analysis_type == 'f'){
		#if(absolute){
		#	p <- p + ggtitle("Numbers of edge types post-pruning")
		#}else{
		#	p <- p + ggtitle("Proportion of edge types post-pruning")	
		#}
		ggsave(paste0(filename,"_postpruning.png"), width = 16, height = 9, units = "cm")
	}else{
		#if(absolute){
		#	p <- p + ggtitle("Numbers of edge types in solution")
		#}else{
		#	p <- p + ggtitle("Proportion of edge types in solution")	
		#}
		ggsave(paste0(filename,"_solution.png"), width = 16, height = 9, units = "cm")
	}
  	
  return(edge_all)
		
}

edge_analysis <- function(n_start, n_end, n_interval, s_start, s_end, type){
	all_absolute <- edge_analysis_full(n_start, n_end, n_interval, s_start, s_end, 'a', TRUE, type)
	pruned_absolute <- edge_analysis_full(n_start, n_end, n_interval, s_start, s_end, 'p', TRUE, type)
	postpruned_absolute <- edge_analysis_full(n_start, n_end, n_interval, s_start, s_end, 'f', TRUE, type)
	solution_absolute <- edge_analysis_full(n_start, n_end, n_interval, s_start, s_end, 's', TRUE, type)
	all_percent <- edge_analysis_full(n_start, n_end, n_interval, s_start, s_end, 'a', FALSE, type)
	print("Numbers for all")
	print(all_percent)
	pruned_percent <- edge_analysis_full(n_start, n_end, n_interval, s_start, s_end, 'p', FALSE, type)
	print("Numbers for pruned")
	print(pruned_percent)
	postpruned_percent <- edge_analysis_full(n_start, n_end, n_interval, s_start, s_end, 'f', FALSE, type)
	print("Numbers for post pruned")
	print(postpruned_percent)
	solution_percent <- edge_analysis_full(n_start, n_end, n_interval, s_start, s_end, 's', FALSE, type)
	print("Numbers for solution")
	print(solution_percent)
	# Save data
	all_absolute_edit <- all_absolute
	all_percent_edit <- all_percent
	pruned_absolute_edit <- pruned_absolute
	pruned_percent_edit <- pruned_percent
	postpruned_absolute_edit <- postpruned_absolute
	postpruned_percent_edit <- postpruned_percent
	solution_absolute_edit <- solution_absolute
	solution_percent_edit <- solution_percent
	# Add column to differentiate each one
	all_absolute_edit$edgeType = "\u0393\u0027"
	all_percent_edit$edgeType = "\u0393\u0027"
	pruned_absolute_edit$edgeType = 'p'
	pruned_percent_edit$edgeType = 'p'
	postpruned_absolute_edit$edgeType = "\u0393"
	postpruned_percent_edit$edgeType = "\u0393"
	solution_absolute_edit$edgeType = 'S'
	solution_percent_edit$edgeType = 'S'
	# Combine data and add levels to labelling
	absolute <- rbind(all_absolute_edit, postpruned_absolute_edit, solution_absolute_edit)
	percent <- rbind(all_percent_edit, postpruned_percent_edit, solution_percent_edit)
	absolute$edgeType = factor(absolute$edgeType, levels=c("\u0393\u0027", '\u0393', 'S'))
	percent$edgeType = factor(percent$edgeType, levels=c("\u0393\u0027", '\u0393', 'S'))
	# Save both graphs
	temp_abs <- absolute %>% gather(key = "Type", value = "value", 2:5) %>% group_by(size, Type, edgeType) %>% summarise(prop = mean(value))
	x <- temp_abs$size[!duplicated(temp_abs$size)]
	interval <- seq(n_start, n_end, round((n_end - n_start) / 4))
	if(tail(interval, 1) != n_end){interval[[length(interval)]] <- n_end}
	p <- ggplot() +
	  geom_bar(aes(size, prop, group = size, fill = Type), temp_abs, stat = "identity") +
	  #scale_fill_discrete(name = "Edge type", labels = c("Type 1", "Type 2", "Type 3", "Type 4")) + 
	  scale_fill_grey(name = "Edge type", labels = c("Type 1", "Type 2", "Type 3", "Type 4")) + 
	  xlab("Polygon size") +
	  #scale_x_discrete(limits = seq(n_start, n_end, n_interval*6)) +
	  scale_x_discrete(limits = interval) +
	  ylab("Number of edges per type") + 
	  facet_wrap(~ edgeType) +
	  theme(panel.spacing = unit(1, "lines"))
	filename <- paste0("data/analysis_plots/", type, "_n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end, "_edge_numbers_combined.png")
	ggsave(filename, width = 16, height = 9, units = "cm")
	
	temp_perc <- percent %>% gather(key = "Type", value = "value", 2:5) %>% group_by(size, Type, edgeType) %>% summarise(prop = mean(value))
	x <- temp_perc$size[!duplicated(temp_perc$size)]
	p <- ggplot() +
	  geom_bar(aes(size, prop, group = size, fill = Type), temp_perc, stat = "identity") +
	  #scale_fill_discrete(name = "Edge type", labels = c("Type 1", "Type 2", "Type 3", "Type 4")) + 
	  scale_fill_grey(name = "Edge type", labels = c("Type 1", "Type 2", "Type 3", "Type 4")) + 
	  xlab("Polygon size") +
	  #scale_x_discrete(limits = seq(n_start, n_end, n_interval*6)) +
	  scale_x_discrete(limits = interval) +
	  ylab("Proportion of edge types (%)") + 
	  facet_wrap(~ edgeType) +
	  theme(panel.spacing = unit(1, "lines"))
	filename <- paste0("data/analysis_plots/", type, "_n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end, "_edge_proportions_combined.png")
	ggsave(filename, width = 16, height = 9, units = "cm")
}




