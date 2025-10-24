//*****************************************************************************
//
// シーンの元となるcppファイル
//
//*****************************************************************************
#define WIN32_LEAN_AND_MEAN
#define _CRT_SECURE_NO_WARNINGS
#define _USING_V110_SDK71_ 1

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include "SceneLobby.h"

using namespace KeyString;
using namespace InputKey;
using namespace WindowSetting;
using namespace Common;

SceneLobby::SceneLobby(Engine* pEngine)
	: Scene(pEngine), m_client(nullptr), m_server(nullptr), m_lastTime(0),
	selectedServerIndex(-1), pressedMouseLast(false)
{
}

SceneLobby::~SceneLobby()
{
	Exit();
}

void SceneLobby::Start()
{
	// Use existing menu buttons for back/start/operation/title
	m_buttons.clear();
	m_buttons.push_back(BACK_BUTTON);
	m_buttons.push_back(START_BUTTON);
	m_buttons.push_back(OPERATION_BUTTON);
	m_buttons.push_back(TITLE_BUTTON);

	m_nowSceneNumber = m_nowSceneData.GetNowScene();
	Initialize(m_pEngine);

	m_client = ClientManager::GetInstance();
	m_server = ServerManager::GetInstance();

	m_lastTime = timeGetTime();
	SoundManager::Play(AK::EVENTS::PLAY_SE_PAUSE, ID_UI);

	// Initially do not auto-refresh. UI user must press "更新" button to populate server list.
	m_client->RefreshAvailableServers(); // optional: initial fill
}

void SceneLobby::Update()
{
	DWORD nowTime = timeGetTime();
	float deltaTime = (nowTime - m_lastTime) / 1000.0f;
	m_lastTime = nowTime;

	// handle menu update (back/start)
	if (MenuManager::Update(m_pEngine, m_gameData, deltaTime)) {
		// If START pressed and we are host, trigger server start
		if (m_gameData.m_nextSceneNumber == Common::SCENE_GAME) {
			if (m_server && m_server->GetClientCount() >= 2) {
				m_server->StartGame();
				// transition to game on server side
				m_nowSceneData.Set(Common::SCENE_GAME, false, nullptr);
				return;
			}
			// otherwise ignore start
		}
		else {
			m_nowSceneData.Set(m_gameData.m_nextSceneNumber, false, nullptr);
			return;
		}
	}

	// Update network
	if (m_server) m_server->Update();
	if (m_client) m_client->Update();

	// UI: handle clicks on Refresh button and server list
	POINT mp = m_pEngine->GetMousePosition();
	bool mouseDown = m_pEngine->GetMouseButtonSync(DIK_LBUTTON) != 0;

	// define refresh button rect
	int refreshX = WINDOW_WIDTH - 220;
	int refreshY = 200;
	int refreshW = 180;
	int refreshH = 48;

	// click detection (rising edge)
	bool clicked = mouseDown && !pressedMouseLast;
	pressedMouseLast = mouseDown;

	if (clicked) {
		// refresh button
		if (mp.x >= refreshX && mp.x <= refreshX + refreshW && mp.y >= refreshY && mp.y <= refreshY + refreshH) {
			m_client->RefreshAvailableServers();
		}
		else {
			// click in server list area?
			int listX = 100;
			int listY = 200;
			int rowH = 36;
			auto &servers = m_client->GetCachedServers();
			for (size_t i = 0; i < servers.size(); ++i) {
				int ry = listY + (int)i * rowH;
				if (mp.x >= listX && mp.x <= listX + 600 && mp.y >= ry && mp.y <= ry + rowH) {
					selectedServerIndex = (int)i;
					// join immediately on click
					// connect to selected server
					char ipbuf[32] = { 0 };
					unsigned long saddr = servers[i].ip;
					in_addr ina; ina.S_un.S_addr = saddr;
					char* ipstr = inet_ntoa(ina);
					if (ipstr) {
						strcpy_s(ipbuf, ipstr);
						m_client->ConnectToServer(ipbuf, servers[i].port);
						// after connecting, client->Update will send join and lobby will populate via network messages
					}
				}
			}
		}
	}
}

void SceneLobby::Draw()
{
	m_pEngine->SpriteBegin();
	MenuManager::Draw(m_pEngine, m_gameData);

	// draw Refresh button
	int refreshX = WINDOW_WIDTH - 220;
	int refreshY = 200;
	int refreshW = 180;
	int refreshH = 48;
	SetRect(&sour, 0, 0, 200, 100);
	SetRect(&dest, refreshX, refreshY, refreshX + refreshW, refreshY + refreshH);
	m_pEngine->Blt(&dest, TEXTURE_BUTTON, &sour);
	m_pEngine->DrawPrintf(refreshX + 8, refreshY + 8, FONT_GOTHIC60, Color::BLACK, "更新");

	// draw server list (cached)
	int drawX = 100;
	int drawY = 200;
	int stepY = 36;

	auto &servers = m_client->GetCachedServers();
	for (size_t i = 0; i < servers.size(); ++i) {
		int y = drawY + (int)i * stepY;
		// row background
		RECT r;
		SetRect(&r, drawX, y, drawX + 600, y + stepY - 4);
		int alpha = (selectedServerIndex == (int)i) ? 200 : 160;
		m_pEngine->Blt(&r, TEXTURE_BUTTON, &sour, alpha, 0.0f);
		// text: name (players/max)
		char buf[256];
		sprintf_s(buf, "%s  (%d/%d)", servers[i].name.c_str(), servers[i].playerCount, servers[i].maxPlayers);
		m_pEngine->DrawPrintf(drawX + 8, y + 6, FONT_GOTHIC60, Color::WHITE, buf);
	}

	// if connected & in-lobby: draw lobby players (client side)
	if (m_client) {
		auto names = m_client->GetLobbyPlayerNames();
		if (!names.empty()) {
			int lx = 100;
			int ly = 100;
			m_pEngine->DrawPrintf(lx, ly - 40, FONT_GOTHIC60, Color::WHITE, "ロビー");
			for (size_t i = 0; i < names.size(); ++i) {
				m_pEngine->DrawPrintf(lx, ly + (int)i * 32, FONT_GOTHIC60, Color::WHITE, names[i].c_str());
			}
		}
	}

	m_pEngine->SpriteEnd();
}

void SceneLobby::PostEffect() {}
void SceneLobby::Exit()
{
	Release(m_pEngine);
}