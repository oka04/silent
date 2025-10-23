#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include "ClientManager.h"
#include <windows.h>
#include <iostream>
#include <algorithm>

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

ClientManager* ClientManager::s_instance = nullptr;

ClientManager* ClientManager::GetInstance()
{
	if (!s_instance) s_instance = new ClientManager();
	return s_instance;
}

ClientManager::ClientManager()
	: clientHost(nullptr)
	, serverPeer(nullptr)
	, discovery(nullptr)
	, gameStarted(false)
	, isHost(false)
	, previousLobbyCount(0)
{
	if (enet_initialize() != 0)
	{
		MessageBoxA(NULL, "ENetの初期化に失敗しました。", "エラー", MB_OK);
	}

	discovery = std::make_unique<Discovery>();
	discovery->StartListener(12346);
}

ClientManager::~ClientManager()
{
	if (discovery) {
		discovery->StopListener();
		discovery.reset();
	}

	Disconnect();
	enet_deinitialize();

	if (s_instance == this) s_instance = nullptr;
}

bool ClientManager::ConnectToServer(const char* ip, int port)
{
	clientHost = enet_host_create(nullptr, 1, 1, 0, 0);
	if (!clientHost)
	{
		MessageBoxA(NULL, "クライアント作成失敗", "エラー", MB_OK);
		return false;
	}

	ENetAddress address;
	enet_address_set_host(&address, ip);
	address.port = (enet_uint16)port;

	serverPeer = enet_host_connect(clientHost, &address, 1, 0);
	if (!serverPeer)
	{
		MessageBoxA(NULL, "サーバー接続要求失敗", "エラー", MB_OK);
		return false;
	}

	// mark host flag if connecting to localhost
	isHost = (strcmp(ip, "127.0.0.1") == 0 || strcmp(ip, "localhost") == 0);

	std::cout << "[Client] 接続要求送信中..." << std::endl;
	return true;
}

void ClientManager::Disconnect()
{
	if (serverPeer)
	{
		enet_peer_disconnect(serverPeer, 0);
		serverPeer = nullptr;
	}

	if (clientHost)
	{
		enet_host_destroy(clientHost);
		clientHost = nullptr;
	}

	std::cout << "[Client] 切断" << std::endl;
}

void ClientManager::SendMessage(const char* msg)
{
	if (!serverPeer) return;
	ENetPacket* packet = enet_packet_create(msg, strlen(msg) + 1, ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(serverPeer, 0, packet);
	enet_host_flush(clientHost);
}

void ClientManager::SendJoin(const std::string& name)
{
	if (!serverPeer || !clientHost) return;
	std::vector<uint8_t> buf;
	buf.push_back((uint8_t)MSG_JOIN);
	uint8_t nl = (uint8_t)std::min<size_t>(255, name.size());
	buf.push_back(nl);
	buf.insert(buf.end(), name.begin(), name.begin() + nl);
	ENetPacket* packet = enet_packet_create(buf.data(), (size_t)buf.size(), ENET_PACKET_FLAG_RELIABLE);
	enet_peer_send(serverPeer, 0, packet);
	enet_host_flush(clientHost);
}

std::vector<std::string> ClientManager::GetLobbyPlayerNames()
{
	std::lock_guard<std::mutex> lk(lobbyMutex);
	return lobbyPlayerNames;
}

void ClientManager::RefreshAvailableServers()
{
	availableServers.clear();
	if (!discovery) return;
	auto servers = discovery->GetServers();
	for (auto &s : servers) {
		if (s.state != 0) continue; // only show lobby servers
		ServerInfoNet n;
		n.ip = s.ip;
		n.port = s.port;
		n.playerCount = s.playerCount;
		n.maxPlayers = s.maxPlayers;
		n.state = s.state;
		n.name = s.name;
		availableServers.push_back(n);
	}
	// swap into cached
	cachedServers = availableServers;
}

void ClientManager::Update()
{
	// discovery runs in background, refresh only when requested via RefreshAvailableServers

	if (!clientHost) return;

	ENetEvent event;
	while (enet_host_service(clientHost, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			std::cout << "[Client] サーバー接続成功" << std::endl;
			// send join with a default name (could be from user input)
			SendJoin("Player");
			break;

		case ENET_EVENT_TYPE_RECEIVE:
		{
			const uint8_t* data = (const uint8_t*)event.packet->data;
			size_t len = event.packet->dataLength;
			if (len >= 1) {
				uint8_t id = data[0];
				if (id == MSG_LOBBY_UPDATE) {
					size_t idx = 1;
					std::vector<std::string> newNames;
					if (idx < len) {
						uint8_t count = data[idx++];
						for (int i = 0; i < count; ++i) {
							if (idx >= len) break;
							uint8_t nl = data[idx++];
							if (idx + nl <= len) {
								std::string name((const char*)(data + idx), nl);
								newNames.push_back(name);
								idx += nl;
							}
							else break;
						}
					}
					{
						std::lock_guard<std::mutex> lk(lobbyMutex);
						int prev = (int)lobbyPlayerNames.size();
						lobbyPlayerNames = newNames;
						int now = (int)lobbyPlayerNames.size();
						if (now > prev) {

							//ここでサウンドを鳴らす
						}
						previousLobbyCount = now;
					}
				}
				else if (id == MSG_START_GAME) {
					gameStarted = true;
				}
			}
			enet_packet_destroy(event.packet);
		}
		break;

		case ENET_EVENT_TYPE_DISCONNECT:
			std::cout << "[Client] サーバーから切断" << std::endl;
			break;

		default:
			break;
		}
	}
}