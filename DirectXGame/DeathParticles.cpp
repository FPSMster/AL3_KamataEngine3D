#include "DeathParticles.h"
#include <cassert>
#include "MeMath.h"
#include <numbers>
#include <algorithm>

using namespace KamataEngine;
using namespace MathUtility;

void DeathParticles::Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position) {
	assert(model);
	assert(camera);

	model_ = model;
	camera_ = camera;
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;

	for (WorldTransform& worldTransform : worldTransforms_) {
		worldTransform_.Initialize();
		worldTransform_.translation_ = position;
	}

}

void DeathParticles::Update() {
	//ワールド変換の更新
	for (WorldTransform& worldTransform : worldTransforms_) {
		//  アフィン変換行列の作成
		worldTransform.matWorld_ = MakeAffineMatrix(worldTransform_.scale_, worldTransform_.rotation_, worldTransform_.translation_);
		// 定数バッファに転送する
		worldTransform.TransferMatrix();
	}
}

void DeathParticles::Draw() {
	for (WorldTransform& worldTransform : worldTransforms_) {
		model_->Draw(worldTransform, *camera_); 
	}
}
