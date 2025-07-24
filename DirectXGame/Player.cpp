#define NOMINMAX
#include "Player.h"
#include <cassert>
#include"MeMath.h"
#include<numbers>
#include<algorithm>
#include"MapChipField.h"

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


	// 1移動入力
	InputMove();

	//衝突情報を初期化
	CollisionMapInfo collisionMapInfo;
	//移動量に速度の値をコピー
	collisionMapInfo.move = velocity_;
	//2マップ衝突チェック
	CheckMapCollision(collisionMapInfo);
	
	// 3判定結果を反映して移動させる
	CheckMapMove(collisionMapInfo);

	//4天井に接触している場合の処理
	CheckMapCeiling(collisionMapInfo);

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

	

	//旋回制御
	AnimateTurn();


	//行列更新
	//  アフィン変換行列の作成
	worldTransform_.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_,worldTransform_.translation_);
	// 定数バッファに転送する
	worldTransform_.TransferMatrix();

}

void Player::Draw() { model_->Draw(worldTransform_, *camera_); }

//1移動入力
void Player::InputMove() 
{
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
	// 空中
	else {
		velocity_ += Vector3(0, -kGravityAcceleration, 0);

		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);
	}
}

// 2マップ衝突判定
void Player::CheckMapCollision(CollisionMapInfo& info) { 
	CheckMapCollisionUp(info); 
	CheckMapCollisionDown(info); 
	//CheckMapCollisionRight(info); 
	//CheckMapCollisionLeft(info); 
}

//マップ衝突判定　上
void Player::CheckMapCollisionUp(CollisionMapInfo& info) {
	
	//上昇あり？
	if (info.move.y<=0) {
		return;
	}

	//移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew; 

	for (uint32_t i = 0; i < positionNew.size(); i++) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	//真上の当たり判定
	bool hit = false;
	//左上の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftTop]);
	mapChipType = mapChipField_->GetmapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType==MapChipType::kBlock) {
		hit = true;
	}
	//右上の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightTop]);
	mapChipType = mapChipField_->GetmapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		//めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_+info.move+ KamataEngine::Vector3(0,kHeight/2.0f,0));
		//めり込み先にブロックがあるか
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y=std::max(0.0f,rect.bottom-worldTransform_.translation_.y-(kHeight/2.0f+kBlank));
		//天井に当たったことを記録する
		info.ceiling=true;
	}
}

//マップ衝突判定　下
void Player::CheckMapCollisionDown(CollisionMapInfo& info) {

	// 下降あり？
	if (info.move.y >= 0) {
		return;
	}

	// 移動後の4つの角の座標
	std::array<Vector3, kNumCorner> positionNew;

	for (uint32_t i = 0; i < positionNew.size(); i++) {
		positionNew[i] = CornerPosition(worldTransform_.translation_ + info.move, static_cast<Corner>(i));
	}

	MapChipType mapChipType;
	// 真下の当たり判定
	bool hit = false;
	// 左下の判定
	MapChipField::IndexSet indexSet;
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kLeftBottom]);
	mapChipType = mapChipField_->GetmapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}
	// 右下の判定
	indexSet = mapChipField_->GetMapChipIndexSetByPosition(positionNew[kRightBottom]);
	mapChipType = mapChipField_->GetmapChiptypeByIndex(indexSet.xIndex, indexSet.yIndex);
	if (mapChipType == MapChipType::kBlock) {
		hit = true;
	}

	if (hit) {
		// めり込みを排除する方向に移動量を設定する
		indexSet = mapChipField_->GetMapChipIndexSetByPosition(worldTransform_.translation_ + info.move + KamataEngine::Vector3(0, kHeight / 2.0f, 0));
		// めり込み先にブロックがあるか
		MapChipField::Rect rect = mapChipField_->GetRectByIndex(indexSet.xIndex, indexSet.yIndex);
		info.move.y = std::max(0.0f, rect.bottom - worldTransform_.translation_.y - (kHeight / 2.0f + kBlank));
		// 天井に当たったことを記録する
		info.ceiling = true;
	}
}

// 3判定結果を反映して移動させる
void Player::CheckMapMove(const CollisionMapInfo& info) {
	//移動
	worldTransform_.translation_ += info.move;

}
//4天井に接触している場合の処理
void Player::CheckMapCeiling(const CollisionMapInfo& info) {

	//天井に当たった？
	if (info.ceiling) {
		DebugText::GetInstance()->ConsolePrintf("hit ceiling\n");
		velocity_.y = 0;
	}

}

// 指定された角の座標計算
KamataEngine::Vector3 Player::CornerPosition(const KamataEngine::Vector3& center, Corner corner) {
	
	Vector3 offsetTable[kNumCorner] = {
	    {+kWidth / 2.0f, -kHeight / 2.0f, 0},
        {-kWidth / 2.0f, -kHeight / 2.0f, 0},
        {+kWidth / 2.0f, +kHeight / 2.0f, 0},
        {-kWidth / 2.0f, +kHeight / 2.0f, 0}
	};

	return center + offsetTable[static_cast<uint32_t>(corner)];

}

// 7旋回制御
void Player::AnimateTurn() 
{
	if (turnTimer_ > 0.0f) {
		turnTimer_ -= 1.0f / 60.0f;
		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {std::numbers::pi_v<float> / 2.0f, std::numbers::pi_v<float> * 3.0f / 2.0f};
		// 状態に応じた角度取得
		float destinationRotationY = destinationRotationYTable[static_cast<uint32_t>(lrDrirection_)];

		worldTransform_.rotation_.y = EaseInOut(destinationRotationY, turnFirstRotationY_, turnTimer_ / kTimeTurn);
	}
}

