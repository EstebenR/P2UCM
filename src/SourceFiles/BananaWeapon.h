#pragma once
#include "ActionableWeapon.h"
#include "Hands.h"
#include "ParticleEmitter.h"
#include "PlayState.h"

class BananaWeapon :public ActionableWeapon
{
private:
	Collider* colBanana_ = nullptr;
	ParticleEmitter* pEmitterBanana_ = nullptr;
	Viewer* viewer_;
	PlayState* playState_;
public:
	BananaWeapon() :ActionableWeapon(ComponentType::Banana, WeaponID::Mancuerna, 0, 0) {
	} //provisional
	~BananaWeapon(){}

	virtual void init();
	virtual void action() override;
	virtual void setActive(bool a, b2Vec2 pos = { 0,0 });
};

