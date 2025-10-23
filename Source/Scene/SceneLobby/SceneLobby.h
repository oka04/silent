//*****************************************************************************
//
// シーンの元となるヘッダファイル
//
//*****************************************************************************

#pragma once

#include "..\\..\\GameBase.h"

#include "..\\Scene\\Scene.h"
#include "..\\..\\Object\\MenuManager\\MenuManager.h"
#include "..\\..\\Object\\Network\\ClientManager\\ClientManager.h"
#include "..\\..\\Object\\Network\\ServerManager\\ServerManager.h"
#include <vector>
#include <string>

class SceneLobby : public Scene, MenuManager
{
public:
	SceneLobby(Engine* pEngine);
	~SceneLobby();

	void Start() override;
	void Update() override;
	void Draw() override;
	void PostEffect() override;
	void Exit() override;

private:
	ClientManager* m_client;
	ServerManager* m_server;
	DWORD m_lastTime;

	// UI selection for server list
	int selectedServerIndex;
	bool pressedMouseLast;
};