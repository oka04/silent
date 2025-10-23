//*****************************************************************************
//
// タイトルシーン
//
//*****************************************************************************

#define _USING_V110_SDK71_ 1

#include "..\\Object\\Network\\ClientManager\\ClientManager.h"
#include "..\\Object\\Network\\ServerManager\\ServerManager.h"
#include "SceneTitle.h"

using namespace KeyString;
using namespace InputKey;
using namespace WindowSetting;
using namespace Common;

using namespace std;

//=============================================================================
// コンストラクタ
// 引　数：Engine* エンジンクラスのアドレス
//=============================================================================
SceneTitle::SceneTitle(Engine *pEngine)
	: Scene(pEngine)
{

}

//=============================================================================
// デストラクタ
//=============================================================================
SceneTitle::~SceneTitle()
{
	Exit();
}

//=============================================================================
// シーンの実行時に１度だけ呼び出される開始処理関数
//=============================================================================
void SceneTitle::Start()
{
	m_buttons.clear();
	// タイトルのボタンをホスト / 探す に変更
	m_buttons.push_back(HOST_BUTTON);
	m_buttons.push_back(FIND_BUTTON);
	m_buttons.push_back(OPERATION_BUTTON);
	m_buttons.push_back(EXIT_BUTTON);
	m_nowSceneNumber = m_nowSceneData.GetNowScene();
	Initialize(m_pEngine);

	m_camera.m_vecEye = D3DXVECTOR3(5.0f, 10.0f, 6.0f);
	m_camera.m_vecAt = D3DXVECTOR3(30.0f, 0.0f, 17.0f);
	m_camera.m_vecUp = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	m_camera.RecalculateUpDirection();
	m_camera.SetDevice(m_pEngine);


	m_projection.SetData(D3DXToRadian(90.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	m_projection.SetDevice(m_pEngine);

	m_viewport.Add(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0.0f, 1.0f);
	m_viewport.SetDevice(m_pEngine, 0);

	m_ambient.SetColor(0.7f, 0.1f, 0.1f, 0.1f);
	m_light.SetDiffuse(0.1f, 0.1f, 0.1f, 0.1f);
	D3DXVECTOR3 direction;
	direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	direction = D3DXVec3Normalize(&direction);
	m_light.SetDirection(direction);

	SetBackColor(0x00000000);
	m_pEngine->AddModel(MODEL_CHARACTER);

	m_patrollerManager.Initialize(m_pEngine, m_nowSceneData.GetNowScene());
	m_lastTime = timeGetTime();

	Camera camera = m_camera;
	m_map.Initialize(m_pEngine, m_patrollerManager, &camera, &m_projection, &m_ambient, &m_light, 0);

	m_patrollerManager.ReleaseSE();
	SoundManager::Play(AK::EVENTS::PLAY_BGM_TITLE, ID_BGM);
}

//=============================================================================
// 更新
//=============================================================================
void SceneTitle::Update()
{
	DWORD nowTime = timeGetTime();
	float deltaTime = (nowTime - m_lastTime) / 1000.0f;
	m_lastTime = nowTime;

	m_patrollerManager.Update(m_pEngine, m_map, m_camera.m_vecEye, nullptr, deltaTime);

	// MenuManager::Update が true を返したらシーン遷移へ
	if (MenuManager::Update(m_pEngine, m_gameData, deltaTime))
	{
		// 次のシーンに行く前に「ホストを選んだらここでサーバーを起動し、自分で接続する」
		if (m_gameData.m_nextSceneNumber == Common::SCENE_LOBBY) {
			// 判断方法：現在選択中のボタンが HOST_BUTTON ならホスト動作を行う
			// MenuManager の m_buttons / m_selectNumber は protected のため直接取れないので
			// 判定は本実装では「最後に選択されたボタンの種類」を GameData に保持するのが望ましい。
			// 簡易実装：ここでは StartServer を呼び、ローカルクライアントで接続してロビーへ行く動作を行う。
			// ポートや maxPlayers は固定（12345 / 4）
			ServerManager::GetInstance()->StartServer(12345, 4);
			ClientManager::GetInstance()->ConnectToServer("127.0.0.1", 12345);
		}

		m_nowSceneData.Set(m_gameData.m_nextSceneNumber, false, nullptr);
	}
}

//=============================================================================
// 描画
//=============================================================================
void SceneTitle::Draw()
{
	vector<SpotLight>* lights = m_patrollerManager.GetLights(m_camera.m_vecEye, m_camera.m_vecAt, m_projection.GetFov());
	m_map.DrawMap(m_pEngine, &m_camera, &m_projection, &m_ambient, &m_light, lights);
	m_patrollerManager.Draw(m_pEngine, &m_camera, &m_projection, &m_ambient, &m_light);

	m_pEngine->SpriteBegin();
	MenuManager::Draw(m_pEngine, m_gameData);

#if _DEBUG	
	m_pEngine->DrawPrintf(0, 1000, FONT_GOTHIC60, Color::WHITE, "%f", (float)m_pEngine->GetFPS());
#endif
	m_pEngine->SpriteEnd();
}

//=============================================================================
// シーンの実行時に繰り返し呼び出されるポストエフェクト関数
//=============================================================================
void SceneTitle::PostEffect()
{

}

//=============================================================================
// シーンの終了時に呼び出される終了処理関数
//=============================================================================
void SceneTitle::Exit()
{
	SoundManager::StopAll(ID_BGM);
	Release(m_pEngine);
	m_map.Release(m_pEngine);
	m_patrollerManager.Release(m_pEngine);

	m_pEngine->ReleaseModel(MODEL_CHARACTER);
}

#ifdef USE_IMGUI
//=============================================================================
// 日本語入力用
//=============================================================================
void SceneTitle::ImGuiFrameProcess()
{

}
#endif
