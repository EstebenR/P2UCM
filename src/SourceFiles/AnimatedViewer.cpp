#include "AnimatedViewer.h"

void AnimatedViewer::init() {
	Viewer::init();
	nFrames_ = tex_->getWidth() / clip_.w;	//Obtenemos el n�mero de frames de la spritesheet
	Viewer::setNFrames(nFrames_);
}

void AnimatedViewer::update()
{
	timeElapsed_++;		//Aumentamos en uno el n�mero de ciclos
	
	if (timeElapsed_ == timePerFrame_) {
		timeElapsed_ = 0;	//Reseteamos el contador de frames

		frame_ = frame_ % nFrames_;
		setClip(SDL_Rect{ (clip_.w * frame_), 0, clip_.w, clip_.h });	//Cogemos el frame del spritesheet

		frame_++;	//Cambiamos al siguiente frame de la animaci�n
	}
}