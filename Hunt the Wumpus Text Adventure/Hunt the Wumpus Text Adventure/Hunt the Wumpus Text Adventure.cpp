#include "Room.hpp"
#include "AI.hpp"

vector<room> rooms = {};
string currentLine = "";

playerstruct player;

mutex mx;
condition_variable currentLineReady;

vector<int> numpadKeyInput =
{
	0x30,
	0x31,
	0x32,
	0x33,
	0x34,
	0x35,
	0x35,
	0x37,
	0x38,
	0x39
};

auto g_lock()
{
	static std::mutex m; // a global living mutyex
	return std::unique_lock<decltype(m)>(m); // RAII based lock
}

bool IsIntInVectorRoom(vector<room> vector, int value)
{
	for (int i = 0; i < vector.size(); i++)
	{
		if (vector[i].roomID == value)
		{
			return true;
		}
	}

	return false;
}
vector<int> IntDevider(int number)
{
	vector<int> returnValue = {};

	int index = 0;

	while (number > 0)
	{
		int digit = number % 10;
		number /= 10;

		returnValue.push_back(digit);

		index++;
	}

	if (returnValue.size() == 2)
	{
		int tempValue = returnValue[1];
		returnValue[1] = returnValue[0];
		returnValue[0] = tempValue;
	}

	return returnValue;
}

void PlayerThread()
{
	player.currentRoom = rooms[15];

	while (GameStart(player))
	{
		player.gameOver = false;
	}
}
void AIThread()
{
	aiplayer aiPlayer;

	while (true)
	{
		unique_lock<mutex> lock(mx);
		currentLineReady.wait(lock);

		//cout << currentLine.find("tunnels wil je doorschieten");

		if (currentLine.find("schieten of lopen") == 7)
		{
			if (aiPlayer.roomsVisited.size() == 0 || !(IsIntInVectorRoom(aiPlayer.roomsVisited, player.currentRoom.roomID)))
			{
				aiPlayer.roomsVisited.push_back(player.currentRoom);
			}

			if (aiPlayer.possibleWumpusRooms.size() > 0)
			{
				AIInputPressDown(0x10); //Shift
				AIInputPressDown(0x53); //S
				AIInputPressDown(0x0D); //Enter

				AIInputPressUp(0x10); //Shift
				AIInputPressUp(0x53); //S
				AIInputPressUp(0x0D); //Enter
			}
			else
			{
				AIInputPressDown(0x10); //Shift
				AIInputPressDown(0x4D); //M
				AIInputPressDown(0x0D); //Enter

				AIInputPressUp(0x10); //Shift
				AIInputPressUp(0x4D); //M
				AIInputPressUp(0x0D); //Enter
			}
		}
		else if(currentLine.find("Naar welke gang?") == 0)
		{
			bool notVisitedRoom = false;
			int notVisitedRoomId = 0;

			for (int i = 0; i < player.currentRoom.adjacentRooms.size(); i++)
			{
				for (int j = 0; j < aiPlayer.roomsVisited.size(); j++)
				{
					if (aiPlayer.roomsVisited[j].roomID != player.currentRoom.adjacentRooms[i] && j == (aiPlayer.roomsVisited.size() - 1))
					{
						notVisitedRoomId = player.currentRoom.adjacentRooms[i];
						notVisitedRoom = true;

						break;
					}
					else if(aiPlayer.roomsVisited[j].roomID == player.currentRoom.adjacentRooms[i])
					{
						break;
					}
				}

				if (notVisitedRoom)
				{
					break;
				}
			}

			vector<int> splittedRoomID = IntDevider(notVisitedRoomId);

			if (splittedRoomID.size() == 1)
			{
				AIInputPressDown(numpadKeyInput[splittedRoomID[0]]);
				AIInputPressDown(0x0D);

				AIInputPressUp(numpadKeyInput[splittedRoomID[0]]);
				AIInputPressUp(0x0D);
			}
			else if (splittedRoomID.size() > 1)
			{
				for (int i = 0; i < splittedRoomID.size(); i++)
				{
					AIInputPressDown(numpadKeyInput[splittedRoomID[i]]);
				}
				AIInputPressDown(0x0D);

				for (int i = 0; i < splittedRoomID.size(); i++)
				{
					AIInputPressUp(numpadKeyInput[splittedRoomID[i]]);
				}

				AIInputPressUp(0x0D);
			}
		}
	}
}

void PrintLineToConsole(string print)
{
	unique_lock<mutex> lock(mx);
	
	currentLine = "";
	cout << print;
	currentLine = print;

	currentLineReady.notify_all();
}

void FillRoomVector(vector<room>& roomVec, const string& fileName)
{
	ifstream file(fileName);		//stream
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
void WumpusRoom(playerstruct& currentPlayer)
{
	if (currentPlayer.currentRoom.wumpus) {
		PrintLineToConsole("Je bent tegen de Wumpus aangelopen...hij is boos en heeft je de grond in geslagen\n\n");
		GameOver(currentPlayer, false);
		return;
	}
	else {
		bool found = false;
		for (unsigned int x = 0; x != 3; x++) {
			int adjacentRoomIndexX = rooms[currentPlayer.currentRoom.adjacentRooms[x] - 1].roomID - 1;
			for (unsigned int y = 0; y != 3; y++) {
				int adjacentRoomIndexY = rooms[adjacentRoomIndexX].adjacentRooms[y] - 1;
				if (rooms[adjacentRoomIndexY].wumpus) {
					PrintLineToConsole("Je ruikt de wumpus...\n");
					found = true;
					break;
				}
			}
			if (rooms[adjacentRoomIndexX].wumpus) {
				PrintLineToConsole("Je ruikt de wumpus...\n");
				found = true;
			}
			if (found) { break; }
		}
	}
}

void PlayerShoot(playerstruct& currentPlayer)
{
	int whichRoom = -1;
	int numberRooms = -1;

	PrintLineToConsole("Je hebt nog " + to_string(currentPlayer.arrows) + " pijlen.\n");

	while (numberRooms < 1 || numberRooms > currentPlayer.arrows) {
		PrintLineToConsole("Hoeveel tunnels wil je doorschieten (1 tot " + to_string(currentPlayer.arrows) + ")?");
		cin >> numberRooms;
	}

	while (true)
	{
		PrintLineToConsole("Welke kamer: ");
		cin >> whichRoom;
		if (whichRoom < 1 || whichRoom > rooms.size()) {
			cout << "Die kamer bestaat niet\n";
			continue;
		}
		if (rooms[whichRoom - 1].wumpus)
		{
			PrintLineToConsole("Gefeliciteerd je hebt de Wumpus verslagen!\n\n");
			GameOver(currentPlayer, true);
			return;
		}
		PrintLineToConsole("Je hebt gemist en bent nu een pijl kwijt.\n");
		currentPlayer.arrows -= 1;
		if (currentPlayer.arrows == 0)
		{
			PrintLineToConsole("Je pijlen zijn op, je hebt verloren.\n\n");
			GameOver(currentPlayer, false);
			return;
		}
		PrintLineToConsole("Je hebt nog " + to_string(currentPlayer.arrows) + " pijlen over\n\n");
	}
}
void RoomSelection(playerstruct& currentPlayer)
{
	int whichRoom = -1;

	while (true)
	{
		PrintLineToConsole("De gangen lijden naar " + to_string(currentPlayer.currentRoom.adjacentRooms[0])
			+ " " + to_string(currentPlayer.currentRoom.adjacentRooms[1]) + " " + to_string(currentPlayer.currentRoom.adjacentRooms[2]) + "\n");

		PrintLineToConsole("Naar welke gang? ");

		cin >> whichRoom;

		PrintLineToConsole("\n");

		if (cin.fail())
		{
			cin.clear();
			//cin.ignore(numeric_limits<streamsize>::max(), '\n');
			PrintLineToConsole("Input is ongeldig. Moet een nummer zijn. Probeer het nog een keer\n\n");
			continue;
		}

		if (!(unsigned(whichRoom) > rooms.size()) && (whichRoom == currentPlayer.currentRoom.adjacentRooms[0] || whichRoom == currentPlayer.currentRoom.adjacentRooms[1] || whichRoom == currentPlayer.currentRoom.adjacentRooms[2]))
		{
			break;
		}

		PrintLineToConsole("Input is ongeldig, Probeer het nog een keer.\n\n");
	}

	currentPlayer.currentRoom = rooms[whichRoom - 1];

	EnterRoom(currentPlayer);
}

void PlayerInteraction(playerstruct& currentPlayer)
{
	char playerChoose = '1';	//Temp value A

	while (playerChoose != 'S' && playerChoose != 'M')
	{
		PrintLineToConsole("Je zit in kamer " + to_string(currentPlayer.currentRoom.roomID) + "\n");
		PrintLineToConsole("Wil je schieten of lopen (S/M) ");

		cin >> playerChoose;

		PrintLineToConsole("\n");
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
	PrintLineToConsole("Welkom bij 'Hunt the Wumpus'! \n\n");

	PrintLineToConsole("De Wumpus leeft in een grot met 20 kamers. Elke kamer heeft 3 tunnels die leiden naar andere kamers. \n\n");

	PrintLineToConsole("Gevaren: \n");
	PrintLineToConsole("De bodemloze put: twee kamers hebben een bodemloze put. \n");
	PrintLineToConsole("Als je in deze kamer komt val je in deze put (en verlies je meteen). \n\n");

	PrintLineToConsole("Super vleermuizen: twee kamers hebben super vleermuizen. \n");
	PrintLineToConsole("Als je de super vleermuizen tegen komt in een kamer word je naar een random kamer gebracht door de super vleermuizen.\n");
	PrintLineToConsole("Nadat jij naar een andere kamer bent gezet, veranderen de super vleermuizen ook van kamer!\n\n");

	PrintLineToConsole("De Wumpus\n");
	PrintLineToConsole("De wumpus is immuun voor de gevaren in de kamers. \n");
	PrintLineToConsole("Over het algemeen slaapt de Wumpus, maar de Wumus is ook wel eens wakker!\n");
	PrintLineToConsole("De wumpus wordt wakker als je een pijl schiet OF als jj zijn kamer binnen komt.\n");
	PrintLineToConsole("Als de Wumpus wakker is verplaatst hij zich naar een andere kamer.\n");
	PrintLineToConsole("Als dit dezelfde kamer is als waar jij in staat (of als jij zijn kamer binnekomt) eet hij je op en verlies je.\n\n");

	PrintLineToConsole("De speler\n");
	PrintLineToConsole("Elke zet mag je lopen OF schieten\n");
	PrintLineToConsole("Lopen: ja kan naar een van de drie aangegeven kamers verplaatsen.\n");
	PrintLineToConsole("Schieten: ja kan kiezen hoeveel pijlen je schiet (1t/m5).\n");
	PrintLineToConsole("Je moet voor elke losse pijl aangeven naar welke kamer je hem wil schieten \n");
	PrintLineToConsole("Als de gekozen kamer niks oplevert (als de Wumpus daar niet zit), kaatst de pijl naar 2 random kamers. \n");
	PrintLineToConsole("De pijl kan ook op je eigen kamer komen. Dan ben je af. \n\n");

	PrintLineToConsole("De game is afgelopen als je de Wumpus weet te raken met je pijlen. Veel succes!\n\n");
}

void RandomBatRoom(playerstruct& currentPlayer)
{
	srand((unsigned)time(NULL));
	int randomRoom = rand() % rooms.size();
	while (randomRoom == (currentPlayer.currentRoom.roomID - 1)) {
		randomRoom = rand() % rooms.size();
	}
	currentPlayer.currentRoom = rooms[randomRoom];
	PrintLineToConsole("Je bent in een kamer gekomen met vleermuizen!\nDe vleermuizen brengen je naar kamer " + to_string(currentPlayer.currentRoom.roomID) + "!\n\n");
}

void EnterRoom(playerstruct& currentPlayer)
{
	if (currentPlayer.currentRoom.bat)
	{
		RandomBatRoom(currentPlayer);
	}
	else if (currentPlayer.currentRoom.pit)
	{
		PrintLineToConsole("Je bent in een bodemloze put gevallen...\n\n");
		GameOver(currentPlayer, false);
		return;
	}

	WumpusRoom(currentPlayer);
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

void GameOver(playerstruct& currentPlayer, const bool& gameWon)
{
	cout << "-------------------------------------------------------------\n\n";
	currentPlayer.gameOver = true;
	if (!gameWon)
	{
		char option = '1';
		PrintLineToConsole("Wil je weten waar de Wumpus zat? (Y) ");
		cin >> option;
		if (option == 'Y')
		{
			for (unsigned int x = 0; x != rooms.size(); x++)
			{
				if (rooms[x].wumpus)
				{
					PrintLineToConsole("De Wumpus zat in room " + to_string(rooms[x].roomID) + "!\n");
				}
			}
		}
	}
}

bool GameStart(playerstruct& currentPlayer)
{
	Intro();
	while (!currentPlayer.gameOver)	 //gameloop
	{
		PlayerInteraction(currentPlayer);
	}
	char option = '1';
	while (option != 'Y' && option != 'N')
	{
		PrintLineToConsole("Wil je opnieuw spelen? (Y/N) ");
		cin >> option;
	}
	switch (option)
	{
	case 'Y':
		PrintLineToConsole("----------------------------------------------------------\n\n");
		return true;
	case 'N':
		exit(0);
	}
}


int main()
{
	FillRoomVector(rooms, "config.json");


	thread playerThread(PlayerThread);
	thread aiThread(AIThread);

	playerThread.join();
	aiThread.join();

	return 0;
}