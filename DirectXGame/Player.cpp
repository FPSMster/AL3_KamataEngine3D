#include "Player.h"
#include <cassert>
#include"MeMath.h"

void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
}

void Player::Update() { // アフィン変換
	                    // アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }