#include <Windows.h>	// Sleep()
#include <stdlib.h>		// abs()
#include <iostream>

#include "cPathing.h"
#include "cBoard.h"
#include "cNode.h"

void cPathing::generatePath(cVector start, cVector end, bool& west_thread, bool& north_thread, bool& east_thread, bool& south_thread)
{
	// generate distances to find route to the end
	generateDistances(start, end, west_thread, north_thread, east_thread, south_thread);

	// generate the final path stack
	traceBack(start, end);

	Sleep(10);
	board.clearBoard();
	board.draw();

	// display the final path
	displayPath();
}

//////////////////////////////////////////////////////
//						CHANGES						//
//////////////////////////////////////////////////////
// From the previous version I have added in the	//
// threads that are created during calculcations.	//
// All of the threads created in this class will be	//
// joined at the end of this function.				//
//////////////////////////////////////////////////////
void cPathing::generateDistances(cVector start, cVector end, bool& west_thread, bool& north_thread, bool& east_thread, bool& south_thread)
{
	cVector curr_point;

	// set start/end to the board
	board.board[start.x][start.y].setI(B_START);
	board.board[end.x][end.y].setI(B_END);

	// update squares adjacent to the start position
	updateAdjSquares(start, end, west_thread, north_thread, east_thread, south_thread);

	// now start looping through the wset until adjacent to the end point, or until the wset is empty
	while (!wset.empty() && !isAdjIndex(wset.top(), B_END))
	{
		curr_point = wset.top();
		wset.pop();

		// Pathing threads are created in here.
		updateAdjSquares(curr_point, end, west_thread, north_thread, east_thread, south_thread);

		board.draw();
		Sleep(10);
	}

	// Joining threads once they have been dealt with.
	// If there are threads that have been created by the path class.
	if (!path_threads.empty())
	{
		// Go through all of the threads.
		for (auto threads = path_threads.begin(); threads != path_threads.end(); threads++)
		{
			// And join them up.
			(**threads).join();
		}
	}
}

//////////////////////////////////////////////////////
//						CHANGES						//
//////////////////////////////////////////////////////
// This is a new function, this is one of the three	//
// threading function in this class.				//
// This class will lock the correct mutex for the	//
// value it is changing, and then use a condition	//
// variable to notify the other threads that this	//
// calculation has been calculated.					//
//////////////////////////////////////////////////////
void cPathing::threadCalculateGValue(cVector point)
{
	// Locking the shared g value with a mutex.
	unique_lock<mutex> lock(g_value_mutex);

	// Calculating the new g value.
	g_value = (board.board[point.x][point.y].getI() + 1);
	g_value_ready = true;
	g_value_cv.notify_one();
}

//////////////////////////////////////////////////////
//						CHANGES						//
//////////////////////////////////////////////////////
// This is a new function, this is one of the three	//
// threading function in this class.				//
// This class will lock the correct mutex for the	//
// value it is changing, and then use a condition	//
// variable to notify the other threads that this	//
// calculation has been calculated.					//
//////////////////////////////////////////////////////
void cPathing::threadCalculateHValue(cVector point, cVector goal)
{
	// Locking the shared h value with a mutex.
	unique_lock<mutex> lock(h_value_mutex);

	// Calculating the new h value.
	h_value = abs(goal.x - point.x) + abs(goal.y - point.y);
	h_value_ready = true;
	h_value_cv.notify_one();
}

//////////////////////////////////////////////////////
//						CHANGES						//
//////////////////////////////////////////////////////
// This is a new function, this is one of the three	//
// threading function in this class.				//
// This class will lock the correct mutex for the	//
// value it is changing, and then use a condition	//
// variable to notify the other threads that this	//
// calculation has been calculated.					//
//////////////////////////////////////////////////////
void cPathing::threadCalculateFValue(cVector point)
{
	// Locking the shared f value with a mutex.
	unique_lock<mutex> lock(f_value_mutex);

	// Calculating the new f value.
	f_value = g_value + h_value;
}

