#pragma once
#include "KamataEngine.h"


class Player {
public:

	enum class LRDirection {
		kRigth,
		kLeft,
	};

	KamataEngine::Vector3 velocity_ = {};

	// 初期化
	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera,const KamataEngine::Vector3& position);

	// 更新
	void Update();

	// 描画
	void Draw();

	static inline const float kAccleration = 0.1f;

	static inline const float kAttenuation = 0.8f;

	static inline const float kLimitRunSpeed = 0.3f;

	float turnFirstRotationY_ = 0.0f;

	float turnTimer_ = 0.3f;

	static inline const float kTimeTurn = 0.3f;

	bool onGround_ = true;

	static inline const float kGravityAcceleration = 0.3f;

	static inline const float kLimitFallSpeed = 0.3f;
	
	static inline const float kJumpAcceLeration = 0.5f;

	LRDirection lrDrirection_ = LRDirection::kRigth;

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	KamataEngine::Camera* camera_ = nullptr;
};