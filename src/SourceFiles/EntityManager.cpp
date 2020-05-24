#include "EntityManager.h"
#include "Entity.h"
#include "Weapon.h"
#include "Component.h"

EntityManager::EntityManager() {}

EntityManager::~EntityManager() {
	entities_.clear();
	externalEntities_.clear();
	weapons_.clear();
}

void EntityManager::update() {
	for (auto& e : entities_)
			if(e->isActive()) e->update();
	for (auto& e : externalEntities_)
			if(e->isActive()) e->update();
}

void EntityManager::render() {
	for (auto& e : entities_)
		if(e->isActive()) e->render();
	for (auto& e : externalEntities_)
		if (e->isActive()) e->render();
}
void EntityManager::handleInput() {
	for (auto& e : entities_)
		if (e->isActive()) e->handleInput();
	for (auto& e : externalEntities_)
		if (e->isActive()) e->handleInput();
}

Entity* EntityManager::addEntity() {
	Entity* e = new Entity(this);
	std::unique_ptr<Entity> uPtr(e);
	entities_.emplace_back(std::move(uPtr));
	return e;
}

void EntityManager::addExistingEntity(Entity* e) {
	e->setEntityManager(this);
	externalEntities_.push_back(e);
}

std::vector<Weapon*>* EntityManager::getWeaponVector() { return &weapons_; }