//////////////////////////////////////////////////////
//						CHANGES						//
//////////////////////////////////////////////////////
// This function has been changed. The threads in	//
// this class will be created here with new			//
// instances using the lambda method of using		//
// functions with threads.							//
// This will also run the checks for the condition	//
// variables.										//
//////////////////////////////////////////////////////
void cPathing::updateAdjSquares(cVector point, cVector goal, bool& west_thread, bool& north_thread, bool& east_thread, bool& south_thread)
{
	// g = path length so far (the index) // h = estimated distance to travel to the goal
	/* 1 update the distance number of the West adjacent node*/
	// (1) Get the West adjacent node from board.board[][] 
	// (2) Check whether the West adjacent node is empty by comparing its index (getI()) with the Macro "B_EMPTY"
	// (3) calculate the tentative g, h, f costs of the West adjacent node, g is the distance of the current node plus one, h is the Manhattan distance from the West adjacent node to target node
	// (4) if the tentative f cost is greater than the old f cost of the West adjacent node, then update the g, h, f cost using the function "board.board[][].setFGH()"
	// (5) push the updated West adjacent point "cVector" to the working set using the member function of "wset.push()".

	// Checks the west adjacent cell.
	if (board.board[point.x - 1][point.y].getI() == B_EMPTY)
	{
		// If the user wants to calculate the west adjacent nodes using threads.
		if (west_thread)
		{
			// Current path length.
			path_threads.push_back(new thread([&] { threadCalculateGValue(point); }));

			// The difference of x + the difference of y.
			// Estimated value to the end point.
			path_threads.push_back(new thread([&] { threadCalculateHValue(point, goal); }));

			// Lock the f value mutex.
			unique_lock<mutex> lock(f_value_mutex);

			// While the g and h values are yet to be calculated.
			while (!g_value_ready || !h_value_ready)
			{
				// Wait for the condition variable to change for the g value.
				g_value_cv.wait(lock);

				// Wait for the condition variable to change for the h value.
				h_value_cv.wait(lock);
			}

			// Unlock the f value mutex.
			lock.unlock();

			// The total distance covered.
			//f_value = g_value + h_value;
			path_threads.push_back(new thread([&] { threadCalculateFValue(point); }));
		}
		// Otherwise, the user did not want to calculate the west adjacent nodes using threads.
		else
		{
			CalculateGValue(point);
			CalculateHValue(point, goal);
			CalculateFValue(point);
		}

		// If the new "f" value is less than the old f value of the cell, update the f, g and h costs.
		if (board.board[point.x - 1][point.y].getF() < f_value)
		{
			board.board[point.x - 1][point.y].setFGH(g_value, h_value);
		}

		cVector new_point(point.x - 1, point.y, board.board[point.x - 1][point.y].getF());
		wset.push(new_point);
	}

	// Checks the north adjacent cell.
	if (board.board[point.x][point.y - 1].getI() == B_EMPTY)
	{
		if (north_thread)
		{
			path_threads.push_back(new thread([&] { threadCalculateGValue(point); }));
			path_threads.push_back(new thread([&] { threadCalculateHValue(point, goal); }));

			unique_lock<mutex> lock(f_value_mutex);

			while (!g_value_ready || !h_value_ready)
			{
				g_value_cv.wait(lock);
				h_value_cv.wait(lock);
			}

			lock.unlock();

			path_threads.push_back(new thread([&] { threadCalculateFValue(point); }));
		}
		else
		{
			CalculateGValue(point);
			CalculateHValue(point, goal);
			CalculateFValue(point);
		}

		if (board.board[point.x][point.y - 1].getF() < f_value)
		{
			board.board[point.x][point.y - 1].setFGH(g_value, h_value);
		}

		cVector new_point(point.x, point.y - 1, board.board[point.x][point.y - 1].getF());
		wset.push(new_point);
	}

	// Checks the east adjacent cell.
	if (board.board[point.x + 1][point.y].getI() == B_EMPTY)
	{
		if (east_thread)
		{
			path_threads.push_back(new thread([&] { threadCalculateGValue(point); }));
			path_threads.push_back(new thread([&] { threadCalculateHValue(point, goal); }));

			unique_lock<mutex> lock(f_value_mutex);

			while (!g_value_ready || !h_value_ready)
			{
				g_value_cv.wait(lock);
				h_value_cv.wait(lock);
			}

			lock.unlock();

			path_threads.push_back(new thread([&] { threadCalculateFValue(point); }));
		}
		else
		{
			CalculateGValue(point);
			CalculateHValue(point, goal);
			CalculateFValue(point);
		}

		if (board.board[point.x + 1][point.y].getF() < f_value)
		{
			board.board[point.x + 1][point.y].setFGH(g_value, h_value);
		}

		cVector new_point(point.x + 1, point.y, board.board[point.x + 1][point.y].getF());
		wset.push(new_point);
	}

	// Checks the south adjacent cell.
	if (board.board[point.x][point.y + 1].getI() == B_EMPTY)
	{
		if (south_thread)
		{
			path_threads.push_back(new thread([&] { threadCalculateGValue(point); }));
			path_threads.push_back(new thread([&] { threadCalculateHValue(point, goal); }));

			unique_lock<mutex> lock(f_value_mutex);

			while (!g_value_ready || !h_value_ready)
			{
				g_value_cv.wait(lock);
				h_value_cv.wait(lock);
			}

			lock.unlock();

			path_threads.push_back(new thread([&] { threadCalculateFValue(point); }));
		}
		else
		{
			CalculateGValue(point);
			CalculateHValue(point, goal);
			CalculateFValue(point);
		}

		if (board.board[point.x][point.y + 1].getF() < f_value)
		{
			board.board[point.x][point.y + 1].setFGH(g_value, h_value);
		}

		cVector new_point(point.x, point.y + 1, board.board[point.x][point.y + 1].getF());
		wset.push(new_point);
	}
}

