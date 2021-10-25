combineArrayResults <- function(runName, arrayStart, arrayEnd){
  combinedData <- read.table(paste0("data/clusterResults/", runName, "_arrayJob", arrayStart, ".txt"), header = TRUE, sep = " ", fill = TRUE)
  for(i in (arrayStart + 1):arrayEnd){
    print(i)
    filename <- paste0("data/clusterResults/", runName, "_arrayJob", i, ".txt")
    if(!file.exists(filename)){
      print(paste0("Skipping file ", filename, " as it does not exist!"))
      next
    }
    newData <- read.table(filename, header = TRUE, sep = " ", fill = TRUE)
    if(nrow(newData[is.na(newData$total),]) > 0){print(paste0("Line ", i, " has nas!"))}
    combinedData <- rbind(combinedData, newData)
  }
  return(combinedData)
}


# Example usage
#combined <- combineArrayResults("smallTest", 1, 48)
#average <- getAveragePerformance("smallTest", 1, 48)
