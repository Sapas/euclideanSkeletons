#ifndef FIND_INTERSECTIONS_CPP
#define FIND_INTERSECTIONS_CPP

// Alright, algorithms to find all the lines that intersect.
// Taken pseudocode from http://courses.csail.mit.edu/6.854/17/Scribe/s25-sweepline.html
// Will do sweep top to bottom
#include <stdio.h>
#include <queue>
#include <map>
#include <math.h>
#include <time.h>
#include <algorithm>
#include "sweep_binary_search_tree.cpp"
#include "line_geometry.cpp"
using namespace std;

// Event struct, records events for the sweep
struct Event{
	char type;	// Options: 'a' activaton
			 	// 		    'd' deactivation
				//			'i' intersection
	Point* eventPoint; 	// Where the event happens
	int line1_id;    // Id of line being activated/deactivated or one of intersection lines
	int line2_id;	// Id of other line (in case of intersection)
	bool operator==(const Event& otherEvent) const{
		if(line1_id != otherEvent.line1_id){
			return false;
		}
		if(type == otherEvent.type){
			if(type != 'i' && line1_id == otherEvent.line1_id){
				return true;
			}
			if(type == 'i' && line1_id == otherEvent.line1_id && line2_id == otherEvent.line2_id){
				return true;
			}
		}
		return false;
	}

	bool operator<(const Event& otherEvent) const{

		if(*eventPoint == *(otherEvent.eventPoint)){
			// Compare by event type, first activation, then intersection, the deactivation
			if(type == 'a'){return false;}
			else if(otherEvent.type == 'a'){return true;}
			else if(type == 'i'){return false;}
			else if(type == 'i' && otherEvent.type == 'i'){
				// // Order by ids
				// if(line1_id == otherEvent.line1_id && line2_id == otherEvent.line2_id){
				// 	return false;
				// }
				// if(min(line1_id,lin2_id) < min(otherEvent.line1_id,otherEvent.lin2_id) &&
				// 	)
				return true;
			}
			return true;
		}
		return *eventPoint < *(otherEvent.eventPoint);
		// if(abs(eventPoint->y - otherEvent.eventPoint->y) < 0.001){
		// 	// Happen at roughly the same time, order by x coordinate
		// 	if(abs(eventPoint->x - otherEvent.eventPoint->x) < 0.001){
		// 		// NEED TO WORK OUT WHAT HAPPENS HERE
		// 		return true;
		// 	}else{
		// 		return eventPoint->x < otherEvent.eventPoint->x;
		// 	}
		// 	// // Check not same event
		// 	// if(line1_id == otherEvent.line1_id && line2_id == otherEvent.line2_id){
		// 	// 	return true;
		// 	// }
		// 	// // Not same time, order by importance of event
		// 	// if(type == 'a'){
		// 	// 	return false;
		// 	// }else if(otherEvent.type == 'a'){
		// 	// 	return true;
		// 	// }else if(type == 'i'){
		// 	// 	return false;
		// 	// }else if(otherEvent.type == 'i'){
		// 	// 	return true;
		// 	// }
		// 	// // Same importance, same place, different events (wow).
		// 	// // Order by line id
		// 	// if(line1_id < otherEvent.line1_id ||
		// 	// 	 (line1_id == otherEvent.line1_id && line2_id < otherEvent.line2_id)){
		// 	// 	return true;
		// 	// }
		// 	// return false;
		// }

		// return  eventPoint->y < otherEvent.eventPoint->y;
	}
};

std::vector<pair<int,int>> find_intersections_brute_force(std::vector<Line> lines){
	std::vector<pair<int,int>> intersects;
	for(int i = 0; i < lines.size(); i++){
		for(int j = i + 1; j < lines.size(); j++){
			if(doIntersect(&lines[i],&lines[j])){
				intersects.push_back(make_pair(i,j));
			}
		}
	}
	sort(intersects.begin(), intersects.end());
	return intersects;
}

