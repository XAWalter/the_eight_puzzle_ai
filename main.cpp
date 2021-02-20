// Can be altered for different sized puzzles
#define ROWS 3
#define COLS 3

// Includes
#include <iostream>		// io
#include <list>			// stl list
#include <array>		// stl array
#include <chrono>		// for timing cpu time

using namespace std;

// struct to track branches in solution tree
struct node {
	array<array<int, ROWS>, COLS> state;		// state of the board at node
	int g_score;								// price to arrive to node
	int h_score;								// heuristic to arrive to solution
	int f_score;								// g + h
	int pos_x_zero;								// x position of blank space
	int pos_y_zero;								// y position of blank space
	node *prev;									// parent node

	// overloaded operator for list<> remove
	bool operator==(node x){
		if ( state == x.state && f_score == x.f_score ) {
			return true;
		}
		return false;
	}
};

// calculate heuristic given the type being used
int calc_h ( int type, node *curr, array<array<int, ROWS>, COLS> *goal ){
	int h_score = 0;

	// check which type is being used
	switch(type){

		// if UCS then heuristic is 0 no matter what
		case 0:
			break;
	
		// if Misplaced Tile then check how many elements deviate from goal state not including blank
		case 1:
			for ( int i = 0; i < ROWS; i++ ){
				for ( int j = 0; j < COLS; j++ ){
					if ( curr->state[i][j] != (*goal)[i][j] && (*goal)[i][j] != 0){
						h_score++;
					}
				}
			}
			break;

		// if Manhattan then for each element count the distance from its position in the goal state
		case 2:
			for ( int i = 0; i < ROWS; i++ ){
				for ( int j = 0; j < COLS; j++ ){
					int temp = curr->state[i][j];
					if ( temp == 0 ){
						continue;
					}

					int flag = 0;
					int k;
					int l;
					
					for ( k = 0; k < ROWS; k++ ){
						for ( l = 0; l < COLS; l++ ){
							if ((*goal)[k][l] == temp){
								flag = 1;
								break;
							}
						}
						if ( flag ){
							break;
						}
					}
					// absolute value of the difference from row it is in from goal row added with
					// absolute value of the difference from col it is in from goal col
					h_score += abs(i - k) + abs(j - l);
				}
			}
			break;
		default:
			break;
	}

	return h_score;
}

// find the minimum f_score in list
node *min_f ( list<node*> *open ){
	list<node*>::iterator ptr = open->begin();
	node *min = *ptr;

	for ( ptr; ptr != open->end(); ptr++ ){
		if ( min->f_score > (*ptr)->f_score ){
			min = *ptr;
		}
	}
	return min;
}

