#include "HealthViewer.h"
#include "Entity.h"
#include "PlayerData.h"

HealthViewer::HealthViewer(int texActiveId, int texDisabledId) : Component(ComponentType::HealthViewer)
{
	activeTextureId_ = texActiveId;
	disableTextureId_ = texDisabledId;
	
}

void HealthViewer::init()
{
	//Diria que no hace falta poner nada mas...
	playerData_ = GETCMP1_(PlayerData);
	he = GETCMP1_(Health);
	full = SDL_Game::instance()->getTexturesMngr()->getTexture(activeTextureId_);
	empty = SDL_Game::instance()->getTexturesMngr()->getTexture(disableTextureId_);
	int playerNumber = playerData_->getPlayerNumber();
	setPos(playerNumber);
	flip_ = (playerNumber % 2 == 0) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;
}
void HealthViewer::update()
{
	//Todo lo hace el draw, este no sirve ninguna funci�n 
}
void HealthViewer::draw() const
{
	int player = playerData_->getPlayerNumber();
	SDL_Rect dest = {pos_.x,pos_.y,CONST(int,"LIFE_WIDTH"),CONST(int,"LIFE_HEIGTH")};
	for (int i = 0; i < he->getHealthMax(); i++) {
		if (player % 2 == 0){
			dest.x = pos_.x + i*((CONST(int,"LIFE_WIDTH") + CONST(double, "LIFE_DRAW_OFFSET")));
		}
		else{
			dest.x = pos_.x -(i*((CONST(int,"LIFE_WIDTH") + CONST(double, "LIFE_DRAW_OFFSET"))));
		}

		if ((i+1) <= he->getHealth()) {		//Si tiene esas X vidas las muertra como llenas
			full->render(dest, 0, SDL_Rect{ 0, 0, 613,667 },flip_);
		}
		else {		//Si no las tiene, se dibujar�n como vacias
			empty->render(dest, 0, SDL_Rect{ 0, 0, 613,667 },flip_);
		}
	}

}

void HealthViewer::handleInput() {

}

void HealthViewer::setPos(int player){
	pos_.x = (player % 2 == 0) ? (CONST(int,"LIFE_MARGIN_H")) : (CONST(int,"WINDOW_WIDTH") - CONST(int,"LIFE_MARGIN_H")- CONST(int,"LIFE_WIDTH"));
	pos_.y = (player < 2) ? (CONST(double,"LIFE_MARGIN_V")) : (CONST(int,"WINDOW_HEIGHT") - CONST(double,"LIFE_MARGIN_V") - CONST(int,"LIFE_HEIGTH"));
}