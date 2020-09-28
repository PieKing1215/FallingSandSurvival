#pragma once
#include <enet/enet.h>
#include <functional>

class NetworkMode {
public:
	static const char HOST = 0;
	static const char CLIENT = 1;
	static const char SERVER = 2;
};

class Networking {
public:
	static bool init();
};

class Client {
public:
	ENetHost* client = nullptr;

    ENetAddress address {};
	ENetPeer* peer = nullptr;

	~Client();

	static Client* start();

	bool connect(const char* ip, enet_uint16 port);
	void disconnect();

	void tick();
};

class Server {
public:
    ENetAddress address {};
	ENetHost* server = nullptr;

	~Server();

	static Server* start(enet_uint16 port);

	void tick();

};
