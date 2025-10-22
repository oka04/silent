#pragma once
#include <enet/enet.h>

class ServerManager
{
public:
	ServerManager();
	~ServerManager();

	bool StartServer(int port, int maxClients);
	void StopServer();
	void Update();  // ★サーバーのメイン処理（接続・切断・受信監視）

private:
	ENetPacket* CreateAdvertisePacket();
	unsigned int lastAdvertiseTime;
	ENetHost* serverHost;
	int clientCount;
};