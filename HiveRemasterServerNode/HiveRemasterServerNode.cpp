// HiveRemasterServerNode.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdio.h>

#include "EnvironmentOptions.h"
#include "ColorCodes.h"
#include "ASCIIART.h"

#include "HiveServerNode.h"
#include "ThreadResolvers.h"
#include <Windows.h>

//WINDOWS SPECIFIC:
#include <thread>


using namespace std;

int main()
{
    WSADATA wsaData;
    DWORD lastError;
    int option;

    //Handle for console output
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    int error;
    //int serverConnectionTimeout = -1;
    //int readValue;

    //Request for WSA 2.2 version
    error = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (error != 0) {
        SetConsoleTextAttribute(hConsole, _RED);
        cerr << "WSAStartup failed with error: " << error << "\n";
        return 1;
    }
    SetConsoleTextAttribute(hConsole, _GREEN);
    cout << "WSA started..." << endl;


    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        /* Couldnt find usable WinSock DLL */
        SetConsoleTextAttribute(hConsole, _RED);
        cerr << "Could not find version of Winsock.dll that can be used" << "\n";
        WSACleanup();
        return -1;
    }

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleTextAttribute(hConsole, _CYAN);




    SetConsoleTextAttribute(hConsole, _WHITE);


    SetConsoleTextAttribute(hConsole, _CYAN);
    GreetingsServer();
    SetConsoleTextAttribute(hConsole, _GREEN);
    HiveServerNode hiveServerNodeObject;

    if (hiveServerNodeObject.CreateServerSocket() < 0) {
        cerr << "Unable to create server socket..." << endl;
        terminate();
    }
    cout << "Socket created..." << endl;
    hiveServerNodeObject.ClearServerFileDescriptorSet();
    hiveServerNodeObject.AddServerSocketToFileDescriptor();

    hiveServerNodeObject.DefineServerIPV4(_SERVER_PORT);
    if (hiveServerNodeObject.BindServerSocket() == SOCKET_ERROR) {
        SetConsoleTextAttribute(hConsole, _RED);
        cerr << "Unable to bind socket... " << WSAGetLastError() << std::endl;
        terminate();
    }
    cout << "Socket binded to port " << _SERVER_PORT << endl;

    hiveServerNodeObject.ClearClientFileDescriptorSet();
    hiveServerNodeObject.PrepareClients();
    cout << "Prepared file descriptors for incoming clients: " << _MAX_CONNECTIONS << endl;

    if (hiveServerNodeObject.Listen() != 0) {
        SetConsoleTextAttribute(hConsole, _RED);
        cerr << "Unable to listen: " << WSAGetLastError() << endl;
        terminate();
    }
    cout << "Listening for the incoming connections..." << endl;


    //Thread needs to run continuously without interrupting the main function, hence it is detached from the process
    thread connectionResolverThread([&hiveServerNodeObject, &hConsole]() {
        ConnectionResolver(hiveServerNodeObject, hConsole);
        });
    connectionResolverThread.detach();


    //Now operator can continue the instructions
    SetConsoleTextAttribute(hConsole, _CYAN);
    char* message;
    SetConsoleTextAttribute(hConsole, _WHITE);
    while (1) {
        //cout<<"Enter the order:"<<endl;
        //cin>>message;
        cout << "Simulating work...\n";
        Sleep(1000);
        hiveServerNodeObject.SendOrdersToAllClients((char*)"test");

    }
}

