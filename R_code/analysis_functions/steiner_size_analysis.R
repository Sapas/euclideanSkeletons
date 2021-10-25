single_steiner_size <- function(n, s, type, endFile){
  con <- file(paste0("data/STP_input/", type, "_n",n,"_s",s,"_STP_format_", endFile, ".txt"), "r")
  # Read file until get to solution
  while(!strcmp(readLines(con, n = 1), "SECTION Graph")){}
  # Read an extra one
  numNodes <- as.numeric(gsub("Nodes ", "", readLines(con, n = 1)))
  # Care about this one
  edges <- as.numeric(gsub("Edges ", "", readLines(con, n = 1)))
  nodes <- rep(FALSE, numNodes)
  #allNodes <- c()
  for(i in 1:edges){
    vals <- str_split(gsub("E ", "", readLines(con, n = 1)), " ")[[1]]
    nodes[[as.numeric(vals[[1]])]] <- TRUE
    nodes[[as.numeric(vals[[2]])]] <- TRUE
    #allNodes <- c(allNodes, as.numeric(vals[[1]]), as.numeric(vals[[2]]))
  }
  close(con)
  #print(paste0("Unique found ", length(unique(allNodes)), " and vector found ", sum(nodes)))
  return(c(sum(nodes), edges))
}

steiner_size <- function(n_start, n_end, n_interval, s_start, s_end, type){
  sizesData <- data.frame(matrix(ncol = 5, nrow = 0))
  rows <- 1
  colnames(sizesData) <- c("n", "seed", "nodes", "edges", "Algorithm")
  for(n in seq(n_start, n_end, n_interval)){
    for(seed in s_start:s_end){
      if(!file.exists(paste0("data/STP_input/", type, "_n",n,"_s",seed,"_STP_format_all.txt"))){
        print(paste0("Skipping file data/STP_input/", type, "_n",n,"_s",s,"_STP_format_all.txt as it does not exist!"))
        next
      }
      print(paste0("Working on n ", n, " seed ", seed))
      sizesData[rows, ] <- c(n, seed, single_steiner_size(n, seed, type, "all"), "Unpruned")
      sizesData[rows+1, ] <- c(n, seed, single_steiner_size(n, seed, type, "pruned"), "Pruned")
      sizesData[rows+2, ] <- c(n, seed, single_steiner_size(n, seed, type, "1-2-3"), "Restricted-type")
      sizesData[rows+3, ] <- c(n, seed, single_steiner_size(n, seed, type, "1-2-3_pruned"), "Restricted-type pruned")
      rows <- rows + 4
      #write.csv(sizesData, "R_code/analysis_functions/currData.txt", col.names = TRUE, row.names = FALSE, quote = FALSE, sep = " ")
    }
  }
  
  sizesData$nodes <- as.numeric(sizesData$nodes)
  sizesData$edges <- as.numeric(sizesData$edges)
  sizesData$n <- factor(sizesData$n, levels=unique(sizesData$n))
  sizesData$Algorithm <- factor(sizesData$Algorithm, levels=c("Unpruned", "Restricted-type", "Pruned", "Restricted-type pruned"), ordered = TRUE)
  
  interval <- seq(n_start, n_end, round((n_end - n_start) / 6))
  if(tail(interval, 1) != n_end){interval[[length(interval)]] <- n_end}
  
  ggplot(sizesData, aes(x=n, y=nodes, fill=Algorithm, order=Algorithm)) +
    geom_boxplot(outlier.shape = 21) + 
    scale_fill_grey(name = "Algorithm", start = 0.4) + #, labels = c("Unpruned", "Pruned", "Restricted-type", "Restricted-type Pruned")) +
    scale_x_discrete(breaks = interval, labels = interval) + 
    labs(x = "Polygon size", y = "Number of nodes in Steiner Problem")
  ggsave(paste0("data/analysis_plots/", type, "_n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end,"_steiner_nodes.png"), width = 20, height = 15, units = "cm")
  
  ggplot(sizesData, aes(x=n, y=edges, fill=Algorithm, order=Algorithm)) +
    geom_boxplot(outlier.shape = 21) + 
    scale_fill_grey(name = "Algorithm", start = 0.4) + #, labels = c("Unpruned", "Pruned", "Restricted-type", "Restricted-type Pruned")) +
    scale_x_discrete(breaks = interval, labels = interval) + 
    labs(x = "Polygon size", y = "Number of edges in Steiner Problem")
  ggsave(paste0("data/analysis_plots/", type, "_n",n_start,"<-",n_interval,"->",n_end,"_s",s_start,"<->",s_end,"_steiner_edges.png"), width = 20, height = 15, units = "cm")
  
  
  return(sizesData)
}




