#include "Room.h";

vector<room> map;
string json = "[";	//waarom?

void InitializeRooms();
void WriteToFile(string filePath, string stringToWrite);

string RoomToJSON(room room);

void RemoveUnavailableRooms(vector<room>& availableRooms)
{
	for (unsigned int x = 0; x != availableRooms.size(); x++)
	{
		if (availableRooms[x].openPositions.size() == 1)
		{
			cout << "\nErased room " << availableRooms[x].roomID << " from available rooms\n\n";
			availableRooms.erase(availableRooms.begin() + x);
			x--;
		}
	}
}

bool PositionAvailable(const vector<room>& map, const vector<int>& rootRoomPosition, const int& newRoomDirection)
{
	//kijkt of de positie van de nieuwe room kan
	for (unsigned int x = 0; x != map.size(); x++)
	{
		if (newRoomDirection == 0 && map[x].gridPosXY[0] == rootRoomPosition[0] + 1 && map[x].gridPosXY[1] == rootRoomPosition[1] ||
			newRoomDirection == 1 && map[x].gridPosXY[1] == rootRoomPosition[1] + 1 && map[x].gridPosXY[0] == rootRoomPosition[0] ||
			newRoomDirection == 2 && map[x].gridPosXY[0] == rootRoomPosition[0] - 1 && map[x].gridPosXY[1] == rootRoomPosition[1] ||
			newRoomDirection == 3 && map[x].gridPosXY[1] == rootRoomPosition[1] - 1 && map[x].gridPosXY[0] == rootRoomPosition[0])
		{
			cout << "botsing gevonden!\n";
			return false;
		}
	}
	return true;
}

void ConnectAdjacentRoom(vector<room>& map, vector<room>& availableRooms, room& newRoom)
{
	int offset = 99;		//offset voor richting
	int axis = 0;			//as (x/y)
	int otherAxis = 1;		//andere as (x/y)
	int reverseOfOpenPos;
	for (unsigned int x = 0; x != availableRooms.size(); x++)
	{
		for (int y = 0; y != newRoom.openPositions.size(); y++)
		{
			switch (newRoom.openPositions[y])
			{
			case 0:
				offset = 1;
				axis = 0;
				otherAxis = 1;
				reverseOfOpenPos = 2;
				break;
			case 1:
				offset = 1;
				axis = 1;
				otherAxis = 0;
				reverseOfOpenPos = 3;
				break;
			case 2:
				offset = -1;
				axis = 0;
				otherAxis = 1;
				reverseOfOpenPos = 0;
				break;
			case 3:
				offset = -1;
				axis = 1;
				otherAxis = 0;
				reverseOfOpenPos = 1;
				break;
			}
			if (newRoom.gridPosXY[axis] + offset == availableRooms[x].gridPosXY[axis] && newRoom.gridPosXY[otherAxis] == availableRooms[x].gridPosXY[otherAxis] &&
				find(availableRooms[x].openPositions.begin(), availableRooms[x].openPositions.end(), reverseOfOpenPos) != availableRooms[x].openPositions.end())
			{
				//maak connectie als newRoom een adjacent room heeft uit available rooms op de grid, en als de richting open is.
				availableRooms[x].openPositions.erase(find(availableRooms[x].openPositions.begin(), availableRooms[x].openPositions.end(), reverseOfOpenPos));
				availableRooms[availableRooms.size() - 1].openPositions.erase(newRoom.openPositions.begin() + y);

				//erase iterator van reverseOfOpenPos voor de availableRoom[x].openPositions en erase iterator y van newRoom.openPositions
				map[availableRooms[x].roomID].adjacentRooms.push_back(newRoom.roomID);
				map[map.size() - 1].adjacentRooms.push_back(availableRooms[x].roomID);

				cout << "\nConnectie gemaakt tussen room " << newRoom.roomID << " en room " << availableRooms[x].roomID << "!\n\n";
				y--;
			}
		}
	}
	RemoveUnavailableRooms(availableRooms);
}

