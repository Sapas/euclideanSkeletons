#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <stdio.h>
#include <time.h>
#include <utility>
#include <cstring>
#include <ctime>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <iomanip>
#include "find_intersections.cpp"
using namespace std;


struct SkeletonLine{
	int id;
	Point* start;
	Point* end;
	int type; //Options are 1 - 4
	bool pruned;
};

struct Polygon{
	vector<Point*> vertices;
	vector<Point*> auxVertices;
	vector<SkeletonLine*> skeleton;
	vector<int> convex;
	int num_convex;
	vector<int> convex_id;
	vector<pair<int,int>> steiner_edge;

	Polygon(){}
	Polygon(int n):
	vertices(n),
	convex(n)
	{};

	~Polygon(){
		for(int i = 0; i < vertices.size(); i++){
			delete vertices[i];
			vertices[i] = NULL;
		}
		for(int i = 0; i < auxVertices.size(); i++){
			delete auxVertices[i];
			auxVertices[i] = NULL;
		}
		for(int i = 0; i < skeleton.size(); i++){
			//delete skeleton[i]->start;
			//skeleton[i]->start = NULL;
			//delete skeleton[i]->end;
			//skeleton[i]->end = NULL;
			delete skeleton[i];
			skeleton[i] = NULL;
		}
		vertices.clear();
		auxVertices.clear();
		skeleton.clear();
		convex.clear();
		convex_id.clear();
		steiner_edge.clear();
		// vertices.erase(vertices.begin(), vertices.end());
		// auxVertices.erase(auxVertices.begin(), auxVertices.end());
		// skeleton.erase(skeleton.begin(), skeleton.end());
		// convex.erase(convex.begin(), convex.end());
		// convex_id.erase(convex_id.begin(), convex_id.end());
		// steiner_edge.erase(steiner_edge.begin(), steiner_edge.end());
	}
};




void read_polygon(Polygon* polygon, string polygonFilePath){
	FILE * fp;
	char buffer[1001];
	char buffer2[1001];
	char *cstr = new char[polygonFilePath.length() + 1];
	strcpy(cstr, polygonFilePath.c_str());
	fp = fopen(cstr, "r");
	if(!fp){
        printf("Could not open file.  Giving up.\n");
        return;
    }
    fgets(buffer, 1000, fp);
	fgets(buffer, 1000, fp);
	double x, y;
    int id;
	while(feof(fp)==0){
		sscanf(buffer, "%d %lf %lf %s", &id, &x,&y, buffer2);
		string convex(buffer2);
		polygon->vertices.push_back(new Point{id-1,x,y});
		if(convex == "TRUE"){
			polygon->convex.push_back(true);
		}else if(convex == "FALSE"){
			polygon->convex.push_back(false);
		}else{
			printf("Did not understand convex reading, setting to false but expect problems!\n");
			polygon->convex.push_back(false);
		}
		fgets(buffer, 1000, fp);
	}
	delete [] cstr;
	return;
}

// First goes all the output/input stuffstuff
void read_special_polygon(Polygon* polygon, string polygonName){
	FILE * fp;
	char buffer[1001];
	char buffer2[1001];
	char *cstr = new char[polygonName.length() + 1];
	strcpy(cstr, polygonName.c_str());
	fp = fopen(cstr, "r");
	if(!fp){
        printf("Could not open file.  Giving up.\n");
        return;
    }
    fgets(buffer, 1000, fp);
	fgets(buffer, 1000, fp);
	double x, y;
    int id = 0;
	while(feof(fp)==0){
		// sscanf(buffer, "%lf %lf %s",&x,&y, buffer2);
		sscanf(buffer, "%lf %lf ",&x,&y);
		polygon->vertices.push_back(new Point{id,x,y});
		id++;
		polygon->convex.push_back(false);
		fgets(buffer, 1000, fp);
	}
	delete [] cstr;
	return;
}


void write_polygon(Polygon* polygon, string polygonName){
	ofstream file;
	file.open("data/polygon/" + polygonName + "_polygon.txt");
	if(!file.is_open()){
		printf("Couldn't open polygon write to file\n");
	}
	file << "id x y convex\n";
	for(int i = 0; i < polygon->vertices.size(); i++){
		file << setprecision(8) << (i + 1) << " " << polygon->vertices[i]->x << " " << polygon->vertices[i]->y;
		if(polygon->convex[i] == 1){
		 	file << " TRUE\n";
		}else{
			file << " FALSE\n";
		}
	}
	file.close();
}

void write_polygon_edges(Polygon* polygon, string polygonName, bool heur){
	ofstream file;
	if(heur){
		file.open("data/polygon/" + polygonName + "_edges_heur.txt");
	}else{
		file.open("data/polygon/" + polygonName + "_edges.txt");
	}
	if(!file.is_open()){
		printf("Couldn't open edges write to file\n");
	}
	file << "id x y xend yend type intersect_prune\n";
	for(int i = 0; i < polygon->skeleton.size(); i++){
		file << setprecision(8) << i + 1 << " " << polygon->skeleton[i]->start->x << " " << polygon->skeleton[i]->start->y << " " << polygon->skeleton[i]->end->x << " " << polygon->skeleton[i]->end->y;
		file << " \"Type " << polygon->skeleton[i]->type << "\" ";
		if(polygon->skeleton[i]->pruned){
			file << "TRUE\n";
		}else{
			file << "FALSE\n";
		}
	}
	file.close();
}


// Next the polygon creation files

// Uncrosses a pair of edges by changing the order of the points
void fix_crossing(Polygon* polygon, int index1, int index2){
	// Only need to reverse from index1 + 1 to index2 (index 2 is larger)
	int num = (index2 - index1) / 2;
	for(int i = 0; i < num; i++){
		// Switch the two
		Point* temp = polygon->vertices[index1 + i + 1];
		polygon->vertices[index1 + i + 1] = polygon->vertices[index2 - i];
		polygon->vertices[index2 - i] = temp;
	}
}

void untangle_polygon(Polygon* polygon, int seed){
	mt19937 mt_rand(seed);
	int n = polygon->vertices.size();
	// Uncross polygon i.e. find edges that intersect, and switch them
	bool crossings = true;
	while(crossings){
		crossings = false;
		// Pick random edges to start comparing from
		int firstStart = mt_rand() % n;
		int secondStart = mt_rand() % n;
		for(int i = 0; i < n && !crossings; i++){
			int i_index = (firstStart + i) % n;
			for(int j = 0; j < n && !crossings; j++){
				int j_index = (secondStart + j) % n;
				// If same or next to each other skip
				if(i_index == j_index || 
					min(i_index,j_index)+1 == max(i_index,j_index) ||
					(max(i_index,j_index) == n - 1 && min(i_index,j_index) == 0)){continue;}

				if(doIntersect(polygon->vertices[i_index], polygon->vertices[(i_index + 1) % n],
					polygon->vertices[j_index], polygon->vertices[(j_index + 1) % n])){
					// printf("Untangle %d (%.2f, %.2f) - (%.2f, %.2f), %d (%.2f, %.2f) - (%.2f, %.2f)\n", i_index,
					// 													polygon->vertices[i_index]->x,
					// 													polygon->vertices[i_index]->y,
					// 													polygon->vertices[(i_index + 1) % n]->x,
					// 													polygon->vertices[(i_index + 1) % n]->y,
					// 													j_index,
					// 													polygon->vertices[j_index]->x,
					// 													polygon->vertices[j_index]->y,
					// 													polygon->vertices[(j_index + 1) % n]->x,
					// 													polygon->vertices[(j_index + 1) % n]->y);
					crossings = true;
					fix_crossing(polygon, min(i_index, j_index), max(i_index, j_index));
				}
			}
		}
	}
}

void remove_colinear(Polygon* polygon){
	int n = polygon->vertices.size();
	for(int i = 0; i < n; i++){
		if(orientation(polygon->vertices[(i - 1 + n) % n], polygon->vertices[i], polygon->vertices[(i+1) % n]) == 0){
			printf("Removing vertex %d since consecutive colinear points!\n", i);
			delete polygon->vertices[i];
			polygon->vertices.erase(polygon->vertices.begin() + i);
			n--;
		}
	}
}

// Find which points are convex
// Point with min y coordinate must be convex
// Find it and go from there
void find_convex_points(Polygon* polygon){
	int minId = 0;
	// Point* minPoint = polygon->vertices[0];
	int n = polygon->vertices.size();
	// Find point with min y coordinate
	for(int i = 1; i < n; i++){
		if(polygon->vertices[minId]->y > polygon->vertices[i]->y){
			// printf("new id %d\n", i);
			// minPoint = polygon->vertices[i];
			// printf("min id %d\n", minPoint->id);
			minId = i;
		}
	}
	// printf("Starting at %d, this is convex\n", polygon->vertices[minId]->id+1);
	// printf("Min is %.4f %d\n",minPoint->y, minPoint->id);
	// This point must be convex
	int i = minId;
	polygon->convex[i] = true;
	int turn = orientation(polygon->vertices[(i - 1 + n) % n], polygon->vertices[i], polygon->vertices[(i + 1) % n]);
	bool add = true;
	int newTurn;
	// Now keep on cycling from this point
	i = (i + 1) % n;
	while(i != minId){
		double val = (polygon->vertices[i]->y - polygon->vertices[(i - 1 + n) % n]->y) * (polygon->vertices[(i + 1) % n]->x - polygon->vertices[i]->x) - 
              (polygon->vertices[i]->x - polygon->vertices[(i - 1 + n) % n]->x) * (polygon->vertices[(i + 1) % n]->y - polygon->vertices[i]->y); 
		newTurn = orientation(polygon->vertices[(i - 1 + n) % n], polygon->vertices[i], polygon->vertices[(i + 1) % n]);
		if((newTurn == turn && add) || 
			(newTurn != turn && !add)){
			polygon->convex[i] = 1;
			add = true;
			// printf("Now at %d, this is convex, orientation was %d, angle %.20f, val %20f\n", polygon->vertices[i]->id+1, newTurn, angle(polygon->vertices[(i - 1 + n) % n], polygon->vertices[i], polygon->vertices[(i + 1) % n]), val);
		}else{
			polygon->convex[i] = 0;
			add = false;
			// printf("Now at %d, this is NOT convex, orientation was %d, angle %.20f, val %20f\n", polygon->vertices[i]->id+1, newTurn, angle(polygon->vertices[(i - 1 + n) % n], polygon->vertices[i], polygon->vertices[(i + 1) % n]), val);
		}
		turn = newTurn;
		i = (i + 1) % n;
	}
	polygon->num_convex = 0;
	for(int i = 0; i < n; i++){
		if(polygon->convex[i]){
			polygon->convex_id.push_back(polygon->num_convex);
		}else{
			polygon->convex_id.push_back(-1);
		}
		polygon->num_convex += polygon->convex[i];

	}
}


