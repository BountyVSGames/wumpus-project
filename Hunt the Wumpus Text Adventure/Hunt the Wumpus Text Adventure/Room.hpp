#pragma once
#include "Game.hpp"

void Intro();

void EnterRoom(playerstruct& currentPlayer);
void RoomSelection(playerstruct& currentPlayer);

void RandomBatRoom(playerstruct& currentPlayer);
int RandomRoom();
void WumpusRoom(playerstruct& currentPlayer);
void FillRoomVector(vector<room>& roomVec, const string& fileName);