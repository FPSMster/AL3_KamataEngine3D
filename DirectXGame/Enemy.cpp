#define NOMINMAX
#include "Enemy.h"
#include <cassert>
#include "MeMath.h"
#include <numbers>
#include <algorithm>

using namespace KamataEngine;
using namespace MathUtility;

void Enemy::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / -2.0f;

	velocity_ = {-kWalkSpeed, 0, 0};

	walkTimer_ = 0.0f;


}

Enemy::~Enemy() {}

void Enemy::Update() {

	worldTransform_.translation_.x -= kWalkSpeed;

	//タイマーを加算
	walkTimer_ += 1.0f / 60.0f;

	//回転アニメーション
	float param = std::sin((2 * PI) * walkTimer_ / kWalkMotionTime);
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = std::sin(degree);

	// 行列更新
	//   アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
	// 定数バッファに転送する
	worldTransform_.TransferMatrix();
}

void Enemy::Draw() { model_->Draw(worldTransform_, *camera_); }
