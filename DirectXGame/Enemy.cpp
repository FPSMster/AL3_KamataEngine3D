#include "Enemy.h"
#include"MeMath.h"

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
}

void Enemy::Update() {
	// 行列更新
	//   アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 定数バッファに転送する
	worldTransform_.TransferMatrix();
}

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }
