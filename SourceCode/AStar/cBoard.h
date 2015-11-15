#ifndef CBOARD_H
#define CBOARD_H

//////////////////////////////////////////////////////
//						CHANGES						//
//////////////////////////////////////////////////////
// Added in extra includes to use threads.			//
//////////////////////////////////////////////////////
#include <thread>
#include <vector>
#include "cNode.h"

#define BOARD_X 20
#define BOARD_Y 20

#define B_EMPTY 1000
#define B_WALL	999
#define B_START	0
#define B_END	1001

///////////////////////////////
// ADDED IN USING STD.
using std::thread;
using std::vector;
///////////////////////////////

class cBoard
{
	public:
		// Attributes.
		cNode board[BOARD_X][BOARD_Y];

		////////////////////////////////
		// NEW ADDED ATTRIBUTES HERE.
		vector<thread*> board_threads_;
		////////////////////////////////

		// Methods.
		cBoard();
		void draw();
		void clearBoard();

		/////////////////////////////////////////////
		// NEW ADDED METHODS HERE.
		void threadCreateWalls(int& x, int& y);
		void threadCreateBoundaries(int& x, int& y);
		/////////////////////////////////////////////
};

#endif CBOARD_H