// print state of board
void print_array ( array<array<int, ROWS>, COLS> arr ){
	for ( int i = 0; i < ROWS; i++ ){
		for ( int j = 0; j < COLS; j++ ){
			cout << arr[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

// move blank space up
node *move_up ( node *curr, array<array<int, ROWS>, COLS> *goal, int type ){
	node *temp = new node;
	int temp_y = curr->pos_y_zero - 1;

	// if position above blank is not possible
	if ( temp_y < 0 ){
		return NULL;
	}

	// set the state to that of the parent node
	temp->state = curr->state;
	
	// switch the position of the blank and the element above the blank
	temp->state[curr->pos_y_zero][curr->pos_x_zero] = curr->state[temp_y][curr->pos_x_zero];
	temp->state[temp_y][curr->pos_x_zero] = 0;
	temp->pos_x_zero = curr->pos_x_zero;
	temp->pos_y_zero = temp_y;

	// calc the scores
	temp->g_score = curr->g_score + 1;
	temp->h_score = calc_h(type, temp, goal);
	temp->f_score = temp->g_score + temp->h_score;

	// save the parent node to be able to be retrieved
	temp->prev = curr;
	return temp;
}

// similar to move_up
node *move_down ( node *curr, array<array<int, ROWS>, COLS> *goal, int type ){
	node *temp = new node;
	int temp_y = curr->pos_y_zero + 1;

	if ( temp_y >= ROWS ){
		return NULL;
	}

	temp->state = curr->state;
	temp->state[curr->pos_y_zero][curr->pos_x_zero] = curr->state[temp_y][curr->pos_x_zero];
	temp->state[temp_y][curr->pos_x_zero] = 0;
	temp->pos_x_zero = curr->pos_x_zero;
	temp->pos_y_zero = temp_y;
	temp->g_score = curr->g_score + 1;
	temp->h_score = calc_h(type, temp, goal);
	temp->f_score = temp->g_score + temp->h_score;
	temp->prev = curr;
	return temp;

}

// similar to move_up
node *move_right ( node *curr, array<array<int, ROWS>, COLS> *goal, int type ){
	node *temp = new node;
	int temp_x = curr->pos_x_zero + 1;

	if ( temp_x >= COLS ){
		return NULL;
	}

	temp->state = curr->state;
	temp->state[curr->pos_y_zero][curr->pos_x_zero] = curr->state[curr->pos_y_zero][temp_x];
	temp->state[curr->pos_y_zero][temp_x] = 0;
	temp->pos_x_zero = temp_x;
	temp->pos_y_zero = curr->pos_y_zero;
	temp->g_score = curr->g_score + 1;
	temp->h_score = calc_h(type, temp, goal);
	temp->f_score = temp->g_score + temp->h_score;
	temp->prev = curr;
	return temp;
}

// similar to move_up
node *move_left ( node *curr, array<array<int, ROWS>, COLS> *goal, int type ){
	node *temp = new node;
	int temp_x = curr->pos_x_zero - 1;

	if ( temp_x < 0 ){
		return NULL;
	}

	temp->state = curr->state;
	temp->state[curr->pos_y_zero][curr->pos_x_zero] = curr->state[curr->pos_y_zero][temp_x];
	temp->state[curr->pos_y_zero][temp_x] = 0;
	temp->pos_x_zero = temp_x;
	temp->pos_y_zero = curr->pos_y_zero;
	temp->g_score = curr->g_score + 1;
	temp->h_score = calc_h(type, temp, goal);
	temp->f_score = temp->g_score + temp->h_score;
	temp->prev = curr;
	return temp;
}



int main(){
	int type;

	// create list for nodes that need to be expanded
	list <node*> open;

	// create goal board
	array<array<int, ROWS>, COLS> goal = {{ { 1, 2, 3},
										    { 4, 5, 6},
										    { 7, 8, 0} }};

	// create root node
	node *init = new node;

	// user input
	cout << "1.Input Puzzle | 2:Use Default" << endl;
	cin >> type;

	if (type == 1){
		cout << "Input Puzzle with spaces between each input and a '0' at the blank's position" << endl;  
		for ( int i = 0; i < ROWS; i++ ){
			cout << "Row " << i+1 << ": ";  
			cin >> init->state[i][0] >> init->state[i][1] >> init->state[i][2];
			if ( init->state[i][0] == 0 ){
				init->pos_x_zero = 0;
				init->pos_y_zero = i;
			}
			if ( init->state[i][1] == 0 ){
				init->pos_x_zero = 1;
				init->pos_y_zero = i;
			}
			if ( init->state[i][2] == 0 ){
				init->pos_x_zero = 2;
				init->pos_y_zero = i;
			}
		}
	}

	// default
	else{
		init->state = {{ { 1, 2, 3},
					     { 5, 0, 6},
					     { 4, 7, 8} }};
		init->pos_x_zero = 1;
		init->pos_y_zero = 1;
	}

	// choose heuristic type
	cout << "0:UCS | 1:A* Misplaced Tile | 2:A* Manhattan\n";
	cin >> type;


	// telemetry for data analysis
	int nodes_expanded = 0;
	int max_list_size = 0;
	auto start = chrono::steady_clock::now();


	// initialize properties of root node
	init->g_score = 0;
	init->h_score = calc_h ( type, init, &goal );
	init->f_score = init->g_score + init->h_score;
	init->prev = NULL;
	open.push_front(init);


	// loop while there exists elements to be expanded
	while ( open.size() > 0  ){

		// save max size of list
		if ( max_list_size < open.size() ){
			max_list_size = open.size();
		}

		// set curr node to element in open that has the lowest f_score
		node *curr = min_f(&open);

		// remove node from open
		open.remove(curr);

		// if goal state is found
		if ( curr->state == goal ){
			// telemetry
			auto end = chrono::steady_clock::now();
			int depth = 0;

			// create solution list
			printf("Solution Found\n");
			list <node*> solution;

			// trace parent nodes from current goal state node to root and push onto solution list
			while ( curr != NULL ){
				solution.push_front(curr);
				curr = curr->prev;
				depth++;
			}

			// print solution list board states
			list<node*>::iterator ptr = solution.begin();
			for ( ptr; ptr != solution.end(); ptr++ ){
				print_array((*ptr)->state);
			}

			// output telemetry
			cout << "depth: " << depth-1 << endl;
			cout << "nodes expanded: " << nodes_expanded << endl;
			cout << "max list size: " << max_list_size << endl;
			cout << "nodes in frontier: " << open.size() << endl;

			// from https://www.techiedelight.com/measure-elapsed-time-program-chrono-library/
			cout << "cpu time: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << " mircoseconds" << endl;

			// exit success
			return 0;
		}

		// create a child node
		node *child = new node;

		// set child to node if moved up
		if ( ( child = move_up(curr, &goal, type) ) != NULL ){
			nodes_expanded++;
			int flag = 0;

			// iterate through open list to check if already has been visited
			list<node*>::iterator ptr = open.begin();
			for ( ptr; ptr != open.end(); ptr++ ){

				// if visited
				if ( child->state == (*ptr)->state ){
					// set flag to not add to open list again
					flag = 1;

					// if more efficient way to arrive to node
					// update node with new score and parent
					if ( child->g_score < (*ptr)->g_score){
						(*ptr)->g_score = child->g_score;
						(*ptr)->f_score = (*ptr)->g_score + (*ptr)->h_score;
						(*ptr)->prev = child->prev;
					}
				}
			}

			// do not add dups to list
			if (!flag){
				open.push_back(child);
			}
		}

		// set child to node if moved down
		// similar to move up
		if ( ( child = move_down(curr, &goal, type) ) != NULL ){
			nodes_expanded++;
			int flag = 0;
			list<node*>::iterator ptr = open.begin();
			for ( ptr; ptr != open.end(); ptr++ ){
				if ( child->state == (*ptr)->state ){
					flag = 1;
					if ( child->g_score < (*ptr)->g_score){
						(*ptr)->g_score = child->g_score;
						(*ptr)->f_score = (*ptr)->g_score + (*ptr)->h_score;
						(*ptr)->prev = child->prev;
					}
				}
			}
			if (!flag){
				open.push_back(child);
			}
		}

		// set child to node if moved left
		// similar to move up
		if ( ( child = move_left(curr, &goal, type) ) != NULL ){
			nodes_expanded++;
			int flag = 0;
			list<node*>::iterator ptr = open.begin();
			for ( ptr; ptr != open.end(); ptr++ ){
				if ( child->state == (*ptr)->state ){
					flag = 1;
					if ( child->g_score < (*ptr)->g_score){
						(*ptr)->g_score = child->g_score;
						(*ptr)->f_score = (*ptr)->g_score + (*ptr)->h_score;
						(*ptr)->prev = child->prev;
					}
				}
			}
			if (!flag){
				open.push_back(child);
			}
		}

		// set child to node if moved right
		// similar to move up
		if ( ( child = move_right(curr, &goal, type) ) != NULL ){
			nodes_expanded++;
			int flag = 0;
			list<node*>::iterator ptr = open.begin();
			for ( ptr; ptr != open.end(); ptr++ ){
				if ( child->state == (*ptr)->state ){
					flag = 1;
					if ( child->g_score < (*ptr)->g_score){
						(*ptr)->g_score = child->g_score;
						(*ptr)->f_score = (*ptr)->g_score + (*ptr)->h_score;
						(*ptr)->prev = child->prev;
					}
				}
			}
			if (!flag){
				open.push_back(child);
			}
		}

	}

	// if open list is empty and no solution was found
	cout << "no solution" << endl;

	// exit failed
	return -1;
}