bool cPathing::isAdjIndex(cVector point, int i)
{
	if (board.board[point.x + 1][point.y].getI() == i)
	{
		return true;
	}
	else
	{
		if (board.board[point.x - 1][point.y].getI() == i)
			return true;
		else
		{
			if (board.board[point.x][point.y + 1].getI() == i)
				return true;
			else
			{
				if (board.board[point.x][point.y - 1].getI() == i)
					return true;
				else
					return false;
			}
		}
	}
}

void cPathing::traceBack(cVector start, cVector end)
{
	// push the end point to the path stack using the function "path_final.push()", note that the data type in the path stack is "cVector"
	path_final.push(end);

	// find the node "board.board[][]" with lowest distance around the end node in clockwise orientation of West, North, East and South  
	cVector lowest_point(end.x - 1, end.y, end.f);

	// Checks if the first lowest point is north.
	if (board.board[end.x][end.y - 1].getI() < board.board[path_final.top().x][path_final.top().y].getI())
	{
		lowest_point = cVector(end.x, end.y - 1, end.f);
	}

	// Checks if the east cell is the lowest point.
	if (board.board[end.x + 1][end.y].getI() < board.board[path_final.top().x][path_final.top().y].getI())
	{
		lowest_point = cVector(end.x + 1, end.y, end.f);
	}

	// Checks if the south cell is the lowest point.
	if (board.board[end.x][end.y + 1].getI() < board.board[path_final.top().x][path_final.top().y].getI())
	{
		lowest_point = cVector(end.x, end.y + 1, end.f);
	}

	// push the adjacent point with lowest distance to the path stack using the function of "path_final.push(cVector())"
	path_final.push(lowest_point);

	/* work back from lowest to S to populate the stack with the final path from S to T*/
	// 1. using a do-while loop, starting from the lowest point "cVector" around the target point, finishing at the start point
	// 2. in each loop, find the lowest node "board.board[][]" from the four adjacent node of the current node in clockwise orientation of West, North, East and South
	// 3. in each loop, push the lowest point "cVetor" to the path stack using the function of "path_final.push()"
	do
	{
		// Checks the west adjacent cell.
		if (board.board[path_final.top().x - 1][path_final.top().y].getI() < board.board[path_final.top().x][path_final.top().y].getI())
		{
			path_final.push(cVector(path_final.top().x - 1, path_final.top().y));
		}

		// Checks the north adjacent cell.
		if (board.board[path_final.top().x][path_final.top().y - 1].getI() < board.board[path_final.top().x][path_final.top().y].getI())
		{
			path_final.push(cVector(path_final.top().x, path_final.top().y - 1));
		}

		// Checks the east adjacent cell.
		if (board.board[path_final.top().x + 1][path_final.top().y].getI() < board.board[path_final.top().x][path_final.top().y].getI())
		{
			path_final.push(cVector(path_final.top().x + 1, path_final.top().y));
		}

		// Checks the south adjacent cell.
		if (board.board[path_final.top().x][path_final.top().y + 1].getI() < board.board[path_final.top().x][path_final.top().y].getI())
		{
			path_final.push(cVector(path_final.top().x, path_final.top().y + 1));
		}

	} while (!isAdjIndex(path_final.top(), B_START));
}

void cPathing::displayPath()
{
	while (!path_final.empty())
	{
		board.board[path_final.top().x][path_final.top().y].setI(path_final.size());
		path_final.pop();

		board.draw();
		Sleep(10);
	}
}