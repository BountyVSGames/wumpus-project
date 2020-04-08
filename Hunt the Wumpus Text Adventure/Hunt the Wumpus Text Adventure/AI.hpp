#pragma once
#include "Game.hpp";
#define WINVER 0x0500
#include <Windows.h>

void AIPrepareInput();

void AIInputPressDown(char inputKey);
void AIInputPressUp(char inputKey);

void AIMove(room currentRoom);
void AIShoot(room currentRoom);