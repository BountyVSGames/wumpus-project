#pragma once

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
#include <mutex>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <thread>

#include "rapidjson-master/include/rapidjson/istreamwrapper.h"
#include "rapidjson-master/include/rapidjson/document.h"
#include "rapidjson-master/include/rapidjson/reader.h"

using namespace std;
using namespace rapidjson;

struct room
{
	int roomID;
	vector<int> adjacentRooms;
	bool bat = false;
	bool pit = false;
	bool wumpus = false;
};

struct aiplayer
{
	vector<room> roomsVisited = {};
	vector<room> batRooms = {};
	vector<room> pitRooms = {};
	vector<room> possibleWumpusRooms = {};
};
struct playerstruct
{
	room currentRoom;
	int arrows = 5;
	bool gameOver = false;
};

bool GameStart(playerstruct& currentRoom);
void GameOver(playerstruct& currentRoom, const bool& gameWon);

void PlayerInteraction(playerstruct& currentRoom);
void PlayerShoot(playerstruct& currentRoom);