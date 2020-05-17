#include "AttachesToObjects.h"
#include "InputHandler.h"
#include "Collision.h"
#include "CollisionHandler.h"
#include "SDL_macros.h"
#include <math.h>

void AttachesToObjects::init() {
	playerData_ = GETCMP1_(PlayerData);
	mainCollider_ = GETCMP1_(Collider); //Obtiene el collider del cuerpo principal.

	playerNumber_ = playerData_->getPlayerNumber();
	ib = GETCMP1_(PlayerData)->getBinder();
	KeyboardBinder* bindAux = static_cast<KeyboardBinder*>(ib);
	if (bindAux != nullptr) kBinder_ = bindAux;
}

void AttachesToObjects::attachToObject(b2Body* attachedObject, b2Vec2 collPoint, b2Vec2 collNormal) {
	if (joint_ == nullptr) {
		attachedObject_ = attachedObject;
		b2Vec2 perp = perpendicularCounterClockwise(collNormal);
		float attachAngle = std::atanf(perp.y / perp.x);
		int tilt = ((attachAngle - mainCollider_->getBody()->GetAngle()) > 0) ? -1 : 1;
		attachAngle += (PI / 2) * tilt;

		normalOnAttach_ = -collNormal;
		normalOnAttach_.Normalize();

		float angleSin = sin(mainCollider_->getBody()->GetAngle());

		if (angleSin >= sin(attachAngle) - CONST(double, "GRAB_ANGLE_TOLERANCE") && angleSin <= sin(attachAngle) + CONST(double, "GRAB_ANGLE_TOLERANCE")) {
			mainCollider_->setTransform(mainCollider_->getPos(), attachAngle);
		}

		else {
			normalOnAttach_.x *= mainCollider_->getH(0) / 2.3;
			normalOnAttach_.y *= mainCollider_->getH(0) / 2.3;

			mainCollider_->setTransform(mainCollider_->getPos() + normalOnAttach_, attachAngle);
		}

		b2WeldJointDef jointDef; //Definici�n del nuevo joint.
		jointDef.bodyA = mainCollider_->getBody(); //Body del jugador.
		jointDef.bodyB = attachedObject; //Body del objeto al que se tiene que atar.
		jointDef.collideConnected = false; //Flag que decide si estos 2 objetos van a ejercer f�sicas el uno contra el otro.
		jointDef.localAnchorA = jointDef.bodyA->GetLocalPoint(collPoint); //Punto donde se ata el cuerpo A al cuerpo B
		jointDef.localAnchorB = jointDef.bodyB->GetLocalPoint(collPoint); //Punto donde se ata el cuerpo B al cuerpo A
		jointDef.referenceAngle = jointDef.bodyB->GetAngle() - jointDef.bodyA->GetAngle(); //�ngulo conjunto del cuerpo
		cout << "se agarra en " << jointDef.localAnchorA.x << " " << jointDef.localAnchorA.y << " con un angulo local de " << jointDef.bodyA->GetAngle() << "y global " << jointDef.referenceAngle << endl;
		b2World* world = mainCollider_->getWorld(); //Obtenemos el mundo f�sico para crear el joint
		joint_ = (b2WeldJoint*)world->CreateJoint(&jointDef); //Crea el joint con la definici�n que hemos definido previamente
	}
}

void AttachesToObjects::deAttachFromObject() {
	if (joint_ != nullptr) {
		mainCollider_->getWorld()->DestroyJoint(joint_); //Destruye el joint
		joint_ = nullptr; //Al hacer la acci�n de arriba, el puntero a joint_ sigue apuntando a una direcci�n de memoria que NO es v�lida. Por eso se iguala a nullptr
		attachedObject_ = nullptr;
		attachedCollider_ = nullptr;
	}
}

bool AttachesToObjects::canAttachToObject() { //Se agarra si est� pretando una tecla v�lida y si no est� agarrado a otra cosa.
	return (attachedObject_ == nullptr && ib->holdGrab());
}

bool AttachesToObjects::isAttached()
{
	if (joint_ == nullptr) return false;
	else return true;
}

void AttachesToObjects::handleInput() { //Si el jugador suelta la tecla de agarre, se suelta.
}

void AttachesToObjects::onCollisionEnter(Collision* c) {
	//si chocamos con un objeto que pueda agarrarse
	if (c->hitFixture->GetFilterData().categoryBits & (Collider::CollisionLayer::Wall | Collider::CollisionLayer::NormalAttachableObject)) {
		if (canAttachToObject()) {
			b2WorldManifold manifold;
			attachedCollider_ = GETCMP2(c->entity, Collider);
			c->contact->GetWorldManifold(&manifold);
			c->collisionHandler->createWeld(CollisionHandler::weldData(this, c->hitFixture->GetBody(),
				b2Vec2(manifold.points[0].x, manifold.points[0].y), manifold.normal));
			if (kBinder_ != nullptr) kBinder_->grabbed = true;
		}
		else {
			cout << "colision sin input con grabbable" << endl;
		}
	}
}

b2Vec2 AttachesToObjects::getAttachmentNormal()
{
	b2Vec2 n;
	float diff = getRotationDifference();
	float cs = std::cos(diff);
	float sn = std::sin(diff);
	//rotamos el vector en funcion de la rotacion del cuerpo desde que nos agarramos
	n.x = normalOnAttach_.x * cs - normalOnAttach_.y * sn;
	n.y = normalOnAttach_.x * sn + normalOnAttach_.y * cs;
	return n;
}

float AttachesToObjects::getRotationDifference()
{
	return attachedObject_->GetAngle() - angleOnAttach_;
}