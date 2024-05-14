#ifndef SOCKET_BASE_H
#define SOCKET_BASE_H

#include <string>

#ifdef _WIN32
// first sockets-h, then windows-h
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "ws2_32.lib")
#else
// libs for unix
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

//#define INVALID_SOCKET ~0// -1
//#define SOCKET_ERROR -1
//#define SD_SEND -1
#endif/* Check os */

class NetSocket {
private:
#ifdef _WIN32
	static unsigned int _win_sock_users;
	WSADATA _wsa_data = { 0 };
#endif/* Window sockets init structures */

	enum SocketState {
		None_State,
		Init_State,
		ListenPort_State,
		AcceptConnection_State,
		ConnectTo_State
	};
	SocketState _socket_state = None_State;

	int _listen_socket = -1;
	sockaddr_in _sock_info = { 0 };

	int _conn_socket = -1;
	sockaddr_in _conn_info = { 0 };
	socklen_t _conn_info_size = 0;
	
	void CloseSocket(int &sock);
	void ThrowSockExcept(const char *error_message);

public:
	NetSocket();
	~NetSocket();

	void ShutdownConnection();

	void ListenPort(const unsigned short &port);
	int AcceptConnection();

	void ConnectTo(const char *ip, const unsigned short &port);

	// Max buffer size 65'535 bytes
	int Recieve(char *buffer, const size_t &size);
	int Send(char *buffer, const size_t &size);
};

#endif/* SOCKET_BASE_H */
