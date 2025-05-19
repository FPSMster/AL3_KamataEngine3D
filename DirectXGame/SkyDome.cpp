#include "SkyDome.h"

using namespace KamataEngine;

SkyDome::SkyDome() {}

SkyDome::~SkyDome() { 
	delete model_;
	model_ = nullptr;
}

void SkyDome::Initialize(Camera* camera) {
	/*assert(model);*/
	model_ = Model::CreateFromOBJ("skydome", true);
	camera_ = camera;
	worldTransform_.Initialize();
}

void SkyDome::Update() { 
	worldTransform_.TransferMatrix(); 
}

void SkyDome::Draw() {
	model_->Draw(worldTransform_, *camera_); 
}
