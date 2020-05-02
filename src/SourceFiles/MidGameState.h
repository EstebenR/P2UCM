#pragma once
#include "GameState.h"
#include "Texture.h"

class AnimatedViewer;

class MidGameState : public GameState
{
private:
	int numPlayers_;
	int roundWinner_;

	bool rocketAnimationStarted_ = false;
	bool rocketAnimationEnded_ = false;
	bool rocketsGoingOutAnimationStarted_ = false;
	bool rocketsGoingOutAnimationEnded_ = false;
	bool spaceStationAnimationStarted_ = false;
	int currentFrame = 0;
	int framesUntilNextShake_ = 0;
	int framesUntilRocketsGoingOutEnds_ = 0;
	float spaceStationScaleFactor_ = 1;

	Texture* fondo= nullptr;
	b2Vec2 spaceStationOffset = b2Vec2(0, 0);
	std::vector<AnimatedViewer*> playerRockets_;
	AnimatedViewer* spaceStationViewer_ = nullptr;

	//Load of constants.
	int distanceBetweenRockets_ = 200;
	int startingXPosition_ = 450;
	int distanceGainedByPoint_ = 150;
	int framesUntilAnimationStart_ = 120;
	int spaceStationAnimationDuration_ = 360;
	int framesUntilSpaceStationAnimationEnds_ = 0;
	int rocketsMovingOutFramesDuration_ = 180;
	int rocketXPositionObjective_ = 0;
	int distanceGainedPerFrame_ = 2;
	int framesBetweenShakes_ = 5;
	int shakeOffsetDistance_ = 3;
	int maxScaleSpaceStation_ = 4;
	float scaleGrowthPerFrame_ = 0.5;
	int rocketMovementY_ = 3;
	int rocketMovementX_ = 1;

	int spaceStationWidht_ = 300;
	int spaceStationHeight_ = 150;

public:
	MidGameState(int numPlayers, int roundWinner) { numPlayers_ = numPlayers; roundWinner_ = roundWinner; };
	~MidGameState();

	virtual void init();
	virtual void render();
	virtual void update();
	virtual void handleInput();
};

