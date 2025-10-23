#pragma once
#include <enet/enet.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class Discovery;

struct ClientInfo
{
	ENetPeer* peer;
	uint32_t id;
	std::string name;
};

class ServerManager
{
public:
	ServerManager();
	~ServerManager();

	// singleton
	static ServerManager* GetInstance();

	bool StartServer(int port, int maxClients);
	void StopServer();
	void Update();  // サーバーのメイン処理（接続・切断・受信監視）

					// lobby utilities
	int GetClientCount() const { return clientCount; }
	void BroadcastLobbyUpdate();
	void StartGame();

private:
	ENetHost* serverHost;
	int clientCount;

	// peer -> ClientInfo*
	std::unordered_map<ENetPeer*, ClientInfo*> clients;
	uint32_t nextClientId;

	// Discovery advertiser (server-side broadcast)
	std::unique_ptr<Discovery> advertiser;

	unsigned int lastAdvertiseTime;
	// server display name / settings
	std::string serverName;

	static ServerManager* s_instance;
};