std::vector<pair<int,int>> find_intersections(std::vector<Line> lines){
	std::vector<pair<int,int>> intersects;
	Line* newLine;
	Line* oldLine;
	Line* leftNeighbour;
	Line* rightNeighbour;
	double new_y;
	std::priority_queue <Event> q;
	SweepBST active_lines;
	// Add all activation and deacivation events to queue
	for(int i = 0; i < lines.size(); i++){
		q.push({'a', lines[i].start, lines[i].id, -1});
		q.push({'d', lines[i].end, lines[i].id, -1});
	}
	// Go through events
	while(!q.empty()){
		// New segment is active
		// Add to active sweep lines
		// Check if it intersects with other lines, if so add 
		// intersection to event
		if(q.top().type == 'a'){
			// printf("\nActivated line %d at point (%.2f,%.2f)\n", q.top().line1_id, q.top().eventPoint->x, q.top().eventPoint->y);
			newLine = &lines[q.top().line1_id];
			active_lines.insert(newLine, *(q.top().eventPoint));
			// Check left and right neighbours
			// printf("Look for left neighbour\n");
			// printf("Look left of %d\n",newLine->id);
			leftNeighbour = active_lines.left_neighbour(newLine, *(q.top().eventPoint));
			// printf("Look right of %d\n",newLine->id);
			rightNeighbour = active_lines.right_neighbour(newLine, *(q.top().eventPoint));
			// if(leftNeighbour){printf("%d - ",leftNeighbour->id);}
			// else{printf("NULL - ");}
			// printf("%d",newLine->id);
			// if(rightNeighbour){printf(" - %d\n",rightNeighbour->id);}
			// else{printf(" - NULL\n");}
			if(leftNeighbour && doIntersect(leftNeighbour, newLine)){
				Point* intersection = lineLineIntersection(leftNeighbour, newLine);
				// printf("1Check (%.2f,%.2f) - (%.2f,%.2f) %d\n", intersection->x, intersection->y, q.top().eventPoint->x, q.top().eventPoint->y,intersection < q.top().eventPoint);
				// printf("1Check (%.2f,%.2f) - (%.2f,%.2f) %d\n", q.top().eventPoint->x, q.top().eventPoint->y, intersection->x, intersection->y, q.top().eventPoint < intersection);
				if(*intersection < *(q.top().eventPoint) || *intersection == *(q.top().eventPoint)){
					// printf("YES\n");
					q.push({'i', intersection, leftNeighbour->id, newLine->id});
					// printf("1 - Added i %d %d at %f\n",leftNeighbour->id,newLine->id,new_y);
				}
			}
			if(rightNeighbour && doIntersect(newLine, rightNeighbour)){
				Point* intersection = lineLineIntersection(newLine, rightNeighbour);
				// printf("2Check (%.2f,%.2f) - (%.2f,%.2f) %d\n", intersection->x, intersection->y, q.top().eventPoint->x, q.top().eventPoint->y,*intersection < *(q.top().eventPoint));
				// printf("2Check (%.2f,%.2f) - (%.2f,%.2f) %d\n", q.top().eventPoint->x, q.top().eventPoint->y, intersection->x, intersection->y, *(q.top().eventPoint) < *intersection);
				if(*intersection < *(q.top().eventPoint) || *intersection == *(q.top().eventPoint)){
					// printf("YES\n");
					q.push({'i', intersection, newLine->id, rightNeighbour->id});
					// printf("2 - Added i %d %d at %f\n",newLine->id,rightNeighbour->id,new_y);
				}
			}
			q.pop();
		}
		// Segment becomes inactive
		// Need to check if neighbours intersect
		else if(q.top().type == 'd'){
			oldLine = &lines[q.top().line1_id];
			// printf("\nDeactivated line %d at point (%.2f,%.2f)\n", q.top().line1_id, q.top().eventPoint->x, q.top().eventPoint->y);
			// printf("Look left of %d\n",oldLine->id);
			leftNeighbour = active_lines.left_neighbour(oldLine, *(q.top().eventPoint));
			// printf("Look right of %d\n",oldLine->id);
			rightNeighbour = active_lines.right_neighbour(oldLine, *(q.top().eventPoint));
			if(leftNeighbour && rightNeighbour &&
				doIntersect(leftNeighbour, rightNeighbour)){
				Point* intersection = lineLineIntersection(leftNeighbour, rightNeighbour);
				// printf("3Check (%.2f,%.2f) - (%.2f,%.2f) %d\n", intersection->x, intersection->y, q.top().eventPoint->x, q.top().eventPoint->y,intersection < q.top().eventPoint);
				// printf("3Check (%.2f,%.2f) - (%.2f,%.2f) %d\n", q.top().eventPoint->x, q.top().eventPoint->y, intersection->x, intersection->y, q.top().eventPoint < intersection);
				if(*intersection < *(q.top().eventPoint) || *intersection == *(q.top().eventPoint)){
					// printf("YES\n");
					q.push({'i', intersection, leftNeighbour->id, rightNeighbour->id});
					// printf("3 - Added i %d %d at %f\n",leftNeighbour->id,rightNeighbour->id,new_y);
				}
			}
			active_lines.remove(oldLine, *(q.top().eventPoint));
			q.pop();
		}
		// Intersection of two lines
		// Need to swap their place in the BST
		// Find their left and right neighbours and check if they intersect
		// with new neighbours
		else if(q.top().type == 'i'){
			Line* leftLine = &lines[q.top().line1_id];
			Line* rightLine = &lines[q.top().line2_id];
			leftNeighbour = active_lines.left_neighbour(leftLine, *(q.top().eventPoint));
			rightNeighbour = active_lines.right_neighbour(rightLine, *(q.top().eventPoint));
			// printf("\nIntersection %d and %d at point (%.2f,%.2f)\n", leftLine->id, rightLine->id, q.top().eventPoint->x, q.top().eventPoint->y);
			// if(leftNeighbour == NULL){printf("NULL - ");}
			// else{printf("%d - ", leftNeighbour->id);}
			// printf("%d <-> %d", leftLine->id, rightLine->id);
			// if(rightNeighbour == NULL){printf(" - NULL\n");}
			// else{printf(" - %d\n", rightNeighbour->id);}
			if(leftNeighbour && doIntersect(leftNeighbour, rightLine)){
				Point* intersection = lineLineIntersection(leftNeighbour, rightLine);
				if(*intersection < *(q.top().eventPoint) || *intersection == *(q.top().eventPoint)){
					// printf("Added intersection %d - %d\n", leftNeighbour->id, rightLine->id);
					q.push({'i', intersection, leftNeighbour->id, rightLine->id});
				}
			}
			if(rightNeighbour && doIntersect(rightNeighbour, leftLine)){
				Point* intersection = lineLineIntersection(rightNeighbour, leftLine);
				if(*intersection < *(q.top().eventPoint) || *intersection == *(q.top().eventPoint)){
					// printf("Added intersection %d - %d\n", leftLine->id, rightNeighbour->id);
					q.push({'i', intersection, leftLine->id, rightNeighbour->id});
				}
			}
			active_lines.swap_lines(leftLine, rightLine, *(q.top().eventPoint));
			if(leftLine->id < rightLine->id){
				intersects.push_back(make_pair(leftLine->id,rightLine->id));
			}else{
				intersects.push_back(make_pair(rightLine->id,leftLine->id));
			}
			// Remove duplicates
			do{
				q.pop();
				// printf("pop\n");
			}while(q.top().type == 'i' && q.top().line1_id == leftLine->id && q.top().line2_id == rightLine->id);
			
		}

		// active_lines.display();
	}
	sort(intersects.begin(), intersects.end());
	return intersects;
}


