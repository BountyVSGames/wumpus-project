#include "Room.hpp"

room playerPlace;

room room1{ 1, { 8, 2, 3 } };
room room2{ 2, { 1, 3, 4 } };
room room3{ 3, { 1, 2, 4 } };
room room4{ 4, { 2, 3, 5 } };
room room5{ 5, { 4, 6, 7 } };
room room6{ 6, { 5, 7, 8 } };
room room7{ 7, { 5, 6, 8 } };
room room8{ 8, { 1, 6, 7 } };

vector<room> rooms = { room1, room2 , room3 ,room4, room5, room6, room7, room8 };

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

		if (cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Input is ongeldig. Moet een nummer zijn. Probeer het nog een keer" << endl << endl;
			continue;
		}

		if (!(whichRoom > rooms.size()) && (whichRoom == currentRoom.adjacentRooms[0] || whichRoom == currentRoom.adjacentRooms[1] || whichRoom == currentRoom.adjacentRooms[2]))
		{
			break;
		}
		
		cout << "Input is ongeldig. Probeer het nog een keer" << endl << endl;
	}

	playerPlace = rooms[whichRoom - 1];

	EnterRoom(playerPlace);
}
void EnterRoom(room & room) 
{
	RoomSelection(room);
}

int main()
{
	playerPlace = rooms[0];

	Intro(playerPlace);

	return 0;
}