#include "line_geometry.h"

#ifndef LINE_GEOMETRY_CPP
#define LINE_GEOMETRY_CPP

void swapLines(Line* line1, Line* line2){
    Line* temp = line1;
    line1->start = line2->start;
    line1->end = line2->end;
    line1->id = line2->id;
    line2->start = temp->start;
    line2->end = temp->end;
    line2->id = temp->id;
    return;
}

bool onSegment(Point* p, Point* q, Point* r){ 
    if (q->x <= max(p->x, r->x) && q->x >= min(p->x, r->x) && 
        q->y <= max(p->y, r->y) && q->y >= min(p->y, r->y)) 
       return true; 
    return false; 
}

double angle(Point* p, Point* q, Point* r){
    // First check points are different, if not say the angle is 0
    if((abs(p->x - q->x) < 0.00000000000001 && abs(p->y - q->y) < 0.00000000000001) ||
        (abs(p->x - r->x) < 0.00000000000001 && abs(p->y - r->y) < 0.00000000000001) ||
        (abs(r->x - q->x) < 0.00000000000001 && abs(r->y - q->y) < 0.00000000000001)){
        // printf("Colinear by default as same value!\n");
        return 0;
    }
    double inside = ((p->x - q->x)*(r->x - q->x) + (p->y - q->y)*(r->y - q->y))/(sqrt(pow(p->x - q->x, 2) + pow(p->y - q->y, 2))*sqrt(pow(r->x - q->x, 2) + pow(r->y - q->y, 2)));
    // if(abs(inside - 1) < 0.0000000000000000000001 || inside > 1){
    if(abs(inside - 1) < 0.000000000000001){
        // printf("Values are %.20f, %.20f, %.20f, %.20f\n", ((p->x - q->x)*(r->x - q->x) + (p->y - q->y)*(r->y - q->y)),
        //                                             sqrt(pow(p->x - q->x, 2) + pow(p->y - q->y, 2)),
        //                                             sqrt(pow(r->x - q->x, 2) + pow(r->y - q->y, 2)),
        //                                             inside);
        return 0;
    }
    if(abs(inside + 1) < 0.000000000000001 || inside < -1){return M_PI;}

    double val = acos(((p->x - q->x)*(r->x - q->x) + (p->y - q->y)*(r->y - q->y))/(sqrt(pow(p->x - q->x, 2) + pow(p->y - q->y, 2))*sqrt(pow(r->x - q->x, 2) + pow(r->y - q->y, 2))));
    if(isnan(val)){printf("Got na when calculating angle! Checked (%.2f, %.2f), (%.2f, %.2f), (%.2f, %.2f), got %.2f (top %.2f, bottom 1 %.2f, bottom 2 %.2f, inside %.40f\n", p->x, p->y, q->x, q->y, r->x, r->y, val,
                                                    ((p->x - q->x)*(r->x - q->x) + (p->y - q->y)*(r->y - q->y)),
                                                    sqrt(pow(p->x - q->x, 2) + pow(p->y - q->y, 2)),
                                                    sqrt(pow(r->x - q->x, 2) + pow(r->y - q->y, 2)),
                                                    inside);}
    return val;
}

int orientation(Point* p, Point* q, Point* r){
    // First check points are different, if not say the angle is 0
    if((abs(p->x - q->x) < 0.00000000000001 && abs(p->y - q->y) < 0.00000000000001) ||
        (abs(p->x - r->x) < 0.00000000000001 && abs(p->y - r->y) < 0.00000000000001) ||
        (abs(r->x - q->x) < 0.00000000000001 && abs(r->y - q->y) < 0.00000000000001)){
        // printf("Colinear by default as same value!\n");
        return 0;
    }
    // Next check colinear by looking at angle, only care if colinear so no need to actually calculate it
    double inside = ((p->x - q->x)*(r->x - q->x) + (p->y - q->y)*(r->y - q->y))/(sqrt(pow(p->x - q->x, 2) + pow(p->y - q->y, 2))*sqrt(pow(r->x - q->x, 2) + pow(r->y - q->y, 2)));
    if(abs(inside - 1) < 0.000000000000001){return 0;}
    if(abs(inside + 1) < 0.000000000000001){return 0;}
    // Now use this formula, should never get colinear (as it should have been reported above)
    // See https://www.geeksforgeeks.org/orientation-3-ordered-points/ 
    // for details of below formula. 
    double val = (q->y - p->y) * (r->x - q->x) - 
              (q->x - p->x) * (r->y - q->y); 
    if(abs(val) < 0.00000000000001){
        printf("Weird, got colinear but angle seems to contradict this! For points (%.2f, %.2f), (%.2f, %.2f), (%.2f, %.2f), got val %.20f, angle %.20f and inside %.20f.\n", p->x, p->y, q->x, q->y, r->x, r->y, val, angle(p, q, r), inside);
        return 0;
    }  
    // colinear
  
    return (val > 0)? 1: 2; // clock or counterclock wise 
}
bool doIntersect(Line* line1, Line* line2){
    Point* p1 = line1->start;
    Point* q1 = line1->end;
    Point* p2 = line2->start;
    Point* q2 = line2->end;
    return doIntersect(p1,q1,p2,q2);
}