void TieLooseEnd(vector<room>& map, vector<room>& availableRooms)
{
	srand(unsigned(time(NULL)));
	int tieStart = 0;
	int tieEnd = 0;
	for (unsigned int x = 0; x != availableRooms.size(); x++)
	{
		//room met meeste open posities heeft voorang bij het verbinden
		if (availableRooms[tieStart].openPositions.size() < availableRooms[x].openPositions.size())
		{
			tieStart = x;
		}
	}

	int randomRoomToTie = (rand() % (availableRooms.size() - 1)) + 1;	//andere room verbinding is random

	for (unsigned int x = 0; x != map[availableRooms[tieStart].roomID].adjacentRooms.size(); x++)
	{
		if (map[availableRooms[tieStart].roomID].adjacentRooms[x] == availableRooms[randomRoomToTie].roomID || availableRooms[tieStart].roomID == availableRooms[randomRoomToTie].roomID)
		{
			return;
		}
	}
	cout << "\nTied room " << availableRooms[tieStart].roomID << " with room " << availableRooms[randomRoomToTie].roomID << "!\n\n";

	//remove een open positie voor beide rooms
	availableRooms[tieStart].openPositions.pop_back();
	availableRooms[randomRoomToTie].openPositions.pop_back();

	//add adjacent rooms voor beide rooms
	map[availableRooms[tieStart].roomID].adjacentRooms.push_back(availableRooms[randomRoomToTie].roomID);
	map[availableRooms[randomRoomToTie].roomID].adjacentRooms.push_back(availableRooms[tieStart].roomID);
}

bool RandomRoomGeneration(vector<room>& map)
{
	srand(unsigned(time(NULL)));	//seed hangt af van runtime values

	//maak init room om andere rooms van te spawnen
	map.push_back({ 0, {}, {0, 0} });
	vector<room> availableRooms{ { 0, {}, {0, 0} } };

	int it = 0;

	for (int roomID = 1; roomID != 20; roomID++)
	{
		//maak de nieuwe room aan een random kant vast van een random room in available rooms
		int randRoom = rand() % availableRooms.size();
		int randPos = rand() % availableRooms[randRoom].openPositions.size();
		int newRoomDirection = availableRooms[randRoom].openPositions[randPos];
		vector<int> rootRoomPosition = availableRooms[randRoom].gridPosXY;

		while (!PositionAvailable(map, rootRoomPosition, newRoomDirection))
		{
			//als de positie van de nieuwe room al van een andere room is, dan wordt er een andere richting gekozen
			randPos = rand() % availableRooms[randRoom].openPositions.size();
			newRoomDirection = availableRooms[randRoom].openPositions[randPos];
			rootRoomPosition = availableRooms[randRoom].gridPosXY;
		}

		cout << "RandRoom = " << availableRooms[randRoom].roomID << ", direction = " << availableRooms[randRoom].openPositions[randPos] << endl;

		switch (newRoomDirection)
		{
			//transleer de openPos naar richting op de grid
		case 0:
			availableRooms.push_back({ roomID, {},
				{availableRooms[randRoom].gridPosXY[0] + 1, availableRooms[randRoom].gridPosXY[1]}, { 0, 1, 3 }
				});
			break;
		case 1:
			availableRooms.push_back({ roomID, {},
				{availableRooms[randRoom].gridPosXY[0], availableRooms[randRoom].gridPosXY[1] + 1}, { 0, 1, 2 }
				});
			break;
		case 2:
			availableRooms.push_back({ roomID, {},
				{availableRooms[randRoom].gridPosXY[0] - 1, availableRooms[randRoom].gridPosXY[1]}, { 1, 2, 3 }
				});
			break;
		case 3:
			availableRooms.push_back({ roomID, {},
				{availableRooms[randRoom].gridPosXY[0], availableRooms[randRoom].gridPosXY[1] - 1}, { 0, 2, 3 }
				});
			break;
		}
		room& rootRoom = availableRooms[randRoom];
		room& newRoom = availableRooms[availableRooms.size() - 1];

		rootRoom.openPositions.erase(rootRoom.openPositions.begin() + randPos);	//verwijder openPos die gebruikt is

		map.push_back(newRoom);													//voeg de room toe aan de map

		map[rootRoom.roomID].adjacentRooms.push_back(newRoom.roomID);			//voeg roomID toe aan adjacentRooms(nieuwe room) van root room
		map[map.size() - 1].adjacentRooms.push_back(rootRoom.roomID);			//voeg roomID toe aan adjacentRooms(root) van nieuwe room

		ConnectAdjacentRoom(map, availableRooms, newRoom);
	}
	while (!availableRooms.empty() && it < 300)
	{
		//stop met tie als aantal available rooms leeg is of als het te lang door gaat
		TieLooseEnd(map, availableRooms);
		RemoveUnavailableRooms(availableRooms);
		it += 1;
	}
	if (it == 300)
	{
		return false;
	}
	else
	{
		return true;
	}
}

void PrintMap(const vector<room>& map)
{
	for (unsigned int x = 0; x != map.size(); x++)
	{
		cout << "RoomID: " << map[x].roomID << endl << "adjacent Rooms: ";
		for (unsigned int y = 0; y != map[x].adjacentRooms.size(); y++)
		{
			cout << map[x].adjacentRooms[y] << ", ";
		}
		cout << "\n\n";
	}
}

