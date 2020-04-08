#include "AI.hpp";

INPUT ip;

void AIMove(playerstruct currentRoom)
{

}

void AIShoot(playerstruct currentRoom)
{

}

void AIPrepareInput()
{
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;
}
void AIInputPressDown(char inputKey)
{
	AIPrepareInput();

	ip.ki.wVk = inputKey; // virtual-key code for the "a" key
	ip.ki.dwFlags = 0; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));
}
void AIInputPressUp(char inputKey)
{
	AIPrepareInput();

	ip.ki.wVk = inputKey; // virtual-key code for the "a" key
	ip.ki.dwFlags = KEYEVENTF_KEYUP; // 0 for key press
	SendInput(1, &ip, sizeof(INPUT));
}