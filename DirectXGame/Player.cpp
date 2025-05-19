#include "Player.h"
#include<cassert>



Player::Player() {}

Player::~Player() { 
	delete model_;
	model_ = nullptr;
}

void Player::Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera) { 
	assert(model);
	model_ = model;
	textureHandle_ = textureHandle;
	camera_ = camera;
	worldTransform_.Initialize();
}

void Player::Update() {
	worldTransform_.TransferMatrix();
	
}

void Player::Draw() {
	
	model_->Draw(worldTransform_, *camera_, textureHandle_);
}