void create_and_store_polygon(Polygon* polygon, int n, int seed, string generationMode, double maxCoord){
	mt19937 mt_rand(seed);
	std::normal_distribution<double> randPosition(0, maxCoord);
	Point* newPoint;
	for(int i = 0; i < n; i++){

		if(generationMode == "uniform"){
			newPoint = new Point{i,
							round(maxCoord * (double) mt_rand() / RAND_MAX),
							round(maxCoord * (double) mt_rand() / RAND_MAX)};
			// polygon->vertices[i] = new Point{i,
			// 		round(maxCoord * (double) mt_rand() / RAND_MAX),
			// 		round(maxCoord * (double) mt_rand() / RAND_MAX)};

		}else if(generationMode == "normal"){
			newPoint = new Point{i,
									round(randPosition(mt_rand)),
									round(randPosition(mt_rand))};
			// polygon->vertices[i] = new Point{i,
			// 		round(randPosition(mt_rand)),
			// 		round(randPosition(mt_rand))};
		
		}else{
			printf("Unkown generation mode for the vertices, stopping now!\n");
			return;
		}

		// Check that this point does not yet exist, and no three colinear points
		bool good = true;
		for(int j = 0; j < i && good; j++){
			// Check point does not overlap
			if(abs(polygon->vertices[j]->x - newPoint->x) < 0.001 &&
				abs(polygon->vertices[j]->y - newPoint->y) < 0.001){good = false;}
			// Check no colinear
			for(int k = j + 1; k < i && good; k++){
				if(orientation(polygon->vertices[j], polygon->vertices[k], newPoint) == 0){
					printf("Colinear points! %d, points (%.2f, %.2f), (%.2f, %.2f), (%.2f, %.2f)\n", i, 
						polygon->vertices[j]->x,
						polygon->vertices[j]->y,
						polygon->vertices[k]->x,
						polygon->vertices[k]->y,
						newPoint->x,
						newPoint->y);
					good = false;
				}
			}
		}
		if(good){
			polygon->vertices[i] = newPoint;
		}else{
			delete newPoint;
			i--;
		}		
	}
	untangle_polygon(polygon, seed);
	remove_colinear(polygon);
	find_convex_points(polygon);
	string polygonName = generationMode + "_n" + to_string(n) + "_s" + to_string(seed);
	write_polygon(polygon, polygonName);
}

void read_and_store_polygon(Polygon* polygon, string polygonName){
	string filename = "data/input_polygon/" + polygonName + "_polygon.txt";
	// string filename = "data/polygon/" + polygonName + "_polygon.txt";
	read_special_polygon(polygon, filename);
	untangle_polygon(polygon, 1);
	remove_colinear(polygon);
	find_convex_points(polygon);
	write_polygon(polygon, polygonName);
}


// Outputs polygon information to text files in the form of the steiner tree problem
// Will have 4: One with only type 1,2 edge, one with types 1,2,3, one with all, one with all prunned
// Adding input to make my life easier
// Type 1: Only skeleton edges 1&2
// Type 2: Only skeleton edges 1,2&3
// Type 3: All skeleton edges
// Type 4: All skeleton edges, prunned
void stp_plotter(string filename, Polygon* polygon, int type){
	ofstream file;
	if(type < 0 && type > 4){
		printf("Wrong usage of function\n");
		return;
	}

	if(type == 0){file.open("data/STP_input/" + filename + "_STP_format_1-2.txt");}
	else if(type == 1){file.open("data/STP_input/" + filename + "_STP_format_1-2-3.txt");}
	else if(type == 2){file.open("data/STP_input/" + filename + "_STP_format_1-2-3_pruned.txt");}
	else if(type == 3){file.open("data/STP_input/" + filename + "_STP_format_all.txt");}
	else if(type == 4){file.open("data/STP_input/" + filename + "_STP_format_pruned.txt");}

	if(!file.is_open()){
		printf("Couldn't open STP write to file\n");
	}

	file << "33D32945 STP File, STP Format Version 1.0\n\n";
	file << "SECTION Comment\n";
	file << "Problem \"Classical Steiner tree problem in graph\"\n";

	if(type == 0){file << "Name " << filename << " only type 1&2 skeleton edges\n";}
	else if(type == 1){file << "Name " << filename << " only type 1,2&3 skeleton edges\n";}
	else if(type == 2){file << "Name " << filename << " only type 1,2&3 skeleton edges, pruned\n";}
	else if(type == 3){file << "Name " << filename << " all skeleton edges\n";}
	else if(type == 4){file << "Name " << filename << " all skeleton edges, pruned\n";}

	file << "Name " << filename << "\n";
	file << "Creator Nico Andres Thio\n";
	file << "END\n\n";
	file << "SECTION Graph\n";
	file << "Nodes " << polygon->num_convex + int(polygon->skeleton.size()) << "\n";
	
	// First one pass to figure out how many edges will be in the output
	int totalEdges = 0;
	for(int i = 0; i < polygon->steiner_edge.size(); i++){
		if(type == 0 && 
			((polygon->steiner_edge[i].first > polygon->num_convex &&
				polygon->skeleton[polygon->steiner_edge[i].first - polygon->num_convex]->type >= 3) ||
			(polygon->steiner_edge[i].second > polygon->num_convex &&
				polygon->skeleton[polygon->steiner_edge[i].second - polygon->num_convex]->type >= 3))){
			continue;
		}
		else if(type == 1 && 
			((polygon->steiner_edge[i].first > polygon->num_convex &&
				polygon->skeleton[polygon->steiner_edge[i].first - polygon->num_convex]->type >= 4) ||
			(polygon->steiner_edge[i].second > polygon->num_convex &&
				polygon->skeleton[polygon->steiner_edge[i].second - polygon->num_convex]->type >= 4))){
			continue;
		}
		else if(type == 2 && 
			((polygon->steiner_edge[i].first > polygon->num_convex &&
				polygon->skeleton[polygon->steiner_edge[i].first - polygon->num_convex]->type >= 4) ||
			(polygon->steiner_edge[i].second > polygon->num_convex &&
				polygon->skeleton[polygon->steiner_edge[i].second - polygon->num_convex]->type >= 4))){
			continue;
		}
		else if(type == 2 &&
			((polygon->steiner_edge[i].first > polygon->num_convex && 
			polygon->skeleton[polygon->steiner_edge[i].first - polygon->num_convex]->pruned) ||
			(polygon->steiner_edge[i].second > polygon->num_convex && 
			polygon->skeleton[polygon->steiner_edge[i].second - polygon->num_convex]->pruned))){
			continue;
		}
		else if(type == 4 &&
			((polygon->steiner_edge[i].first > polygon->num_convex && 
			polygon->skeleton[polygon->steiner_edge[i].first - polygon->num_convex]->pruned) ||
			(polygon->steiner_edge[i].second > polygon->num_convex && 
			polygon->skeleton[polygon->steiner_edge[i].second - polygon->num_convex]->pruned))){
			continue;
		}
		totalEdges++;
	}

	file << "Edges " << totalEdges << "\n";

	for(int i = 0; i < polygon->steiner_edge.size(); i++){
		if(type == 0 && 
			((polygon->steiner_edge[i].first > polygon->num_convex &&
				polygon->skeleton[polygon->steiner_edge[i].first - polygon->num_convex]->type >= 3) ||
			(polygon->steiner_edge[i].second > polygon->num_convex &&
				polygon->skeleton[polygon->steiner_edge[i].second - polygon->num_convex]->type >= 3))){
			continue;
		}
		else if(type == 1 && 
			((polygon->steiner_edge[i].first > polygon->num_convex &&
				polygon->skeleton[polygon->steiner_edge[i].first - polygon->num_convex]->type >= 4) ||
			(polygon->steiner_edge[i].second > polygon->num_convex &&
				polygon->skeleton[polygon->steiner_edge[i].second - polygon->num_convex]->type >= 4))){
			continue;
		}
		else if(type == 2 && 
			((polygon->steiner_edge[i].first > polygon->num_convex &&
				polygon->skeleton[polygon->steiner_edge[i].first - polygon->num_convex]->type >= 4) ||
			(polygon->steiner_edge[i].second > polygon->num_convex &&
				polygon->skeleton[polygon->steiner_edge[i].second - polygon->num_convex]->type >= 4))){
			continue;
		}
		else if(type == 2 &&
			((polygon->steiner_edge[i].first > polygon->num_convex && 
			polygon->skeleton[polygon->steiner_edge[i].first - polygon->num_convex]->pruned) ||
			(polygon->steiner_edge[i].second > polygon->num_convex && 
			polygon->skeleton[polygon->steiner_edge[i].second - polygon->num_convex]->pruned))){
			continue;
		}
		else if(type == 4 &&
			((polygon->steiner_edge[i].first > polygon->num_convex && 
			polygon->skeleton[polygon->steiner_edge[i].first - polygon->num_convex]->pruned) ||
			(polygon->steiner_edge[i].second > polygon->num_convex && 
			polygon->skeleton[polygon->steiner_edge[i].second - polygon->num_convex]->pruned))){
			continue;
		}

		// // Add a check to not add pruned
		// if((polygon->steiner_edge[i].first > polygon->num_convex && 
		// 	polygon->skeleton[polygon->steiner_edge[i].first - polygon->num_convex]->pruned) ||
		// 	(polygon->steiner_edge[i].second > polygon->num_convex && 
		// 	polygon->skeleton[polygon->steiner_edge[i].second - polygon->num_convex]->pruned)){
		// 	// printf("")
		// 	continue;
		// }
		file << "E " << polygon->steiner_edge[i].first + 1 << " " << polygon->steiner_edge[i].second + 1 << " 1\n";
	}
	file << "END\n";

	file << "\nSECTION Terminals\n";
	file << "Terminals " << polygon->num_convex << "\n";
	for(int i = 0; i < polygon->vertices.size(); i++){
		if(polygon->convex[i]){
			file << "T " << polygon->convex_id[i] + 1 << "\n";
		}
	}
	file << "END\n\n";
	file << "EOF\n";
	file.close();

}

