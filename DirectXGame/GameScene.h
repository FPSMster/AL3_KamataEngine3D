#pragma once
#include "KamataEngine.h"
#include <vector>
#include"SkyDome.h"
#include"Player.h"

class GameScene {
public:
	// 初期化
	void Initialize();

	// デストラクタ
	~GameScene();

	// 更新
	void Update();

	// 描画
	void Draw();

	// 3Dモデル
	KamataEngine::Model* modelBlock_ = nullptr;

	KamataEngine::Model* playerModel_ = nullptr;

	Player* player_ = nullptr;

	SkyDome* skydome_ = nullptr;

	// 箱
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	// カメラ
	KamataEngine::Camera camera_;

	// デバックカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;


	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;
};