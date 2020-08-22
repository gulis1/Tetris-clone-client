#pragma once
#include<iostream>
#include<irrKlang.h>
#include "color.h"

using namespace std;


typedef enum tStates { Empty, Occupied };

typedef struct tCasilla {
	tStates state;
	RGB color;
};

class tBoard
{
	public:
		tCasilla board[22][10] = { Empty };
		pair<int, int> fullLineCheck(irrklang::ISoundEngine* soundEngine);

	private:
		void removeFullLine(int ind);
	
};


