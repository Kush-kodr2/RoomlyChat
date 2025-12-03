#include <iostream>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <mutex>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")
using namespace std;

mutex clients_mutex;
vector<SOCKET> clients;
bool running = true;

void broadcast_msg(const string &message)
{
    lock_guard<mutex> guard(clients_mutex);
    for (SOCKET client : clients)
    {

        send(client, message.c_str(), message.size(), 0);
    }
}

void handle_client(SOCKET clientsocket)
{
    char buffer[1024];
    int bytesRecv;
    // for receiving the username
    ZeroMemory(buffer, 1024);
    bytesRecv = recv(clientsocket, buffer, 1024, 0);

    if (bytesRecv <= 0)
    {
        closesocket(clientsocket);
        return;
    }
    buffer[bytesRecv] = '\0';
    string username = buffer;

    cout << username << " joined.\n";
    string joinMsg = username + " joined the chat. \n";
    broadcast_msg(joinMsg);

    // for handling chat messages loop
    while (running)
    {
        ZeroMemory(buffer, 1024);
        bytesRecv = recv(clientsocket, buffer, 1024, 0);

        if (bytesRecv > 0)
        {
            buffer[bytesRecv] = '\0';
            string userMsg = buffer;

            if (userMsg == "/exit" || userMsg == "/exit\n")
            {
                cout << username << " requested exit.\n";
                break;
            }
            string message = username + ": " + userMsg;
            cout << "[MSG] " << message << endl;
            // if (msg == "exit")
            // {
            //     cout << "Client disconnected. \n";
            //     break;
            // }

            broadcast_msg(message);
        }
        else if (bytesRecv == 0)
        {
            cout << username << " disconnected. \n";
            break;
        }
        else
        {
            cerr << "recv() failed! \n";
            break;
        }
    }
    {
        lock_guard<mutex> guard(clients_mutex);
        clients.erase(remove(clients.begin(), clients.end(), clientsocket), clients.end());
    }
    closesocket(clientsocket);
    string leaveMsg = username + " left the chat.\n"; 
    cout<<leaveMsg;
    broadcast_msg(leaveMsg);
}
int main()
{
    WSADATA wsaData;
    SOCKET serverSocket, client_socket;
    sockaddr_in serverAddr, clientAddr;
    int clientAddrSize = sizeof(clientAddr);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "WSAStartup failed! /n";
        return 1;
    }
    // Creating Socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        cerr << "Socket creation failed! \n";
        WSACleanup();
        return 1;
    }
    // Binding
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(8000);

    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerr << "Bind failed! \n";
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Listening to client
    listen(serverSocket, 1);
    cout << "Server is listening on port 8000...\n";

    // Accepting client connection
    while (running)
    {
        client_socket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrSize);
        if (client_socket == INVALID_SOCKET)
        {
            cerr << "Accept failed! \n";
            continue;
        }
        {
            lock_guard<mutex> guard(clients_mutex);
            clients.push_back(client_socket);
        }

        // Starting receiver threads
        cout << "New client connected! Total: " << clients.size() << endl;
        thread client_thread(handle_client, client_socket);
        client_thread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();

    cout << "Server closed. \n";
    return 0;
}