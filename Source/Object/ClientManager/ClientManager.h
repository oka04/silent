#pragma once
#include <enet/enet.h>
#include <vector>

struct ServerInfo
{
	unsigned int ip;
	enet_uint16 port;
};

class ClientManager
{
public:
	ClientManager();
	~ClientManager();
	const std::vector<ServerInfo>& GetAvailableServers() const { return availableServers; }
	bool ConnectToServer(const char* ip, int port);
	void Disconnect();
	void SendMessage(const char* msg);
	void Update();

private:
	ENetHost* clientHost;
	ENetPeer* serverPeer; 
	ENetHost* finderHost;
	std::vector<ServerInfo> availableServers;
};
