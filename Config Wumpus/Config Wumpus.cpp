#include "Room.h";

#include "rapidjson-master/include/rapidjson/writer.h";
#include "rapidjson-master/include/rapidjson/stringbuffer.h";

using namespace rapidjson;

vector<Room> rooms;
string json = "[";

void InitializeRooms();
void WriteToFile(string filePath, string stringToWrite);

string RoomToJSON(Room room);

int main()
{
	string levelName;

	int roomBat[2] = { 0,0 };
	int roomPit[2] = { 0,0 };
	int roomWumpus = 0;

	InitializeRooms();

	cout << "Insert level name: ";
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

			if (rooms[roomPit[i] - 1].bat)
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

	for (int i = 0; i < rooms.size(); i++)
	{
		json += RoomToJSON(rooms[i]);

		if (i != rooms.size() - 1)
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
}

void InitializeRooms()
{
	for (int i = 0; i < 20; i++)
	{
		Room newRoom;
		newRoom.roomID = i + i;

		rooms.push_back(newRoom);
	}

	rooms[0].adjacentRooms = { 2,3,4 };
	rooms[1].adjacentRooms = { 1,3,5 };
	rooms[2].adjacentRooms = { 1,2,4 };
	rooms[3].adjacentRooms = { 1,3,7 };
	rooms[4].adjacentRooms = { 2,6,8 };
	rooms[5].adjacentRooms = { 5,7,9 };
	rooms[6].adjacentRooms = { 4,6,10 };
	rooms[7].adjacentRooms = { 5,9,11 };
	rooms[8].adjacentRooms = { 8,6,10 };
	rooms[9].adjacentRooms = { 7,9,13 };
	rooms[10].adjacentRooms = { 8,12,14 };
	rooms[11].adjacentRooms = { 11,13,15 };
	rooms[12].adjacentRooms = { 10,12,16 };
	rooms[13].adjacentRooms = { 11,15,17 };
	rooms[14].adjacentRooms = { 12,14,16 };
	rooms[15].adjacentRooms = { 13,15,19 };
	rooms[16].adjacentRooms = { 14,18,20 };
	rooms[17].adjacentRooms = { 17,19,20 };
	rooms[18].adjacentRooms = { 16,18,20 };
	rooms[19].adjacentRooms = { 17,18,19 };
}
void WriteToFile(string filePath, string stringToWrite)
{
	ofstream myFile;

	myFile.open(filePath);
	myFile << stringToWrite;
	myFile.close();
}

string RoomToJSON(Room room)
{
	StringBuffer s;
	Writer<StringBuffer> writer(s);

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

	return s.GetString();
}