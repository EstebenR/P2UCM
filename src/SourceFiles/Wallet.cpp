#include "Wallet.h"
#include "Entity.h"
#include "CapitalismGameMode.h"
#include "Collision.h"

void Wallet::init()
{
	numCoins_ = CONST(int, "INITIAL_COINS");
	collPlayer_ = GETCMP1_(Collider);
	coinBaseSize_ = CONST(double, "COIN_BASE_SIZE");

	INV_FRAMES_HIT_ = CONST(int, "INVULNERABILITY_FRAMES_HIT");
	INV_FRAMES_RESPAWN_ = CONST(int, "INVULNERABILITY_FRAMES_RESPAWN");
	invFrames_ = 0;
}

void Wallet::update()
{
	if (invFrames_ > 0) {
		invFrames_--;
	}
}

void Wallet::dropCoins(int damage, int player)
{
	//hard cap para evitar gastar la pool
	if (damage > 3)
		damage = 3;
	int coinsToDrop = numCoins_ * (damage * CONST(double, "COIN_DROP_RATE"));
	if (coinsToDrop == 0) coinsToDrop = numCoins_;
	numCoins_ -= coinsToDrop;
	if (numCoins_ < 0) numCoins_ = 0;
	cout << "HE SOLTADO " << coinsToDrop << endl;
	for (int k = 0; k < coinsToDrop; k++) {
		gameMode_->createCoin(b2Vec2(collPlayer_->getTransform().p.x, collPlayer_->getTransform().p.y), player);
	}
	invFrames_ = INV_FRAMES_HIT_;
}

void Wallet::onCollisionEnter(Collision* c)
{
	if(invFrames_ <=0) {

		b2Fixture* fix = c->hitFixture;
		if (!fix->IsSensor())
		{
			//Measure de impact of an object with the player
			b2Vec2 force = fix->GetBody()->GetMass() * fix->GetBody()->GetLinearVelocity();

			int impact = force.Length();

			Weapon* w = GETCMP_FROM_FIXTURE_(fix, Weapon);
			//Si se impacta con un arma al umbral m�s alto de fuerza, se recibe su da�o de impacto
			if (w != nullptr) {
				//Cogemos nuestras manos
				Hands* h = GETCMP1_(Hands);

				if (impact >= CONST(double, "DISARM_IMPACT")) {
					//Nos desarman con el golpe
					Weapon* we = nullptr;
					if (h != nullptr) we = h->getWeapon();

					//Si tenemos un arma cogida la soltamos al haber sido golpeados
					if (we != nullptr) c->collisionHandler->dropWeapon(we);
				}

				impact = (impact >= CONST(double, "HIGH_DAMAGE")) ? w->getImpactDamage() : 0;
			}
			else {
				//Depending on the force of impact we apply damage to the player

				if (impact < CONST(int, "LOW_DAMAGE")) impact = 0;

				else if (impact >= CONST(int, "LOW_DAMAGE") && impact < CONST(double, "MEDIUM_DAMAGE")) impact = 1;

				else if (impact >= CONST(double, "MEDIUM_DAMAGE") && impact < CONST(double, "HIGH_DAMAGE")) impact = 2;

				else if (impact >= CONST(double, "HIGH_DAMAGE")) /*&& impact < CONST(double, "HIGH_DAMAGE"))*/ impact = 3;
			}

			if (impact > 0)
				c->collisionHandler->addCoinDrop(std::make_tuple(this, GETCMP1_(PlayerData), impact));
		}
	}
}
