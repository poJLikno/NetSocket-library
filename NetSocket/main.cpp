#include <iostream>
#include <stdlib.h>
#include <chrono>
#include <thread>

#include "NetSocket/NetSocket.h"

#define SERVER_PI "127.0.0.1"
#define SERVER_PORT 8080

void func() {
	std::this_thread::sleep_for(std::chrono::seconds(2));
	
	try {
		NetSocket client;
		char client_buffer[64] = "Client message";

		client.ConnectTo(SERVER_PI, SERVER_PORT);
		client.Send(client_buffer, sizeof(client_buffer));
		memset(client_buffer, 0, sizeof(client_buffer));
		if (client.Recieve(client_buffer, sizeof(client_buffer))) {
			std::cout << client_buffer << "\n";
		}
		client.ShutdownConnection();
	}
	catch (std::string &error) {
		std::cout << error << "\n";
	}
}

int main(int argc, const char **argv) {
#ifdef _WIN32
	SetConsoleCP(CP_UTF8);
	SetConsoleOutputCP(CP_UTF8);
#endif

	try {
		NetSocket server;
		char server_buffer[64] = { 0 };

		server.ListenPort(8080);

		std::thread thrd(func);
		thrd.detach();
		
		if (server.AcceptConnection()) {
			while (server.Recieve(server_buffer, sizeof(server_buffer))) {
				std::cout << server_buffer << "\n";
				server.Send((char *)"Server message", 14ull);

				memset(server_buffer, 0, sizeof(server_buffer));
			}
			server.ShutdownConnection();
		}
	}
	catch (std::string &error) {
		std::cout << error << "\n";
	}

	return 0;
}
