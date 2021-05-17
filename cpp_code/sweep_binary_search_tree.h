#ifndef SWEEP_BINARY_SEARCH_TREE_H
#define SWEEP_BINARY_SEARCH_TREE_H

#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include "line_geometry.h"
using namespace std;

// Special implementation of a binary search tree 
// Code adapted mainly from (with other additions)
// https://gist.github.com/harish-r/a7df7ce576dda35c9660
class SweepBST{
    struct Node {
        Line* line;
        Node* left;
        Node* right;
    };
    
    Node* root;
    // Clear up memory from tree
    Node* makeEmpty(Node* t);
    // Find smallest node (uses tree geomtry)
    Node* findMin(Node* t);
	// Find largets node (uses tree geomtry)
    Node* findMax(Node* t);
    // Finds node containing node
    Node* find(Node* t, Line* line, Point sweepPoint);
	// Inserts line, sweep specifies y coordinate of sweep line,
	// used to compare segment's positions
    Node* insert(Line* newLine, Node* t, Point sweepPoint);
    // Remove given line
    Node* remove(Line* oldLine, Node* t, Point sweepPoint);
    // Prints all entries in tree in order
    void inorder(Node* t, int order);
    // Find smaller neighbour, procedure taken and adapted from
    // https://codereview.stackexchange.com/questions/204105/find-largest-smaller-key-in-binary-search-tree
    Node* find_largest_smaller(Node* root, Line* line, Point sweepPoint);
    // Find larger neighbour, adapted from previous link
    Node* find_smallest_larger(Node* root, Line* line, Point sweepPoint);
    // Compares two lines based on state of sweep line
    // 0 = same
    // to the left = -1
    // to the right = 1
    int compare(Line* line, Point sweepPoint);

public:
    SweepBST();

    ~SweepBST();

    void insert(Line* line, Point sweepPoint);

    void remove(Line* line, Point sweepPoint);

    void display();

    Line* left_neighbour(Line* line, Point sweepPoint);

    Line* right_neighbour(Line* line, Point sweepPoint);

    void swap_lines(Line* line1, Line* line2, Point sweepPoint);

};

#endif