// int main(){


// 	// // // INITIALISE MEMORY SO NOT WASTEFULL

// 	// // find_intersections(lines);
// 	// // Create n lines, see if the two agree
// 	// // srand(static_cast <unsigned> (time(0)));
// 	clock_t t;
// 	srand(0);
	
// 	std::vector<Line> lines;
// 	for(int i = 0; i < 3000; i++){
// 		// Point* point1 = new Point{-1,
// 		// 	10.0 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
// 		// 	10.0 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX)};
// 		// Point* point2 = new Point{-1,
// 		// 	10.0 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX),
// 		// 	10.0 * static_cast <float> (rand()) / static_cast <float> (RAND_MAX)};
// 		// Determine start and end based on y pos
// 		// Point* point1;
// 		// Point* point2;
// 		Point* start;
// 		Point* end;
// 		bool flag;
// 		// do{
// 			Point* point1 = new Point{-1,
// 				static_cast <float> (rand() % 100),
// 				static_cast <float> (rand() % 100)};
// 			Point* point2 = new Point{-1,
// 				static_cast <float> (rand() % 100),
// 				static_cast <float> (rand() % 100)};
// 			if((*point2) < (*point1)){
// 				start = point1;
// 				end = point2;
// 			}else{
// 				start = point2;
// 				end = point1;
// 			}
// 		// 	// Check this point does not exist already
// 		// 	flag = false;
// 		// 	for(int i = 0; i < lines.size(); i++){
// 		// 		if((abs(lines[i].start->x - start->x) < 0.01 &&
// 		// 			abs(lines[i].start->y - start->y) < 0.01) ||
// 		// 			(abs(lines[i].end->x - end->x) < 0.01 &&
// 		// 			abs(lines[i].end->y - end->y) < 0.01) || 
// 		// 			(abs(lines[i].start->x - end->x) < 0.01 &&
// 		// 			abs(lines[i].start->y - end->y) < 0.01) ||
// 		// 			(abs(lines[i].end->x - start->x) < 0.01 &&
// 		// 			abs(lines[i].end->y - start->y) < 0.01)){
// 		// 			flag = true;
// 		// 			break;
// 		// 		}
// 		// 	}
// 		// }while(flag);
// 		lines.push_back({i,start,end});
// 	}

