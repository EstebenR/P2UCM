#include "TomatoPool.h"
#include "TomatoLogic.h"
#include "Entity.h"
#include "ObjectFactory.h"

TomatoPool::TomatoPool() : tomatoPool_([](Entity* e) { return e->isActive(); }) {};

void TomatoPool::init(EntityManager* eMan, b2World* physicsWorld) {
	vector<Entity*> tomatoPool = tomatoPool_.getPool();
	for (Entity* e : tomatoPool) {
		ObjectFactory::makeTomato(e, eMan, physicsWorld, b2Vec2(0, 0));
		e->getComponent<TomatoLogic>(ComponentType::TomatoLogic)->setActive(false);
	}
}

void TomatoPool::addTomato(b2Vec2 pos) {
	Entity* e = tomatoPool_.getObj();
	if (e != nullptr) {
		TomatoLogic* tomato = e->getComponent<TomatoLogic>(ComponentType::TomatoLogic);
		tomato->setActive(true, pos);
	}
}
