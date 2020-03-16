#include "Collider.h"

Collider::Collider(b2World* world, b2BodyType type, float x, float y, float width, float height,

	float density, float friction, float restitution, float linearDrag, float angDrag, CollisionLayer c, bool sensor, bool canBeAttached) :

	world_(world),
	Component(ComponentType::Collider)
{
	bodyDef_.type = type;
	bodyDef_.position.Set(x, y);
	bodyDef_.linearDamping = linearDrag;
	bodyDef_.angularDamping = angDrag;
	body_ = world_->CreateBody(&bodyDef_);

	if (!canBeAttached) createFixture(width, height, density, friction, restitution, c, sensor);
	else createFixture(width, height, density, friction, restitution, CollisionLayer::NormalAttachableObject, sensor);
	/*Este modelo se puede expandir de la siguiente manera.
	Una situaci�n probable ser�a necesitar, por ejemplo, una capa Chair y otra AttachableChair, ya que la capa NormalGrabbableObject no nos sirve.
	A continuaci�n, cambiar�amos estas 2 l�neas anteriores de la siguiente manera:
		if (!canBeAttached) createFixture(width, height, density, friction, restitution, c, sensor);
		else createFixture(width, height, density, friction, restitution, (CollisionLayer)(c+1), sensor);*/
}

void Collider::createFixture(float width, float height, float density,
	float friction, float restitution, CollisionLayer c, bool sensor) {
	widths_.push_back(width);
	heights_.push_back(height);
	b2PolygonShape shape;
	shape.SetAsBox(width, height);
	shapes_.push_back(shape);
	b2FixtureDef aux;
	aux.shape = &shapes_.back();
	aux.density = density;
	aux.friction = friction;
	aux.restitution = fmod(restitution, 1.0);
	switch (c) {
	case NormalObject:
		aux.filter.categoryBits = NormalObject; //what am I?
		aux.filter.maskBits = NormalObject | NormalAttachableObject | Player; //what do I collide with?
		break;
	case NormalAttachableObject:
		aux.filter.categoryBits = NormalAttachableObject; //what am I?
		aux.filter.maskBits = NormalObject | NormalAttachableObject | Player; //what do I collide with?
		break;
	case Player:
		aux.filter.categoryBits = Player;
		aux.filter.maskBits = NormalObject | NormalAttachableObject | Player | Trigger | Weapon;
		break;
	case Trigger:
		aux.filter.categoryBits = Trigger;
		aux.filter.maskBits = Player;
		break;
	case Weapon:
		aux.filter.categoryBits = Weapon;
		aux.filter.maskBits = Player;
		break;
	}
	aux.isSensor = sensor;
	fixtureDefs_.push_back(aux);
	fixtures_.push_back(body_->CreateFixture(&fixtureDefs_.back()));
}

void Collider::destroyFixture(int i) {
	body_->DestroyFixture(fixtures_[i]);
	/*fixtures_.erase(fixtures_.begin() + i);
	fixtureDef_.erase(fixtureDef_.begin() + i);
	shape_.erase(shape_.begin() + i);*/

}
