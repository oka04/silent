#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include "ServerManager.h"
#include <windows.h>
#include <iostream>

#pragma warning(disable:4996)
#pragma warning(disable:26812)
#pragma warning(disable:26495)
#pragma warning(disable:6387)

#pragma comment(lib, "enetlib.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

ServerManager::ServerManager()
	: serverHost(nullptr)
	, clientCount(0)
{
	if (enet_initialize() != 0)
	{
		MessageBoxA(NULL, "ENetの初期化に失敗しました。", "エラー", MB_OK);
	}
}

ServerManager::~ServerManager()
{
	StopServer();
	enet_deinitialize();
}

bool ServerManager::StartServer(int port, int maxClients)
{
	ENetAddress address;
	address.host = ENET_HOST_ANY;
	address.port = (enet_uint16)port;

	serverHost = enet_host_create(&address, maxClients, 1, 0, 0);
	if (serverHost == nullptr)
	{
		MessageBoxA(NULL, "サーバーホスト作成失敗", "エラー", MB_OK);
		return false;
	}

	std::cout << "[Server] 起動: ポート " << port << std::endl;
	return true;
}

void ServerManager::StopServer()
{
	if (serverHost)
	{
		enet_host_destroy(serverHost);
		serverHost = nullptr;
		clientCount = 0;
		std::cout << "[Server] 停止" << std::endl;
	}
}

void ServerManager::Update()
{
	if (!serverHost) return;

	// 追加: サーバー告知 (5秒に一度など)
	// enet_time_get() はENet独自の時刻取得関数 (ミリ秒単位)
	// 適切なタイミングでサーバーの存在をネットワークに告知
	if (serverHost->serviceTime - lastAdvertiseTime >= 5000)
	{
		ENetPacket* advertisePacket = enet_packet_create("GAMETAG_GUESSTAG_1", 17, ENET_PACKET_FLAG_RELIABLE);
		ENetAddress broadcastAddress;
		broadcastAddress.host = ENET_HOST_BROADCAST;
		broadcastAddress.port = serverHost->address.port; // サーバーと同じポート

		lastAdvertiseTime = serverHost->serviceTime;
	}


	ENetEvent event;
	while (enet_host_service(serverHost, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			clientCount++;
			std::cout << "[Server] クライアント接続 (" << clientCount << "人)" << std::endl;
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			std::cout << "[Server] 受信: " << (char*)event.packet->data << std::endl;
			enet_packet_destroy(event.packet);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			clientCount--;
			std::cout << "[Server] クライアント切断 (" << clientCount << "人)" << std::endl;
			break;

		default:
			break;
		}
	}
}
