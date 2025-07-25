#include "GameScene.h"
#include"MeMath.h"

using namespace KamataEngine;
using namespace MathUtility;

void GameScene::Initialize() {
	// 3dモデルの生成
	modelBlock_ = Model::CreateFromOBJ("block", true);

	modelSkydome_ = Model::CreateFromOBJ("Skydome", true);

	modelPlayer_ = Model::CreateFromOBJ("player", true);

	modelEnemy_ = Model::CreateFromOBJ("enemy", true);

	// skydomeの生成
	skydome_ = new Skydome();

	// skydomeの初期化
	skydome_->Initialize(modelSkydome_, &camera_);

	// playerの生成
	player_ = new Player();

	// 座標をマップチップ番号で指定
	Vector3 playerPosition = mapChipField_->GetMapChippositionByIndex(1, 18);

	// playerの初期化
	player_->Initialize(modelPlayer_, &camera_,playerPosition);

	//enemy の生成
	enemy_ = new Enemy();

	// enemyの場所
	Vector3 enemyPosition = mapChipField_->GetMapChippositionByIndex(15, 18);
	enemy_->Initialize(modelEnemy_, &camera_, enemyPosition);

	//// 3dモデルの生成

	mapChipField_ = new MapChipField;
	mapChipField_->LoadMapChipCsv("Resources/blocks.csv");

	// カメラの初期化
	camera_.Initialize();

	player_->SetMapChipField(mapChipField_);

	// デバックカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	//カメラコントローラーの生成
	cameraController_ = new CameraController();
	cameraController_->Initialize();
	cameraController_->setTarget(player_);
	cameraController_->Reset();

	// 移動範囲の指定
	CameraController::Rect cameraArea = {12.0f, 100 - 12.0f, 6.0f, 6.0f};
	cameraController_->SetMovableArea(cameraArea);

	

	//// 要素数
	//const uint32_t kNumBlockVertical = 10;   // 縦
	//const uint32_t kNumBlockHorizontal = 20; // 横

	//// ブロック1個分の幅
	//const float kBlockWidth = 1.0f;  // 縦
	//const float kBlockHeight = 1.0f; // 横

	//// 要素数を変更する
	//worldTransformBlocks_.resize(kNumBlockVertical);
	//for (uint32_t i = 0; i < kNumBlockVertical; i++) {
	//	// 1列の要素数を設定(横方向のブロック数)
	//	worldTransformBlocks_[i].resize(kNumBlockHorizontal);
	//}

	//// ブロックの生成
	//for (uint32_t i = 0; i < kNumBlockVertical; i++) {
	//	for (uint32_t j = 0; j < kNumBlockHorizontal; j++) {
	//		if ((j % 2 == 0 && i % 2 == 0) || (j % 2 == 1 && i % 2 == 1)) {

	//			// ワールドトランスフォームの生成
	//			worldTransformBlocks_[i][j] = new WorldTransform();

	//			// ワールドトランスフォームの初期化
	//			worldTransformBlocks_[i][j]->Initialize();

	//			// x座標
	//			worldTransformBlocks_[i][j]->translation_.x = kBlockWidth * j;

	//			// y座標
	//			worldTransformBlocks_[i][j]->translation_.y = kBlockHeight * i;
	//		}
	//	}
	//}

	GenerateBlocks();

}

GameScene::~GameScene() {
	// モデルの解放
	delete modelBlock_;
	modelBlock_ = nullptr;

	// skydomeの解放
	delete modelSkydome_;
	modelSkydome_ = nullptr;

	delete skydome_;
	skydome_ = nullptr;

	// モデルplayerの開放
	delete modelPlayer_;
	modelPlayer_ = nullptr;

	// playerの開放
	delete player_;
	player_ = nullptr;

	//マップの解放
	delete mapChipField_;

	//enemyの解放
	delete enemy_;
	enemy_ = nullptr;

	// 箱の解放
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			delete worldTransformBlock;
		}
	}
	worldTransformBlocks_.clear();

	// デバックカメラの解放
	delete debugCamera_;
	debugCamera_ = nullptr;
}

void GameScene::Update() {
	// ブロックの更新
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			// ブロックの穴あきを許容
			if (!worldTransformBlock) {
				continue; // nullチェック
			}

			// アフィン変換行列の作成
			worldTransformBlock->matWorld_ = MakeAffineMatrix(worldTransformBlock->scale_, worldTransformBlock->rotation_, worldTransformBlock->translation_);
			// 定数バッファに転送する
			worldTransformBlock->TransferMatrix();
		}
	}

	// skydomeのUPdate
	skydome_->Update();

	// playerのUPdate
	player_->Update();

	// デバックカメラの更新
	debugCamera_->Update();

	cameraController_->Update();

	enemy_->Update();

#ifdef _DEBUG
	if (Input::GetInstance()->TriggerKey(DIK_D)) {
		isDebugCameraActive_ = !isDebugCameraActive_; // デバックカメラの有効無効を切り替え
	}
#endif

	// カメラの更新
	if (isDebugCameraActive_) {
		// デバックカメラの更新
		debugCamera_->Update();

		camera_.matView = debugCamera_->GetCamera().matView;
		camera_.matProjection = debugCamera_->GetCamera().matProjection;
		// ビュープロダクションの転送
		camera_.TransferMatrix();
	} else {
		// ビュープロダクション行列の更新と転送
		camera_.UpdateMatrix();
		camera_.matView = cameraController_->GetViewProjection().matView;
		camera_.matProjection = cameraController_->GetViewProjection().matProjection;

		camera_.TransferMatrix();
	}
}

void GameScene::Draw() {

	// DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	// 3dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	// 箱の描画
	for (std::vector<WorldTransform*>& worldTransformBlockLine : worldTransformBlocks_) {
		for (WorldTransform* worldTransformBlock : worldTransformBlockLine) {
			// ブロックの穴あきを許容
			if (!worldTransformBlock) {
				continue; // nullチェック
			}
			modelBlock_->Draw(*worldTransformBlock, camera_);
		}
	}

	skydome_->Draw();

	player_->Draw();

	enemy_->Draw();

	// 3Dモデルの描画後処理
	Model::PostDraw();
}

void GameScene::GenerateBlocks() { 
	uint32_t numBlockVirtical = mapChipField_->GetNumBlockVirtical(); 
	uint32_t numBlockHorizontal=mapChipField_->GetNumBlockHorizontal();
	
	worldTransformBlocks_.resize(numBlockVirtical);
		for (uint32_t i = 0; i < numBlockVirtical; ++i) {
			worldTransformBlocks_[i].resize(numBlockHorizontal);
		}

		for (uint32_t i = 0; i < numBlockVirtical; i++) {
		    for (uint32_t j = 0; j < numBlockHorizontal; j++) {
			    if (mapChipField_->GetmapChiptypeByIndex(j,i)==MapChipType::kBlock) {
				    WorldTransform* worldTransform = new WorldTransform();
				    worldTransform->Initialize();
				    worldTransformBlocks_[i][j] = worldTransform;
				    worldTransformBlocks_[i][j]->translation_ = mapChipField_->GetMapChippositionByIndex(j, i);
			    }
			}
		}
}

