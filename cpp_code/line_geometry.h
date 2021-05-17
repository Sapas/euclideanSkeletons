// Functions used to check whether lines intersect, taken from
// https://www.geeksforgeeks.org/check-if-two-given-line-segments-intersect/
#ifndef LINE_GEOMETRY_H
#define LINE_GEOMETRY_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <float.h>
#include "line_geometry.h"
using namespace std;

struct Point {
	int id;
	double x, y;

	bool operator==(const Point& otherPoint) const{
		return abs(otherPoint.x - x) < 0.001 && abs(otherPoint.y - y) < 0.001;
	}

	bool operator<(const Point& otherPoint) const{
		if(abs(otherPoint.y - y) < 0.001){
			if(abs(otherPoint.x - x) < 0.001){
				return false;
			}
			return x < otherPoint.x;
		}
		return y < otherPoint.y;
	}
};

struct Line {
	int id;
	Point *start;
	Point *end;
};

// Swap info stored at two lines
void swapLines(Line* line1, Line* line2);

// Given three colinear points p, q, r, the function checks if 
// point q lies on line segment 'pr' 
bool onSegment(Point* p, Point* q, Point* r);
  
// To find orientation of ordered triplet (p, q, r). 
// The function returns following values 
// 0 --> p, q and r are colinear 
// 1 --> Clockwise 
// 2 --> Counterclockwise
// See https://www.geeksforgeeks.org/orientation-3-ordered-points/ 
// for details of below formula.  
int orientation(Point* p, Point* q, Point* r);
     
// Just calls function below after extracting points
bool doIntersect(Line* line1, Line* line2);
// The main function that returns true if line segment 'p1q1' 
// and 'p2q2' intersect. 
bool doIntersect(Point* p1, Point* q1, Point* p2, Point* q2);

Point* lineLineIntersection(Line* line1, Line* line2);

// Intersection between lines p1-q1 and p2-q2
Point* lineLineIntersection(Point* p1, Point* q1, Point* p2, Point* q3);

// Check function which checks if a line is vertical or not
bool isVertical(Point* p, Point* q);

#endif