#include "MultiplayerHost.h"
#include "SDL_Game.h"
#include "MatchInfo.h"
#include <vector>
#include <sstream>
#include <iostream>

MultiplayerHost::MultiplayerHost() {
	if (SDLNet_Init() < 0) {
		throw;
	}

	hostIPAddress_ = getHostIpAddress();

	if (SDLNet_ResolveHost(&hostIp_, nullptr, 2000) < 0)
		throw;

	masterSocket_ = SDLNet_TCP_Open(&hostIp_);
	if (!masterSocket_) throw;

	socketSet_ = SDLNet_AllocSocketSet(4);
	SDLNet_TCP_AddSocket(socketSet_, masterSocket_);

	for (int i = 0; i < 3; i++)
		clients_[i] = nullptr;
}

MultiplayerHost::~MultiplayerHost() {
	SDLNet_Quit();
}

std::string MultiplayerHost::getHostIpAddress() {
	//Thanks Samir for all you've done for us
	IPaddress ip;
	if (SDLNet_ResolveHost(&ip, "ipinfo.io", 80) < 0)
		throw;
	TCPsocket conn = SDLNet_TCP_Open(&ip);
	if (!conn) throw;

	// envia peticion para conseguir la ip
	const char* getter = "GET /ip HTTP/1.0\nHost: ipinfo.io\n\n";
	int size = strlen(getter);
	int result = SDLNet_TCP_Send(conn, getter, size);
	if (result != size) throw;

	// get the response, we assume it is at most bufferSize chars
	const int bufferSize = 255;
	char buffer[bufferSize + 1];
	int read = 0;
	memset(buffer, '\0', bufferSize + 1);
	while (read < bufferSize) {
		result = SDLNet_TCP_Recv(conn, buffer + read, bufferSize);
		if (result < 0) {
			throw;
		}
		else if (result == 0) {
			break;
		}
		else {
			read = read + result;
		}
	}

	// The IP is the last token of the response
	std::vector<std::string> tokens;
	std::stringstream iss(buffer);
	for (std::string s; iss >> s;) {
		tokens.push_back(s);
	}

	SDLNet_TCP_Close(conn);


	return tokens.back();
}

void MultiplayerHost::checkActivity() {
	if (SDLNet_CheckSockets(socketSet_, 0) > 0) {

		//Alguien se une
		if (SDLNet_SocketReady(masterSocket_)) {
			TCPsocket client = SDLNet_TCP_Accept(masterSocket_);

			int clientPlace = 0;
			while (clientPlace < 3 && clients_[clientPlace] != nullptr)
				clientPlace++;

			if (clientPlace < 3) {
				//std::cout << "Jugador conectado, id asignada: " << clientPlace << std::endl;
				SDLNet_TCP_AddSocket(socketSet_, client);
				SDLNet_TCP_Send(client, "C", 9);
			}
			else {
				SDLNet_TCP_Send(client, "L", 1);
			}
		}

		//Revisar actividad de los demas
		for (int i = 0; i < 3; i++) {
			if (clients_[i] != nullptr && SDLNet_SocketReady(clients_[i])) {
				memset(buffer, 0, 2048);
				receivedBytes_ = SDLNet_TCP_Recv(clients_[i], buffer, 1);
				if (receivedBytes_ <= 0) {
					//Usuario ha desconectado o perdido conexion
					SDLNet_TCP_Close(clients_[i]);
					SDLNet_TCP_DelSocket(socketSet_, clients_[i]);
					clients_[i] = nullptr;
				}
				else {
					//Tratamiento de mensajes de los jugadores
					switch (buffer[0]) {
					case 'P':
						handlePlayerJoin(i);
						break;
					case 'I':
						//Nos llega input del jugador
						handlePlayerInput(i);
						break;
					}
				}
			}
		}
	}
}


void MultiplayerHost::handlePlayerJoin(int clientNumber) {
	//Recibimos cuantos jugadores quiere agregar el jugador
	receivedBytes_ = SDLNet_TCP_Recv(clients_[clientNumber], buffer, sizeof(PlayerInfoPacket) - 1);
	//Revisamos si caben el numero que quieren entrar
	int nStartingPlayers = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getNumberOfPlayers();
	int nPlayersIncoming = (int)buffer[0];
	if (nPlayersIncoming + nStartingPlayers < 5) {
		//Aceptamos
		std::vector<MatchInfo::PlayerInfo*>* playersVector = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getPlayersInfo();
		for (int i = 0; i < nPlayersIncoming; i++) {
			playersVector->push_back(new MatchInfo::PlayerInfo(SDL_Game::instance()->getStateMachine()->getMatchInfo()->getNumberOfPlayers(), new ClientBinder(), buffer[i + 1]));
			SDL_Game::instance()->getStateMachine()->getMatchInfo()->updateNumberOfPlayers();
		}

		//Enviamos de vuelta al cliente la informacion de los jugadores
		buffer[0] = 'P';
		buffer[1] = nPlayersIncoming;
		for (int i = 0; i < nPlayersIncoming; i++) {
			buffer[i + 2] = nStartingPlayers + i;
		}

		SDLNet_TCP_Send(clients_[clientNumber], buffer, sizeof(PlayerInfoPacket));
	}
	else { //Ya hay demasiados jugadores, avisamos que estamos llenos y tiramos la conexion
		SDLNet_TCP_Send(clients_[clientNumber], "L", 1);
		SDLNet_TCP_Close(clients_[clientNumber]);
		SDLNet_TCP_DelSocket(socketSet_, clients_[clientNumber]);
		clients_[clientNumber] = nullptr;
	}
}

void MultiplayerHost::handlePlayerInput(int clientNumber) {
	receivedBytes_ = SDLNet_TCP_Recv(clients_[clientNumber], buffer, sizeof(InputPacket) - 1);

	std::vector<MatchInfo::PlayerInfo*>* playersVector = SDL_Game::instance()->getStateMachine()->getMatchInfo()->getPlayersInfo();

	InputPacket inputPacket{ 'I',buffer[0],(bool)buffer[1],(bool)buffer[2],(bool)buffer[3],
	(bool)buffer[4] ,(bool)buffer[5], (*(float*)buffer[6]), (*(float*)buffer[8]),
	(bool)buffer[10] ,(bool)buffer[11] ,(bool)buffer[12] ,(bool)buffer[13] ,(bool)buffer[14] ,
	(bool)buffer[15] ,(bool)buffer[16] }; //Duda para pasar de buffer a float

	(*playersVector)[buffer[0]]->inputBinder->syncInput(inputPacket);
}