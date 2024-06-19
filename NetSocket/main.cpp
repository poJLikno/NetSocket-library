#include <iostream>
#include <pthread.h>

#include "NetSocket/NetSocket.h"

static void *client_thread_func(void *) {
	NetSocketClient client("127.0.0.1", 8080u);
	char buffer[1024] = { 0 };

	Sleep(1000);
	client.Send((char *)"Client request!", 16ull);
	if (client.Recieve(buffer, sizeof(buffer)) > 0) {
		std::cout << "Client: " << buffer << "\n";
		client.ShutdownConnection();
	}

	pthread_exit(nullptr);
	return nullptr;
}

int main(int argc, const char **argv) {
	NetSocketServer server(8080u);
	char buffer[1024] = { 0 };
	pthread_t client_thread = { 0 };


	pthread_create(&client_thread, nullptr, client_thread_func, nullptr);

	server.AcceptConnection();
	if (server.Recieve(buffer, sizeof(buffer)) > 0) {
		std::cout << "Server: " << buffer << "\n";
		server.Send((char *)"Server answer!", 15ull);
		server.ShutdownConnection();
	}

	pthread_join(client_thread, nullptr);

	return 0;
}