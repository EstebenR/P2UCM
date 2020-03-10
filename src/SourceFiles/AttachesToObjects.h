#pragma once
#include "Entity.h"
#include "Collider.h"
#include "checkML.h"

class AttachesToObjects : public Component
{
private:
	Collider* mainCollider = nullptr; //Collider del objeto principal (el Tinky)
	Collider* objectToBeAttached = nullptr; //Este ser� el collider con el que colisionara y se agarrara
	b2WeldJoint* joint = nullptr; //Joint entre el jugador y otro objeto
	int playerNumber_;
public:
	AttachesToObjects(int playerNumber) : Component(ComponentType::AttachesToObjects) { playerNumber_ = playerNumber; };
	~AttachesToObjects() {};
	void attachToObject(Collider* attachableObject, b2Vec2 collPoint); //Ata al jugador a otro collider
	void deAttachFromObject(); //Rompe el joint entre el jugador y el otro objeto
	virtual void init() override;
	virtual void handleInput();
};

