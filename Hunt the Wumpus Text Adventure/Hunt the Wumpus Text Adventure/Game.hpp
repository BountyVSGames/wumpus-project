#pragma once

bool GameStart(player& currentPlayer, const string fileName);
void GameOver(player& currentRoom, const bool& gameWon);

void PlayerInteraction(player& currentRoom);
void PlayerShoot(player& currentRoom);