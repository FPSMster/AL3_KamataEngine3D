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

	//自キャラの生成
	player_ = new Player();

	//自キャラの初期化
	player_->Initialize(model_,textureHandle_,&camera_);
}

//更新
void GameScene::Update() {

	//自キャラの更新
	player_->Update();


}

//描画
void GameScene::Draw() {
	
	//DirectXCommonインスタンスの取得
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	//3Dモデル描画前処理
	Model::PreDraw(dxCommon->GetCommandList());

	//3Dモデル描画
	//model_->Draw(worldTransform_, camera_, textureHandle_);

	// 自キャラの描画
	player_->Draw();

	//3Dモデル描画後処理
	Model::PostDraw();

}

//デストラクタ
GameScene::~GameScene() {
	


	//3Dモデルデータの解放
	delete model_;

	//自キャラの解放
	delete player_;
}




