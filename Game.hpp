#ifndef GAME_HPP
#define GAME_HPP

#include <iostream>;

using namespace std;

void GameStart();
void GameOver(int winState);

void PlayerShoot(int numberOfRooms);
void PlayerMove(int roomNumber);

#endif // GAMEHPP
