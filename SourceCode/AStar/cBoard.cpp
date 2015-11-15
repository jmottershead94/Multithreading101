#include <iostream>
#include "cBoard.h"

using std::cout;
using std::endl;

//////////////////////////////////////////////////////
//						CHANGES						//
//////////////////////////////////////////////////////
// From the previous version I have added in the	//
// threads that are created during the creation of	//
// the board.										//
// These threads will be created here with new		//
// instances and will be joined up after all of the	//
// threads are created.								//
//////////////////////////////////////////////////////
cBoard::cBoard()
{
	// Creating the walls and empty nodes of the board.
	for (int y = 0; y < BOARD_Y; y++)
	{
		for (int x = 0; x < BOARD_X; x++)
		{
			// Adding in new loads of new threads here.
			board_threads_.push_back(new thread([&] { threadCreateBoundaries(x, y); }));
			board_threads_.push_back(new thread([&] { threadCreateWalls(x, y); }));
		}
	}

	// Joining all of the threads up after they have been created.
	for (auto threads = board_threads_.begin(); threads != board_threads_.end(); threads++)
	{
		(**threads).join();
	}
}

//////////////////////////////////////////////////////
//						CHANGES						//
//////////////////////////////////////////////////////
// This is a new function, this creates the walls	//
// inside of the surrounding box for the algorithm.	//
// These are obstacles for the algorithm to avoid.	//
//////////////////////////////////////////////////////
void cBoard::threadCreateWalls(int& x, int& y)
{
	if ((x % 2 == 0) && (y % 2 == 0))
	{
		board[x][y].setI(B_WALL);
	}
}

//////////////////////////////////////////////////////
//						CHANGES						//
//////////////////////////////////////////////////////
// This is a new function, this creates the walls	//
// of the surrounding box for the algorithm.		//
// This will also leave the rest of the cells		//
// empty.											//
//////////////////////////////////////////////////////
void cBoard::threadCreateBoundaries(int& x, int& y)
{
	if (x == 0 || y == 0 || x == BOARD_X - 1 || y == BOARD_Y - 1)
	{
		board[x][y].setI(B_WALL);
	}
	else
	{
		board[x][y].setI(B_EMPTY);
	}
}

void cBoard::draw()
{
	system("cls");

	for (int y = 0; y < BOARD_Y; y++)
	{
		cout << endl;
		for (int x = 0; x < BOARD_X; x++)
		{
			switch (board[x][y].getI())
			{
			case B_EMPTY:	cout << "   ";	break;
			case B_WALL:	cout << "#  ";	break;
			case B_START:	cout << "S  ";	break;
			case B_END:		cout << "E  ";	break;
			default:
				if (board[x][y].getI() < 10)
					cout << board[x][y].getI() << "  ";
				else
					cout << board[x][y].getI() << " ";
				break;
			}
		}
	}
	cout << endl << endl << endl;
}

void cBoard::clearBoard()
{
	for (int y = 0; y < BOARD_Y; y++)
	{
		for (int x = 0; x < BOARD_X; x++)
		{
			if (board[x][y].getI() != B_WALL && board[x][y].getI() != B_START && board[x][y].getI() != B_END)
				board[x][y].setI(B_EMPTY);
		}
	}
}