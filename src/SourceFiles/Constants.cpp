#include "Constants.h"
#include <fstream>
#include <iostream>

Constants::Constants(const std::string &path)
{
	std::ifstream read(path);
#ifdef _DEBUG
	char fullPath[_MAX_PATH];
	_fullpath(fullPath, path.c_str(), _MAX_PATH);
	std::cout << "looking for constants in:" << fullPath << std::endl;
#endif

	if (!read.is_open())
		throw std::runtime_error("No se ha abierto el archivo de constantes"); //hehe
	read >> data;
	initialized_ = true;
}

//La plantilla gen�rica no hace comprobaciones de tipo
template <typename T>
T Constants::getConstant(const std::string &key) const
{
	T d;
	//std::cout << "BUSCANDO " << key << "\n";
	if (!data[key].is_null())
		try
		{
			d = data[key].get<T>();
			return d;
		}
		catch (...)
		{
			throw "Incorrect constant";
		}
}
template <>
int Constants::getConstant<int>(const std::string &key) const
{
	int pt = 0;
	//std::cout << "BUSCANDO " << key << "\n";
	if (!data[key].is_null() && data[key].is_number_integer())
		try
		{
			pt = data[key].get<int>();
			return pt;
		}
		catch (...)
		{
			throw "Incorrect string constant";
		}
}
template <>
bool Constants::getConstant<bool>(const std::string &key) const
{
	bool r;
	//std::cout << "BUSCANDO " << key << "\n";
	if (!data[key].is_null() && data[key].is_boolean())

		r = data[key].get<bool>();
	return r;
}
template <>
double Constants::getConstant<double>(const std::string &key) const
{
	double pt = 0;
	//std::cout << "BUSCANDO " << key << "\n";
	if (!data[key].is_null() && data[key].is_number())

		pt = data[key].get<double>();
	return pt;
}
template <>
std::string Constants::getConstant<std::string>(const std::string &key) const
{
	std::string pt = "";
	//std::cout << "BUSCANDO " << key << "\n";

	if (!data[key].is_null() && data[key].is_string())
		pt = data[key].get<std::string>();
	return pt;
}
template <>
float Constants::getConstant<float>(const std::string &key) const
{
	float pt = 0.0;
	//std::cout << "BUSCANDO " << key << "\n";
	if (!data[key].is_null() && data[key].is_number())
		pt = data[key].get<float>();
	return pt;
}


//-------------------------------ONLINE PACKETS--------------------------------------------------

SpritePacket::SpritePacket(unsigned char tId, short posx, short posy, short w, short h,
	unsigned char flip, float velx, float vely, float rotVel, short rotDeg, unsigned char fX, unsigned char fY) :
	packetId('S'),
	textureId(tId),
	posX(posx), posY(posy), angVel(rotVel),
	velX(velx), velY(vely),
	width(w), height(h),
	rotationDegrees(rotDeg),
	frameNumberX(fX), frameNumberY(fY) {	

}

void SpritePacket::to_bin()
{	
	alloc_data(SIZE);
	memset(_data, 0, SIZE);

	//Serializar los campos

	char* aux = _data;

	memcpy(aux, &packetId, sizeof(char));
	aux += sizeof(char);
	memcpy(aux, &textureId, sizeof(unsigned char));
	aux += sizeof(unsigned char);
	memcpy(aux, &posX, sizeof(short));
	aux += sizeof(short);
	memcpy(aux, &posY, sizeof(short));
	aux += sizeof(short);
	memcpy(aux, &velX, sizeof(float));
	aux += sizeof(float);
	memcpy(aux, &velY, sizeof(float));
	aux += sizeof(float);
	memcpy(aux, &angVel, sizeof(float));
	aux += sizeof(float);
	memcpy(aux, &width, sizeof(short));
	aux += sizeof(short);
	memcpy(aux, &height, sizeof(short));
	aux += sizeof(short);
	memcpy(aux, &rotationDegrees, sizeof(short));
	aux += sizeof(short);
	memcpy(aux, &frameNumberX, sizeof(unsigned char));
	aux += sizeof(unsigned char);
	memcpy(aux, &frameNumberY, sizeof(unsigned char));
	aux += sizeof(unsigned char);
	memcpy(aux, &flip, sizeof(unsigned char));
	aux += sizeof(unsigned char);
}

