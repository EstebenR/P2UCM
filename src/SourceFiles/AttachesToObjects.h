#pragma once
#include "Entity.h"
#include "Collider.h"
#include "checkML.h"
#include "PlayerData.h"

class AttachesToObjects : public Component
{
private:
	Collider* mainCollider_ = nullptr; //Collider del objeto principal (el Tinky)
	b2Body* attachedObject_ = nullptr; //Este ser� el collider con el que colisionara y se agarrara
	b2WeldJoint* joint_ = nullptr; //Joint entre el jugador y otro objeto
	PlayerData* playerData_ = nullptr;
public:
	AttachesToObjects() : Component(ComponentType::AttachesToObjects) {};
	~AttachesToObjects() {};
	void attachToObject(b2Body* attachableObject, b2Vec2 collPoint); //Ata al jugador a otro collider
	void deAttachFromObject(); //Rompe el joint entre el jugador y el otro objeto
	bool canAttachToObject(); //Comprueba si el jugador est� pulsando la tecla v�lida y si no est� sujeto a otro objeto
	bool isAttached(); //Te dice si actualmente est� agarrado a algo o no
	virtual void init() override;
	virtual void handleInput();
};