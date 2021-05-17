#ifndef SWEEP_BINARY_SEARCH_TREE_CPP
#define SWEEP_BINARY_SEARCH_TREE_CPP

#include "sweep_binary_search_tree.h"

SweepBST::Node* SweepBST::makeEmpty(Node* t){
	if(t == NULL){return NULL;}
	makeEmpty(t->left);
    makeEmpty(t->right);
    delete t;
    return NULL;
}

SweepBST::Node* SweepBST::findMin(Node* t){
    if(t == NULL){
        return NULL;
    }
    else if(t->left == NULL){
        return t;
    }
    else{
        return findMin(t->left);
    }
}

SweepBST::Node* SweepBST::findMax(Node* t){
	if(t == NULL){
		return NULL;
	}
	else if(t->right == NULL){
		return t;
	}
	else {
		return findMax(t->right);
	}
}

SweepBST::Node* SweepBST::find(Node* t, Line* line, Point sweepPoint){
	if(t == NULL){return NULL;}
	if(t->line->id == line->id){
		// printf("Found\n");
		return t;
	}
	int comparison = compare(t->line, sweepPoint);
	// If same, check sweep point a bit earlier on the line
	if(comparison == 0){
		// double gradient = (line->start->x - line->end->x)/(line->start->y - line->end->y);
		// sweepPoint.y = sweepPoint.y + 1;
		// sweepPoint.x = sweepPoint.x - gradient;
		// comparison = compare(t->line, sweepPoint);
		// sweepPoint.y = sweepPoint.y - 1;
		// sweepPoint.x = sweepPoint.x + gradient;
		// if(comparison == 0){
	 //    	// Can only happen if t->line is horizontal,
	 //    	// or lines are colinear. Say it is smaller
	 //    	return find(t->left, line, sweepPoint);
	 //    }else if(comparison == 1){
	 //    	return find(t->left, line, sweepPoint);
	 //    }else{
	 //    	return find(t->right, line, sweepPoint);
	 //    }
		Node* left = find(t->left, line, sweepPoint);
		Node* right = find(t->right, line, sweepPoint);
		if(left && right){
			printf("MISTAKE!!\n");
			return NULL;
		}
		if(left){return left;}
		if(right){return right;}
		else{
			printf("Coulnd't find it!\n");
			return NULL;}
	}else if(comparison == -1){
		// printf("Left\n");
		return find(t->left, line, sweepPoint);
	}else{
		// printf("Right\n");
		return find(t->right, line, sweepPoint);
	}
}

SweepBST::Node* SweepBST::insert(Line* newLine, Node* t, Point sweepPoint){
    // Tree is empty
    if(t == NULL){
        t = new Node;
        t->line = newLine;
        t->left = t->right = NULL;
        return t;
    }
    int comparison = compare(t->line, sweepPoint);
    if(comparison == -1){
        t->left = insert(newLine, t->left, sweepPoint);
    }else if(comparison == 1){
    	t->right = insert(newLine, t->right, sweepPoint);
    }else{
	    // If same, inserting line which starts on another line
	    // Check what would happen a bit higher, do so what making the
	    // sweep point a bit higher
	    // If we are comparing with a horizontal line, line is to the right by default,
	    // so insert to the left
	    if(abs(t->line->start->y - t->line->end->y) < 0.001){
	    	t->left = insert(newLine, t->left, sweepPoint);
	    	return t;
	    }
	    // If inserting horizontal line, to the right by default.
	    else if(abs(newLine->start->y - newLine->end->y) < 0.001){
	    	t->right = insert(newLine, t->right, sweepPoint);
	    	return t;
	    }
	    // If here, neither of the lines are horizontal, check how they would compare a bit "earlier"
	    double gradient = (newLine->start->x - newLine->end->x)/(newLine->start->y - newLine->end->y);
		sweepPoint.y = sweepPoint.y + 1;
		sweepPoint.x = sweepPoint.x + gradient;
		comparison = compare(t->line, sweepPoint);
		sweepPoint.y = sweepPoint.y - 1;
		sweepPoint.x = sweepPoint.x - gradient;
	    if(comparison == 0){
	    	printf("Shouldn't happen");
	    	// Can only happen if t->line is horizontal,
	    	// or lines are colinear. Say it is smaller
	    	t->left = insert(newLine, t->left, sweepPoint);
	    }else if(comparison == -1){
	    	t->left = insert(newLine, t->left, sweepPoint);
	    }else{
	    	t->right = insert(newLine, t->right, sweepPoint);
	    }
    }
    

    return t;
}

