#pragma once

bool GameStart(player& currentRoom);
void GameOver(player& currentRoom, const bool& gameWon);

void PlayerInteraction(player& currentRoom);
void PlayerShoot(player& currentRoom);