int SpritePacket::from_bin(char* bobj)
{
	if (bobj == nullptr){
        std::cout << "bobj was nullptr in SpritePacket::from_bin\n";
        return -1;
    }

    alloc_data(SIZE);
    if (strlen(bobj) >= _size)
    {
        std::cout << "Invalid data buffer in SpritePacket::from_bin\n";
        return -1;
    }

    memcpy(static_cast<void *>(_data), bobj, SIZE);

	char* aux = _data;

	memcpy(&packetId, aux, sizeof(char));
	aux += sizeof(char);
	memcpy(&textureId, aux, sizeof(unsigned char));
	aux += sizeof(unsigned char);
	memcpy(&posX, aux, sizeof(short));
	aux += sizeof(short);
	memcpy(&posY, aux, sizeof(short));
	aux += sizeof(short);
	memcpy(&velX, aux, sizeof(float));
	aux += sizeof(float);
	memcpy(&velY, aux, sizeof(float));
	aux += sizeof(float);
	memcpy(&angVel, aux, sizeof(float));
	aux += sizeof(float);
	memcpy(&width, aux, sizeof(short));
	aux += sizeof(short);
	memcpy(&height, aux, sizeof(short));
	aux += sizeof(short);
	memcpy(&rotationDegrees, aux, sizeof(short));
	aux += sizeof(short);
	memcpy(&frameNumberX, aux, sizeof(unsigned char));
	aux += sizeof(unsigned char);
	memcpy(&frameNumberY, aux, sizeof(unsigned char));
	aux += sizeof(unsigned char);
	memcpy(&flip, aux, sizeof(unsigned char));
	aux += sizeof(unsigned char);

    return 0;
}


AudioPacket::AudioPacket(char pcktid, bool ismsic, char id, char loops) :
	packetId(pcktid),
	isMusic(ismsic),
	soundId(id),
	nLoops(loops) {
}
	
void AudioPacket::to_bin()
{
	alloc_data(SIZE);
	memset(_data, 0, SIZE);

	//Serializar los campos

	char* aux = _data;

	memcpy(aux, &packetId, sizeof(char));
	aux += sizeof(char);
	memcpy(aux, &isMusic, sizeof(bool));
	aux += sizeof(bool);
	memcpy(aux, &soundId, sizeof(char));
	aux += sizeof(char);
	memcpy(aux, &nLoops, sizeof(char));
	aux += sizeof(char);
}

int AudioPacket::from_bin(char* bobj)
{
	if (bobj == nullptr){
        std::cout << "bobj was nullptr in AudioPacket::from_bin\n";
        return -1;
    }

    alloc_data(SIZE);
    if (strlen(bobj) >= _size)
    {
        std::cout << "Invalid data buffer in AudioPacket::from_bin\n";
        return -1;
    }

    memcpy(static_cast<void *>(_data), bobj, SIZE);

	char* aux = _data;
	
	memcpy(&packetId, aux, sizeof(char));
	aux += sizeof(char);
	memcpy(&isMusic, aux, sizeof(bool));
	aux += sizeof(bool);
	memcpy(&soundId, aux, sizeof(char));
	aux += sizeof(char);
	memcpy(&nLoops, aux, sizeof(char));
	aux += sizeof(char);

    return 0;
}

PlayerInfoPacket::PlayerInfoPacket(char num, char p1, char p2, char p3) : 
	packetId('P'),
	numberOfPlayers(num),
	player1Info(p1),
	player2Info(p2),
	player3Info(p3) {
}

void PlayerInfoPacket::to_bin() 
{
	alloc_data(SIZE);
	memset(_data, 0, SIZE);

	//Serializar los campos

	char* aux = _data;

	memcpy(aux, &packetId, sizeof(char));
	aux += sizeof(char);
	memcpy(aux, &numberOfPlayers, sizeof(char));
	aux += sizeof(char);
	memcpy(aux, &player1Info, sizeof(char));
	aux += sizeof(char);
	memcpy(aux, &player2Info, sizeof(char));
	aux += sizeof(char);
	memcpy(aux, &player3Info, sizeof(char));
	aux += sizeof(char);
}

int PlayerInfoPacket::from_bin(char* bobj)
{
	if (bobj == nullptr) {
	std::cout << "bobj was nullptr in PlayerInfoPacket::from_bin\n";
	return -1;
    }

    alloc_data(SIZE);
    if (strlen(bobj) >= _size)
    {
        std::cout << "Invalid data buffer in PlayerInfoPacket::from_bin\n";
        return -1;
    }

    memcpy(static_cast<void *>(_data), bobj, SIZE);

	char* aux = _data;

	memcpy(&packetId, aux, sizeof(char));
	aux += sizeof(char);
	memcpy(&numberOfPlayers, aux, sizeof(char));
	aux += sizeof(char);
	memcpy(&player1Info, aux, sizeof(char));
	aux += sizeof(char);
	memcpy(&player2Info, aux, sizeof(char));
	aux += sizeof(char);
	memcpy(&player3Info, aux, sizeof(char));
	aux += sizeof(char);

    return 0;
}


