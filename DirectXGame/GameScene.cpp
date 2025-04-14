#include "GameScene.h"

using namespace KamataEngine;
		
//初期化
void GameScene::Initialize() {
	// ファイル名を指定してテクスチャを読み込む
	textureHandle_ = TextureManager::Load("mario.jpg");

	//3Dモデル
	model_ = Model::Create();

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();

	//カメラの初期化
	camera_.Initialize();
}

//更新
void GameScene::Update() {

	

}

//描画
void GameScene::Draw() {
	
	//DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	//3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	//3Dモデル描画
	model_->Draw(worldTransform_, camera_, textureHandle_);

	//3Dモデル描画後処理
	Model::PostDraw();

}

GameScene::~GameScene() {
	
	delete model_;

}