// Just a printer function to make sure no colinear points
void check_colinear(Polygon* polygon){
	if(orientation(polygon->vertices.back(), polygon->vertices[0], polygon->vertices[1]) == 0){
			printf("Colinear 0\n");
	}
	if(orientation(polygon->vertices[polygon->vertices.size() - 2], polygon->vertices.back(), polygon->vertices[0]) == 0){
			printf("Colinear n\n");
	}
	for(int i =1; i < polygon->vertices.size() - 1; i++){
		if(orientation(polygon->vertices[i-1], polygon->vertices[i], polygon->vertices[i+1]) == 0){
			printf("Colinear %d\n",i);
		}
	}
}




bool adjacent(int n, int i, int j){
	if(abs(i - j) == 1 || abs(i-j) == n - 1){return true;}
	return false;
}
// Returns angle between lines p->q and p->r
double line_angle(Point* p, Point* q, Point* r){
	// Get vectors
	Point line1 = {-1, q->x - p->x, q->y - p->y};
	Point line2 = {-1, r->x - p->x, r->y - p->y};
	double cosTheta = (line1.x * line2.x + line1.y * line2.y)/(sqrt(line1.x*line1.x + line1.y*line1.y)*sqrt(line2.x*line2.x + line2.y*line2.y));
	return 180*acos(cosTheta)/3.14;
	
}

// Cycle through edges of polygon, check if can see each other
// IMPORTANT: Assume i < j
bool can_see(Polygon* polygon, int i, int j){
	int n = polygon->vertices.size();
	// Check if line goes towards inside of polygon
	// Only need to check on of the endpoints,
	// But checking both might save some troubles
	// If convex, if different turn orientation or same 
	// but larger angle, outside polygon
	if(polygon->convex[i] == 1 && 
		(orientation(polygon->vertices[(i-1+n)%n], polygon->vertices[i], polygon->vertices[i+1]) != 
			orientation(polygon->vertices[(i-1+n)%n], polygon->vertices[i], polygon->vertices[j]) ||
			line_angle(polygon->vertices[i], polygon->vertices[(i-1+n)%n], polygon->vertices[i+1]) < 
			line_angle(polygon->vertices[i], polygon->vertices[(i-1+n)%n], polygon->vertices[j]))){

		return false;
	}
	if(polygon->convex[j] && 
		(orientation(polygon->vertices[j-1], polygon->vertices[j], polygon->vertices[(j+1) % n]) != 
			orientation(polygon->vertices[j-1], polygon->vertices[j], polygon->vertices[i]) ||
			line_angle(polygon->vertices[j], polygon->vertices[j-1], polygon->vertices[(j+1)%n]) < 
			line_angle(polygon->vertices[j], polygon->vertices[j-1], polygon->vertices[i]))){
		
		return false;
	}

	// If concave, same turn orientation and smaller angle, outside of polygon
	if(polygon->convex[i] == 0 &&
		orientation(polygon->vertices[(i-1+n)%n], polygon->vertices[i], polygon->vertices[i+1]) == 
			orientation(polygon->vertices[(i-1+n)%n], polygon->vertices[i], polygon->vertices[j]) &&
		line_angle(polygon->vertices[i], polygon->vertices[(i-1+n)%n], polygon->vertices[i+1]) > 
			line_angle(polygon->vertices[i], polygon->vertices[(i-1+n)%n], polygon->vertices[j])){

		return false;
	}
	if(!polygon->convex[j] &&
		orientation(polygon->vertices[j-1], polygon->vertices[j], polygon->vertices[(j+1) % n]) == 
			orientation(polygon->vertices[j-1], polygon->vertices[j], polygon->vertices[i]) &&
		line_angle(polygon->vertices[j], polygon->vertices[j-1], polygon->vertices[(j+1) % n]) > 
			line_angle(polygon->vertices[j], polygon->vertices[j-1], polygon->vertices[i])){
		return false;
	}
	// printf("Passed angle test! For point %d (%d) got orientations %d - %d, for point %d (%d) got orientations %d - %d\n",
	// 	i+1,
	// 	polygon->convex[i],
	// 	orientation(polygon->vertices[(i-1+n)%n], polygon->vertices[i], polygon->vertices[i+1]),
	// 	orientation(polygon->vertices[(i-1+n)%n], polygon->vertices[i], polygon->vertices[j]),
	// 	j+1,
	// 	polygon->convex[j],
	// 	orientation(polygon->vertices[j-1], polygon->vertices[j], polygon->vertices[(j+1) % n]),
	// 	orientation(polygon->vertices[j-1], polygon->vertices[j], polygon->vertices[i]));
	// Check every edge for intersections
	for(int k = 0; k < n; k++){
		// if index is 1 smaller or same, skip (they share a point)
		if((i == 0 && k == n - 1) ||
			i - k == 1 ||
			i == k ||
			j - k == 1 ||
			j == k){continue;}
		// Now check if intersect, if so end
		if(doIntersect(polygon->vertices[i], polygon->vertices[j],
						polygon->vertices[k], polygon->vertices[(k+1) % n])){

			return false;
		}
	}

	return true;
}