bool doIntersect(Point* p1, Point* q1, Point* p2, Point* q2){
     
    // Find the four orientations needed for general and 
    // special cases 
    int o1 = orientation(p1, q1, p2); 
    int o2 = orientation(p1, q1, q2); 
    int o3 = orientation(p2, q2, p1); 
    int o4 = orientation(p2, q2, q1); 
  
    // General case 
    if (o1 != o2 && o3 != o4) 
        return true; 
    // Special Cases 
    // p1, q1 and p2 are colinear and p2 lies on segment p1q1 
    if (o1 == 0 && onSegment(p1, p2, q1)) return true; 
  
    // p1, q1 and q2 are colinear and q2 lies on segment p1q1 
    if (o2 == 0 && onSegment(p1, q2, q1)) return true; 
  
    // p2, q2 and p1 are colinear and p1 lies on segment p2q2 
    if (o3 == 0 && onSegment(p2, p1, q2)) return true; 
  
     // p2, q2 and q1 are colinear and q1 lies on segment p2q2 
    if (o4 == 0 && onSegment(p2, q1, q2)) return true; 
  
    return false; // Doesn't fall in any of the above cases 
}  


Point* lineLineIntersection(Line* line1, Line* line2){
    // Line AB represented as a1x + b1y = c1 
    double a1 = line1->end->y - line1->start->y;
    double b1 = line1->start->x - line1->end->x; 
    double c1 = a1*(line1->start->x) + b1*(line1->start->y); 
  
    // Line CD represented as a2x + b2y = c2 
    double a2 = line2->end->y - line2->start->y; 
    double b2 = line2->start->x - line2->end->x; 
    double c2 = a2*(line2->start->x)+ b2*(line2->start->y); 
  
    double determinant = a1*b2 - a2*b1; 
  
    if (determinant == 0){ 
        // The lines are parallel. This is simplified 
        // by returning a pair of FLT_MAX 
        // Point intersection = {-1, FLT_MAX, FLT_MAX};
        return new Point {-2, FLT_MAX, FLT_MAX};
    } 
    else
    { 
        double x = (b2*c1 - b1*c2)/determinant; 
        double y = (a1*c2 - a2*c1)/determinant;
        // Point intersection = {-1, x, y}; 
        return new Point {-1, x, y}; 
    } 
}

Point* lineLineIntersection(Point* p1, Point* q1, Point* p2, Point* q2){
    double a1 = q1->y - p1->y;
    double b1 = p1->x - q1->x; 
    double c1 = a1*(p1->x) + b1*(p1->y); 
  
    // Line CD represented as a2x + b2y = c2 
    double a2 = q2->y - p2->y; 
    double b2 = p2->x - q2->x; 
    double c2 = a2*(p2->x)+ b2*(p2->y); 
  
    double determinant = a1*b2 - a2*b1; 
  
    if (abs(determinant) < 0.00000000000001){ 
        // The lines are parallel. This is simplified 
        // by returning a pair of FLT_MAX 
        // Point intersection = {-1, FLT_MAX, FLT_MAX};
        return new Point {-2, FLT_MAX, FLT_MAX};
    } 
    else
    { 
        double x = (b2*c1 - b1*c2)/determinant; 
        double y = (a1*c2 - a2*c1)/determinant;
        // Point intersection = {-1, x, y}; 
        return new Point {-1, x, y}; 
    } 

}

bool isVertical(Point* p, Point* q){
    return abs(p->x - q->x) < 0.00000000000001;
}

// int main(){
//     Line* line1 = new Line {10,new Point{-1, 10, 2.5}, new Point{-1, 0, 2.5}};
//     Line* line2 = new Line {10,new Point{-1, 0, 2.499}, new Point{-1, 0, 0}};
//     printf("%d\n",doIntersect(line1, line2));
// }

#endif