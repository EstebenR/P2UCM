#include "TimeGameMode.h"
#include "PlayState.h"
#include "ThrownByPlayer.h"
#include "TomatoPool.h"

TimeGameMode::TimeGameMode(int nPlayers) : AbstractTimedGameMode(nPlayers) {
}

TimeGameMode::~TimeGameMode() {
}

void TimeGameMode::init(PlayState* game)
{
	AbstractTimedGameMode::init(game);
	tomatoPool_.init(game->getEntityManager(), game->getPhysicsWorld());

	timeToEnd_ = CONST(double, "TIMEMODE_TIME_TO_END");
	suddenDeathTexture_ = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::SuddenDeathTimeModeText);

	killsMarkerWidth_ = CONST(int, "KILLS_WIDTH");
	killsMarkerHeight_ = CONST(int, "KILLS_HEIGHT");

	for (int i = 0; i < nPlayers_; i++) {
		players_.push_back(PlayerFactory::createPlayerWithHealth(game->getEntityManager(), game->getPhysicsWorld(), i,
			Resources::Body, tilemap_->getPlayerSpawnPoint(i).x, tilemap_->getPlayerSpawnPoint(i).y, 3));
		playerKills_.push_back(0); //Initializes kills vector with 0 for all players.
	}

	for (int i = 0; i < players_.size(); i++) {

		Entity* e = players_[i];
		playersHealth_.push_back(e->getComponent<Health>(ComponentType::Health)); //Initializes playersHealth vector catching a reference to Health on entity e.
		HealthViewer* hV = (e->getComponent<HealthViewer>(ComponentType::HealthViewer)); //Obtains a reference to the HealthViewer component of entity e. This is used to calculate where the kills will be drawn
		b2Vec2 p = hV->getPos();
		int hVW = hV->getWidth();

		if (i % 2 != 0) {
			p.x = p.x - hVW + killsMarkerWidth_;
		}

		// draws P1 and P2 kill marker below health viewer, P3 and P4 kill marker above health viewer
		if (i < 2)	
			p.y += (CONST(int, "KILLS_VERTICAL_OFFSET") + CONST(int, "LIFE_HEIGTH"));
		else		
			p.y -= (CONST(int, "LIFE_HEIGTH") - CONST(int, "KILLS_VERTICAL_OFFSET"));

		playersPointsPos_.push_back(p);
	}

	//UI Elements.
	skullUISize_ = CONST(int, "SKULL_UI_SIZE");
	skullUIMarginX_ = CONST(int, "SKULL_UI_OFFSET_X");
	skullTextureUI_ = SDL_Game::instance()->getTexturesMngr()->getTexture(Resources::SkullUI);

	tomatoPool_.addTomato({ 40, 20 });
}

void TimeGameMode::render()
{
	AbstractTimedGameMode::render();

	renderKillMarker();
}

void TimeGameMode::update()
{
	updateTime(playerKills_);
}

void TimeGameMode::addPoints(int playerID)
{
	playerKills_[playerID]++;
}

void TimeGameMode::playerKillsPlayer(int killerId, int deadId)
{
	addPoints(killerId);
}


void TimeGameMode::renderKillMarker() {

	for (int k = 0; k < playerKills_.size(); k++) {

		string killsNumb = "x" + to_string(playerKills_[k]);
		Texture killsNumbTexture(SDL_Game::instance()->getRenderer(), killsNumb,
			SDL_Game::instance()->getFontMngr()->getFont(Resources::NES_Chimera), { COLOR(0xffffffff) });
		
		SDL_Rect killsTextTexture;
		killsTextTexture.x = (int)playersPointsPos_[k].x;
		killsTextTexture.y = (int)playersPointsPos_[k].y;
		killsTextTexture.w = killsMarkerWidth_ * (calculateDigits(playerKills_[k] / 10) + 2);
		killsTextTexture.h = killsMarkerHeight_;

		killsNumbTexture.render(killsTextTexture);

		SDL_Rect coinImageRect;
		coinImageRect.x = killsTextTexture.x + killsTextTexture.w + skullUIMarginX_;
		coinImageRect.y = killsTextTexture.y - killsTextTexture.h/2;
		coinImageRect.w = skullUISize_;
		coinImageRect.h = skullUISize_;

		skullTextureUI_->render(coinImageRect);
	}
}

int TimeGameMode::calculateDigits(int points) {
	if (points > 0) return calculateDigits(points / 10) + 1;
	else return 0;
}