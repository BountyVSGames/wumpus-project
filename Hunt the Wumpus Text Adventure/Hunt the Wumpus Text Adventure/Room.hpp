#pragma once

#include <vector>
#include <iostream>
#include <limits>
#include <fstream>
#include "rapidjson-master/include/rapidjson/istreamwrapper.h"
#include "rapidjson-master/include/rapidjson/document.h"
#include "rapidjson-master/include/rapidjson/reader.h"
#include <cstdlib>
#include <stdlib.h>
#include <time.h>


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

struct player
{
	room currentRoom;
	int arrows = 5;
	bool gameOver = false;
};

void Intro();

void EnterRoom(player& currentPlayer);
void RoomSelection(player& currentPlayer);

void RandomBatRoom(player& currentPlayer);
int RandomRoom();
void WumpusRoom(player& currentPlayer);
void FillRoomVector(vector<room>& roomVec, const string& fileName);