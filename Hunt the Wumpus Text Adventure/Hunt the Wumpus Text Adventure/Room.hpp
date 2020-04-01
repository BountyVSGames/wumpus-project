#ifndef ROOM_HPP
#define ROOM_HPP

#include <vector>
#include <iostream>
#include <limits>
#include <stdlib.h>
#include <time.h>

using namespace std;

struct room
{
	int roomID;
	vector<int> adjacentRooms;
	bool bat = false;
	bool pit = false;
};

void Intro(room& startRoom);

void EnterRoom(room& room);
void RoomSelection(room& currentRoom);

void BatRoom();
void RandomBatRoom(room& playerplace);
int RandomRoom();
void WumpusRoom();
void PitRoom(room& room);

#endif // ROOM_HPP