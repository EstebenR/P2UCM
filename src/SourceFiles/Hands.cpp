#include "Hands.h"
#include "Entity.h"
#include "Resources.h"
#include "InputHandler.h"
#include "InputBinder.h"

Hands::Hands(int textureId, WeaponID wId) :
	AnimatedViewer(textureId, 20, ComponentType::Hands)
{
}

void Hands::init()
{
	AnimatedViewer::init();
	stopAnimation();
	
	playerData_ = GETCMP1_(PlayerData);
	collider_ = GETCMP1_(Collider);
	armLengthPhysics_ = CONST(double, "ARM_LENGTH_PHYSICS");
	handSize_ = (CONST(double, "HAND_SIZE"));
	ib_ = playerData_->getBinder();
}

void Hands::draw() const
{
	b2Vec2 stickDir = dir_;
	double angle = std::atan2((double)stickDir.x, -(double)stickDir.y) * (180.0 / PI) + 90;
	
	if (drawable_) {
		SDL_Rect drawRect = transform_->getRectRender();
		drawRect.x += renderOffset_.x;
		drawRect.y += renderOffset_.y;
		tex_->render(drawRect, angle, frameX_, frameY_, Flipped_); // getAngle devuelve radianes, hay que pasarlos a �ngulos
	}
}

void Hands::handleInput()
{
	b2Vec2 vI = (ib_->getAimDir()).NormalizedVector();
	if (vI.Length() != 0) dir_.Set(vI.x, vI.y);
}

void Hands::update()
{
	AnimatedViewer::update();
	angle_ = (std::asin(dir_.x) * -180.0 / PI) - 90;

	//el arcoseno solo nos devuelve angulos en el intervalo 0� - 180�, si apuntamos hacia abajo hay que coger el angulo inverso
	if (dir_.y < 0) angle_ = (int)(360 - angle_) % 360;

	float dispAngHand = (int)(180 - angle_) % 360;	 //el angulo estandarizado de la mano

	float dispAngPlayer = (360 + (90 + (int)(-collider_->getAngleInDegrees()) % 360)) % 360;	//el angulo estandarizado del jugador

	if ((dispAngHand < (dispAngPlayer + 180)) && (dispAngHand > dispAngPlayer)) {	//si la mano esta a su espalda
		Flipped_ = SDL_FLIP_NONE;
	}
	else Flipped_ = SDL_FLIP_VERTICAL;

	pos_.Set(collider_->getPos().x, collider_->getPos().y);
	handPos_.Set(pos_.x + dir_.x * 1, pos_.y - dir_.y * 1);
}

void Hands::setWeapon(WeaponID wId, Weapon* w)
{
	currentWeaponID_ = wId;
	currentWeapon_ = w;
	frameY_ = wId;
}
