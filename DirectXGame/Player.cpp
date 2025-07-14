#define NOMINMAX
#include "Player.h"
#include <cassert>
#include"MeMath.h"
#include<numbers>
#include<algorithm>

using namespace KamataEngine;
using namespace MathUtility;


void Player::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera,const KamataEngine::Vector3& position) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
}

void Player::Update() { // アフィン変換
	                    // アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);

	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();

	//移動入力
	// 
	
	if (onGround_) {
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) || Input::GetInstance()->PushKey(DIK_LEFT)) {
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {

				if (velocity_.x < 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				if (lrDrirection_ != LRDirection::kRigth) {
					lrDrirection_ = LRDirection::kRigth;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}

				acceleration.x += kAccleration;
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {

				if (velocity_.x > 0.0f) {
					velocity_.x *= (1.0f - kAttenuation);
				}

				if (lrDrirection_ != LRDirection::kLeft) {
					lrDrirection_ = LRDirection::kLeft;
					turnFirstRotationY_ = worldTransform_.rotation_.y;
					turnTimer_ = kTimeTurn;
				}

				acceleration.x -= kAccleration;
			}
			// 加速減速
			velocity_ += acceleration;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);

		} else {
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			velocity_ += Vector3(0, kJumpAcceLeration, 0);
		}
	}
	//空中
	else {
		velocity_ += Vector3(0, -kGravityAcceleration, 0);

		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}

	bool landing = false;

	if (velocity_.y < 0) {
		if (worldTransform_.translation_.y <= 1.0f) {
			landing = true;
		}
	}

	if (onGround_) {
		if (velocity_.y > 0.0f) {
			onGround_ = false;
		}
	} else {
		if (landing) {
			worldTransform_.translation_.y = 1.0f;

			velocity_.x *= (1.0f - kAttenuation);

			velocity_.y = 0.0f;

			onGround_ = true;
		
		}
	}

	//移動
	worldTransform_.translation_ += velocity_;

	//旋回制御
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		//左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {
			std::numbers::pi_v<float> / 2.0f,
			std::numbers::pi_v<float> * 3.0f / 2.0f
		};
		//状態に応じた角度取得
		float destinationRotationY = 
			destinationRotationYTable[static_cast<uint32_t>(lrDrirection_)];

		worldTransform_.rotation_.y = 
			EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}


	//行列更新
	//  アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_,worldTransform_.translation_);
	// 定数バッファに転送する
	worldTransform_.TransferMatrix();

}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }