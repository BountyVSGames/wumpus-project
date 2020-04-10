#include "Room.hpp"
#include "AI.hpp"

bool aiThreadDone = false;

vector<room> rooms = {}; //alleen ReadConfigInfo veranderd rooms, dus globale variable is oke in dit geval
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
	0x36,
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
	string value = to_string(number);

	for (int i = 0; i < value.size(); i++)
	{
		returnValue.push_back(value[i] - '0');
	}

	return returnValue;
}

//void PlayerThread()
//{
//	player.currentRoom = rooms[19];
//	string mapFile;
//
//	while (MenuScreen(mapFile, player));
//}
void AIThread()
{
	aiplayer aiPlayer;

	while (true)
	{
		unique_lock<mutex> lock(mx);
		currentLineReady.wait(lock);

		if (currentLine.find("Wil je schieten of lopen") == 0)
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

				Sleep(1000);

				AIInputPressUp(0x10); //Shift
				AIInputPressUp(0x53); //S
				AIInputPressUp(0x0D); //Enter
			}
			else
			{
				AIInputPressDown(0x10); //Shift
				AIInputPressDown(0x4D); //M
				AIInputPressDown(0x0D); //Enter

				Sleep(1000);

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
					if (player.currentRoom.adjacentRooms[i] == aiPlayer.roomsVisited[j].roomID)
					{
						notVisitedRoom = false;
						break;
					}
					
					notVisitedRoom = true;
					notVisitedRoomId = player.currentRoom.adjacentRooms[i];
				}

				if (notVisitedRoom)
				{
					break;
				}
			}

			Sleep(1000);

			vector<int> splittedRoomID = IntDevider(notVisitedRoomId + 1);

			for (int i = 0; i < splittedRoomID.size(); i++)
			{
				AIInputPressDown(numpadKeyInput[splittedRoomID[i]]);
				Sleep(500);
				AIInputPressUp(numpadKeyInput[splittedRoomID[i]]);
			}

			AIInputPressDown(0x0D);
			Sleep(500);
			AIInputPressUp(0x0D);

			splittedRoomID = {};
		}
		else if (currentLine.find("Wil je weten waar de") == 0)
		{
			AIInputPressDown(0x10);
			AIInputPressDown(0x4E); //N
			AIInputPressDown(0x0D);

			Sleep(1000);

			AIInputPressUp(0x10);
			AIInputPressUp(0x4E); //N
			AIInputPressUp(0x0D);
		}
		else if (currentLine.find("Wil je opnieuw spelen?") == 0)
		{
			AIInputPressDown(0x10);
			AIInputPressDown(0x59); //Y
			AIInputPressDown(0x0D);

			Sleep(1000);

			AIInputPressUp(0x10);
			AIInputPressUp(0x59); //Y
			AIInputPressUp(0x0D);
		}
		else if (currentLine.find("Je bent in een bodemloze put gevallen") == 0)
		{
			cout << "TEST";
			aiPlayer.pitRooms.push_back(player.currentRoom);
		}
		else if (currentLine.find("Je ruikt de wumpus") == 0)
		{
			vector<int> roomsToAdd = {};

			for (int i = 0; i < player.currentRoom.adjacentRooms.size(); i++)
			{
				for (int j = 0; j < rooms[player.currentRoom.adjacentRooms[i]].adjacentRooms.size(); j++)
				{
					if (rooms[player.currentRoom.adjacentRooms[i]].adjacentRooms[j] != player.currentRoom.adjacentRooms[i])
					{
						roomsToAdd.push_back(rooms[player.currentRoom.adjacentRooms[i]].adjacentRooms[j]);
					}
				}

				roomsToAdd.push_back(player.currentRoom.adjacentRooms[i]);
			}
			for (int i = 0; i < roomsToAdd.size() - 1; i++)
			{
				for (int j = i + 1; j < roomsToAdd.size(); j++)
				{
					if (roomsToAdd[i] == roomsToAdd[j])
					{
						roomsToAdd.erase(roomsToAdd.begin() + j);
					}
				}
			}

			aiPlayer.possibleWumpusRooms = roomsToAdd;
		}
		else if (currentLine.find("Hoeveel tunnels wil je doorschieten") == 0)
		{	
			AIInputPressDown(numpadKeyInput[5]);
			AIInputPressDown(0x0D);
		}
		else if (currentLine.find("Welke kamer:") == 0)
		{
			vector<int> splittedRoomID = IntDevider(aiPlayer.possibleWumpusRooms[0]);

			for (int i = 0; i < splittedRoomID.size(); i++)
			{
				AIInputPressDown(numpadKeyInput[splittedRoomID[i]]);
			}
			AIInputPressDown(0x0D);

			Sleep(1000);

			for (int i = 0; i < splittedRoomID.size(); i++)
			{
				AIInputPressUp(numpadKeyInput[splittedRoomID[i]]);
			}
			AIInputPressUp(0x0D);
		}
		else if (currentLine.find("Je hebt gemist en bent nu een pijl kwijt") == 0)
		{
			aiPlayer.possibleWumpusRooms.erase(aiPlayer.possibleWumpusRooms.begin());
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

void ReadConfigInfo(vector<room>& roomVec, const string& fileName, playerstruct& currentPlayer)
{
	ifstream file(fileName);		//stream
	IStreamWrapper wrapper(file);		//wrapper

	Document configFile;
	configFile.ParseStream(wrapper);	//parse file

	vector<int> adjacentRoomsConf;
	for (unsigned int x = 0; x != configFile["Rooms"].Size(); x++)
	{
		for (Value& y : configFile["Rooms"][x]["Adjacent Rooms"].GetArray())
		{
			//maak vector aan met values van json file
			adjacentRoomsConf.push_back(y.GetInt());
		}
		roomVec.push_back({									//maak nieuwe room object met json values en zet die in de vector
			configFile["Rooms"][x]["Room ID"].GetInt(),		//roomID
			adjacentRoomsConf,								//adjacentRooms
			configFile["Rooms"][x]["Bat"].GetBool(),		//bat
			configFile["Rooms"][x]["Pit"].GetBool(),		//pit
			configFile["Rooms"][x]["Wumpus"].GetBool()		//wumpus
			});
		adjacentRoomsConf = {};
	}
	cout << fileName << " is succesvol ingeladen!\n\n";
	if (configFile["Players"].Empty())
	{
		cout << "Er zijn nog geen bestaande spelers op" << fileName << "\n\n";
	}
	else
	{
		cout << "Spelers op " << fileName << ": ";
		for (unsigned int x = 0; x != (configFile["Players"].Size() - 1); x++)
		{
			cout << configFile["Players"][x]["Player Name"].GetString() << ", ";
		}
		cout << configFile["Players"][configFile["Players"].Size() - 1]["Player Name"].GetString() << "\n\n";
	}

	cout << "Geef je speler een naam van maximaal 20 characters, of kies een player: ";
	while (currentPlayer.playerName.empty() || currentPlayer.playerName.size() > 20)
	{
		getline(cin, currentPlayer.playerName);
	}
	for (unsigned int x = 0; x != configFile["Players"].Size(); x++)
	{
		//check of de opgegeven naam al bestaat
		if (currentPlayer.playerName == configFile["Players"][x]["Player Name"].GetString())
		{
			//laad player informatie in
			currentPlayer.gamesPlayed = configFile["Players"][x]["Games Played"].GetInt();
			currentPlayer.turns = configFile["Players"][x]["Turns"].GetInt();
			currentPlayer.mapCompleted = configFile["Players"][x]["Map Completed"].GetBool();

			cout << "informatie opgehaald\n\n";
			break;
		}
	}
	file.close();
}

void WritePlayerInfo(const playerstruct& currentPlayer, const string& fileName)
{
	//open filestream voor read en write
	ifstream inFile(fileName);			//input stream
	IStreamWrapper inWrapper(inFile);		//wrapper

	Document configFile, configFileAppend;
	configFile.ParseStream(inWrapper);	//parse file

	configFile.IsObject();
	inFile.close();

	for (unsigned int x = 0; x != configFile["Players"].Size(); x++)
	{
		//check of de opgegeven naam al bestaat
		if (currentPlayer.playerName == configFile["Players"][x]["Player Name"].GetString())
		{
			//remove
			configFile["Players"].Erase(configFile["Players"].Begin() + x);
			break;
		}
	}

	//add

	configFileAppend.SetObject();

	Value playerObject(kObjectType);	//maak nieuw object

	Value name;
	char buffer[20];														//char buffer
	int len = sprintf_s(buffer, "%s", currentPlayer.playerName.c_str());	//converteer naar dynamische char pointer
	name.SetString(buffer, len, configFileAppend.GetAllocator());			//maak name char pointer

	//voeg members toe aan object
	playerObject.AddMember("Player Name", name, configFileAppend.GetAllocator());
	playerObject.AddMember("Games Played", currentPlayer.gamesPlayed, configFileAppend.GetAllocator());
	playerObject.AddMember("Turns", currentPlayer.turns, configFileAppend.GetAllocator());
	playerObject.AddMember("Map Completed", currentPlayer.mapCompleted, configFileAppend.GetAllocator());

	//voeg object toe aan Players lijst
	configFile["Players"].PushBack(playerObject, configFileAppend.GetAllocator());


	ofstream outFile(fileName);				//output stream
	OStreamWrapper outWrapper(outFile);		//wrapper

	Writer<OStreamWrapper> writer(outWrapper);
	configFile.Accept(writer);

	outFile.close();
}

string ChooseMap()
{
	string configFile = "";			//tmp value configFile
	while (true)
	{
		cout << "Instructies voor verschillende mappen: " << endl << endl;
		cout << "Je kan een map zelf laten generen voor de liefhebber." << endl;
		cout << "U heeft 2 mappen aangeleverd gekregen(als het goed is)." << endl;
		cout << "In de \"Config Wumpus\" map staat een .exe bestand. Als je dit runt kan je zelf een kamer structuur creeren" << endl;
		cout << "Zorg dat het nieuwe bestand (dat eindigt op .json) in dezelfde map staat als het \"Hunt...Adventure.exe\" bestand." << endl;
		cout << "Als je deze dan wil gebruiken hoef je alleen de naam van het nieuwe bestand in te geven en spelen maar!" << endl << endl << endl;
		cout << "Welke map wil je inladen? (typ \"config\" voor de test)" << endl << endl;

		cin >> configFile;														//vraag naar input om een map te kiezen
		configFile += ".json";							// er wordt hier al ".json" voor je toegevoegd dus je hoeft alleen een naar op te geven
		ifstream checkConf(configFile);
		if (checkConf.good())
		{
			break;
		}
		else {							//als de filenaam niet bestaat dan geeft hij dit terug
			cout << "Deze file bestaat niet, probeer het opnieuw\n\n";
		}
	}
	system("CLS");						//leeg scherm
	return configFile;
}

void PrintLeaderboard(const string& fileName)
{
	ifstream file(fileName);			//stream
	IStreamWrapper wrapper(file);		//wrapper

	Document configFile;
	configFile.ParseStream(wrapper);	//parse file

	cout << "Leaderboards van " << fileName << endl;
	cout << "-------------------------------------------------------------\n";
	cout << "Speler Naam		Aantal keer gespeeld		Beurten			Map Compleet\n\n";
	for (unsigned int x = 0; x != configFile["Players"].Size(); x++)
	{
		cout << configFile["Players"][x]["Player Name"].GetString() << "\t\t";
		cout << configFile["Players"][x]["Games Played"].GetInt() << "\t\t\t\t";
		cout << configFile["Players"][x]["Turns"].GetInt() << "\t\t\t";
		if (configFile["Players"][x]["Map Completed"].GetBool())
		{
			cout << "x";
		}
		cout << "\n\n";
	}
	system("pause");
	file.close();
}
void WumpusRoom(playerstruct& currentPlayer)
{
	if (currentPlayer.currentRoom.wumpus) {							//als de kamer waar je in staat gelijk is met de kamer van de wumpus dan ga je naar de gamever functie
		cout << "Je bent tegen de Wumpus aangelopen...hij is boos en heeft je de grond in geslagen\n\n";
		GameOver(currentPlayer, false);								//zie functie voor functionaliteit
		return;
	}
	bool found = false;
	for (unsigned int x = 0; x != 3; x++) {							//dit is een nested for-loop die checkt in alle kamer door in de adjacentRoom van elke kamer te kijken of de wumpus daarin zit
		int adjacentRoomIndexX = rooms[currentPlayer.currentRoom.adjacentRooms[x]].roomID;
		for (unsigned int y = 0; y != 3; y++) {
			int adjacentRoomIndexY = rooms[adjacentRoomIndexX].adjacentRooms[y];
			if (rooms[adjacentRoomIndexY].wumpus) {
				found = true;
				break;
			}
		}
		if (rooms[adjacentRoomIndexX].wumpus) {							//als de wumpus binnen de range van 2 kamer is  krijg je de melding dat je de wumpus ruikt
			found = true;
		}
		if (found) { 
			PrintLineToConsole("Je ruikt de wumpus...\n");
			break; 
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

	for (unsigned int x = numberRooms; x != 0; x--)
	{
		PrintLineToConsole("Welke kamer: ");
		cin >> whichRoom;
		whichRoom--;

		if (whichRoom < 0 || whichRoom >= rooms.size()) {
			cout << "Die kamer bestaat niet\n";
			x++;
			continue;
		}
		if (rooms[whichRoom].wumpus)
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
		PrintLineToConsole("De gangen lijden naar " + to_string(currentPlayer.currentRoom.adjacentRooms[0] + 1)
			+ " " + to_string(currentPlayer.currentRoom.adjacentRooms[1] + 1) + " " + to_string(currentPlayer.currentRoom.adjacentRooms[2] + 1) + "\n");		//je krijgt hier de aanliggende kamers te zien

		PrintLineToConsole("Naar welke gang? ");

		cin >> whichRoom;						//de vraag naar input waar je één van de hiervoor genoemde kamers moet kiezen
		whichRoom--;

		PrintLineToConsole("\n");

		if (cin.fail())							//hier krijg je een check over je input heen of je wel een gelige invoer geeft (denk aan letter ipv cijfers)
		{
			cin.clear();
			//cin.ignore(numeric_limits<streamsize>::max(), '\n');
			PrintLineToConsole("Input is ongeldig. Moet een nummer zijn. Probeer het nog een keer\n\n");
			continue;
		}

		if (whichRoom == currentPlayer.currentRoom.adjacentRooms[0] ||				//hier wordt er nog even gekeken of je gekozen kamer wel hetzelfde is aan een van de aangegeven kamers
			whichRoom == currentPlayer.currentRoom.adjacentRooms[1] ||
			whichRoom == currentPlayer.currentRoom.adjacentRooms[2])
		{
			break;
		}

		cout << "Input is ongeldig, Probeer het nog een keer.\n\n";
	}

	currentPlayer.currentRoom = rooms[whichRoom];						//hier wordt je dan ook daadwerkrelijk naar die gekozen kamer gezet

	EnterRoom(currentPlayer);											//zie functie voor functionaliteit
}

void PlayerInteraction(playerstruct& currentPlayer)
{
	string playerChoose = "value";				//Temp value A

	while (playerChoose != "S" && playerChoose != "M")			//een while-loop die checkt of de invoer niet "S" of "M" is want dan moet je opnieuwe de keuze krijgen
	{
		PrintLineToConsole("Je zit in kamer " + to_string(currentPlayer.currentRoom.roomID + 1) + "\n");
		PrintLineToConsole("Wil je schieten of lopen (S/M) ");

		cin >> playerChoose;									//de vraag naar invoer om een keuze tussen schieten of bewegen te maken

		PrintLineToConsole("\n");
	}
	currentPlayer.turns++;

	if (playerChoose == "S") {					//als je "S" ingeeft dan heb je de mogelijkheid om te schieten
		PlayerShoot(currentPlayer);				//zie functie voor functionaliteit
	}
	else if (playerChoose == "M") {					//als je "M" ingeeft dan krijg je de mogelijkheid om van kamer te verplaatsen
		RoomSelection(currentPlayer);			//zie functie voor functionaliteit
	}
}

void Intro()			//dit is de intro tekst
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

void RandomBatRoom(playerstruct& currentPlayer)			//je wordt hierdoor naar een random kamer gezet door de "bats"
{
	srand((unsigned)time(NULL));	//maak seed aan gebasseerd op runtime zodat deze uniek is

	int randomRoom = rand() % rooms.size();
	while (randomRoom == currentPlayer.currentRoom.roomID) {			//kies een random integer todat deze integer niet gelijk staat aan de kamer waar de player al in zit
		randomRoom = rand() % rooms.size();
	}

	currentPlayer.currentRoom = rooms[randomRoom];	//verander de current room van de player naar de nieuwe random room
	PrintLineToConsole("Je bent in een kamer gekomen met vleermuizen!\nDe vleermuizen brengen je naar kamer " + to_string(currentPlayer.currentRoom.roomID + 1) + "!\n\n");
}

void EnterRoom(playerstruct& currentPlayer)
{
	if (currentPlayer.currentRoom.bat)		//als bat = true bij de kamer waar de player in zit, roep dan RandomBatRoom() aan
	{	
		RandomBatRoom(currentPlayer);
	}
	else if (currentPlayer.currentRoom.pit)				//als pit = true bij huidige kamer roep dan GameOver() aan
	{
		PrintLineToConsole("Je bent in een bodemloze put gevallen...\n\n");
		GameOver(currentPlayer, false);
		return;
	}

	WumpusRoom(currentPlayer);
}

void GameOver(playerstruct& currentPlayer, const bool& gameWon)
{
	cout << "-------------------------------------------------------------\n\n";
	currentPlayer.gameOver = true;
	currentPlayer.gamesPlayed++;

	if (!gameWon)						//als je niet hebt gewonnen runt deze code
	{
		char option = '1';
		PrintLineToConsole("Wil je weten waar de Wumpus zat? (Y) ");
		cin >> option;
		if (option == 'Y') {								//dit is een vraag naar input waarbij "Y" je in dit geval de daadwerkelijke kamer van de wumpus geeft
			for (unsigned int x = 0; x != rooms.size(); x++)		//met deze for-loop kijkt hij naar alle kamer en checkt hij in welke kamer de wumpus "true"
			{
				if (rooms[x].wumpus)
				{
					PrintLineToConsole("De Wumpus zat in room " + to_string(rooms[x].roomID) + "!\n");
				}
			}
		}
	}
	else
	{
		currentPlayer.mapCompleted = true;
	}
}

bool GameStart(playerstruct& currentPlayer, const string fileName)
{
	Intro();							//de intro tekst word gedisplayed
	while (!currentPlayer.gameOver)	 //als je verliest wordt deze code ge-runt
	{
		PlayerInteraction(currentPlayer);		//de keuze om te schieten of lopen
	}

	WritePlayerInfo(currentPlayer, fileName);		//alle info wordt naar het bestand geschreven (voor eventuele leaderboards)

	char option = '1';			//tmp value option
	while (option != 'Y' && option != 'N')		//while-loop die blijft lopen zolang je geen "Y" of "N" hebt ingegeven		
	{
		PrintLineToConsole("Wil je opnieuw spelen? (Y/N) ");
		cin >> option;
	}
	switch (option)
	{
	case 'Y':				//als je "Y" ingeeft begint je game opnieuw met standaardwaarden
		PrintLineToConsole("----------------------------------------------------------\n\n");
		currentPlayer.currentRoom = rooms[0];			//standaardwaarde zodat je in kamer 1 begint
		currentPlayer.arrows = 5;
		currentPlayer.gameOver = false;
		return true;
	case 'N':				//als je "N" ingeeft stopt het programma met runnen
		return false;
	}
}

bool MenuScreen(string& mapFile, playerstruct& currentPlayer)
{
	system("CLS");								//leeg console
	int choice = 0;								//tmp-variable value

	cout << "Huidige map: " << mapFile << endl;
	cout << " _________________________________________________________________________________________________________" << endl;
	cout << "|                                             HUNT THE WUMPUS                                             |" << endl;
	cout << "|                                                                                                         |" << endl;
	cout << "|                                                                                                         |" << endl;
	cout << "|                                             Kies een optie                                              |" << endl;
	cout << "|    _________________   _________________   _________________   _________________   _________________    |" << endl;
	cout << "|   |                 | |                 | |                 | |                 | |                 |   |" << endl;
	cout << "|   | 1.   Spelen     | | 2. Leaderboard  | | 3.    AI        | | 4.    Quit      | | 5.    Map       |   |" << endl;
	cout << "|   |_________________| |_________________| |_________________| |_________________| |_________________|   |" << endl;
	cout << "|_________________________________________________________________________________________________________|" << endl;

	cin >> choice;							//vraag naar input

	system("CLS");

	if (choice == 1)
	{
		if (mapFile.empty())				//als er nog geen map is gekozen dan laat het je bij deze kiezen
		{
			mapFile = ChooseMap();
		}
		ReadConfigInfo(rooms, mapFile, currentPlayer);
		currentPlayer.currentRoom = rooms[0];			//standaardwaarde die je in kamer 0 laat spawnen
		currentPlayer.gameOver = false;

		while (GameStart(currentPlayer, mapFile)) {}
		return true;
	}
	else if(choice == 2)
	{
		if (mapFile.empty())
		{
			mapFile = ChooseMap();			//ja krijgt de keuze van welke map je de leaderboards wil zien
		}
		PrintLeaderboard(mapFile);			//hier worden de leaderboards ook daadwerkelijk naar het scherm geprint zie deze functie
		return true;
	}
	else if (choice == 3)
	{
		thread aiThread(AIThread);

		if (mapFile.empty())				//als er nog geen map is gekozen dan laat het je bij deze kiezen
		{
			mapFile = ChooseMap();
		}
		ReadConfigInfo(rooms, mapFile, currentPlayer);
		currentPlayer.currentRoom = rooms[0];			//standaardwaarde die je in kamer 0 laat spawnen
		currentPlayer.gameOver = false;

		while (GameStart(currentPlayer, mapFile)) {}

		aiThread.join();
		return true;
	}
	else if (choice == 4)
	{
		return false;
	}
	else if (choice == 5)
	{
		mapFile = ChooseMap();				//als je "5" ingeeft dan moet je de map kiezn waarin je wil spelen
		return true;
	}

	return true;
}

int main()
{
	string mapFile;
	while (MenuScreen(mapFile, player)) {};

	return 0;
}