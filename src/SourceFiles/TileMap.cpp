#include "TileMap.h"
#include "SDL_Game.h"
#include "Resources.h"
#include "json.hpp"


TileMap::TileMap(int w, int h, string map, EntityManager* eM, b2World* pW, GameMode* gMode) :Component(ComponentType::Tilemap),  //w y h son de la ventana
width_(w),
height_(h),
tm_(nullptr),
entityManager_(eM),
physicsWorld_(pW),
gameMode_(gMode){
	loadTileson(map);
}

TileMap::~TileMap() {
	tm_ = nullptr;
}

void TileMap::init() {
	tm_ = SDL_Game::instance()->getTexturesMngr();
	layers_ = tMap_.getLayers();
	//recorremos todas las capas del mapa
	for (auto& tileLayer : layers_)
	{
		//podemos distinguir entre capas de tiles, objetos, imagenes m�s adelante
		if (tileLayer.getType() == tson::Layer::Type::ObjectGroup)
		{
			//pos = position in tile units
			vector<tson::Object> objetos = tileLayer.getObjects();
			for (auto obj : objetos)
			{
				if (tileLayer.getName() == "Paredes") { //muros
					tson::Vector2i p, s;
					s = obj.getSize();
					b2Vec2 size = b2Vec2(s.x / CONST(double, "PIXELS_PER_METER"), (s.y) / CONST(double, "PIXELS_PER_METER"));
					p = obj.getPosition();
					b2Vec2 pos = b2Vec2(p.x / CONST(double, "PIXELS_PER_METER") + (size.x / 2), (CONST(int, "WINDOW_HEIGHT") - p.y) / CONST(double, "PIXELS_PER_METER") - (size.y / 2));
					WeaponFactory::makePared(entityManager_, physicsWorld_, pos, size);
				}
				else if (tileLayer.getName() == "Spawns") { //spawns
					b2Vec2 pos = b2Vec2(obj.getPosition().x / CONST(double, "PIXELS_PER_METER"), obj.getPosition().y / CONST(double, "PIXELS_PER_METER"));
					gameMode_->addSpawnPoint(new b2Vec2(pos)); //a�ade un punto de spawn
				}
			}
		}
	}
}

void TileMap::update() {
	//apa�o para que draw sea const
	layers_ = tMap_.getLayers();
}

void TileMap::draw() const {
	//c�digo bastante modificado basado en https://github.com/SSBMTonberry/tileson 
	//He a�adido sistema de escalado
	//soporte autom�tico para varios tilesets
	//soporte de texturas de SDL

	Texture* tilesetT_;//Textura del tileset a utilizar
	const tson::Tileset* tSet;//Datos del tileset a utilizar

	//recorremos todas las capas del mapa
	for (auto& tileLayer : layers_)
	{
		//podemos distinguir entre capas de tiles, objetos, imagenes m�s adelante
		if (tileLayer.getType() == tson::Layer::Type::TileLayer)
		{
			//pos = position in tile units
			for (auto& [pos, tile] : tileLayer.getTileData()) //Loops through absolutely all existing tiles
			{
				if (tile != nullptr)
				{
					//buscamos el tileset correspondiente
					int i = 0;
					int id = tile->getId();
					while (i < tileSets_.size()
						&& !(id >= tileSets_[i].getFirstgid()
							&& id <= (tileSets_[i].getFirstgid() + tileSets_[i].getTileCount()) - 1))
					{
						i++;
					}
					if (i < tileSets_.size())
						tSet = &tileSets_[i];
					else throw exception("No se encontr� el tileset");

					//variables auxiliares para el dibujado del tile
					int firstId = tSet->getFirstgid(); //First tile id of the tileset
					int tSetColumns = tSet->getColumns();
					int tSetRows = tSet->getTileCount() / tSetColumns;
					int lastId = (tSet->getFirstgid() + tSet->getTileCount()) - 1;
					int tileWidth = tSet->getTileSize().x;
					int tileHeight = tSet->getTileSize().y;
					int spacing = tSet->getSpacing();
					int margin = tSet->getMargin();


					tilesetT_ = tm_->getTexture(Resources::tilesetTag_.find(tSet->getName())->second);;
					//Posicion de dibujado del vector
					tson::Vector2i position = { std::get<0>(pos) * width_ / mapCols_,std::get<1>(pos) * height_ / mapRows_ };

					//posicion unidimensional del tile en el tileset
					int baseTilePosition = (tile->getId() - firstId); //This will determine the base position of the tile.

					//fila y columna del tile en el tileset
					int currentCol = (baseTilePosition % tSetColumns);
					int currentRow = (baseTilePosition / tSetRows);

					//posiciones del tile en el tileset
					int offsetX = currentCol * (tileWidth + spacing);
					int offsetY = (currentRow) * (tileHeight + margin);

					SDL_Rect drawPos = { position.x,position.y,width_ / mapCols_,height_ / mapRows_ };
					SDL_Rect tilesetClip = { offsetX,offsetY,tileWidth ,tileWidth };
					tilesetT_->render(drawPos, tilesetClip);
				}
			}
		}
	}
}

bool TileMap::loadTileson(string path) {
	tson::Tileson parser;
	tMap_ = parser.parse(fs::path(path));
	if (tMap_.getStatus() == tson::Map::ParseStatus::OK) {
		//variables de escala
		mapCols_ = tMap_.getSize().x;
		mapRows_ = tMap_.getSize().y;
		//guardamos los datos de los tilesets que se usan
		tileSets_ = tMap_.getTilesets();
		return true;
	}
	else
		return false;
}
