#include "Room.hpp"

room playerPlace;
vector<room> rooms = {};

void FillRoomVector(vector<room>& roomVec)
{
	ifstream file("config.json");		//stream
	IStreamWrapper wrapper(file);		//wrapper

	Document configFile;
	configFile.ParseStream(wrapper);	//parse file

	vector<int> adjacentRoomsConf;
	for (unsigned int x = 0; x < configFile.Size(); x++) {
		for (Value& y : configFile[x]["adjacentRooms"].GetArray()) {
			//maak vector aan met values van json file
			adjacentRoomsConf.push_back(y.GetInt());
		}
		roomVec.push_back({						//maak nieuwe room object met json values en zet die in de vector
			configFile[x]["roomID"].GetInt(),	//roomID
			adjacentRoomsConf,					//adjacentRooms
			configFile[x]["bat"].GetBool(),		//bat
			configFile[x]["pit"].GetBool(),		//pit
			configFile[x]["wumpus"].GetBool()	//wumpus
			});
		adjacentRoomsConf = {};
	}
}

void Intro(room & startRoom)
{
	cout << "Welkom bij 'Hunt the Wumpus'! " << endl << endl;

	cout << "De Wumpus leeft in een grot met 20 kamers. Elke kamer heeft 3 tunnels die leiden naar andere kamers. " << endl << endl;

	cout << "Gevaren: " << endl;
	cout << "De bodemloze put: twee kamers hebben een bodemloze put. " << endl;
	cout << "Als je in deze kamer komt val je in deze put (en verlies je meteen). " << endl << endl;

	cout << "Super vleermuizen: twee kamers hebben super vleermuizen. " << endl;
	cout << "Als je de super vleermuizen tegen komt in een kamer word je naar een random kamer gebracht door de super vleermuizen." << endl;
	cout << "Nadat jij naar een andere kamer bent gezet, veranderen de super vleermuizen ook van kamer!" << endl << endl;

	cout << "De Wumpus" << endl;
	cout << "De wumpus is immuun voor de gevaren in de kamers. " << endl;
	cout << "Over het algemeen slaapt de Wumpus, maar de Wumus is ook wel eens wakker!" << endl;
	cout << "De wumpus wordt wakker als je een pijl schiet OF als jj zijn kamer binnen komt." << endl;
	cout << "Als de Wumpus wakker is verplaatst hij zich naar een andere kamer." << endl;
	cout << "Als dit dezelfde kamer is als waar jij in staat (of als jij zijn kamer binnekomt) eet hij je op en verlies je." << endl << endl;

	cout << "De speler" << endl;
	cout << "Elke zet mag je lopen OF schieten" << endl;
	cout << "Lopen: ja kan naar een van de drie aangegeven kamers verplaatsen." << endl;
	cout << "Schieten: ja kan kiezen hoeveel pijlen je schiet (1t/m5)." << endl;
	cout << "Je moet voor elke losse pijl aangeven naar welke kamer je hem wil schieten " << endl;
	cout << "Als de gekozen kamer niks oplevert (als de Wumpus daar niet zit), kaatst de pijl naar 2 random kamers. " << endl;
	cout << "De pijl kan ook op je eigen kamer komen. Dan ben je af. " << endl << endl;

	cout << "De game is afgelopen als je de Wumpus weet te raken met je pijlen. Veel succes!" << endl << endl;

	RoomSelection(startRoom);
}

void RoomSelection(room& currentRoom) 
{
	int whichRoom = -1;

	while (true)
	{
		cout << "Je zit in kamer " << playerPlace.roomID << endl;
		cout << "De gangen lijden naar " << currentRoom.adjacentRooms[0]
			 << " " << currentRoom.adjacentRooms[1] << " " << currentRoom.adjacentRooms[2] << endl;

		cout << "Naar welke gang?" << endl;

		cin >> whichRoom;

		cout << endl;

		if (cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input Moet een nummer zijn. Probeer het nog een keer" << endl << endl;
			continue;
		}

		if (!(unsigned(whichRoom) > rooms.size()) && (whichRoom == currentRoom.adjacentRooms[0] || whichRoom == currentRoom.adjacentRooms[1] || whichRoom == currentRoom.adjacentRooms[2]))
		{
			break;
		}
		
		cout << "Input is ongeldig. Probeer het nog een keer" << endl << endl;
	}

	playerPlace = rooms[whichRoom - 1];

	EnterRoom(playerPlace);
}

void RandomBatRoom(room& playerPlace)
{
	cout << "Je bent in een kamer gekomen met vleermuizen!\n";
	cout << "De vleermuizen brengen je naar een nieuwe kamer.\n\n";
	srand((unsigned)time(NULL));
	int randomRoom = rand() % 8;
	while(randomRoom == (playerPlace.roomID - 1)) {
		randomRoom = rand() % 8;
	}
	playerPlace = rooms[randomRoom];
}

void EnterRoom(room & room) 
{
	if (playerPlace.bat) {
		RandomBatRoom(room);
	}
	RoomSelection(room);
}

int main()
{
	FillRoomVector(rooms);

	playerPlace = rooms[0];

	Intro(playerPlace);

	return 0;
}