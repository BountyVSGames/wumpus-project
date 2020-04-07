#include "Room.hpp"
#include "Game.hpp"

vector<room> rooms = {};

void FillRoomVector(vector<room>& roomVec, const string& fileName)
{
	ifstream file(fileName);		//stream
	IStreamWrapper wrapper(file);		//wrapper

	Document configFile;
	configFile.ParseStream(wrapper);	//parse file

	vector<int> adjacentRoomsConf;
	for (unsigned int x = 0; x < configFile.Size(); x++)
	{
		for (Value& y : configFile[x]["Adjacent Rooms"].GetArray())
		{
			//maak vector aan met values van json file
			adjacentRoomsConf.push_back(y.GetInt());
		}
		roomVec.push_back({						//maak nieuwe room object met json values en zet die in de vector
			configFile[x]["Room ID"].GetInt(),	//roomID
			adjacentRoomsConf,					//adjacentRooms
			configFile[x]["Bat"].GetBool(),		//bat
			configFile[x]["Pit"].GetBool(),		//pit
			configFile[x]["Wumpus"].GetBool()	//wumpus
			});
		adjacentRoomsConf = {};
	}
}

void WumpusRoom(player& currentPlayer)
{
	if (currentPlayer.currentRoom.wumpus) {
		cout << "Je bent tegen de Wumpus aangelopen...hij is boos en heeft je de grond in geslagen\n\n";
		GameOver(currentPlayer, false);
		return;
	}
	bool found = false;
	for (unsigned int x = 0; x != 3; x++) {
		int adjacentRoomIndexX = rooms[currentPlayer.currentRoom.adjacentRooms[x]].roomID;
		for (unsigned int y = 0; y != 3; y++) {
			int adjacentRoomIndexY = rooms[adjacentRoomIndexX].adjacentRooms[y];
			if (rooms[adjacentRoomIndexY].wumpus) {
				found = true;
				break;
			}
		}
		if (rooms[adjacentRoomIndexX].wumpus) {
			found = true;
		}
		if (found) { cout << "Je ruikt de wumpus...\n"; break; }
	}
}

void PlayerShoot(player& currentPlayer)
{
	int whichRoom = -1;
	int numberRooms = -1;

	cout << "Je hebt nog " << currentPlayer.arrows << " pijlen." << endl;

	while (numberRooms < 1 || numberRooms > currentPlayer.arrows)
	{
		cout << "Hoeveel tunnels wil je doorschieten (1 tot " << currentPlayer.arrows << ")?";
		cin >> numberRooms;
	}

	while (true)
	{
		cout << "kamer: ";
		cin >> whichRoom;
		whichRoom--;

		if (whichRoom < 0 || whichRoom >= rooms.size()) {
			cout << "Die kamer bestaat niet\n";
			continue;
		}
		if (rooms[whichRoom].wumpus)
		{
			cout << "Gefeliciteerd je hebt de Wumpus verslagen!\n\n";
			GameOver(currentPlayer, true);
			return;
		}
		cout << "Je hebt gemist en bent nu een pijl kwijt." << endl;
		currentPlayer.arrows -= 1;
		if (currentPlayer.arrows == 0)
		{
			cout << "Je pijlen zijn op, je hebt verloren.\n\n";
			GameOver(currentPlayer, false);
			return;
		}
		cout << "Je hebt nog " << currentPlayer.arrows << " pijlen over" << endl << endl;
	}
}

void RoomSelection(player& currentPlayer)
{
	int whichRoom = -1;

	while (true)
	{
		cout << "De gangen lijden naar " << (currentPlayer.currentRoom.adjacentRooms[0] + 1)
			<< " " << (currentPlayer.currentRoom.adjacentRooms[1] + 1) << " " << (currentPlayer.currentRoom.adjacentRooms[2] + 1) << endl;

		cout << "Naar welke gang? ";

		cin >> whichRoom;
		whichRoom--;

		cout << '\n';

		if (cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input is ongeldig. Moet een nummer zijn. Probeer het nog een keer\n\n";
			continue;
		}

		if (whichRoom == currentPlayer.currentRoom.adjacentRooms[0] ||
			whichRoom == currentPlayer.currentRoom.adjacentRooms[1] ||
			whichRoom == currentPlayer.currentRoom.adjacentRooms[2])
		{
			break;
		}

		cout << "Input is ongeldig, Probeer het nog een keer.\n\n";
	}

	currentPlayer.currentRoom = rooms[whichRoom];

	EnterRoom(currentPlayer);
}

