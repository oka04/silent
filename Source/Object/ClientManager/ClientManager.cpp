#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#include "ClientManager.h"
#include <windows.h>
#include <iostream>

#pragma warning(disable:4996)
#pragma warning(disable:26812)
#pragma warning(disable:26495)
#pragma warning(disable:6387)

#pragma comment(lib, "enetlib.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")

ClientManager::ClientManager()
	: clientHost(nullptr)
	, serverPeer(nullptr)
{
	if (enet_initialize() != 0)
	{
		MessageBoxA(NULL, "ENetの初期化に失敗しました。", "エラー", MB_OK);
	}
}

ClientManager::~ClientManager()
{
	Disconnect();
	enet_deinitialize();
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
	enet_host_flush(clientHost);
	enet_peer_send(serverPeer, 0, packet);
}

void ClientManager::Update()
{
	if (!clientHost) return;

	ENetEvent event;
	while (enet_host_service(clientHost, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_CONNECT:
			std::cout << "[Client] サーバー接続成功" << std::endl;
			break;

		case ENET_EVENT_TYPE_RECEIVE:
			std::cout << "[Client] サーバーから受信: " << (char*)event.packet->data << std::endl;
			enet_packet_destroy(event.packet);
			break;

		case ENET_EVENT_TYPE_DISCONNECT:
			std::cout << "[Client] サーバーから切断" << std::endl;
			break;

		default:
			break;
		}
	}
}
