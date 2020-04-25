#include "TomatoLogic.h"
#include "Collider.h"
#include "Entity.h"
#include "Collision.h"
#include "Health.h"
#include "Wallet.h"
#include "PlayerData.h"
#include "Viewer.h"
#include "InputHandler.h"
#include "ParticleEmitter.h"
#include "Hands.h"
#include "CollisionHandler.h"

void TomatoLogic::init() {
	ActionableWeapon::init();
	colTomato_ = GETCMP1_(Collider);
	tomatoViewer_ = GETCMP1_(Viewer);
	particleEmitterTomato_ = GETCMP1_(ParticleEmitter);

	timeForExplosion_ = CONST(int, "TOMATO_TIME_CHARGE");
	timeForExplosionExpire_ = CONST(int, "TOMATO_TIME_EXPLOSION");
	nFramesCharge_ = CONST(int, "TOMATO_N_FRAMES_ACTIVATED");
	nFramesExplosion_ = CONST(int, "TOMATO_N_FRAMES_EXPLOSION");
	damageOnExplosionImpact_ = CONST(int, "TOMATO_DAMAGE");
	explosionSize_ = CONST(int, "TOMATO_EXPLOSION_SIZE");
	frameSize_ = tomatoViewer_->getTexture()->getHeight();

	frameSpeedCharge_ = timeForExplosion_ / nFramesCharge_;
	frameSpeedExplosion_ = timeForExplosionExpire_ / nFramesExplosion_;
}

void TomatoLogic::update() {
	if (currentHand_) mainCollider_->setTransform(currentHand_->getPointerPos(), 0.0);

	if (activated_) {

		if (SDL_Game::instance()->getTime() > timeForExplosion_) {
			colTomato_->createCircularFixture(explosionSize_, 0, 0, 0, Collider::CollisionLayer::NormalObject, true);
			timeForExplosionExpire_ = SDL_Game::instance()->getTime() + timeForExplosionExpire_;
			timeExploded_ = SDL_Game::instance()->getTime();
			exploded_ = true;
			activated_ = false;
			if (picked_) UnPickObject();
			colTomato_->setLinearVelocity({ 0,0 });
		}
		frame = 1 + (SDL_Game::instance()->getTime() - timeActivated_) / frameSpeedCharge_;
		tomatoViewer_->setClip(SDL_Rect{ frame * frameSize_, 0, frameSize_, frameSize_ });
	}
	else if (exploded_) {
		if (SDL_Game::instance()->getTime() > timeForExplosionExpire_) {
			colTomato_->destroyFixture(1);
			setActive(false);
		}
		frame = 1 + nFramesCharge_ + (SDL_Game::instance()->getTime() - timeExploded_) / frameSpeedExplosion_;
		tomatoViewer_->setClip(SDL_Rect{ frame * frameSize_, 0, frameSize_, frameSize_ });
	}
	
}

void TomatoLogic::onCollisionEnter(Collision* c) {
	Weapon::onCollisionEnter(c);

	if (exploded_) {
		Entity* other = c->entity;

		Health* healthPlayer = GETCMP2(other, Health);
		Wallet* walletPlayer = GETCMP2(other, Wallet);
		PlayerData* playerData = GETCMP2(other, PlayerData);
		Collider* collPlayer = GETCMP2(other, Collider);

		if (healthPlayer && collPlayer) {
			healthPlayer->subtractLife(damageOnExplosionImpact_);
		}
		else if (walletPlayer && collPlayer) {
			c->collisionHandler->addCoinDrop(std::make_tuple(walletPlayer, GETCMP2(c->entity, PlayerData), 3));
		}

		b2Vec2 dir = (collPlayer->getPos() - colTomato_->getPos()).NormalizedVector();
		collPlayer->applyForce({ dir.x * 10000, dir.y * 10000 }, { 0,0 });
	}
}

void TomatoLogic::action() {
	if (!activated_) {
		activated_ = true;
		timeForExplosion_ = SDL_Game::instance()->getTime() + timeForExplosion_;
		timeActivated_ = SDL_Game::instance()->getTime();
		particleEmitterTomato_->setPositionCollider(colTomato_);
		particleEmitterTomato_->setDirection({ 0, -1 });
		particleEmitterTomato_->PlayStop();
	}
}

void TomatoLogic::PickObjectBy(Hands* playerHands)
{
	if (playerHands->getWeaponID() == NoWeapon) {
		currentHand_ = playerHands;
		picked_ = true;
		currentHand_->setWeapon(weaponType_, this);
		vw_->setDrawable(false);
		b2Filter pickUpCollider = mainCollider_->getFixture(0)->GetFilterData();
		pickUpCollider.categoryBits = 0;
		pickUpCollider.maskBits = 0;
		mainCollider_->getFixture(0)->SetFilterData(pickUpCollider);
	}
}

void TomatoLogic::UnPickObject() {
	ActionableWeapon::UnPickObject();
	b2Filter pickUpCollider = mainCollider_->getFixture(0)->GetFilterData();
	pickUpCollider.categoryBits = Collider::CollisionLayer::PickableObject;
	pickUpCollider.maskBits = Collider::CollisionLayer::Player | Collider::CollisionLayer::Wall;
	mainCollider_->getFixture(0)->SetFilterData(pickUpCollider);

}

void TomatoLogic::setActive(bool a, b2Vec2 pos) {
	entity_->setActive(a);
	tomatoViewer_->setDrawable(a);
	colTomato_->getBody()->SetEnabled(a);
	if (a) colTomato_->getBody()->SetTransform(pos, 0);
	else {
		exploded_ = false;
		activated_ = false;
	}
}
