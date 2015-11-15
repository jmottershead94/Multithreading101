#include <iostream>
#include <Windows.h>

#include "cBoard.h"
#include "cPathing.h"

using std::cout;
using std::endl;

//////////////////////////
// ADD NEW METHOD HERE.
// Function Prototypes.
void checkUserAnswers();
//////////////////////////

// Globals.
cBoard board;
cPathing pathing;

// NEW FUNCTION.
// This just checks the answers of the user if they want individual adjacent node thread calculations.
void checkUserAnswers(bool& direction, char& answer)
{
	if (answer == 'Y')
	{
		direction = true;
	}
	else if (answer == 'N')
	{
		direction = false;
	}
	else
	{
		cout << "You had your chance, threads will compute this direction." << endl;
	}
}

void main()
{
	// Local main attributes.
	bool running = true;
	bool west_thread, north_thread, east_thread, south_thread;
	char user_answer_all_threads, user_answer_west, user_answer_north, user_answer_east, user_answer_south;

	// Threads will be used to calculate all of the adjacent nodes.
	west_thread = true;
	north_thread = true;
	east_thread = true;
	south_thread = true;

	// Asking if the user wants to use threads to calculate the distances of all adjacent nodes.
	cout << "Would you like to use threads to calculate ALL of the adjacent nodes? (Y / N)" << endl;
	cin >> user_answer_all_threads;

	while (running)
	{
		// If the user wants to have the threads calculating all the adjacent nodes, run the calculations as normal.
		if (user_answer_all_threads == 'Y')
		{
			// Generating the algorithm with all of the adjacent nodes calculated with threads.
			pathing.generatePath(cVector(2, 16), cVector(15, 10), west_thread, north_thread, east_thread, south_thread);

			// The algorithm has finished, end the program.
			running = false;
		}
		// Otherwise, ask them for each specific direction.
		else if (user_answer_all_threads == 'N')
		{
			// Asking the user if they want the threads to be calculated in specific directions.
			cout << "Would you like threads to calculate west adjacent nodes? (Y / N)" << endl;
			cin >> user_answer_west;
			checkUserAnswers(west_thread, user_answer_west);

			cout << "Would you like threads to calculate north adjacent nodes? (Y / N)" << endl;
			cin >> user_answer_north;
			checkUserAnswers(north_thread, user_answer_north);

			cout << "Would you like threads to calculate east adjacent nodes? (Y / N)" << endl;
			cin >> user_answer_east;
			checkUserAnswers(east_thread, user_answer_east);

			cout << "Would you like threads to calculate south adjacent nodes? (Y / N)" << endl;
			cin >> user_answer_south;
			checkUserAnswers(south_thread, user_answer_south);

			// Generating the paths based on what the user wants calculated with certain threads.
			pathing.generatePath(cVector(2, 16), cVector(15, 10), west_thread, north_thread, east_thread, south_thread);

			// The algorithm has finished, end the program.
			running = false;
		}
		// Otherwise, they have put an invalid response.
		else
		{
			cout << "Error: Please type a valid answer either: Y or N" << endl;
			cout << "Would you like to use threads to calculate ALL of the adjacent nodes? (Y / N)" << endl;
			cin >> user_answer_all_threads;
		}
	}
}