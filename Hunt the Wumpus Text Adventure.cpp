// Hunt the Wumpus Text Adventure.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "Game.hpp"
#include "Room.hpp"

room playerPlace;

void RoomSelection(room currentRoom){
    int whichRoom = -1;
    cout << "Je zit in kamer " << playerPlace << endl;
    cout << "De gangen lijden naar " << currentRoom.adjecentRooms[0] 
         << " " << currentRoom.adjecentRooms[1] << " " << currentRoom.adjecentRooms[2] << endl;
    
    cout << "Naar welke gang?" << endl;
    
    while(whichRoom != currentRoom.adjecentRooms[0] || whichRoom != currentRoom.adjecentRooms[1] || whichRoom != currentRoom.adjecentRooms[2]){
        cin >> whichRoom;
    }
     
    playerPlace = rooms[whichRoom - 1];
    
    EnterRoom(room);
}


void EnterRoom(room room){
    PlayerMove(playerPlace);
}


int main()
{

}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
