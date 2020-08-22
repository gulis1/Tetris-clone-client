#include "Board.h"

using namespace std;

pair<int,int> tBoard::fullLineCheck(irrklang::ISoundEngine* soundEngine) {
	int addScore = 0;
	int cont = 0;
	int i = 0, j = 0;
	bool full;

	while (i < 20 && cont < 4) {
		
		full = true;
		j = 0;
		while (j < 10 && full) {

			if (board[i][j].state != Occupied)
				full = false;
			j++;
		}

		if (full) {
			removeFullLine(i);
			i--;
			cont++;
		}
			

		i++;
	}

	if (cont == 1) {
		addScore = 40;
		soundEngine->play2D("clear.mp3");
	}
		

	else if (cont == 2) {
		addScore = 100;
		soundEngine->play2D("clear.mp3");
	}
		

	else if (cont == 3) {
		soundEngine->play2D("clear.mp3");
		addScore = 300;
	}
		

	else if (cont == 4) {
		addScore = 1200;
		soundEngine->play2D("tetris.mp3");
	}
		

	pair<int, int> ret;

	ret.first = addScore;
	ret.second = cont;

	return ret;
}

void tBoard::removeFullLine(int ind) {

	for (int i = ind; i < 19; i++) {
		for (int j = 0; j < 10; j++) {
			board[i][j].state = board[i + 1][j].state;
			board[i][j].color = board[i + 1][j].color;
		}
			
	}

	for (int j = 0; j < 10; j++) 
		board[19][j].state = Empty;
		
}