// 	// lines.clear();
// 	// lines.push_back({0,new Point {-1, 0, 2}, new Point {-1, 2, 0}});
// 	// lines.push_back({1,new Point {-1, 1, 2}, new Point {-1, 1, 0}});
// 	// lines.push_back({2, new Point {-1, 2, 2},new Point {-1, 0, 0}});
	
// 	// Horizontal Line
// 	// lines.push_back({10,new Point{-1, 10, 2.5}, new Point{-1, 0, 2.5}});

// 	// lines.push_back({11,new Point{-1, 4, 7}, new Point{-1, 0, 3}});
// 	// lines.push_back({11,new Point{-1, 4, 7}, new Point{-1, 0, 7}});
// 	t = clock();
// 	std::vector<pair<int,int>> intersections = find_intersections_brute_force(lines);
// 	t = clock() - t;
// 	printf("Brute force: %.20f\n",((float)t)/CLOCKS_PER_SEC);
// 	for(int i = 0; i < intersections.size(); i++){
// 		printf("%d - %d\n",intersections[i].first,intersections[i].second);
// 	}
// 	// t = clock();
// 	// std::vector<pair<int,int>> intersections2 = find_intersections(lines);
// 	// t = clock() - t;
// 	// printf("Sweep: %.20f\n",((float)t)/CLOCKS_PER_SEC);
// 	// for(int i = 0; i < intersections.size(); i++){
// 	// 	printf("%d - %d\n",intersections2[i].first,intersections2[i].second);
// 	// }

// 	// for(int i = 0; i < lines.size(); i++){
// 	// 	printf("%d: (%.2f,%.2f) - (%.2f,%.2f)\n",i,lines[i].start->x,lines[i].start->y,lines[i].end->x,lines[i].end->y);
// 	// }
// 	// bool flag = true;
// 	// for(int i = 0; i < intersections.size(); i++){
// 	// 	if(intersections[i].first != intersections2[i].first ||
// 	// 		intersections[i].second != intersections2[i].second){
// 	// 		flag = false;
// 	// 		printf("======== Different!===========\n");
// 	// 		printf("%d - %d vs %d - %d\n",intersections[i].first, intersections[i].second,
// 	// 										intersections2[i].first, intersections2[i].second);
// 	// 		break;
// 	// 	}
// 	// }
// 	// if(flag){
// 	// 	printf("======== Same!===========\n");
// 	// }
// 	return 0;
// }

#endif