void PlayerInteraction(player& currentPlayer)
{
	char playerChoose = '1';	//Temp value A

	while (playerChoose != 'S' && playerChoose != 'M')
	{
		cout << "Je zit in kamer " << (currentPlayer.currentRoom.roomID + 1) << endl;
		cout << "Wil je schieten of lopen (S/M) ";

		cin >> playerChoose;

		cout << '\n';
	}

	switch (playerChoose)
	{
	case 'S':
		PlayerShoot(currentPlayer);
		break;
	case 'M':
		RoomSelection(currentPlayer);
		break;
	}
}

void Intro()
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
}

void RandomBatRoom(player& currentPlayer)
{
	srand((unsigned)time(NULL));	//maak seed aan gebasseerd op runtime zodat deze uniek is

	int randomRoom = rand() % rooms.size();
	while (randomRoom == currentPlayer.currentRoom.roomID) {
		//kies een random integer todat deze integer niet gelijk staat aan de kamer waar de player al in zit
		randomRoom = rand() % rooms.size();
	}

	currentPlayer.currentRoom = rooms[randomRoom];	//verander de current room van de player naar de nieuwe random room

	cout << "Je bent in een kamer gekomen met vleermuizen!\nDe vleermuizen brengen je naar kamer " << (currentPlayer.currentRoom.roomID + 1) << "!\n\n";
}

void EnterRoom(player& currentPlayer)
{
	if (currentPlayer.currentRoom.bat)
	{
		//als bat = true bij de kamer waar de player in zit, roep dan RandomBatRoom() aan
		RandomBatRoom(currentPlayer);
	}
	else if (currentPlayer.currentRoom.pit)
	{
		//als pit = true bij huidige kamer roep dan GameOver() aan
		cout << "Je bent in een bodemloze put gevallen...\n\n";
		GameOver(currentPlayer, false);
		return;
	}

	WumpusRoom(currentPlayer);
}

int RandomRoom()
{
	//niet in gebruik
	srand((unsigned)time(NULL));
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

void GameOver(player& currentPlayer, const bool& gameWon)
{
	cout << "-------------------------------------------------------------\n\n";
	currentPlayer.gameOver = true;
	if (!gameWon)
	{
		char option = '1';
		cout << "Wil je weten waar de Wumpus zat? (Y) ";
		cin >> option;
		if (option == 'Y')
		{
			for (unsigned int x = 0; x != rooms.size(); x++)
			{
				if (rooms[x].wumpus)
				{
					cout << "De Wumpus zat in room " << (rooms[x].roomID + 1) << "!\n";
				}
			}
		}
	}
}

bool GameStart(player& currentPlayer)
{
	Intro();
	while (!currentPlayer.gameOver)	 //gameloop
	{
		PlayerInteraction(currentPlayer);
	}
	char option = '1';
	while (option != 'Y' && option != 'N')
	{
		cout << "Wil je opnieuw spelen? (Y/N) ";
		cin >> option;
	}
	switch (option)
	{
	case 'Y':
		cout << "\n----------------------------------------------------------" << endl << endl;
		currentPlayer.currentRoom = rooms[0];
		currentPlayer.gameOver = false;
		return true;
	case 'N':
		return false;
	}
}

bool MenuScreen() {
	system("CLS");	//leeg console
	char choice = '0';
	cout << " ___________________________________________________________________" << endl;
	cout << "|                         HUNT THE WUMPUS                           |" << endl;
	cout << "|                                                                   |" << endl;
	cout << "|                                                                   |" << endl;
	cout << "|                         Kies een optie(1, 2, 3):                  |" << endl;
	cout << "|    _________________   ___________________   _________________    |" << endl;
	cout << "|   |                 | |                   | |                 |   |" << endl;
	cout << "|   | 1. spelen       | | 2. Leaderboards   | | 3.     AI       |   |" << endl;
	cout << "|   |_________________| |___________________| |_________________|   |" << endl;
	cout << "|___________________________________________________________________|" << endl;

	cin >> choice;

	system("CLS");

	if (choice == '1') {

		FillRoomVector(rooms, "config.json");
		player player{ rooms[0] };

		while (GameStart(player)) {}
		return true;
	}
	else if (choice == '2') {
		cout << "Hier komt het leaderboard" << endl;
		return true;
	}
	else if (choice == '3') {
		cout << "Hier komt de aanroep van de AI" << endl;
		return true;
	}
	else {
		cout << "Je hebt een foute invoer gegeven!";
		return false;
	}
}

int main()
{
	while(MenuScreen());
	return 0;
}