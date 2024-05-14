#include "NetSocket.h"

#ifdef _WIN32
unsigned int NetSocket::_win_sock_users = 0u;
#endif

void NetSocket::ThrowSockExcept(const char *error_message) {
#ifdef _WIN32
	throw error_message + std::string(": ") + std::to_string(WSAGetLastError());
#else
	throw std::string(error_message);
#endif
}

void NetSocket::CloseSocket(int &sock) {
	if (sock < 0) return;
#ifdef _WIN32
	closesocket((SOCKET)sock);
#else
	close((int)sock);
#endif
	sock = -1;
}

NetSocket::NetSocket() {
#ifdef _WIN32
	// Initialize Winsock
	if (!_win_sock_users) {
		int result = WSAStartup(MAKEWORD(2, 2), &_wsa_data);
		if (result) {
			throw std::string("WSAStartup failed with error: " + result);// Windows Only exception
		}
	}
	++_win_sock_users;
#endif

	_conn_info_size = sizeof(_conn_info);

	_socket_state = Init_State;
}

NetSocket::~NetSocket() {
	// Clean up
	CloseSocket(_conn_socket);
	CloseSocket(_listen_socket);
	
#ifdef _WIN32
	if (_win_sock_users == 1) {
		WSACleanup();
	}
	--_win_sock_users;
#endif

	_socket_state = None_State;
}

void NetSocket::ShutdownConnection() {
	// shutdown the connection since we're done
	if (shutdown(_conn_socket, 1/*SD_SEND*/) < 0) {
		ThrowSockExcept("shutdown failed");
	}
}

void NetSocket::ListenPort(const unsigned short &port) {
	// Check state
	if (_socket_state != Init_State) {
		throw std::string("Couldn't start listening port!");
	}

	_sock_info.sin_family = AF_INET;
	_sock_info.sin_addr.s_addr = htonl(INADDR_ANY);
	_sock_info.sin_port = htons(port);

	// Create a SOCKET for the server to listen for client connections
	if ((_listen_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		ThrowSockExcept("socket failed");
	}

	// Setup the TCP listening socket
	if (bind(_listen_socket, (const sockaddr *)&_sock_info, sizeof(_sock_info)) < 0) {
		ThrowSockExcept("bind failed");
	}
	
	if (listen(_listen_socket, SOMAXCONN) < 0) {
		ThrowSockExcept("listen failed");
	}

	_socket_state = ListenPort_State;
}

int NetSocket::AcceptConnection() {
	// Check state
	if (_socket_state != ListenPort_State) {
		throw std::string("Couldn't accept connection!");
	}
    
    _conn_socket = accept(_listen_socket, (sockaddr *)&_conn_info, &_conn_info_size);
	if (_conn_socket < 0) {
		ThrowSockExcept("accept failed");
	}

	_socket_state = AcceptConnection_State;

	return (int)_conn_socket;
}

void NetSocket::ConnectTo(const char *ip, const unsigned short &port) {
	// Check state
	if (_socket_state != Init_State) {
		throw std::string("Couldn't connect to server!");
	}

	_sock_info.sin_family = AF_INET;
	_sock_info.sin_port = htons(port);
	inet_pton(AF_INET, ip, &_sock_info.sin_addr);
    
	// Create a SOCKET for connecting to server
	if ((_conn_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		ThrowSockExcept("socket failed");
	}

	// Connect to server.
	if (connect(_conn_socket, (const sockaddr *)&_sock_info, sizeof(_sock_info)) < 0) {
		ThrowSockExcept("socket connection failed");
	}

	_socket_state = ConnectTo_State;
}

// If return 0, close connection
int NetSocket::Recieve(char *buffer, const size_t &size) {
	int result = 0;
	if ((result = (int)recv(_conn_socket, buffer, size, 0)) < 0) {
		ThrowSockExcept("recv failed");
	}

	return result;
}

int NetSocket::Send(char *buffer, const size_t &size) {
	int result = 0;
	if ((result = (int)send(_conn_socket, buffer, size, 0)) < 0) {
		ThrowSockExcept("send failed");
	}

	return result;
}
