#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <limits>
#include <fstream>
#include "rapidjson-master/include/rapidjson/istreamwrapper.h"
#include "rapidjson-master/include/rapidjson/ostreamwrapper.h"
#include "rapidjson-master/include/rapidjson/document.h"
#include "rapidjson-master/include/rapidjson/reader.h"
#include "rapidjson-master/include/rapidjson/writer.h"
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
	string playerName;
	room currentRoom;
	int arrows = 5;
	bool gameOver = false;
	int gamesPlayed = 0;
	int turns = 0;
	bool mapCompleted = false;
};

void Intro();

void EnterRoom(player& currentPlayer);
void RoomSelection(player& currentPlayer);

void RandomBatRoom(player& currentPlayer);
int RandomRoom();
void WumpusRoom(player& currentPlayer);

void ReadConfigInfo(vector<room>& roomVec, const string& fileName, player& currentPlayer);
void WritePlayerInfo(const player& currentPlayer, const string& fileName);
string ChooseMap();
bool MenuScreen(string& mapFile, player& currentPlayer);

void PrintLeaderboard(const string& fileName);