#include "Hands.h"
#include "Entity.h"
#include "Resources.h"
#include "InputHandler.h"

Hands::Hands(int playerID, int textureId, WeaponID wId) :
	Component(ComponentType::Hands),
	textureId_(textureId),
	playerID_(playerID)
{
}

void Hands::init()
{
	collider_ = GETCMP1_(Collider);
	ih_ = SDL_Game::instance()->getInputHandler();
	if (tex_ == nullptr) {
		tex_ = game_->getTexturesMngr()->getTexture(textureId_);
	}
}

void Hands::draw() const
{
	SDL_Rect colRec = collider_->getRectRender();
	SDL_Rect destRect;
	destRect.w = colRec.w / 2;
	destRect.h = colRec.h / 2;
	destRect.x = pos_.x;
	destRect.y = pos_.y;

	SDL_Rect clip;
	clip.w = tex_->getWidth() / WEAPON_NUMBER;
	clip.h = tex_->getHeight();
	clip.y = 0; clip.x = clip.w * currentWeapon_;

	tex_->render(destRect, collider_->getAngle(), clip);
}

void Hands::update()
{
	b2Vec2 vI = ih_->getStickDir(getPlayerId(), InputHandler::GAMEPADSTICK::LEFTSTICK);
	if (vI.Length() != 0) {
		dir_.Set(vI.x,vI.y);
		SDL_Rect colRec = collider_->getRectRender();
		pos_.Set((colRec.x + colRec.w / 4) + dir_.x * bodyOffset_,
			(colRec.y + colRec.h / 4) + dir_.y * bodyOffset_);
	}
}

void Hands::setWeapon(WeaponID wId)
{
	currentWeapon_ = wId;
}
