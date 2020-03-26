#include <iostream>
#include <vector>
using namespace std;

struct room {
	int roomID;
	vector<int> adjacentRooms;
	bool bat = false;
	bool pit = false;
};

room room1{ 1, { 1, 3, 4 } };
room room2{ 2, { 2, 4, 5 } };
room room3{ 3, { 2, 3, 5 } };
room room4{ 4, { 3, 4, 6 } };
room room5{ 5, { 5, 7, 8 } };
room room6{ 6, { 1, 6, 8 } };
room room7{ 7, { 1, 6, 7 } };
room room8{ 8, { 2, 7, 8 } };

int main() {
	return 0;
}