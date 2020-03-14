#include "AttachesToObjects.h"
#include "InputHandler.h"

void AttachesToObjects::init() {
	mainCollider_ = GETCMP1_(Collider); //Obtiene el collider del cuerpo principal.
	mainCollider_->setUserData(this);
}

void AttachesToObjects::attachToObject(b2Body* attachableObject, b2Vec2 collPoint) {
	b2WeldJointDef jointDef; //Definici�n del nuevo joint.
	jointDef.bodyA = mainCollider_->getBody(); //Body del jugador.
	jointDef.bodyB = attachableObject; //Body del objeto al que se tiene que atar.
	jointDef.collideConnected = true; //Flag que decide si estos 2 objetos van a ejercer f�sicas el uno contra el otro.
	jointDef.localAnchorA = collPoint; //Punto en el que se crea el joint en referencia al JUGADOR. Esto queda por revisar una vez est� implementado el collider fancy de �lvar.

	b2World* world = mainCollider_->getWorld();
	joint_ = (b2WeldJoint*)world->CreateJoint(&jointDef); //Crea el joint con la definici�n que hemos creado previamente
}

void AttachesToObjects::deAttachFromObject() {
	if (joint_ != nullptr) { //Destruye el joint. Si haces un delete(joint) peta por alg�n motivo. CREO que esto no deja basura.
		mainCollider_->getWorld()->DestroyJoint(joint_);
		joint_ = nullptr;
	}
}

void AttachesToObjects::handleInput() {
	InputHandler* ih = SDL_Game::instance()->getInputHandler();

	if (ih->isButtonJustDown(playerNumber_, SDL_CONTROLLER_BUTTON_A)) {
		cout << "just pressed A" << endl;
	}

	else if (ih->isButtonJustUp(playerNumber_, SDL_CONTROLLER_BUTTON_A)) {
		deAttachFromObject();
	}
}