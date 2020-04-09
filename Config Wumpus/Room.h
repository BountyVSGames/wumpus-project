#pragma once
#include <iostream>
#include <fstream>;
#include <vector>
#include <string>
#include <limits>
#include <cstdlib>
#include <time.h>
#include "rapidjson-master/include/rapidjson/writer.h";
#include "rapidjson-master/include/rapidjson/stringbuffer.h";

using namespace rapidjson;
using namespace std;

struct room
{
	int roomID;
	vector<int> adjacentRooms;
	vector<int> gridPosXY;
	vector<int> openPositions = { 0, 1, 2, 3 };
	bool bat = false;
	bool pit = false;
	bool wumpus = false;
};