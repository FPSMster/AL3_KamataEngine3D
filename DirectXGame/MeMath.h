#pragma once

#include "KamataEngine.h"

/// <summary>
/// アフィン変換
/// </summary>
KamataEngine::Matrix4x4 MakeAffineMatrix(KamataEngine::Vector3& scale, KamataEngine::Vector3& rotation, KamataEngine::Vector3& translation);

float EaseInOut(float x1, float x2, float t);

/// <summary>
/// 行列を計算・転送する
/// </summary>
// void UpdateMatrix(KamataEngine::WorldTransform& worldTransform);