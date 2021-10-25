library(pracma)
library(tidyverse)

plot_polygon <- function(filename){
  polygon <- read.table(paste0("data/polygon/",filename,"_polygon.txt"), header = TRUE)
  
  p <- ggplot() +
    geom_polygon(aes(x, y), polygon, alpha = 0.75) + 
    #geom_point(aes(x, y), polygon, size = 0.02) +
    geom_text(aes(x = x, y = y, label = id), polygon, size = 1) + 
    coord_equal() +
    theme_void()
  ggsave(paste0("data/plots/",filename,".png"), p, width = 20, height = 20, units = "cm", dpi = 500)
}

plot_types <- function(filename){
	polygon <- read.table(paste0("data/polygon/",filename,"_polygon.txt"), header = TRUE)
	edges <- read.table(paste0("data/polygon/",filename,"_edges.txt"), header = TRUE)
	# Need to get all the aux points, simply the end points of type 4 and second point of type 3 (by convention)
	aux_points <- data.frame(x = double(), y = double())
	for(n in 1:nrow(edges)){
		if(edges$type[n] == "Type 4" || edges$type[n] == "Type 3"){
			aux_points[nrow(aux_points) + 1, ] = c(edges[n,]$x, edges[n,]$y)
			aux_points[nrow(aux_points) + 1, ] = c(edges[n,]$xend, edges[n,]$yend)

		}
		else if(edges$type[n] == "Type 2"){
			aux_points[nrow(aux_points) + 1, ] = c(edges[n,]$xend, edges[n,]$yend)
		}
	}
	p <- ggplot() +
    	  geom_polygon(aes(x, y), polygon %>% mutate(type = "Type 1"), alpha = 0.25) +
	      geom_polygon(aes(x, y), polygon %>% mutate(type = "Type 2"), alpha = 0.25) +
	      geom_polygon(aes(x, y), polygon %>% mutate(type = "Type 3"), alpha = 0.25) +
   	  	  geom_polygon(aes(x, y), polygon %>% mutate(type = "Type 4"), alpha = 0.25) +
   	   	  geom_segment(aes(x, y, xend = xend, yend = yend, linetype = type, colour = intersect_prune, size = intersect_prune), edges, lineend = "round") +
   	   	  geom_point(aes(x, y), polygon, size = 0.5) +
   	   	  #geom_point(aes(x, y), aux_points, colour = "red", size = 0.5) +
   	   	  facet_wrap(~type, nrow = 2) +
   	      scale_linetype_manual(values = c("Type 1" = "solid", "Type 2" = "solid", "Type 3" = "solid", "Type 4" = "dashed"), guide = FALSE) +
   	      scale_colour_manual(name = "Prune", values = c("FALSE" = "black", "TRUE" = "red"), guide = FALSE) +
   	      scale_size_manual(name = "Prune", values = c("FALSE" = 0.5, "TRUE" = 0.25), guide = FALSE) +
   	      coord_equal() +
   	      theme_void()
	ggsave(paste0("data/plots/",filename,".png"), p, width = 20, height = 20, units = "cm", dpi = 500)

}

plot_solution <- function(filename){
	polygon <- read.table(paste0("data/polygon/",filename,"_polygon.txt"), header = TRUE)
	edges <- read.table(paste0("data/polygon/",filename,"_edges.txt"), header = TRUE)
	n_convex <- nrow(polygon %>% filter(convex))
	# Read solution file
	con <- file(paste0("data/STP_output/", filename, "_output_STP_format_all.txt"), "r")
	# Read file until get to solution, count lines to skip
	while(!strcmp(readLines(con, n = 1), "SECTION FinalSolution")){}
	# Read extra line (tells number of vertices)
	n <- as.numeric(gsub("Vertices", "", readLines(con, n = 1)))
	# Read n lines, turn into ints
	solution <- as.numeric(gsub("V", "", readLines(con, n = n))) - n_convex
	# Change back to edge id, remove convex points
	solution <- solution[solution > 0]
	close(con)
	skeleton <- edges[solution,]
	# Create plot
	p <- ggplot() + 
		geom_polygon(aes(x, y), polygon, alpha = 0.25) + 
		geom_segment(aes(x, y, xend = xend, yend = yend), skeleton, size = 0.3) + 
		coord_equal() + 
		theme_void()
	
	# Save plot
	ggsave(paste0("data/plots/", filename, "_solution.png"), p, width = 20, height = 20, units = "cm")	
	
}

plot_heuristic_solution <- function(filename){
	polygon <- read.table(paste0("data/polygon/",filename,"_polygon.txt"), header = TRUE)
	edges <- read.table(paste0("data/polygon/",filename,"_edges_heur.txt"), header = TRUE)
	n_convex <- nrow(polygon %>% filter(convex))
	# Read solution file
	con <- file(paste0("data/STP_output/", filename, "_output_STP_format_1-2-3.txt"), "r")
	# Read file until get to solution, count lines to skip
	while(!strcmp(readLines(con, n = 1), "SECTION FinalSolution")){}
	# Read extra line (tells number of vertices)
	n <- as.numeric(gsub("Vertices", "", readLines(con, n = 1)))
	# Read n lines, turn into ints
	solution <- as.numeric(gsub("V", "", readLines(con, n = n))) - n_convex
	# Change back to edge id, remove convex points
	solution <- solution[solution > 0]
	close(con)
	skeleton <- edges[solution,]
	# Create plot
	p <- ggplot() + 
		geom_polygon(aes(x, y), polygon, alpha = 0.25) + 
		geom_segment(aes(x, y, xend = xend, yend = yend), skeleton, size = 0.3) + 
		coord_equal() + 
		theme_void()
	
	# Save plot
	ggsave(paste0("data/plots/", filename, "_solution_heur.png"), p, width = 20, height = 20, units = "cm")	
	
}