SweepBST::Node* SweepBST::remove(Line* oldLine, Node* t, Point sweepPoint){
    Node* temp;
    if(t == NULL){return NULL;}
    int comparison = compare(t->line, sweepPoint);
    // Check if current node is the one we want to delete
    if(t->line->id == oldLine->id){
    	if(t->left && t->right){
    		temp = findMin(t->right);
    		t->line = temp->line;
    		t->right = remove(t->line, t->right, sweepPoint);
    	}else{
    		temp = t;
    		if(t->left == NULL){
            	t = t->right;
    		}
        	else if(t->right == NULL){
            	t = t->left;
            }
        	delete temp;
    	}
    }
    // If line is at the same place, do test again a bit higher
    else if(comparison == 0){
  //   	double gradient = (oldLine->start->x - oldLine->end->x)/(oldLine->start->y - oldLine->end->y);
		// sweepPoint.y = sweepPoint.y + 1;
		// sweepPoint.x = sweepPoint.x - gradient;
		// comparison = compare(t->line, sweepPoint);
		// sweepPoint.y = sweepPoint.y - 1;
		// sweepPoint.x = sweepPoint.x + gradient;
	 //    if(comparison == 0){
	 //    	t->left = remove(oldLine, t->left, sweepPoint);
	 //    }else if(comparison == -1){
	 //    	t->left = remove(oldLine, t->left, sweepPoint);
	 //    }else{
	 //    	t->right = remove(oldLine, t->right, sweepPoint);
	 //    }
    	t->left = remove(oldLine, t->left, sweepPoint);
    	t->right = remove(oldLine, t->right, sweepPoint);
    }else if(comparison == -1){
        t->left = remove(oldLine, t->left, sweepPoint);
    }else{
        t->right = remove(oldLine, t->right, sweepPoint);
    }

    return t;
}

void SweepBST::inorder(Node* t, int order){
    if(t == NULL){return;}
    inorder(t->left, order + 1);
    cout << t->line->id << " (" << order << ") ";
    inorder(t->right, order + 1);
}

// SweepBST::Node* SweepBST::find_largest_smaller(Node* root, Line* line, Point sweepPoint){
//     Node* temp;
//     if(root == NULL){
//         return NULL;
//     }
//     int comparison = compare(root->line, sweepPoint);
//     if(comparison == 0){
//     	// Check 
//     	// printf("YEA\n");
//     	// // If line is at the same place, do test again a bit higher
//     	// sweepPoint.y = sweepPoint.y + 1;
// 	    // comparison = compare(root->line, sweepPoint);
// 	    // sweepPoint.y = sweepPoint.y - 1;
// 	    // if(comparison == 0){
// 	    // 	return find_largest_smaller(root->left, sweepPoint);
// 	    // }else if(comparison == 1){
// 	    // 	return find_largest_smaller(root->left, sweepPoint);
// 	    // }else{
// 	    // 	return find_largest_smaller(root->right, sweepPoint);
// 	    // }
//     	Node* left = find_largest_smaller(root->left, line, sweepPoint);
//     	Node* right = find_largest_smaller(root->right, line, sweepPoint);
//     	if(left){return left;}
//     	else{return right;}
//     }else if(comparison == 1){
//     	temp = find_largest_smaller(root->right, line, sweepPoint);
//     	if(temp == NULL){
//     		return root;
//     	}
//     	return temp;
//     }else{
//     	return find_largest_smaller(root->left, line, sweepPoint);
//     }
// }