void extend_skeleton_edge(Polygon* polygon, int edgeId, int i, int j){
	int n = polygon->vertices.size();
	Point* left = NULL;
	Point* right = NULL;
	Point* up = NULL;
	Point* down = NULL;
	Point* intersection;
	for(int k = 0; k < n; k++){
		// Ignore edges that share a node
		if((i == 0 && k == n - 1) ||
			i - k == 1 ||
			i == k ||
			j - k == 1 ||
			j == k){continue;}
		// Find intersection of lines (should exist)
		intersection = lineLineIntersection(polygon->vertices[i], polygon->vertices[j], polygon->vertices[k], polygon->vertices[(k+1) % n]);
		// If id of intersection is -2, means lines are parallel, skip
		if(intersection->id == -2){
			delete intersection;
			continue;
		}
		// Check if intersection is on line of boundary, must account for vertical lines
		if(isVertical(polygon->vertices[k],polygon->vertices[(k+1)%n])){
			if(intersection->y < min(polygon->vertices[k]->y,polygon->vertices[(k+1)%n]->y) ||
				intersection->y > max(polygon->vertices[k]->y,polygon->vertices[(k+1)%n]->y)){
				delete intersection;
				continue;
			}
		}else{
			if(intersection->x < min(polygon->vertices[k]->x,polygon->vertices[(k+1)%n]->x) ||
				intersection->x > max(polygon->vertices[k]->x,polygon->vertices[(k+1)%n]->x)){
				delete intersection;				
				continue;
			}
		}
		bool update = false;
		// Save if "better", must account for vertical lines
		if(isVertical(polygon->vertices[i],polygon->vertices[j])){
			if(intersection->y > max(polygon->vertices[i]->y,polygon->vertices[j]->y) &&
				(up == NULL || intersection->y < up->y)){
				delete up;
				up = intersection;
				update = true;
			}
			if(intersection->y < min(polygon->vertices[i]->y,polygon->vertices[j]->y) &&
				(down == NULL || intersection->y > down->y)){
				delete down;
				down = intersection;
				update = true;
			}
		}else{
			if(intersection->x > max(polygon->vertices[i]->x,polygon->vertices[j]->x) &&
				(right == NULL || intersection->x < right->x)){
				delete right;
				right = intersection;
				update = true;
			}
			if(intersection->x < min(polygon->vertices[i]->x,polygon->vertices[j]->x) &&
				(left == NULL || intersection->x > left->x)){
				delete left;
				left = intersection;
				update = true;
			}
		}
		if(!update){
			delete intersection;
		}	
	}
	// Here make sure save the correct ones
	// If type 3, expand both
	if(polygon->skeleton[edgeId]->type == 3){
		if(isVertical(polygon->vertices[i],polygon->vertices[j])){
			polygon->skeleton[edgeId]->start = up;
			polygon->skeleton[edgeId]->end = down;
			polygon->auxVertices.push_back(up);
			polygon->auxVertices.push_back(down);
			if(up == NULL){printf("Warning, looking to add a NULL pointer for skeleton %d to the up in type 3 edge! Should not happen, stopping here...\n", edgeId);exit(0);}
			if(down == NULL){printf("Warning, looking to add a NULL pointer for skeleton %d to the down in type 3 edge! Should not happen, stopping here...\n", edgeId);exit(0);}
		}else{
			polygon->skeleton[edgeId]->start = left;
			polygon->skeleton[edgeId]->end = right;
			polygon->auxVertices.push_back(left);
			polygon->auxVertices.push_back(right);
			if(right == NULL){printf("Warning, looking to add a NULL pointer for skeleton %d to the right in type 3 edge! Should not happen, stopping here...\n", edgeId);exit(0);}
			if(left == NULL){printf("Warning, looking to add a NULL pointer for skeleton %d to the left in type 3 edge! Should not happen, stopping here...\n", edgeId);exit(0);}
		}

	}
	// If type 2, expand only the concave one (should be j by default)
	if(polygon->skeleton[edgeId]->type == 2){
		if(isVertical(polygon->vertices[i],polygon->vertices[j])){
			if(polygon->skeleton[edgeId]->start->y < polygon->skeleton[edgeId]->end->y){
				if(up == NULL){printf("Warning, looking to add a NULL pointer for skeleton %d (%d, %d) to the up in type 2 edge! Should not happen, stopping here...\n", edgeId, polygon->skeleton[edgeId]->start->id, polygon->skeleton[edgeId]->end->id);exit(0);}
				polygon->skeleton[edgeId]->end = up;
				polygon->auxVertices.push_back(up);
				delete down;
			}else{
				if(down == NULL){printf("Warning, looking to add a NULL pointer for skeleton %d (%d, %d) to the down in type 2 edge! Should not happen, stopping here...\n", edgeId, polygon->skeleton[edgeId]->start->id, polygon->skeleton[edgeId]->end->id);exit(0);}
				delete up;
				polygon->skeleton[edgeId]->end = down;
				polygon->auxVertices.push_back(down);
			}
		}else{
			if(polygon->skeleton[edgeId]->start->x < polygon->skeleton[edgeId]->end->x){
				delete left;
				if(right == NULL){printf("Warning, looking to add a NULL pointer for skeleton %d (%d, %d) to the right in type 2 edge! Should not happen, stopping here...\n", edgeId, polygon->skeleton[edgeId]->start->id, polygon->skeleton[edgeId]->end->id);exit(0);}
				polygon->skeleton[edgeId]->end = right;
				polygon->auxVertices.push_back(right);
			}else{
				delete right;
				if(left == NULL){printf("Warning, looking to add a NULL pointer for skeleton %d (%d, %d) to the left in type 2 edge! Should not happen, stopping here...\n", edgeId, polygon->skeleton[edgeId]->start->id, polygon->skeleton[edgeId]->end->id);exit(0);}
				polygon->skeleton[edgeId]->end = left;
				polygon->auxVertices.push_back(left);
			}
		}
	}
}
// Need a slighlty different function, previous logic doesn't really apply
void extend_aux_edge(Polygon* polygon, int edgeId){
	int n = polygon->vertices.size();
	// Check which side we are extending on
	bool right = false, left = false, up = false, down = false;
	if(isVertical(polygon->skeleton[edgeId]->start, polygon->skeleton[edgeId]->end)){
		if(polygon->skeleton[edgeId]->start->y < polygon->skeleton[edgeId]->end->y){
			up = true;
		}else{
			down = true;
		}
	}else{
		if(polygon->skeleton[edgeId]->start->x < polygon->skeleton[edgeId]->end->x){
			right = true;
		}else{
			left = true;
		}
	}
	
	Point* extended = NULL;
	Point* intersection = NULL;
	for(int k = 0; k < n; k++){
		// Find intersection
		intersection = lineLineIntersection(polygon->skeleton[edgeId]->start, polygon->skeleton[edgeId]->end, polygon->vertices[k], polygon->vertices[(k+1) % n]);
		// If id of intersection is -1, means lines are parallel, skip
		if(intersection->id == -2){
			delete intersection;
			continue;
		}
		// Check if this is on the line, account for vertical lines
		if(isVertical(polygon->vertices[k],polygon->vertices[(k+1)%n])){
			if(intersection->y < min(polygon->vertices[k]->y,polygon->vertices[(k+1)%n]->y) ||
				intersection->y > max(polygon->vertices[k]->y,polygon->vertices[(k+1)%n]->y) ){
				delete intersection;				
				continue;
			}
		}else{
			if(intersection->x < min(polygon->vertices[k]->x,polygon->vertices[(k+1)%n]->x) ||
				intersection->x > max(polygon->vertices[k]->x,polygon->vertices[(k+1)%n]->x) ){
				delete intersection;				
				continue;
			}
		}
		
		// It is, check if better choice
		if(right && intersection->x > polygon->skeleton[edgeId]->end->x && 
			abs(intersection->x - polygon->skeleton[edgeId]->end->x) > 0.00000000000001 && 
			(extended == NULL || intersection->x < extended->x)){
			delete extended;
			extended = intersection;
		}
		else if(left && intersection->x < polygon->skeleton[edgeId]->end->x &&
			abs(intersection->x - polygon->skeleton[edgeId]->end->x) > 0.00000000000001 && 
			(extended == NULL || intersection->x > extended->x)){
			delete extended;			
			extended = intersection;
		}
		else if(up && intersection->y > polygon->skeleton[edgeId]->end->y &&
			abs(intersection->y - polygon->skeleton[edgeId]->end->y) > 0.00000000000001 && 
			(extended == NULL || intersection->y < extended->y)){
			delete extended;		
			extended = intersection;
		}
		else if(down && intersection->y < polygon->skeleton[edgeId]->end->y &&
			abs(intersection->y - polygon->skeleton[edgeId]->end->y) > 0.00000000000001 && 
			(extended == NULL || intersection->y > extended->y)){
			delete extended;	
			extended = intersection;
		}else{
			delete intersection;
		}
	}
	if(extended == NULL){printf("Warning, looking to add a NULL pointer for skeleton %d in type 4 edge! Should not happen, stopping here...\n", edgeId); exit(0);}
				
	polygon->skeleton[edgeId]->end = extended;
	polygon->auxVertices.push_back(extended);

}

void add_aux_edge(Polygon* polygon, Point* start, Point* end, int side){
	// Cycle through vertices, see if this is an concave vertex than can be seen and has the right rotation
	int n = polygon->vertices.size();
	for(int i = 0; i < n; i++){
		// printf("%d ", i);
		// Only care if concave
		if(polygon->convex[i]){continue;}
		// Only are if counter-clockwise turn
		// Also should check boundary is to the right of line
		if(orientation(start, end, polygon->vertices[i]) != 2 ||
			orientation(end, polygon->vertices[i], polygon->vertices[(i + 1) % n]) != 2 ||
			orientation(end, polygon->vertices[i], polygon->vertices[(i - 1 + n) % n]) != 2 ||
			orientation(end, polygon->vertices[i], polygon->vertices[(i + 1) % n]) != side ||
			orientation(end, polygon->vertices[i], polygon->vertices[(i - 1 + n) % n]) != side){
			continue;
		}
		// printf("\n%d orientation %d\n", i+1, orientation(start, end, polygon->vertices[i]));
		// if(i == 28){
		// 	double val = (end->y - start->y) * (polygon->vertices[i]->x - end->x) - 
  //             				(end->x - start->x) * (polygon->vertices[i]->y - end->y); 
  //           printf("Got val %.20f\n", val);
		// }
		// if(orientation(end, polygon->vertices[i], polygon->vertices[(i + 1) % n]) != side ||
		// 	orientation(end, polygon->vertices[i], polygon->vertices[(i - 1 + n) % n]) != side){
		// 	continue;
		// }
		// if(orientation(start, end, polygon->vertices[i]) != 
		// 	orientation(end, polygon->vertices[i], polygon->vertices[(i + 1) % n]) || 
		// 	orientation(end, polygon->vertices[i], polygon->vertices[(i + 1) % n]) !=
		// 	orientation(end, polygon->vertices[i], polygon->vertices[(i - 1 + n) % n])){
		// 	continue;
		// }
		
		// If still here, need to see if can see the point.
		bool ok = true;
		for(int j = 0; j < n; j++){
			// Skip two lines (as intersect due to same points)
			if((i == 0 && j == n-1) ||
				i - j  == 1 ||
				i == j){
				continue;
			}

			// if(i == 28 && j == 100){printf("Checking %d against %d %d, orientations %d %d %d %d\n", i+1, j+1, j+2, orientation(end, polygon->vertices[i], polygon->vertices[j]),
			// 																				orientation(end, polygon->vertices[i], polygon->vertices[(j + 1) % n]),
			// 																				orientation(end, polygon->vertices[j], polygon->vertices[(j + 1) % n]),
			// 																				orientation(polygon->vertices[i], polygon->vertices[j], polygon->vertices[(j + 1) % n]));}

			if(doIntersect(end, polygon->vertices[i], polygon->vertices[j], polygon->vertices[(j + 1) % n])){
				// printf("%d orientation %d, %d intersects\n", i+1, orientation(end, polygon->vertices[j], polygon->vertices[(j + 1) % n]), j+1);
				// Check if this is line that end point is on (Simply should ignore)
				// Have to also make sure that edge is not simply going outside the polygon
				if(orientation(end, polygon->vertices[j], polygon->vertices[(j + 1) % n]) == 0 &&
					orientation(start, end, polygon->vertices[j]) == orientation(polygon->vertices[i], end, polygon->vertices[j]) &&
					orientation(start, end, polygon->vertices[(j + 1) % n]) == orientation(polygon->vertices[i], end, polygon->vertices[(j + 1) % n])){
					// printf("%d orientation %d, %d intersects, it's ok as colinear with %d %d\n", i+1, orientation(end, polygon->vertices[j], polygon->vertices[(j + 1) % n]), j+1, j+1, j+2);
					// double val = (end->y - polygon->vertices[(j + 1) % n]->y) * (polygon->vertices[j]->x - end->x) - 
     //          						(end->x - polygon->vertices[(j + 1) % n]->x) * (polygon->vertices[j]->y - end->y); 
     //          		printf("Colinear val is (%d) %.20f\n", abs(val) < 0.00001, val);
					continue;
				}
				// Otherwise this is a plain old intersection, discard this vertgex
				ok = false;
				break;
			}
		}
		if(ok){
			// printf("Looking at vertex %d\n", i + 1);
			// printf("Angle is %.20f, orientation is %d\n", angle(start, end, polygon->vertices[i]), orientation(start, end, polygon->vertices[i]));
			// Have a new edge, add it and extend it. Also need to call on the same procedure!
			// printf("New aux edge from (%.20f, %.20f) - (%d)(%.20f, %.20f)\n", end->x, end->y, polygon->vertices[i]->id + 1, polygon->vertices[i]->x, polygon->vertices[i]->y);
			polygon->skeleton.push_back(new SkeletonLine{int(polygon->skeleton.size()),end,polygon->vertices[i],4, false});
			// printf("Extend\n");
			extend_aux_edge(polygon, int(polygon->skeleton.size()) - 1);
			// printf("Extended to aux edge from (%.20f, %.20f) - (%.20f, %.20f)\n", polygon->skeleton.back()->start->x,
			// 																	polygon->skeleton.back()->start->y,
			// 																	polygon->skeleton.back()->end->x,
			// 																	polygon->skeleton.back()->end->y);
			// And keep going!
			// printf("Add aux\n");
			add_aux_edge(polygon, polygon->skeleton.back()->start, polygon->skeleton.back()->end, side);

		}
	}
}

