#ifndef CPATHING_H
#define CPATHING_H

//////////////////////////////////////////////////////
//						CHANGES						//
//////////////////////////////////////////////////////
// Added in extra includes to use threads, mutexes,	//
// and condition variables.							//
//////////////////////////////////////////////////////
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <stack>
#include <queue> 
#include <vector>

#include "cNode.h"
#include "cBoard.h"

using namespace std;

///////////////////////////////
// ADDED IN USING STD.
using std::thread;
using std::mutex;
using std::unique_lock;
using std::condition_variable;
using std::stack;
using std::queue;
using std::vector;
using std::greater;
///////////////////////////////

struct cVector
{
	int x, y, f;

	cVector(){};

	cVector(int a, int b)
	{
		x = a;
		y = b;
	}

	cVector(int a, int b, int c)
	{
		x = a;
		y = b;
		f = c;
	}

	// The comparison operator used to order the priority queue.
	// Overloads the operator  >

	bool operator>(const cVector &a) const
	{
		return f > a.f;
	}
};

extern cBoard board;

class cPathing
{
	public:
		// Methods.
		void generatePath(cVector start, cVector end, bool& west_thread, bool& north_thread, bool& east_thread, bool& south_thread);

	private:
		// Attributes.
		stack<cVector> path_final;
		priority_queue<cVector, vector<cVector>, greater<cVector>> wset;
		int g_value, h_value, f_value;

		///////////////////////////////////////////////////
		// NEW ATTRIBUTES ADDED HERE.
		vector<thread*> path_threads;
		mutex g_value_mutex, h_value_mutex, f_value_mutex;
		bool g_value_ready, h_value_ready;
		condition_variable g_value_cv, h_value_cv;
		////////////////////////////////////////////////////
		
		// Methods.
		void generateDistances(cVector start, cVector end, bool& west_thread, bool& north_thread, bool& east_thread, bool& south_thread);
		void traceBack(cVector start, cVector end);
		void displayPath();
		void updateAdjSquares(cVector point, cVector goal, bool& west_thread, bool& north_thread, bool& east_thread, bool& south_thread);
		bool isAdjIndex(cVector point, int i);

		/////////////////////////////////////////////////////////
		// NEW METHODS ADDED HERE.
		void threadCalculateGValue(cVector point);				
		void threadCalculateHValue(cVector point, cVector goal);	
		void threadCalculateFValue(cVector point);	

		inline int CalculateGValue(cVector point)				{ return (g_value = (board.board[point.x][point.y].getI() + 1)); };
		inline int CalculateHValue(cVector point, cVector goal) { return (h_value = abs(goal.x - point.x) + abs(goal.y - point.y)); };
		inline int CalculateFValue(cVector point)				{ return (f_value = (g_value + h_value)); };
		/////////////////////////////////////////////////////////
};

#endif