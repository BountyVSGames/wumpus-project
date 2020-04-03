#include "Room.hpp"
#include "Game.hpp";
#include "rapidjson-master/include/rapidjson/reader.h"

room playerPlace;

int amountArrows = 5;

vector<room> rooms = {};

void FillRoomVector(vector<room>& roomVec)
{
	ifstream file("config.json");		//stream
	IStreamWrapper wrapper(file);		//wrapper

	Document configFile;
	configFile.ParseStream(wrapper);	//parse file

	vector<int> adjacentRoomsConf;
	for (unsigned int x = 0; x < configFile.Size(); x++)
	{
		for (Value& y : configFile[x]["adjacentRooms"].GetArray())
		{
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
			cout << "Input is ongeldig. Moet een nummer zijn. Probeer het nog een keer" << endl << endl;
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
void PlayerInteraction()
{
	char playerChoose = 'A'; //Temp value A

	while (playerChoose != 'S' && playerChoose != 'M')
	{
		cout << "Je zit in kamer " << playerPlace.roomID << endl;
		cout << "Wil je schieten of lopen" << endl;
		cout << "(Type 'S' voor schieten en 'M' voor lopen)" << endl;

		cin >> playerChoose;
	}

	switch (playerChoose)
	{
	case 'S':
		PlayerShoot();
		break;
	case 'M':
		RoomSelection(playerPlace);
		break;
	}
}

void WumpusRoom(room& room) {
	if (playerPlace.wumpus) {
		cout << "Je bent tegen de Wumpus aangelopen...hij is boos en heeft je de grond in geslagen" << endl;
		cout << endl << "Druk op de [ENTER] om opnieuw te beginnen" << endl;
		cin.ignore();
		cin.get();
		cout << "----------------------------------------------------------" << endl << endl;
		//TODO: Game Over
		playerPlace = rooms[0];
		Intro(playerPlace);
	}
	else {
		bool found = false;
		for (unsigned int x = 0; x != 3; x++) {
			int adjacentRoomIndexX = rooms[room.adjacentRooms[x] - 1].roomID - 1;
			for (unsigned int y = 0; y != 3; y++) {
				int adjacentRoomIndexY = rooms[adjacentRoomIndexX].adjacentRooms[y] - 1;
				if (rooms[adjacentRoomIndexY].wumpus) {
					cout << "Je ruikt de wumpus...\n";
					found = true;
				}
			}
			if (rooms[adjacentRoomIndexX].wumpus) {
				cout << "Je ruikt de wumpus...\n";
				found = true;
			}
			if (found) { break; }
		}
	}
}

void PlayerShoot()
{
	int which_room = -1;
	int number_rooms = -1;

	cout << "Je hebt nog " << amountArrows << " pijlen." << endl;

	while (number_rooms < 1 || number_rooms > amountArrows) {
		cout << "Hoeveel tunnels wil je doorschieten (1 tot " << amountArrows << ")?";
		cin >> number_rooms;
	}

	while (which_room < 1 || which_room > rooms.size())
	{
		cout << "kamer: ";
		cin >> which_room;

		if (which_room < 1 || which_room > rooms.size())
		{
			continue;
		}

		if (rooms[which_room - 1].wumpus)
		{
			cout << "Gefeliciteerd je hebt de Wumpus verslagen!" << endl;
			//GAME OVER
		}
		cout << "Je hebt gemist en bent nu een pijl kwijt." << endl;
		amountArrows -= 1;
		if (amountArrows == 0)
		{
			cout << "Je pijlen zijn op je hebt verloren.";
			//GAMEOVER
		}
		cout << "Je hebt nog " << amountArrows << " pijlen over" << endl << endl;
	}

	PlayerInteraction();
}

void Intro(room& startRoom)
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

	PlayerInteraction();
}

void RandomBatRoom(room& playerPlace)
{
	cout << "Je bent in een kamer gekomen met vleermuizen!\n";
	cout << "De vleermuizen brengen je naar een nieuwe kamer.\n\n";
	srand((unsigned)time(NULL));
	int randomRoom = rand() % rooms.size();
	while (randomRoom == (playerPlace.roomID - 1)) {
		randomRoom = rand() % rooms.size();
	}
	playerPlace = rooms[randomRoom];
}

void PitRoom(room& room)
{
	cout << "Je bent in een bodemloze put gevallen...probeer het nog een keer!" << endl;
	cout << endl << "Druk op de [ENTER] om opnieuw te beginnen" << endl;
	cin.ignore();
	cin.get();
	cout << "----------------------------------------------------------" << endl << endl;
	playerPlace = rooms[0]; //TODO: Game Over
	Intro(playerPlace);
}

void EnterRoom(room& room)
{
	if (playerPlace.pit)
	{
		PitRoom(room);
	}
	else if (playerPlace.bat)
	{
		RandomBatRoom(room);
	}

	WumpusRoom(room);

	PlayerInteraction();
}

int RandomRoom()
{
	srand((unsigned)time(0));
	int randomRoom = rand() % 8 + 1;

	for (unsigned int i = 0; i < 20; i++) {
		if (rooms[randomRoom - 1].pit == true || rooms[randomRoom - 1].roomID == 1) {
			randomRoom = rand() % 8 + 1;
		}
		else {
			return randomRoom;
		}
	}
}

int main()
{
	FillRoomVector(rooms);
	//RandomRoom();
	playerPlace = rooms[0];
	Intro(playerPlace);
	return 0;
}