void find_skeleton_edges(Polygon* polygon, bool heur){
	// First thing is to just check if can create all the edges that should be present
	int n = polygon->vertices.size();
	for(int i = 0; i < n; i++){
		for(int j = i + 1; j < n; j++){
			// printf("%d (%d) %d (%d)\n", i+1, polygon->convex[i], j+1, polygon->convex[j]);
			// Type 1: both convex
			if(polygon->convex[i] && polygon->convex[j]){
				// If can see, add to list. Cannot extend, so that is all
				if(adjacent(n,i,j) || can_see(polygon,i,j)){
					int newId = int(polygon->skeleton.size());
					polygon->skeleton.push_back(new SkeletonLine{newId,polygon->vertices[i],polygon->vertices[j],1, false});
					// Gotta add this as a STP edge for later
					polygon->steiner_edge.push_back(make_pair(polygon->convex_id[i], newId + polygon->num_convex));
					polygon->steiner_edge.push_back(make_pair(polygon->convex_id[j], newId + polygon->num_convex));
				}
			}
			// Type 2: one convex, one concave
			else if(polygon->convex[i] || polygon->convex[j]){
				// If can't see each other, stop
				if(!adjacent(n,i,j) && !can_see(polygon,i,j)){continue;}
				int concave, convex;
				if(polygon->convex[i]){
					convex = i;
					concave = j;
				}else{
					convex = j;
					concave = i;
				}
				// If not adjacent (i.e. can see), check if can extend
				// Note doing not adjacent because much cheaper check
				// If can't, stop
				if(!adjacent(n,i,j) && 
					orientation(polygon->vertices[convex], polygon->vertices[concave], polygon->vertices[(concave-1+n) % n]) !=
					orientation(polygon->vertices[convex], polygon->vertices[concave], polygon->vertices[(concave+1) % n])){
					continue;
				}
				// Time to add then extend
				int newId = int(polygon->skeleton.size());
				polygon->skeleton.push_back(new SkeletonLine{newId,polygon->vertices[convex],polygon->vertices[concave], 2, false});
				extend_skeleton_edge(polygon, polygon->skeleton.size()-1, i, j);
				polygon->steiner_edge.push_back(make_pair(polygon->convex_id[convex], newId + polygon->num_convex));
				// Add possible aux edge
				// printf("Trying from (%.1f,%.1f) to (%.1f,%.1f)\n", polygon->vertices[convex]->x, polygon->vertices[convex]->y,
				// 													polygon->vertices[concave]->x, polygon->vertices[concave]->y);
				// Need orientation of turn
				int side;
				if(!adjacent(n,i,j) || convex != (concave-1+n) % n){
					side = orientation(polygon->vertices[convex], polygon->vertices[concave], polygon->vertices[(concave-1+n) % n]);
				}else{
					side = orientation(polygon->vertices[convex], polygon->vertices[concave], polygon->vertices[(concave+1) % n]);
				}
				if(!heur){add_aux_edge(polygon, polygon->skeleton.back()->start, polygon->skeleton.back()->end, side);}
			}
			// Type 3: Two concave on separate sides
			else{
				// printf("Type 3\n");
				// Have two concave. Want them on separate sides.
				// If adjacent, this can't be the case
				if(adjacent(n,i,j)){continue;}
				// Check they can see each other
				if(!can_see(polygon,i,j)){continue;}
				// Check whether orientation is the same for all, if so we are good
				// So if different stop
				if(orientation(polygon->vertices[i], polygon->vertices[j], polygon->vertices[(j+1) % n]) != 
					orientation(polygon->vertices[i], polygon->vertices[j], polygon->vertices[j-1]) ||
					orientation(polygon->vertices[i], polygon->vertices[j], polygon->vertices[j-1]) != 
					orientation(polygon->vertices[j], polygon->vertices[i], polygon->vertices[i+1]) ||
					orientation(polygon->vertices[j], polygon->vertices[i], polygon->vertices[i+1]) != 
					orientation(polygon->vertices[j], polygon->vertices[i], polygon->vertices[(i-1+n)%n])){

					continue;
				}
				// printf("Continue\n");
				// Extend both sides, and add!
				polygon->skeleton.push_back(new SkeletonLine{int(polygon->skeleton.size()),polygon->vertices[i],polygon->vertices[j],3, false});
				extend_skeleton_edge(polygon, polygon->skeleton.size()-1, i, j);
				int currId = polygon->skeleton.size() - 1;
				if(!heur){
					// printf("Add aux edge in direction of %d\n", polygon->skeleton[currId]->end->id + 1);
					add_aux_edge(polygon, polygon->skeleton[currId]->start, polygon->skeleton[currId]->end,
						orientation(polygon->vertices[i], polygon->vertices[j], polygon->vertices[j-1]));
					// printf("Add aux edge in direction of %d\n", polygon->skeleton[currId]->start->id + 1);
					add_aux_edge(polygon, polygon->skeleton[currId]->end, polygon->skeleton[currId]->start,
						orientation(polygon->vertices[j], polygon->vertices[i], polygon->vertices[i+1]));
				}
			}

		}
	}
}	



void prune_skeleton_edges(Polygon* polygon){
	// Basically cycle through each pair of skeleton edges,
	// see if one of them intersects everything the other one 
	// intersects
	int n = polygon->skeleton.size();
	for(int i = 0; i < n; i++){
		// printf("%d(%d)\n", i, n);
		for(int j = 0; j < n; j++){
			// printf("\n%d %d (%d)", i, j, n);
			if(i == j){continue;}
			// Check if i can be pruned due to j
			// If j has been pruned, skip this
			// printf(" - Check 1\n");
			if(polygon->skeleton[j]->pruned){continue;}
			// Some types allow us to skip
			// printf(" - Check 2\n");
			if(polygon->skeleton[i]->type == 1){break;}
			// If this is type 2, this edge must also start or end on the convex vertex
			// printf(" - Check 3\n");
			if(polygon->skeleton[i]->type == 2){
				// printf(" - Check 3b\n");
				// printf("%d\n", polygon->skeleton[j]->type);
				// printf(" %.2f \n", polygon->skeleton[i]->start->x);
				// printf(" %.2f \n", polygon->skeleton[i]->start->y);
				// printf(" %.2f \n", polygon->skeleton[i]->end->x);
				// printf(" %.2f \n", polygon->skeleton[i]->end->y);
				// printf(" %.2f \n", polygon->skeleton[j]->start->x);
				// printf(" %.2f \n", polygon->skeleton[j]->start->y);
				// printf(" %.2f \n", polygon->skeleton[j]->end->x);
				// printf(" %.2f \n", polygon->skeleton[j]->end->y);
				
				if((abs(polygon->skeleton[i]->start->x - polygon->skeleton[j]->start->x) > 0.00000000000001 ||
					abs(polygon->skeleton[i]->start->y - polygon->skeleton[j]->start->y) > 0.00000000000001) &&
					(abs(polygon->skeleton[i]->start->x - polygon->skeleton[j]->end->x) > 0.00000000000001 ||
					abs(polygon->skeleton[i]->start->y - polygon->skeleton[j]->end->y) > 0.00000000000001)){
					// printf(" - do not prune!");
					continue;
				}
			}
			// printf(" - finish");
			// Nothing left but to check against every other edge
			// Keep looking until find an edge that intersect with i but not with j
			// If do not find such an edge, can prune i
			bool prune = true;
			for(int k = 0; k < n; k++){
				// printf("%d %d %d\n", i, j, k);
				if(doIntersect(polygon->skeleton[i]->start, polygon->skeleton[i]->end,
					polygon->skeleton[k]->start, polygon->skeleton[k]->end) &&
					!doIntersect(polygon->skeleton[j]->start, polygon->skeleton[j]->end,
					polygon->skeleton[k]->start, polygon->skeleton[k]->end)){

					prune = false;
					break;
				}
			}
			if(prune){
				polygon->skeleton[i]->pruned = true;
				break;
			}

		}
	}
}

void stp_intersections(Polygon* polygon){
	int v = polygon->num_convex;
	int n = polygon->skeleton.size();
	for(int i = 0; i < n; i++){
		for(int j = i + 1; j < n; j++){
			// printf("%d %d\n", i, j);
			if(doIntersect(polygon->skeleton[i]->start, polygon->skeleton[i]->end,
							polygon->skeleton[j]->start, polygon->skeleton[j]->end)){
				polygon->steiner_edge.push_back(make_pair(v + i,v + j));
			}
		}
	}
}






