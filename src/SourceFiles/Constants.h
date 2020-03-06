#pragma once
#include <string>
#include "checkML.h"


/*Agregar aqu� todas las constantes del juego. Cada vez que haya que hacer uso de las constantes,
simplemente hay que hacer #include "Constants.h" en el archivo en el que estemos programando.*/

//Constantes referentes a la ventana del juego
const int WINDOW_HEIGHT = 720;
const int WINDOW_WIDTH = 1280;
const std::string WINDOW_NAME = "PCE";
const double MS_PER_FRAME = 1000.0/60.0; //Asumiendo 60fps

//Modos de juegos
enum States
{
	menu,
	play,
	pause,
	//No toques
	NUMBER_OF_STATES
};