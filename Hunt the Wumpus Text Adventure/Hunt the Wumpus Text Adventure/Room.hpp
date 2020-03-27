#ifndef ROOM_HPP
#define ROOM_HPP

#include <vector>
#include <iostream>

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
void WumpusRoom();
void PitRoom();

#endif // ROOM_HPP
