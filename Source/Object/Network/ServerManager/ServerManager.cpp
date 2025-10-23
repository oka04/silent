#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include "ServerManager.h"
#include "..\\ClientManager\\ClientManager.h"
#include "..\\Discovery\\Discovery.h"
#include <windows.h>
#include <iostream>
#include <cstring>

#pragma warning(disable:4996)
#pragma warning(disable:26812)
#pragma warning(disable:26495)
#pragma warning(disable:6387)

#pragma comment(lib, "enetlib.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

// message ids
enum {
	MSG_JOIN = 1,
	MSG_JOIN_ACK = 2,
	MSG_LOBBY_UPDATE = 3,
	MSG_START_GAME = 4,
};

ServerManager* ServerManager::s_instance = nullptr;

ServerManager* ServerManager::GetInstance()
{
	if (!s_instance) s_instance = new ServerManager();
	return s_instance;
}

ServerManager::ServerManager()
	: serverHost(nullptr)
	, clientCount(0)
	, advertiser(nullptr)
	, lastAdvertiseTime(0)
	, nextClientId(1)
	, serverName("Silent Host")
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

	if (s_instance == this) s_instance = nullptr;
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

	// Start discovery advertisement on a discovery port (separate from ENet port)
	const uint16_t discoveryPort = 12346;
	advertiser = std::make_unique<Discovery>();
	advertiser->StartAdvertise(discoveryPort, (uint16_t)port, serverName, (uint8_t)maxClients);
	advertiser->SetAdvertisePlayerCount(0);
	advertiser->SetAdvertiseState(0); // lobby

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
		for (auto &kv : clients) {
			delete kv.second;
		}
		clients.clear();
		std::cout << "[Server] 停止" << std::endl;
	}

	if (advertiser) {
		advertiser->StopAdvertise();
		advertiser.reset();
	}
}

void ServerManager::BroadcastLobbyUpdate()
{
	if (!serverHost) return;

	// format: [MSG_LOBBY_UPDATE][count][nameLen,name...]* (reliable)
	std::vector<uint8_t> payload;
	payload.push_back((uint8_t)MSG_LOBBY_UPDATE);
	uint8_t count = (uint8_t)clients.size();
	payload.push_back(count);
	for (auto &kv : clients) {
		const std::string &name = kv.second->name; 
		uint8_t nl = (uint8_t)(name.size() > 255 ? 255 : name.size());
		payload.push_back(nl);
		payload.insert(payload.end(), name.begin(), name.begin() + nl);
	}

	ENetPacket* packet = enet_packet_create(payload.data(), payload.size(), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(serverHost, 0, packet);
	enet_host_flush(serverHost);

	// update advertiser player count
	if (advertiser) advertiser->SetAdvertisePlayerCount(count);
}

void ServerManager::StartGame()
{
	if (!serverHost) return;
	if (clients.size() < 2) {
		std::cout << "[Server] プレイヤーが足りません。開始できません。\n";
		return;
	}

	// set advertise state to in-game so discovery filters it out
	if (advertiser) advertiser->SetAdvertiseState(1);

	std::vector<uint8_t> payload;
	payload.push_back((uint8_t)MSG_START_GAME);
	ENetPacket* packet = enet_packet_create(payload.data(), payload.size(), ENET_PACKET_FLAG_RELIABLE);
	enet_host_broadcast(serverHost, 0, packet);
	enet_host_flush(serverHost);

	std::cout << "[Server] ゲーム開始通知を送信しました\n";
}

void ServerManager::Update()
{
	if (!serverHost) return;

	ENetEvent event;
	while (enet_host_service(serverHost, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
		{
			ClientInfo* ci = new ClientInfo();
			ci->peer = event.peer;
			ci->id = nextClientId++;
			ci->name = "Player";
			event.peer->data = ci;
			clients[event.peer] = ci;
			clientCount++;
			std::cout << "[Server] クライアント接続 (" << clientCount << "人)" << std::endl;
			// broadcast updated lobby
			BroadcastLobbyUpdate();
		}
		break;

		case ENET_EVENT_TYPE_RECEIVE:
		{
			const uint8_t* data = (const uint8_t*)event.packet->data;
			size_t len = event.packet->dataLength;
			if (len >= 1) {
				uint8_t id = data[0];
				if (id == MSG_JOIN) {
					size_t idx = 1;
					if (idx < len) {
						uint8_t nl = data[idx++];
						if (idx + nl <= len) {
							std::string name((const char*)(data + idx), nl);
							idx += nl;
							ClientInfo* ci = (ClientInfo*)event.peer->data;
							if (ci) {
								ci->name = name;
							}
							// broadcast updated lobby to all
							BroadcastLobbyUpdate();
						}
					}
				}
				// other message types can be added here
			}
			enet_packet_destroy(event.packet);
		}
		break;

		case ENET_EVENT_TYPE_DISCONNECT:
		{
			ClientInfo* ci = (ClientInfo*)event.peer->data;
			if (ci) {
				clients.erase(event.peer);
				delete ci;
				event.peer->data = nullptr;
			}
			clientCount--;
			std::cout << "[Server] クライアント切断 (" << clientCount << "人)" << std::endl;
			BroadcastLobbyUpdate();
		}
		break;

		default:
			break;
		}
	}
}