// Big function which does most of the work
// n is size of polygon
// seed is seed for random generator
// create_polygon whether to create or read polygon
// print_info whether to print times 
void create_skeleton(Polygon* polygon, bool heur, bool print_info, string polygonName, string outputFilename){
	clock_t t, start;
	ofstream file;
	t = clock();
	start = t;

	// if(create_polygon){
	// 	file.open(filename, ios::out | ios::app);
	// 	file << to_string(n) << " " << to_string(seed);
	// 	if(heur){
	// 		file << " TRUE";
	// 	}else{
	// 		file << " FALSE";
	// 	}
	// 	name = "random_n" + to_string(n) + "_s" + to_string(seed);
	// 	polygon = new Polygon(n);
	// 	generate_polygon(polygon, n, 1000000.0, 1000000.0, true, seed);
	// 	t = clock() - t;
	// 	if(print_info){printf("Created size %d instance:    %.5fs\n",n,((float)t)/CLOCKS_PER_SEC);}
	// 	file << " " << to_string(((float)t)/CLOCKS_PER_SEC);
	// }else{
	// 	polygon = new Polygon();
	// 	string input = "data/input_polygon/" + filename + "_polygon.txt";
	// 	read_in_polygon(polygon, input);
	// 	generate_polygon(polygon, int(polygon->vertices.size()), 1000000.0, 1000000.0, false, seed);
	// 	name = filename;
	// 	t = clock() - t;
	// 	file << " FALSE " << to_string(((float)t)/CLOCKS_PER_SEC);
	// }
	t = clock();
	find_convex_points(polygon);
	t = clock() - t;
	if(print_info){printf("Determined convex points:    %.5fs\n",((float)t)/CLOCKS_PER_SEC);}
	if(outputFilename != ""){
		file.open(outputFilename, ios_base::app);
		file << " " << to_string(((float)t)/CLOCKS_PER_SEC);
		file.close();
	}

	t = clock();
	find_skeleton_edges(polygon, heur);
	t = clock() - t;
	if(print_info){printf("Found skeleton edges:        %.5fs\n",((float)t)/CLOCKS_PER_SEC);}
	if(outputFilename != ""){
		file.open(outputFilename, ios_base::app);
		file << " " << to_string(((float)t)/CLOCKS_PER_SEC);
		file.close();
	}

	t = clock();
	prune_skeleton_edges(polygon);
	t = clock() - t;
	if(print_info){printf("Prunned polygon:             %.5fs\n",((float)t)/CLOCKS_PER_SEC);}
	if(outputFilename != ""){
		file.open(outputFilename, ios_base::app);
		file << " " << to_string(((float)t)/CLOCKS_PER_SEC);
		file.close();
	}

	t = clock();
	stp_intersections(polygon);
	t = clock() - t;
	if(print_info){printf("Found STP intersections:     %.5fs\n",((float)t)/CLOCKS_PER_SEC);}
	if(outputFilename != ""){
		file.open(outputFilename, ios_base::app);
		file << " " << to_string(((float)t)/CLOCKS_PER_SEC);
		file.close();
	}

	t = clock();
	// polygon_plotter(name, polygon, heur);
	write_polygon_edges(polygon, polygonName, heur);
	t = clock() - t;
	if(print_info){printf("Polygon info to text file:   %.5fs\n",((float)t)/CLOCKS_PER_SEC);}
	if(outputFilename != ""){
		file.open(outputFilename, ios_base::app);
		file << " " << to_string(((float)t)/CLOCKS_PER_SEC);
		file.close();
	}

	t = clock();
	if(heur){
		stp_plotter(polygonName, polygon, 0);
		stp_plotter(polygonName, polygon, 1);
		stp_plotter(polygonName, polygon, 2);
	}else{
		stp_plotter(polygonName, polygon, 3);
		stp_plotter(polygonName, polygon, 4);
	}

	t = clock() - t;
	if(print_info){printf("Polygon info to STP file:    %.5fs\n",((float)t)/CLOCKS_PER_SEC);}
	if(outputFilename != ""){
		file.open(outputFilename, ios_base::app);
		file << " " << to_string(((float)t)/CLOCKS_PER_SEC);
		file.close();
	}

	t = clock() - start;
	if(print_info){printf("Total execution time:        %.5fs\n\n",((float)t)/CLOCKS_PER_SEC);}
	if(outputFilename != ""){
		file.open(outputFilename, ios_base::app);
		file << " " << to_string(((float)t)/CLOCKS_PER_SEC) << "\n";
		file.close();
	}
	// Deal with memory
	// delete polygon;

}

int main(int argc, char** argv){
	clock_t t = clock();
	int start, end, interval, seed_min, seed_max, seed_per_run;
	bool create_polygon, heur, print_info = false;
	Polygon* polygon;
	// Redoing this a bit, start by making sure there is at least one input to specify what is going to happen
	if(argc == 1){
		printf("Wrong usage of exec file, note need to call ./skeleton followed by one of the following:\n");
		printf("		-setup: This creates a text file where each line is a specification of settings to run the executable on\n");
		printf("		-graphCreation: This creates a text file for each graph that does not yet exist\n");
		printf("		-runReadin: This reads in a specific graph from a file and solves it\n");
		printf("		-runLine: This reads a run specification from a file and runs that specification\n\n");
		return 0;
	}
	string specification = argv[1];
	// Now deal with each option
	if(specification == "setup"){
		if(argc != 10){
			printf("Wrong usage of exec file with mode setup!\n");
			printf("To use, please call ./skeleton setup setupName n_start n_end n_int seed_min seed_max seed_per_run generationMode\n");
			printf("Where:\n");
			printf("       -setupName: string, name of setup script file being created\n");
			printf("       -n_start: int, min size of polygon instances\n");
			printf("       -n_end: int, max size of polygon instances\n");
			printf("       -n_int: int, interval of polygon instances size\n");
			printf("       -seed_min: int, min seed for random polygons analysed for each size\n");
			printf("       -seed_max: int, max seed for random polygons analysed for each size\n");
			printf("       -seed_per_run: int, number of seeds per cluster specification run\n");
			printf("       -generationMode: string, type of generation of the instances, either with a 'normal' distribution or a 'uniform' distribution\n");
			printf("Example: cpp_code/skeleton setup setupExample 10 80 10 1 50 5 uniform\n\n");
			return 0;
		}
		string scriptName(argv[2]);
		start = stoi(argv[3]);
		end = stoi(argv[4]);
		interval = stoi(argv[5]);
		seed_min = stoi(argv[6]);
		seed_max = stoi(argv[7]);
		seed_per_run = stoi(argv[8]);
		string generationMode = string(argv[9]);
		string setupFile = "data/runScripts/" + scriptName + "-runScript.txt";
		ofstream outputFile;
		outputFile.open(setupFile);
		if(!outputFile.is_open()){printf("Error: Could not open output file %s to output experiment results! Stopping now...\n", setupFile.c_str()); return 0;}
		outputFile << "size seeds generationMode heur\n";
		outputFile.close();
		int jobs = 0;
		for(int n = start; n <= end; n += interval){
			for(int s = seed_min; s <= seed_max; s += seed_per_run){
				outputFile.open(setupFile, ios_base::app);
				outputFile << to_string(n) << " " << to_string(s) << "-" << to_string(min(s + seed_per_run - 1, seed_max)) << " " << generationMode << " TRUE\n";
				outputFile << to_string(n) << " " << to_string(s) << "-" << to_string(min(s + seed_per_run - 1, seed_max)) << " " << generationMode << " FALSE\n";
				jobs += 2;
				outputFile.close();
			}
		}
		printf("Successfully setup runScript for job %s with %d jobs!\n", scriptName.c_str(), jobs);

	

	}else if(specification == "graphCreation"){
		if(argc != 9){
			printf("Wrong usage of exec file with mode graphCreation!\n");
			printf("To use, please call ./skeleton graphCreation n_start n_end n_int seed_min seed_max generationMode print_info\n");
			printf("Where:\n");
			printf("       -n_start: int, min size of polygon instances\n");
			printf("       -n_end: int, max size of polygon instances\n");
			printf("       -n_int: int, interval of polygon instances size\n");
			printf("       -seed_min: int, min seed for random polygons analysed for each size\n");
			printf("       -seed_max: int, max seed for random polygons analysed for each size\n");
			printf("       -generationMode: type of generation of the instances, either with a 'normal' distribution or a 'uniform' distribution\n");
			printf("       -print_info: true/false, whether to show details of execution\n");
			printf("Example: cpp_code/skeleton graphCreation 10 80 10 1 50 uniform false\n");
			return 0;
		}
		// Read in inputs
		start = stoi(argv[2]);
		end = stoi(argv[3]);
		interval = stoi(argv[4]);
		seed_min = stoi(argv[5]);
		seed_max = stoi(argv[6]);
		string generationMode(argv[7]);
		if(generationMode != "normal" && generationMode != "uniform"){
			printf("Please either use 'normal' or 'uniform' for 8th input!\n");
			return 0;
		}
		string s1(argv[8]);
		if(s1.compare("true") == 0){
			print_info = true;
		}else if(s1.compare("false") == 0){
			print_info = false;
		}else{
			printf("Please either use true or false for 9th input!\n");
			return 0;
		}
		// Now do the work, which for this mode simply is creation of graphs and storage
		for(int n = start; n <= end; n += interval){
			for(int s = seed_min; s <= seed_max; s++){
				if(print_info){printf("Working on graph creation, size %d and seed %d\n", n, s);}
				polygon = new Polygon(n);
				create_and_store_polygon(polygon, n, s, generationMode, 1000000);
				delete polygon;
			}
		}
	
	


	}else if(specification == "runReadin"){
		if(argc != 4){
			printf("Wrong usage of exec file with mode runReadin!\n");
			printf("To use, please call ./skeleton runReadin polygonName print_info\n");
			printf("Where:\n");
			printf("       -polygonName: name of polygon, filename with specifications of input polygon should be named polygonName_polygon.txt\n");
			printf("       -print_info: true/false, whether to show details of execution\n");
			printf("Remember to put the file in data/R_polygon in this case\n");
			printf("Example: cpp_code/skeleton runReadin claw true \n\n");
		}
		// Work goes here
		string s1(argv[3]);
		if(s1.compare("true") == 0){
			print_info = true;
		}else if(s1.compare("false") == 0){
			print_info = false;
		}else{
			printf("Please either use true or false for 4th input!\n");
			return 0;
		}
		string polygonName(argv[2]);
		polygon = new Polygon();
		read_and_store_polygon(polygon, polygonName);
		create_skeleton(polygon, false, print_info, polygonName, "");
		delete polygon;
		// Repeat
		polygon = new Polygon();
		read_and_store_polygon(polygon, polygonName);
		create_skeleton(polygon, true, print_info, polygonName, "");
		delete polygon;

		// Run steiner solver on each of the files
		system(("./steiner_solver/staynerd data/STP_input/" + polygonName + "_STP_format_1-2.txt 3600 1 data/STP_output/" + polygonName + "_output_STP_format_1-2.txt").c_str());
		system(("./steiner_solver/staynerd data/STP_input/" + polygonName + "_STP_format_1-2-3.txt 3600 1 data/STP_output/" + polygonName + "_output_STP_format_1-2-3.txt").c_str()); 
		system(("./steiner_solver/staynerd data/STP_input/" + polygonName + "_STP_format_1-2-3.txt 3600 1 data/STP_output/" + polygonName + "_output_STP_format_1-2-3_pruned.txt").c_str()); 
		system(("./steiner_solver/staynerd data/STP_input/" + polygonName + "_STP_format_all.txt 3600 1 data/STP_output/" + polygonName + "_output_STP_format_all.txt").c_str()); 
		system(("./steiner_solver/staynerd data/STP_input/" + polygonName + "_STP_format_pruned.txt 3600 1 data/STP_output/" + polygonName + "_output_STP_format_pruned.txt").c_str()); 


	}else if(specification == "runLine"){
		if(argc != 5){
			printf("Wrong usage of exec file with mode runLine!\n");
			printf("To use, please call ./skeleton runLine runName lineNumber print_info\n");
			printf("Where:\n");
			printf("       -runName: string, name of the run. A file data/runScripts/runName-runScript.txt should exist\n");
			printf("       -lineNumber: int, line number in the file specifying the run\n");
			printf("       -print_info: true/false, whether to show details of execution\n");
			printf("Example: cpp_code/skeleton runLine setupExample 1 true \n\n");
		}
		// Work goes here
		string runName(argv[2]);
		int runLine = stoi(argv[3]);
		string s1(argv[4]);
		if(s1.compare("true") == 0){
			print_info = true;
		}else if(s1.compare("false") == 0){
			print_info = false;
		}else{
			printf("Please either use true or false for 2nd input!\n");
			return 0;
		}

		if(runLine == 0){printf("Error: Specified first line of file as inputs to run, but this is the header line! Stopping now...\n"); return 0;}
		string inputFilename = "data/runScripts/" + runName + "-runScript.txt";
		ifstream inputFile(inputFilename, ios::in);
		if(!inputFile.is_open()){printf("Error: Could not open input file %s to read experiment specifications! Stopping now...\n", inputFilename.c_str()); return 0;}
		string line;
		int lineNum = 0;
		bool found = false;
		while(getline(inputFile, line)){
			if(lineNum == runLine){found = true; break;}
			lineNum++;
		}
		// Check that for the right line insted of being finished
		if(!found){printf("Error: Did not get to the desired array job, specified job %d but file only has %d specifications! Stopping now...\n", runLine, lineNum-1); return 0;}
		inputFile.close();
		// Since found instructions, create file where output will be written
		string outputFilename = "data/clusterResults/" + runName + "_arrayJob" + to_string(runLine) + ".txt";
		ofstream outputFile(outputFilename);
		if(!outputFile.is_open()){printf("Error: Could not open output file %s to output experiment results! Stopping now...\n", outputFilename.c_str()); return 0;}
		outputFile << "n seed instance_type heur find_convex find_edges prune_edges STP_intersection polygon_file STP_file total\n";
		outputFile.close();

		// Now have to extract the data
		stringstream ss(line);
		string token;
		// Size
		getline(ss, token, ' ');
		string size = token;
		// Seed values
		getline(ss, token, ' ');
		stringstream ss2(token);
		getline(ss2, token, '-');
		seed_min = stoi(token);
		getline(ss2, token, '-');
		seed_max = stoi(token);
		// Instance type
		getline(ss, token, ' ');
		string generationMode(token);
		if(generationMode != "normal" && generationMode != "uniform"){
			printf("Please either use 'normal' or 'uniform' for 8th input!\n");
			return 0;
		}
		// Heuristic
		getline(ss, token, ' ');
		string val(token);
		if(token == "FALSE"){heur = false;}
		else if(token == "TRUE"){heur = true;}
		else{printf("Did not understand whether to do heuristic or not! Stopping now...\n"); return 0;}
		if(print_info){printf("Got specified size %d with seeds %d-%d, instance type %s and heuristic %d\n", stoi(size), seed_min, seed_max, generationMode.c_str(), heur);}
		for(int seed = seed_min; seed <= seed_max; seed++){
			if(print_info){printf("Running size %d with seed %d, instance type %s and heuristic %d\n", stoi(size), seed, generationMode.c_str(), heur);}
			// So now want to find skeleton edges 
			polygon = new Polygon();
			string polygonName = generationMode + "_n" + size + "_s" + to_string(seed);
			string polygonFilePath = "data/polygon/" + polygonName + "_polygon.txt";
			read_polygon(polygon, polygonFilePath);

			if(print_info){printf("Read polygon in, start finding edges\n");}
			// Do begining of output to file
			outputFile.open(outputFilename, ios_base::app);
			outputFile << size << " " << to_string(seed) << " " << generationMode << " " << val;
			outputFile.close(); 
			create_skeleton(polygon, heur, print_info, polygonName, outputFilename);
			delete polygon;

			// Now to run steiner solver!
			if(heur){
				system(("./steiner_solver/staynerd data/STP_input/" + polygonName + "_STP_format_1-2.txt 3600 1 data/STP_output/" + polygonName + "_output_STP_format_1-2.txt").c_str());
				system(("./steiner_solver/staynerd data/STP_input/" + polygonName + "_STP_format_1-2-3.txt 3600 1 data/STP_output/" + polygonName + "_output_STP_format_1-2-3.txt").c_str()); 
				system(("./steiner_solver/staynerd data/STP_input/" + polygonName + "_STP_format_1-2-3.txt 3600 1 data/STP_output/" + polygonName + "_output_STP_format_1-2-3_pruned.txt").c_str()); 
			}else{
				system(("./steiner_solver/staynerd data/STP_input/" + polygonName + "_STP_format_all.txt 3600 1 data/STP_output/" + polygonName + "_output_STP_format_all.txt").c_str()); 
				system(("./steiner_solver/staynerd data/STP_input/" + polygonName + "_STP_format_pruned.txt 3600 1 data/STP_output/" + polygonName + "_output_STP_format_pruned.txt").c_str()); 
			}	
		}
	
	}else{
		printf("Wrong usage of exec file, note need to call ./skeleton followed by one of the following:\n");
		printf("		-setup: This creates a text file where each line is a specification of settings to run the executable on\n");
		printf("		-graphCreation: This creates a text file for each graph that does not yet exist\n");
		printf("		-runAll: This is the standard (i.e. non cluster) way of running the solver, specify all problems and all will be solved by the end of the run\n");
		printf("		-runReadin: This reads in a specific graph from a file and solves it\n");
		printf("		-runLine: This reads a run specification from a file and runs that specification\n\n");
		return 0;
	}
	t = clock() - t;
	if(print_info){printf("======== Total time: %.5fs =======\n",((float)t)/CLOCKS_PER_SEC);}
	return 0;
}
	
	
	
