#include <iostream>
#include <vector>
using namespace std;

struct room {
	int roomID;
	vector<int> adjacentRooms;
	bool bat = false;
	bool pit = false;
};

room room1{ 1, { 8, 2, 3 } };
room room2{ 2, { 1, 3, 4 } };
room room3{ 3, { 1, 2, 4 } };
room room4{ 4, { 2, 3, 5 } };
room room5{ 5, { 4, 6, 7 } };
room room6{ 6, { 5, 7, 8 } };
room room7{ 7, { 5, 6, 8 } };
room room8{ 8, { 1, 6, 7 } };
vector<room> rooms = {room1, room2 , room3 ,room4, room5, room6, room7, room8};

int main() {
	return 0;
}