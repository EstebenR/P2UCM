#include "AttachesToObjects.h"
#include "InputHandler.h"
#include "Collision.h"
#include "CollisionHandler.h"

void AttachesToObjects::init() {
	playerData_ = GETCMP1_(PlayerData);
	mainCollider_ = GETCMP1_(Collider); //Obtiene el collider del cuerpo principal.

	playerNumber_ = playerData_->getPlayerNumber();
}

void AttachesToObjects::attachToObject(b2Body* attachedObject, b2Vec2 collPoint) {
	if (joint_ == nullptr) {
		attachedObject_ = attachedObject;
		b2WeldJointDef jointDef; //Definici�n del nuevo joint.
		jointDef.bodyA = mainCollider_->getBody(); //Body del jugador.
		jointDef.bodyB = attachedObject; //Body del objeto al que se tiene que atar.
		jointDef.collideConnected = false; //Flag que decide si estos 2 objetos van a ejercer f�sicas el uno contra el otro.
		jointDef.localAnchorA = jointDef.bodyA->GetLocalPoint(collPoint); //Punto donde se ata el cuerpo A al cuerpo B
		jointDef.localAnchorB = jointDef.bodyB->GetLocalPoint(collPoint); //Punto donde se ata el cuerpo B al cuerpo A
		jointDef.referenceAngle = jointDef.bodyB->GetAngle() - jointDef.bodyA->GetAngle(); //�ngulo conjunto del cuerpo
		b2World* world = mainCollider_->getWorld(); //Obtenemos el mundo f�sico para crear el joint
		joint_ = (b2WeldJoint*)world->CreateJoint(&jointDef); //Crea el joint con la definici�n que hemos definido previamente
	}
}

void AttachesToObjects::deAttachFromObject() {
	if (joint_ != nullptr) {
		mainCollider_->getWorld()->DestroyJoint(joint_); //Destruye el joint
		joint_ = nullptr; //Al hacer la acci�n de arriba, el puntero a joint_ sigue apuntando a una direcci�n de memoria que NO es v�lida. Por eso se iguala a nullptr
		attachedObject_ = nullptr;
	}
}

bool AttachesToObjects::canAttachToObject() { //Se agarra si est� pretando una tecla v�lida y si no est� agarrado a otra cosa.
	InputHandler* ih = SDL_Game::instance()->getInputHandler();
	if (ih->getTrigger(playerNumber_, InputHandler::GAMEPADTRIGGER::LEFTTRIGGER) || ih->isKeyDown(SDLK_SPACE)){
		if (attachedObject_ == nullptr) return true;
	}
	return false;
}

bool AttachesToObjects::isAttached()
{
	if (joint_ == nullptr) return false;
	else return true;
}

void AttachesToObjects::handleInput() { //Si el jugador suelta la tecla de agarre, se suelta.

}

void AttachesToObjects::onCollisionEnter(Collision* c){
	//si chocamos con un objeto que pueda agarrarse
	if (c->hitFixture->GetFilterData().categoryBits == Collider::CollisionLayer::Wall || c->hitFixture->GetFilterData().categoryBits == Collider::CollisionLayer::NormalAttachableObject) {
		if (canAttachToObject()) {
			b2WorldManifold manifold;
			c->contact->GetWorldManifold(&manifold);
			c->collisionHandler->createWeld
			(CollisionHandler::weldData(this, c->hitFixture->GetBody(), b2Vec2(manifold.points[0].x, manifold.points[0].y)));
		}
		else {
			cout << "colision sin input con grabbable" << endl;
		}
	}
}