// }else if(specification == "runAll"){
	// 	if(argc != 9){
	// 		printf("Wrong usage of exec file with mode runAll!\n");
	// 		printf("To use, please call ./skeleton runAll n_start n_end n_int seed_min seed_max heur print_info\n");
	// 		printf("Where:\n");
	// 		printf("       -n_start: int, min size of polygon instances\n");
	// 		printf("       -n_end: int, max size of polygon instances\n");
	// 		printf("       -n_int: int, interval of polygon instances size\n");
	// 		printf("       -seed_min: int, min seed for random polygons analysed for each size\n");
	// 		printf("       -seed_max: int, max seed for random polygons analysed for each size\n");
	// 		printf("       -heur: true/false, whether to run heuristic approach (i.e. ignore type 4 edges).\n");
	// 		printf("                          Will always run full approach regardless.\n");
	// 		printf("       -print_info: true/false, whether to show details of execution\n");
	// 		printf("Example: ./skeleton runAll 10 80 10 50 true true\n");
	// 	}
	// 	// Work goes here
	// 	
	// 	start = stoi(argv[2]);
	// 	end = stoi(argv[3]);
	// 	interval = stoi(argv[4]);
	// 	seed_min = stoi(argv[5]);
	// 	seed_max = stoi(argv[6]);
	// 	string s1(argv[8]);
	// 	if(s1.compare("true") == 0){
	// 		print_info = true;
	// 	}else if(s1.compare("false") == 0){
	// 		print_info = false;
	// 	}else{
	// 		printf("Please either use true or false for 9th input!\n");
	// 		return 0;
	// 	}

	// 	string s2(argv[7]);
	// 	if(s2.compare("true") == 0){
	// 		heur = true;
	// 	}else if(s2.compare("false") == 0){
	// 		heur = false;
	// 	}else{
	// 		printf("Please either use true or false for 8th input!\n");
	// 		return 0;
	// 	}
	// 	// So far so good, create file which will log the exec times
	// 	string exec_times_filename = "data/exec_times/run_n" + to_string(start) + "<-" + to_string(interval) + "->" + to_string(end) + 
	// 									"_s" + to_string(seed_min) + "<->" + to_string(seed_max) + ".txt";
	// 	ofstream file;
	// 	file.open(exec_times_filename);
	// 	// file.open(exec_times_filename + ".txt");
	// 	if(!file.is_open()){
	// 		printf("Couldn't open exec_times_filename write to file\n");
	// 		return 0;
	// 	}
	// 	file << "n seed heur create find_convex find_edges prune_edges STP_intersection polygon_file STP_file total\n";
	// 	file.close();		
	// 	for(int n = start; n <= end; n += interval){
	// 		for(int seed = seed_min; seed <= seed_max; seed++){
	// 			if(print_info){printf("\nn = %d seed = %d\n", n, seed);}
	// 			create_skeleton(n,seed, false, print_info, true, exec_times_filename);
	// 			if(heur){
	// 				if(print_info){printf("\nn = %d seed = %d (heuristic)\n", n, seed);}
	// 				create_skeleton(n, seed, true, print_info, true, exec_times_filename);
	// 			}

	// 		}
	// 	}



	// if(argc != 8 && argc != 3){
	// 	printf("Wrong usage of exec file!\n");
	// 	printf("To use, please call ./skeleton n_start n_end n_int seed_min seed_max heur print_info\n");
	// 	printf("Where:\n");
	// 	printf("       -n_start: int, min size of polygon instances\n");
	// 	printf("       -n_end: int, max size of polygon instances\n");
	// 	printf("       -n_int: int, interval of polygon instances size\n");
	// 	printf("       -seed_min: int, min seed for random polygons analysed for each size\n");
	// 	printf("       -seed_max: int, max seed for random polygons analysed for each size\n");
	// 	printf("       -heur: true/false, whether to run heuristic approach (i.e. ignore type 4 edges).\n");
	// 	printf("                          Will always run full approach regardless.\n");
	// 	printf("       -print_info: true/false, whether to show details of execution\n");
	// 	printf("Example: ./skeleton 10 80 10 50 true true\n");
	// 	printf("\n\nAlternatively, you can simply specify a scenario name and whether to print info:\n");
	// 	printf("./skeleton filename print_info\n");
	// 	printf("Remember to put the file in data/R_polygon in this case\n\n");
	// 	return 0;
	// }

	// // Assume inputs are correct, I am not that good. Read inputs
	// string filename;
	// clock_t t = clock();
	// bool print_info;
	// if(argc == 8){
	// 	int start, end, interval, seed_min, seed_max;
	// 	bool create_polygon, heur;
	// 	start = stoi(argv[1]);
	// 	end = stoi(argv[2]);
	// 	interval = stoi(argv[3]);
	// 	seed_min = stoi(argv[4]);
	// 	seed_max = stoi(argv[5]);
	// 	string s1(argv[7]);
	// 	if(s1.compare("true") == 0){
	// 		print_info = true;
	// 	}else if(s1.compare("false") == 0){
	// 		print_info = false;
	// 	}else{
	// 		printf("Please either use true or false for 5th input!\n");
	// 		return 0;
	// 	}

	// 	string s2(argv[6]);
	// 	if(s2.compare("true") == 0){
	// 		heur = true;
	// 	}else if(s2.compare("false") == 0){
	// 		heur = false;
	// 	}else{
	// 		printf("Please either use true or false for 6th input!\n");
	// 		return 0;
	// 	}
	// 	// So far so good, create file which will log the exec times
	// 	string exec_times_filename = "data/exec_times/run_n" + to_string(start) + "<-" + to_string(interval) + "->" + to_string(end) + 
	// 									"_s" + to_string(seed_min) + "<->" + to_string(seed_max) + ".txt";
	// 	ofstream file;
	// 	file.open(exec_times_filename);
	// 	// file.open(exec_times_filename + ".txt");
	// 	if(!file.is_open()){
	// 		printf("Couldn't open exec_times_filename write to file\n");
	// 		return 0;
	// 	}
	// 	file << "n seed heur create find_convex find_edges prune_edges STP_intersection polygon_file STP_file total\n";
	// 	file.close();		
	// 	for(int n = start; n <= end; n += interval){
	// 		for(int seed = seed_min; seed <= seed_max; seed++){
	// 			if(print_info){printf("\nn = %d seed = %d\n", n, seed);}
	// 			create_skeleton(n,seed, false, print_info, true, exec_times_filename);
	// 			if(heur){
	// 				if(print_info){printf("\nn = %d seed = %d (heuristic)\n", n, seed);}
	// 				create_skeleton(n,seed, true, print_info, true, exec_times_filename);
	// 			}

	// 		}
	// 	}
		
	// }

	// if(argc == 3){
	// 	string s1(argv[2]);
	// 	if(s1.compare("true") == 0){
	// 		print_info = true;
	// 	}else if(s1.compare("false") == 0){
	// 		print_info = false;
	// 	}else{
	// 		printf("Please either use true or false for 2nd input!\n");
	// 		return 0;
	// 	}
	// 	string filename(argv[1]);
	// 	create_skeleton(0,0, false, print_info,false,filename);
	// 	create_skeleton(0,0, true, print_info,false,filename);
	// }




















