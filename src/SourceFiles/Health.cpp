#include "Health.h"
#include <iostream>
#include "Collider.h"
#include "Collision.h"
#include "Weapon.h"
#include "AttachesToObjects.h"
#include "CollisionHandler.h"
#include "ThrownByPlayer.h"

Health::Health(int l) : Component(ComponentType::Health)
{
	lives_ = livesMax_ = l;
}

void Health::init() {
	//col_ = GETCMP1_(Collider);
}

Health::~Health()
{
}

bool Health::subtractLife(int damage)
{
	if (lives_ > 0) {
		lives_ -= damage;
		if (lives_ <= 0) {
			lives_ = 0;
			return false;}	//Evitar vidas negativas
		return true;
	}
	else return false;
}

void Health::addLife(int sum)
{
	if (lives_ + sum > livesMax_) lives_ = livesMax_;
	else lives_ += sum;
}

void Health::playerDead(Collision* c)
{
	//reset player

	//soltar objetos agarrados
	AttachesToObjects* a = GETCMP1_(AttachesToObjects);
	if (a != nullptr && a->isAttached())
		c->collisionHandler->breakAttachment(a);
	//soltar arma
	Hands* h = GETCMP1_(Hands);
	Weapon* w = nullptr;
	if (h != nullptr) w = h->getWeapon();
	if (w != nullptr) c->collisionHandler->dropWeapon(w);
	//respawn
	GameMode* s = c->collisionHandler->getGamemode();
	PlayerData* p = GETCMP1_(PlayerData);
	CollisionHandler::moveData mov;
	mov.body = GETCMP1_(Collider)->getFixture(0)->GetBody();
	mov.pos = c->collisionHandler->getPlayerRespawnPoint(p->getPlayerNumber());
	if (s->onPlayerDead(p->getPlayerNumber())) {	//avisa al jugador si puede respawnear
		resetHealth();
	}
	else{	//si no le quedan vidas le mandamos lejos provisionalmente
		mov.pos = b2Vec2((1 + p->getPlayerNumber()) * 50, 0);
	}
	c->collisionHandler->addMove(mov);

	//cuerpo muerto
	CollisionHandler::bodyData body;
	b2Fixture* fix= GETCMP1_(Collider)->getFixture(0);
	body.pos = fix->GetBody()->GetPosition();
	body.angle = fix->GetBody()->GetAngle();
	c->collisionHandler->addCorpse(body);
}

void Health::onCollisionEnter(Collision* c)
{
	b2Fixture* fix = c->hitFixture;
	if (!fix->IsSensor())
	{
		b2Vec2 force = c->hitFixture->GetBody()->GetMass() * c->hitFixture->GetBody()->GetLinearVelocity();
		int impact = force.Length();
		Weapon* w = GETCMP_FROM_FIXTURE_(fix, Weapon);
		ThrownByPlayer* objThrown = nullptr;
		PlayerData* playerWhoHitMe = nullptr;
		//Si se impacta con un arma al umbral m�s alto de fuerza, se recibe su daño de impacto
		if (w != nullptr) {
			impact = (impact >= CONST(double, "HIGH_DAMAGE")) ? w->getImpactDamage() : 0;
			objThrown = GETCMP_FROM_FIXTURE_(fix, ThrownByPlayer);
		}
		else {
			//Depending on the force of impact we apply damage to the player

			if (impact < CONST(int, "LOW_DAMAGE")) impact = 0;

			else if (impact >= CONST(int, "LOW_DAMAGE") && impact < CONST(double, "MEDIUM_DAMAGE")) impact = 1;

			else if (impact >= CONST(double, "MEDIUM_DAMAGE") && impact < CONST(double, "HIGH_DAMAGE")) impact = 2;

			else if (impact >= CONST(double, "HIGH_DAMAGE")) impact = 3;

			// we get the Id of the player who hit me at high speed
			// (it may not exist, so we check later that it's not nullptr
			playerWhoHitMe = GETCMP_FROM_FIXTURE_(fix, PlayerData);
		}

		if (!subtractLife(impact)) {
			// player is killed by a weapon
			if (objThrown != nullptr) objThrown->addPointsToOwner();

			// player is killed by another player at high speed
			else if (playerWhoHitMe != nullptr) {
				GameMode* s = c->collisionHandler->getGamemode();
				s->playerKillsPlayer(playerWhoHitMe->getPlayerNumber());
			}

			playerDead(c);
		}
	}
}
