#include "naive.h"

void draw_points(int points, vertex** polygon, int height, int width)
{
	int i;
	for(i=0; i<points; ++i)
	{
		int x = rand() % width;
		int y = rand() % height;
		int j; 
		for(j=0; j<nb_points; ++j)
		{
			while(x == polygon[j]->x && y == polygon[j]->y)
			{
				x = rand() % width;
				y = rand() % height;
			}	
		}
		polygon[nb_points]->x = x;
		polygon[nb_points]->y = y;
		++nb_points;
	}
}