// int read_in_polygon(Polygon* polygon, string filename){
// 	FILE * fp;
// 	char buffer[1001];
// 	char buffer2[1001];
// 	char *cstr = new char[filename.length() + 1];
// 	strcpy(cstr, filename.c_str());
// 	fp = fopen(cstr, "r");
// 	if(!fp){
//         printf("Could not open file.  Giving up.\n");
//         return 1;
//     }
//     fgets(buffer, 1000, fp);
// 	fgets(buffer, 1000, fp);
// 	double x, y;
//     int id = 0;
// 	while(feof(fp)==0){
// 		sscanf(buffer, "%lf %lf %s",&x,&y, buffer2);
// 		polygon->vertices.push_back(new Point{id,x,y});
// 		id++;
// 		polygon->convex.push_back(false);
// 		fgets(buffer, 1000, fp);
// 	}
// 	delete [] cstr;
// 	return 0;
// }













// Outputs polygon information to text files
// void polygon_plotter(string filename, Polygon* polygon, bool heur){
// 	ofstream file;
// 	// Start with polygon file
// 	file.open("data/polygon/" + filename + "_polygon.txt");
// 	if(!file.is_open()){
// 		printf("Couldn't open polygon write to file\n");
// 	}
// 	file << "id x y convex\n";
// 	for(int i = 0; i < polygon->vertices.size(); i++){
// 		file << (i + 1) << " " << polygon->vertices[i]->x << " " << polygon->vertices[i]->y;
// 		if(polygon->convex[i] == 1){
// 		 	file << " TRUE\n";
// 		}else{
// 			file << " FALSE\n";
// 		}
// 	}
// 	file.close();
// 	// Now do the edge file
// 	// Not if heurisitc (don't want two files), should always call heuristic
// 	if(heur){
// 		file.open("data/polygon/" + filename + "_edges_heur.txt");
// 	}else{
// 		file.open("data/polygon/" + filename + "_edges.txt");
// 	}
// 	if(!file.is_open()){
// 		printf("Couldn't open edges write to file\n");
// 	}
// 	file << "id x y xend yend type intersect_prune\n";
// 	for(int i = 0; i < polygon->skeleton.size(); i++){
// 		file << i + 1 << " " << polygon->skeleton[i]->start->x << " " << polygon->skeleton[i]->start->y << " " << polygon->skeleton[i]->end->x << " " << polygon->skeleton[i]->end->y;
// 		file << " \"Type " << polygon->skeleton[i]->type << "\" ";
// 		if(polygon->skeleton[i]->pruned){
// 			file << "TRUE\n";
// 		}else{
// 			file << "FALSE\n";
// 		}
// 	}
// 	file.close();
// }


// Generate polygon obstacle
// Create n random points
// Uncross randomly chosen edges until no crossings
// void generate_polygon(Polygon* polygon, int n, double X, double Y, bool rand, int seed){
// 	mt19937 mt_rand(seed);
// 	std::normal_distribution<double> randPosition(0, 1000000);
// 	// vector<Point> polygon(n);
// 	if(rand){
// 		for(int i = 0; i < n && rand; i++){
// 			// point =  new Point{i,
// 			// 		X * (double) mt_rand() / RAND_MAX,
// 			// 		Y * (double) mt_rand() / RAND_MAX};
// 					// static_cast <float> (rand() % 10),
// 					// static_cast <float> (rand() % 10)};
// 			polygon->vertices[i] = new Point{i,
// 					X * (double) mt_rand() / RAND_MAX,
// 					Y * (double) mt_rand() / RAND_MAX};
// 			// polygon->vertices[i] = new Point{i,
// 			// 		randPosition(mt_rand),
// 			// 		randPosition(mt_rand)};
// 		}
// 	}
// 	// printf("Here\n");
// 	// for(int i = 0; i < n; i++){
// 	// 	printf("(%.2f,%.2f)\n",polygon->vertices[i]->x,polygon->vertices[i]->y);
// 	// }
// 	n = polygon->vertices.size();
// 	// Uncross polygon i.e. find edges that intersect, and switch them
// 	bool crossings = true;
// 	while(crossings){
// 		crossings = false;
// 		// Pick random edges to start comparing from
// 		int firstStart = mt_rand() % n;
// 		int secondStart = mt_rand() % n;
// 		for(int i = 0; i < n && !crossings; i++){
// 			int i_index = (firstStart + i) % n;
// 			for(int j = 0; j < n && !crossings; j++){
// 				int j_index = (secondStart + j) % n;
// 				// If same or next to each other skip
// 				if(i_index == j_index || 
// 					min(i_index,j_index)+1 == max(i_index,j_index) ||
// 					(max(i_index,j_index) == n - 1 && min(i_index,j_index) == 0)){continue;}
// 				// if(doIntersect(&polygon[i_index], &polygon[(i_index + 1) % n], &polygon[j_index], &polygon[(j_index + 1) % n])){
// 				if(doIntersect(polygon->vertices[i_index], polygon->vertices[(i_index + 1) % n],
// 					polygon->vertices[j_index], polygon->vertices[(j_index + 1) % n])){
// 					// printf("Crossed %d (%.2f,%.2f) %d (%.2f,%.2f) and %d (%.2f,%.2f) %d (%.2f,%.2f)\n",
// 					// 	i_index, polygon->vertices[i_index]->x,polygon->vertices[i_index]->y,
// 					// 	(i_index + 1) % n, polygon->vertices[(i_index + 1) % n]->x,polygon->vertices[(i_index + 1) % n]->y,
// 					// 	j_index, polygon->vertices[j_index]->x,polygon->vertices[j_index]->y, 
// 					// 	(j_index + 1) % n, polygon->vertices[(j_index + 1) % n]->x,polygon->vertices[(j_index + 1) % n]->y);
// 					// printf("orientations: %d %d %d %d\n",orientation(polygon->vertices[i_index], polygon->vertices[(i_index + 1) % n], polygon->vertices[j_index]),
// 					// 										orientation(polygon->vertices[i_index], polygon->vertices[(i_index + 1) % n], polygon->vertices[(j_index + 1) % n]),
// 					// 										orientation(polygon->vertices[j_index], polygon->vertices[(j_index + 1) % n], polygon->vertices[i_index]),
// 					// 										orientation(polygon->vertices[j_index], polygon->vertices[(j_index + 1) % n], polygon->vertices[(i_index + 1) % n]));
// 					// cin.get();
// 					crossings = true;
// 					fix_crossing(polygon, min(i_index, j_index), max(i_index, j_index));
// 				}
// 			}
// 		}
// 	}
// }




