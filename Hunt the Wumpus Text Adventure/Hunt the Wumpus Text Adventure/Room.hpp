#pragma once
#include "Game.hpp"

void Intro();

void EnterRoom(playerstruct& currentPlayer);
void RoomSelection(playerstruct& currentPlayer);

void RandomBatRoom(playerstruct & currentPlayer);
void WumpusRoom(playerstruct& currentPlayer);

void ReadConfigInfo(vector<room>& roomVec, const string& fileName, playerstruct& currentPlayer);
void WritePlayerInfo(const playerstruct& currentPlayer, const string& fileName);
string ChooseMap(playerstruct& currentPlayer);
bool MenuScreen(string& mapFile, playerstruct& currentPlayer);

void PrintLeaderboard(const string& fileName);
