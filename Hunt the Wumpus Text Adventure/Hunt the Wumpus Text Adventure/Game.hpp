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
#include "rapidjson-master/include/rapidjson/ostreamwrapper.h"
#include "rapidjson-master/include/rapidjson/document.h"
#include "rapidjson-master/include/rapidjson/reader.h"
#include "rapidjson-master/include/rapidjson/writer.h"

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

struct playerstruct
{
	string playerName;
	room currentRoom;
	int arrows = 5;
	bool gameOver = false;
	int gamesPlayed = 0;
	int turns = 0;
	bool mapCompleted = false;
};

struct aiplayer
{
	vector<room> roomsVisited = {};
	vector<room> batRooms = {};
	vector<room> pitRooms = {};
	vector<room> possibleWumpusRooms = {};
};

bool GameStart(playerstruct& currentPlayer, const string fileName);
void GameOver(playerstruct& currentRoom, const bool& gameWon);

void PlayerInteraction(playerstruct& currentRoom);
void PlayerShoot(playerstruct& currentRoom);