SweepBST::Node* SweepBST::find_largest_smaller(Node* root, Line* line, Point sweepPoint){
    Node* temp;
    if(root == NULL){
        return NULL;
    }if(root->line->id == line->id){
    	return find_largest_smaller(root->left, line, sweepPoint);
    }
    // Whether to look left or not
    bool left = true;
    int comparison = compare(root->line, sweepPoint);
    if(comparison == 0){
    	// Check whether either line is horizontal, if so do 
    	// default behaviour
    	if(abs(root->line->start->y - root->line->end->y) < 0.001){
	    	left = true;
	    }
	    // If inserting horizontal line, to the right by default.
	    else if(abs(line->start->y - line->end->y) < 0.001){
	    	left = false;
	    }
	    // Go a bit earlier, check again
	    double gradient = (line->start->x - line->end->x)/(line->start->y - line->end->y);
		sweepPoint.y = sweepPoint.y + 1;
		sweepPoint.x = sweepPoint.x + gradient;
		// printf("Gradient is %.2f\n",gradient);
		// printf("(%.2f,%.2f) - (%.2f,%.2f)\n",line->start->x, line->start->y, line->end->x, line->end->y);
		comparison = compare(root->line, sweepPoint);
		sweepPoint.y = sweepPoint.y - 1;
		sweepPoint.x = sweepPoint.x - gradient;
		if(comparison == 1){
			left = false;
		}else if(comparison == -1){
			left = true;
		}else{
			printf("Shouldn't happen");
		}
    }else if(comparison == 1){
    	left = false;
    }else{
    	left = true;
    }
    // Now do behaviour based on results
    if(left){
    	return find_largest_smaller(root->left, line, sweepPoint);
    }else{
    	temp = find_largest_smaller(root->right, line, sweepPoint);
    	if(temp == NULL){
    		return root;
    	}
    	return temp;
    }
}

SweepBST::Node* SweepBST::find_smallest_larger(Node* root, Line* line, Point sweepPoint){
    Node* temp;
    if(root == NULL){
    	// printf("Bust\n");
        return NULL;
    }if(root->line->id == line->id){
    	// printf("Right\n");
    	return find_smallest_larger(root->right, line, sweepPoint);
    }
    bool left = true;
    int comparison = compare(root->line, sweepPoint);
    if(comparison == 0){
    	// Check whether either line is horizontal, if so do 
    	// default behaviour
    	if(abs(root->line->start->y - root->line->end->y) < 0.001){
	    	left = false;
	    }
	    // If inserting horizontal line, to the right by default.
	    else if(abs(line->start->y - line->end->y) < 0.001){
	    	left = true;
	    }
	    // Go a bit earlier, check again
	    double gradient = (line->start->x - line->end->x)/(line->start->y - line->end->y);
		sweepPoint.y = sweepPoint.y + 1;
		sweepPoint.x = sweepPoint.x + gradient;
		comparison = compare(root->line, sweepPoint);
		sweepPoint.y = sweepPoint.y - 1;
		sweepPoint.x = sweepPoint.x - gradient;
		if(comparison == -1){
			// printf("Here\n");
			left = true;
		}else if(comparison == 1){
			left = false;
		}else{
			printf("Shouldn't happen");
		}
    }else if(comparison == -1){
    	left = true;
    }else{
    	left = false;
    }
    // Now do behaviour based on results
    if(left){
    	// printf("Left\n");
    	temp = find_smallest_larger(root->left, line, sweepPoint);

    	if(temp == NULL){
    		return root;
    	}
    	return temp;
    }else{
    	// printf("Right\n");
    	return find_smallest_larger(root->right, line, sweepPoint);
    }
}

// SweepBST::Node* SweepBST::find_smallest_larger(Node* root, Line* line, Point sweepPoint){
//     Node* temp;
//     if(root == NULL){
//         return NULL;
//     }
//     int comparison = compare(root->line, sweepPoint);
//     if(comparison == 0){

//     	// printf("YEA\n");
//     	// sweepPoint.y = sweepPoint.y + 1;
// 	    // comparison = compare(root->line, sweepPoint);
// 	    // sweepPoint.y = sweepPoint.y - 1;
//     	// if(comparison == 0){
//     	// 	return find_smallest_larger(root->right, sweepPoint);
//     	// }else if(comparison == -1){
//     	// 	return find_smallest_larger(root->left, sweepPoint);
//     	// }else{
//     	// 	return find_smallest_larger(root->right, sweepPoint);
//     	// }
//     	Node* left = find_smallest_larger(root->left, line, sweepPoint);
//     	Node* right = find_smallest_larger(root->right, line, sweepPoint);
//     	if(left){return left;}
//     	else{return right;}
//     }else if(comparison == -1){
//     	temp = find_smallest_larger(root->left, line, sweepPoint);
//     	if(temp == NULL){
//     		return root;
//     	}
//     	return temp;
//     }else{
//     	return find_smallest_larger(root->right, line, sweepPoint);
//     }
// }

