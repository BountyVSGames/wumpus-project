#include "Room.hpp"
#include "Game.hpp"

vector<room> rooms = {};	//alleen ReadConfigInfo veranderd rooms, dus globale variable is oke in dit geval

void ReadConfigInfo(vector<room>& roomVec, const string& fileName, player& currentPlayer)
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

void WritePlayerInfo(const player& currentPlayer, const string& fileName)
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
		cout << "Welke map wil je inladen? (typ \"config\" voor de test) ";
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

void WumpusRoom(player& currentPlayer)
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
		cout << "Hoeveel tunnels wil je doorschieten (1 tot " << currentPlayer.arrows << ")? ";
		cin >> numberRooms;
	}

	for (unsigned int x = numberRooms; x != 0; x--)
	{
		cout << "kamer: ";
		cin >> whichRoom;
		whichRoom--;

		if (whichRoom < 0 || whichRoom >= rooms.size()) {
			cout << "Die kamer bestaat niet\n";
			x++;
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
			<< " " << (currentPlayer.currentRoom.adjacentRooms[1] + 1) << " " << (currentPlayer.currentRoom.adjacentRooms[2] + 1) << endl;		//je krijgt hier de aanliggende kamers te zien

		cout << "Naar welke gang? ";

		cin >> whichRoom;						//de vraag naar input waar je één van de hiervoor genoemde kamers moet kiezen
		whichRoom--;

		cout << '\n';

		if (cin.fail())							//hier krijg je een check over je input heen of je wel een gelige invoer geeft (denk aan letter ipv cijfers)
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input is ongeldig. Moet een nummer zijn. Probeer het nog een keer\n\n";			
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

void PlayerInteraction(player& currentPlayer)
{
	string playerChoose = "value";				//Temp value A

	while (playerChoose != "S" && playerChoose != "M")			//een while-loop die checkt of de invoer niet "S" of "M" is want dan moet je opnieuwe de keuze krijgen
	{
		cout << "Je zit in kamer " << (currentPlayer.currentRoom.roomID + 1) << endl;
		cout << "Wil je schieten of lopen (S/M) ";

		cin >> playerChoose;									//de vraag naar invoer om een keuze tussen schieten of bewegen te maken

		cout << '\n';
	}
	currentPlayer.turns++;

	if (playerChoose == "S") {					//als je "S" ingeeft dan heb je de mogelijkheid om te schieten
		PlayerShoot(currentPlayer);				//zie functie voor functionaliteit
	}
	if (playerChoose == "M") {					//als je "M" ingeeft dan krijg je de mogelijkheid om van kamer te verplaatsen
		RoomSelection(currentPlayer);			//zie functie voor functionaliteit
	}
}

void Intro()			//dit is de intro tekst
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

void RandomBatRoom(player& currentPlayer)			//je wordt hierdoor naar een random kamer gezet door de "bats"
{
	srand((unsigned)time(NULL));	//maak seed aan gebasseerd op runtime zodat deze uniek is

	int randomRoom = rand() % rooms.size();
	while (randomRoom == currentPlayer.currentRoom.roomID) {			//kies een random integer todat deze integer niet gelijk staat aan de kamer waar de player al in zit
		randomRoom = rand() % rooms.size();			
	}

	currentPlayer.currentRoom = rooms[randomRoom];	//verander de current room van de player naar de nieuwe random room

	cout << "Je bent in een kamer gekomen met vleermuizen!\nDe vleermuizen brengen je naar kamer " << (currentPlayer.currentRoom.roomID + 1) << "!\n\n";
}

void EnterRoom(player& currentPlayer)
{
	if (currentPlayer.currentRoom.bat)		//als bat = true bij de kamer waar de player in zit, roep dan RandomBatRoom() aan
	{	
		RandomBatRoom(currentPlayer);
	}
	else if (currentPlayer.currentRoom.pit)				//als pit = true bij huidige kamer roep dan GameOver() aan
	{
		cout << "Je bent in een bodemloze put gevallen...\n\n";
		GameOver(currentPlayer, false);
		return;
	}

	WumpusRoom(currentPlayer);
}

void GameOver(player& currentPlayer, const bool& gameWon)
{
	cout << "-------------------------------------------------------------\n\n";
	currentPlayer.gameOver = true;
	currentPlayer.gamesPlayed++;

	if (!gameWon)						//als je niet hebt gewonnen runt deze code
	{
		char option = '1';
		cout << "Wil je weten waar de Wumpus zat? (Y) ";
		cin >> option;
		if (option == 'Y'){								//dit is een vraag naar input waarbij "Y" je in dit geval de daadwerkelijke kamer van de wumpus geeft
 			for (unsigned int x = 0; x != rooms.size(); x++)		//met deze for-loop kijkt hij naar alle kamer en checkt hij in welke kamer de wumpus "true"
			{
				if (rooms[x].wumpus)
				{
					cout << "De Wumpus zat in room " << (rooms[x].roomID + 1) << "!\n";
				}
			}
		}
	}
	else
	{
		currentPlayer.mapCompleted = true;
	}
}

bool GameStart(player& currentPlayer, const string fileName)
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
		cout << "Wil je opnieuw spelen? (Y/N) ";
		cin >> option;					
	}
	switch (option)
	{
	case 'Y':				//als je "Y" ingeeft begint je game opnieuw met standaardwaarden
		cout << "\n----------------------------------------------------------" << endl << endl;
		currentPlayer.currentRoom = rooms[0];			//standaardwaarde zodat je in kamer 1 begint
		currentPlayer.gameOver = false;
		return true;
	case 'N':				//als je "N" ingeeft stopt het programma met runnen
		return false;
	}
}

bool MenuScreen(string& mapFile, player& currentPlayer)
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

	switch (choice)
	{
	case 1:									//als je "1" ingeeft dan start hij de game op met beginnende waardes
		if (mapFile.empty())				//als er nog geen map is gekozen dan laat het je bij deze kiezen
		{
			mapFile = ChooseMap();
		}
		ReadConfigInfo(rooms, mapFile, currentPlayer);		
		currentPlayer.currentRoom = rooms[0];			//standaardwaarde die je in kamer 0 laat spawnen
		currentPlayer.gameOver = false;					

		while (GameStart(currentPlayer, mapFile)) {}	
		return true;
	case 2:									//als je "2" ingeeft krijg jje het leaderboard te zien
		if (mapFile.empty())
		{
			mapFile = ChooseMap();			//ja krijgt de keuze van welke map je de leaderboards wil zien
		}
		PrintLeaderboard(mapFile);			//hier worden de leaderboards ook daadwerkelijk naar het scherm geprint zie deze functie
		return true;
	case 3:									//als je "3" ingeeft dan gaat hij een AI-speler in de map zetten die de game zelf kan uitspelen
		cout << "AI\n";
		return true;
	case 4:
		return false;						//als je "4" ingeeft dan sluit je het spel af
	case 5:
		mapFile = ChooseMap();				//als je "5" ingeeft dan moet je de map kiezn waarin je wil spelen
		return true;
	}
	return true;
}

int main()
{
	player player;
	string mapFile;
	while (MenuScreen(mapFile, player));
	return 0;
}