InputPacket::InputPacket(char pcktId, char id, bool hG, bool rG, bool pT, bool pP,
	bool hI, bool pI, float dirX, float dirY, bool rI, bool pA,
	bool mF, bool mB, bool pPause, bool mM) :
	packetId(pcktId),
	playerId(id),
	holdGrab(hG),
	releaseGrab(rG),
	pressThrow(pT),
	pressPick(pP),
	holdImpulse(hI),
	pressImpulse(pI),
	aimDirX(dirX), aimDirY(dirY),
	releaseImpulse(rI),
	pressAttack(pA),
	menuForward(mF),
	menuBack(mB),
	pressPause(pPause),
	menuMove(mM) {

}

void InputPacket::to_bin()
{
	alloc_data(SIZE);
	memset(_data, 0, SIZE);

	//Serializar los campos

	char* aux = _data;

	memcpy(aux, &packetId, sizeof(char));
	aux += sizeof(char);
	memcpy(aux, &playerId, sizeof(char));
	aux += sizeof(char);
	memcpy(aux, &holdGrab, sizeof(bool));
	aux += sizeof(bool);
	memcpy(aux, &releaseGrab, sizeof(bool));
	aux += sizeof(bool);
	memcpy(aux, &pressThrow, sizeof(bool));
	aux += sizeof(bool);
	memcpy(aux, &pressPick, sizeof(bool));
	aux += sizeof(bool);
	memcpy(aux, &holdImpulse, sizeof(bool));
	aux += sizeof(bool);
	memcpy(aux, &pressImpulse, sizeof(bool));
	aux += sizeof(bool);
	memcpy(aux, &aimDirX, sizeof(float));
	aux += sizeof(float);
	memcpy(aux, &aimDirY, sizeof(float));
	aux += sizeof(float);
	memcpy(aux, &releaseImpulse, sizeof(bool));
	aux += sizeof(bool);
	memcpy(aux, &pressAttack, sizeof(bool));
	aux += sizeof(bool);
	memcpy(aux, &menuForward, sizeof(bool));
	aux += sizeof(bool);
	memcpy(aux, &menuBack, sizeof(bool));
	aux += sizeof(bool);
	memcpy(aux, &pressPause, sizeof(bool));
	aux += sizeof(bool);
	memcpy(aux, &menuMove, sizeof(char));
	aux += sizeof(char);
}

int InputPacket::from_bin(char* bobj)
{
	if (bobj == nullptr) {
	std::cout << "bobj was nullptr in InputPacket::from_bin\n";
	return -1;
    }

    alloc_data(SIZE);
    if (strlen(bobj) >= _size)
    {
        std::cout << "Invalid data buffer in InputPacket::from_bin\n";
        return -1;
    }

    memcpy(static_cast<void *>(_data), bobj, SIZE);

	char* aux = _data;

	memcpy(&packetId, aux, sizeof(char));
	aux += sizeof(char);
	memcpy(&playerId, aux, sizeof(char));
	aux += sizeof(char);
	memcpy(&holdGrab, aux, sizeof(bool));
	aux += sizeof(bool);
	memcpy(&releaseGrab, aux, sizeof(bool));
	aux += sizeof(bool);
	memcpy(&pressThrow, aux, sizeof(bool));
	aux += sizeof(bool);
	memcpy(&pressPick, aux, sizeof(bool));
	aux += sizeof(bool);
	memcpy(&holdImpulse, aux, sizeof(bool));
	aux += sizeof(bool);
	memcpy(&pressImpulse, aux, sizeof(bool));
	aux += sizeof(bool);
	memcpy(&aimDirX, aux, sizeof(float));
	aux += sizeof(float);
	memcpy(&aimDirY, aux, sizeof(float));
	aux += sizeof(float);
	memcpy(&releaseImpulse, aux, sizeof(bool));
	aux += sizeof(bool);
	memcpy(&pressAttack, aux, sizeof(bool));
	aux += sizeof(bool);
	memcpy(&menuForward, aux, sizeof(bool));
	aux += sizeof(bool);
	memcpy(&menuBack, aux, sizeof(bool));
	aux += sizeof(bool);
	memcpy(&pressPause, aux, sizeof(bool));
	aux += sizeof(bool);
	memcpy(&menuMove, aux, sizeof(char));
	aux += sizeof(char);

    return 0;
}