int SweepBST::compare(Line* line, Point sweepPoint){
	if(abs(line->end->y - line->start->y) < 0.001){
		if(line->end->x <= sweepPoint.x && sweepPoint.x >= line->start->y){
			return 0;
		}else if(sweepPoint.x < line->end->x){
			return -1;
		}else{
			return 1;
		}
	}
	double currX = line->start->x + (line->end->x - line->start->x) * (sweepPoint.y - line->start->y) / (line->end->y - line->start->y);
	// printf("X = %.2f vs point %.2f\n",currX, sweepPoint.x);
	if(abs(currX - sweepPoint.x) < 0.001){
		return 0;
	}else if(currX > sweepPoint.x){
		// printf("Return -1\n");
		return -1;
	}else{
		// printf("Return 1\n");
		return 1;
	}
}

SweepBST::SweepBST(){
	root = NULL;
}

SweepBST::~SweepBST(){
	root = makeEmpty(root);
    }

void SweepBST::insert(Line* line, Point sweepPoint){
    root = insert(line, root, sweepPoint);
}

void SweepBST::remove(Line* line, Point sweepPoint){
    root = remove(line, root, sweepPoint);
}

void SweepBST::display()
{
	inorder(root, 0);
    cout << endl;
}

Line* SweepBST::left_neighbour(Line* line, Point sweepPoint){
	Node* temp = find_largest_smaller(root, line, sweepPoint);
	if(temp){return temp->line;}
	else{return NULL;}
}

Line* SweepBST::right_neighbour(Line* line, Point sweepPoint){
	Node* temp = find_smallest_larger(root, line, sweepPoint);
	if(temp){return temp->line;}
	else{return NULL;}
}

void SweepBST::swap_lines(Line* line1, Line* line2, Point sweepPoint){
	// printf("Look for %d (%f,%f) - (%f,%f)\n", line1->id,line1->start->x,line1->start->y,
														// line1->end->x,line1->end->y);
	Node* node1 = find(root, line1, sweepPoint);
	// printf("Look for %d (%f,%f) - (%f,%f)\n", line2->id,line2->start->x,line2->start->y,
														// line2->end->x,line2->end->y);
	Node* node2 = find(root, line2, sweepPoint);
	if(node1 == NULL){
		printf("\nDIDNT FIND - %d\n",line1->id);
	}
	if(node2 == NULL){
		printf("\nDIDNT FIND - %d\n",line2->id);
	}
	if(node1 == NULL || node2 == NULL){
		return;
	}
	Line* temp = node1->line;
	node1->line = node2->line;
	node2->line = temp; 
}





// int main()
// {
// 	Point one = {1, 1.0, 6.0};
// 	Point two = {2, 1.0, 0.0};
// 	Point three = {3, 2.0, 4.0};
// 	Point four = {4, 3.0, 0.0};
// 	Point five = {5, 3.0, 5.0};
// 	Point six = {6, 3.0, 0.0};
// 	Point seven = {7, 4.0, 2.0};
// 	Point eight = {8, 4.0, 0.0};
	
// 	std::vector<Line> lines = {{0, &one, &two},
// 								{1, &three, &four},
// 								{2, &five, &six},
// 								{3, &seven, &eight}};
//     SweepBST t;
//     for(int i = 0; i < lines.size(); i++){
//     	t.insert(&lines[i], 0.0);
//     }
//     // t.display();
//     for(int i = 0; i < lines.size(); i++){
//     	Line* temp = t.left_neighbour(&lines[i], 0.0);
//     	if(temp){printf("Left of %d is %d\n", i, temp->id);}
//     	else{printf("Left of %d is NULL\n", i);}
//     	temp = t.right_neighbour(&lines[i], 0.0);
//     	if(temp){printf("Right of %d is %d\n", i, temp->id);}
//     	else{printf("Right of %d is NULL\n", i);}
//     }
// }

#endif