int main()
{
	string levelName;

	int roomBat[2] = { 0,0 };
	int roomPit[2] = { 0,0 };
	int roomWumpus = 0;

	char option = '-1';
	while (option != 'R' && option != 'T')
	{
		cout << "Wil je een random map genereren of een template map gebruiken? (R/T) ";
		cin >> option;
	}
	if (option == 'R') {
		while (!RandomRoomGeneration(map))
		{
			cout << "\nmap is ongeldig, nieuwe map aan het genereren...\n\n";
			map = {};
		}
	}
	else
	{
		InitializeRooms();
	}
	cout << "\n-----------------------------------------------------------\n\nmap:\n\n";
	PrintMap(map);

	cout << "\nInsert level name: ";
	cin >> levelName;

	for (int i = 0; i < sizeof(roomBat); i++)
	{
		while (roomBat[i] == 0)
		{
			cout << "Insert room name that contains a bat (1-20): ";
			cin >> roomBat[i];

			if (roomBat[i] < 1 || roomBat[i] > 20)
			{
				cout << "Wrong value. Please insert a value between 1 and 20" << endl << endl;
				roomBat[i] = 0;
			}
		}
	}
	for (int i = 0; i < sizeof(roomPit); i++)
	{
		while (roomPit[i] == 0)
		{
			cout << "Insert room name that contains a pit (1-20): ";
			cin >> roomPit[i];

			if (map[roomPit[i] - 1].bat)
			{
				cout << "Room already has a bat. Can't contain both" << endl << endl;
			}
			else if (roomPit[i] < 1 || roomPit[i] > 20)
			{
				cout << "Wrong value. Please insert a value between 1 and 20" << endl << endl;
				roomPit[i] = 0;
			}
		}
	}

	while (roomWumpus == 0)
	{
		cout << "Insert room name that contains the wumpus (1-20): ";
		cin >> roomWumpus;
	}

	for (int i = 0; i < map.size(); i++)
	{
		json += RoomToJSON(map[i]);

		if (i != map.size() - 1)
		{
			json += ",";
		}
		else
		{
			json += "]";
		}
	}

	WriteToFile(levelName + ".json", json);

	cout << levelName + ".json" << " succesfully generated";

	return 0;
}

void InitializeRooms()
{
	for (int i = 0; i < 20; i++)
	{
		room newRoom;
		newRoom.roomID = i;

		map.push_back(newRoom);
	}

	//template map
	map[0].adjacentRooms = { 2,3,4 };
	map[1].adjacentRooms = { 1,3,5 };
	map[2].adjacentRooms = { 1,2,4 };
	map[3].adjacentRooms = { 1,3,7 };
	map[4].adjacentRooms = { 2,6,8 };
	map[5].adjacentRooms = { 5,7,9 };
	map[6].adjacentRooms = { 4,6,10 };
	map[7].adjacentRooms = { 5,9,11 };
	map[8].adjacentRooms = { 8,6,10 };
	map[9].adjacentRooms = { 7,9,13 };
	map[10].adjacentRooms = { 8,12,14 };
	map[11].adjacentRooms = { 11,13,15 };
	map[12].adjacentRooms = { 10,12,16 };
	map[13].adjacentRooms = { 11,15,17 };
	map[14].adjacentRooms = { 12,14,16 };
	map[15].adjacentRooms = { 13,15,19 };
	map[16].adjacentRooms = { 14,18,20 };
	map[17].adjacentRooms = { 17,19,20 };
	map[18].adjacentRooms = { 16,18,20 };
	map[19].adjacentRooms = { 17,18,19 };
}

void WriteToFile(string filePath, string stringToWrite)
{
	ofstream myFile;

	myFile.open(filePath);
	myFile << stringToWrite;
	myFile.close();
}

string RoomToJSON(room room)
{
	StringBuffer sBuf;
	Writer<StringBuffer> writer(sBuf);

	writer.StartObject();

	writer.Key("Room ID");
	writer.Uint(room.roomID);

	writer.Key("Adjacent Rooms");
	writer.StartArray();

	for (int i = 0; i < room.adjacentRooms.size(); i++)
	{
		writer.Uint(room.adjacentRooms[i]);
	}

	writer.EndArray();

	writer.Key("Bat");
	writer.Bool(room.bat);
	writer.Key("Pit");
	writer.Bool(room.pit);
	writer.Key("Wumpus");
	writer.Bool(room.wumpus);

	writer.EndObject();

	return sBuf.GetString();
}