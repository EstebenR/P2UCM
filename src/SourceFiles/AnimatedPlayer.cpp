#include "AnimatedPlayer.h"

AnimatedPlayer::AnimatedPlayer(int textureId, int timePerFrame, int skin) : Viewer(textureId, ComponentType::AnimatedPlayer) {
	animationsInfo_ = vector<AnimationInfo>(CONST(int, "NUM_ANIMS"));
	timePerFrame_ = timePerFrame;
	skin_ = skin;
}

AnimatedPlayer::~AnimatedPlayer()
{
}

void AnimatedPlayer::init()
{
	Viewer::init(); 

	unsigned short int nextAnimOrigin_ = 0;
	int i = 0;
	for (AnimationInfo& a : animationsInfo_) {
		a = {
			nextAnimOrigin_,														// guardamos su origen
			(unsigned short int)CONST(int, "NFRAMES_ANIM" + std::to_string(i))		//"cargamos" la duraci�n de cada animacion
		};
		nextAnimOrigin_ += a.numFrames_;											//calculamos la posicion de la siguiente animaci�n
		i++;
	}

	frameX_ = animationsInfo_[anim_].animOrigin_;
	frameY_ = skin_;
}

void AnimatedPlayer::update()
{
	if (isPlaying_ && anim_ != -1 && updateTime(animationsInfo_[anim_].numFrames_)) {
			frameX_ = animationsInfo_[anim_].animOrigin_ + frame_;
			if (loops_ != -1 && currentLoop_ >= loops_) {	// para saber si hay que volver al idle
				resetAnimation();
			}
	}
}

void AnimatedPlayer::resetAnimation()
{
	startAnimation(-1, 0);
}