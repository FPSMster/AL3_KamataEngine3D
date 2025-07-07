#pragma once
#include "KamataEngine.h"

class MeMath {
public:

	KamataEngine::Matrix4x4 MathMakeAffineMatrix(KamataEngine::Vector3& scale, 
		KamataEngine::Vector3& rotation, KamataEngine::Vector3& translation);

	

};
