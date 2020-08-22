#include "ficha.h"

using namespace std;


tFicha::tFicha(tShapes shape1) {

	shape = shape1;
	
	color.r = colorArray[shape][0];
	color.g = colorArray[shape][1];
	color.b = colorArray[shape][2];

	coords.first = 17;
	coords.second = 12;
	
	int cont = 0;

	calcCoords();
	


}

void tFicha::calcCoords() {
	int cont = 0;

	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
		{
			if (shapeArray[shape][rotation][i][j] == 1) {
				pos[cont].first = coords.first - i;
				pos[cont].second = coords.second + j;
				cont++;
			}

		}
}

void tBag::refill() {
	for (tShapes s = (tShapes)0; s != 7; s = (tShapes)(s + 1))
		bag.push_back(s);
	
}

tShapes tBag::pick() {
	
	if (bag.size() == 0)
		refill();

	
	int ind = rand() % bag.size();

	tShapes s = bag[ind];

	bag.erase(bag.begin() + ind);

	return s;
	
}