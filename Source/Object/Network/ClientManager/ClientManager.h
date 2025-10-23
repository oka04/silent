#pragma once
#include <enet/enet.h>
#include <vector>
#include <memory>
#include <string>
#include <mutex>
#include "..\\Discovery\\Discovery.h"

struct ServerInfoNet
{
	unsigned int ip;
	enet_uint16 port;
	uint8_t playerCount;
	uint8_t maxPlayers;
	uint8_t state;
	std::string name;
};

class ClientManager
{
public:
	ClientManager();
	~ClientManager();

	// singleton
	static ClientManager* GetInstance();

	// discovery cached list (only updated when RefreshAvailableServers is called)
	const std::vector<ServerInfoNet>& GetCachedServers() const { return cachedServers; }
	bool ConnectToServer(const char* ip, int port);
	void Disconnect();
	void SendMessage(const char* msg);
	void Update();

	// lobby state
	void SendJoin(const std::string& name);
	std::vector<std::string> GetLobbyPlayerNames();
	bool IsGameStarted() const { return gameStarted; }
	bool IsHost() const { return isHost; }

	// manual refresh: copy discovery->GetServers() into cachedServers
	void RefreshAvailableServers();

private:
	ENetHost* clientHost;
	ENetPeer* serverPeer;
	ENetHost* finderHost;
	std::unique_ptr<Discovery> discovery;
	std::vector<ServerInfoNet> availableServers; // internal temp
	std::vector<ServerInfoNet> cachedServers;    // UI-visible snapshot

												 // lobby
	std::vector<std::string> lobbyPlayerNames;
	bool gameStarted;
	bool isHost;
	std::mutex lobbyMutex;

	int previousLobbyCount;

	// singleton storage
	static ClientManager* s_instance;
};