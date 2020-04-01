#pragma once

#include "Entity.h"
#include "checkML.h"

class Weapon;

class WeaponFactory
{
private:
public:

	WeaponFactory() {};

	~WeaponFactory() {};
	//Crea un mando de la tele (espec�fico para el modo Pelea Por El Mando)
	Weapon* makeController(EntityManager* entityManager, b2World* physiscsWorld, b2Vec2 pos, b2Vec2 size); 

	//Crea una chancla (Podemos pasarle los p�rametros que queramos segun las necesidades del objeto)
	static void makeChancla(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size);

	//Crea una pelota saltarina (Podemos pasarle los p�rametros que queramos segun las necesidades del objeto)
	static void makePelota(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size);

	//Crea una pelota saltarina (Podemos pasarle los p�rametros que queramos segun las necesidades del objeto)
	static void makeGrapadora(EntityManager* entityManager, b2World* physicsWorld, b2Vec2 pos, b2Vec2 size);

};

