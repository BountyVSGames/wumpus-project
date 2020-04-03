#pragma once
#include <iostream>
#include <fstream>;

#include <vector>
#include <string>
#include <limits>

using namespace std;

struct Room
{
	int roomID;
	vector<int> adjacentRooms;
	bool bat = false;
	bool pit = false;
	bool wumpus = false;
};