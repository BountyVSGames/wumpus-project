#ifndef ROOM_HPP
#define ROOM_HPP

#include <vector>
#include <iostream>
#include <limits>
#include <fstream>
#include "rapidjson-master/include/rapidjson/istreamwrapper.h"
#include "rapidjson-master/include/rapidjson/document.h"
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

void Intro(room& startRoom);

void EnterRoom(room& room);
void RoomSelection(room& currentRoom);

void BatRoom();
void RandomBatRoom(room& playerplace);
int RandomRoom();
void WumpusRoom();
void PitRoom();
void FillRoomvector();
void PitRoom(room& room);

#endif // ROOM_HPP