#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <math.h>  
#include <assert.h>

#define MAXPATHLEN 100
#define AGAUCHE 0
#define ADROITE 1
#define ALIGNE 2
#define ARRAY_SIZE 3
#define DEDANS 0
#define SUR 1
#define DEHORS 2
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1


typedef struct vertex vertex;

struct vertex
{
	double x;
	double y;
	vertex* next;
	vertex* previous;
};

vertex** polygon = NULL; 
int nb_points = 0;
int is_polygon_closed = 0;




void find_line_equation(vertex* d, vertex* e, double* array)
{
	//array stores the equation parameters a, b and c in this order
	//line equation is the following : ax + by + c
	double a,b,c;
	a = d->y - e->y;
	b = e->x - d->x;
	c = (d->x - e->x)*d->y + (e->y - d->y)*d->x;
	array[0] = a;
	array[1] = b;
	array[2] = c;
}


double find_signed_distance(vertex* c, double* array)
{
	double divisor = sqrt(array[0]*array[0] + array[1]*array[1]);
	return (c->x*array[0] + c->y*array[1] + array[2])/divisor;
}


int puissance(vertex* a, vertex* b, vertex* c)
{
	double* array = malloc(ARRAY_SIZE*sizeof(double));
	find_line_equation(a, b, array);
	double distance = find_signed_distance(c, array);
	free(array);
	if(distance > 0)
		return AGAUCHE;
	else if(distance < 0)
		return ADROITE; 
	else if(distance == 0)
		return ALIGNE;
	else
		return -1;
	return 0;
}


int in_triangle(vertex* a, vertex* b, vertex* c, vertex* d)
{
	int ab, bc, ca;
	ab = puissance(a, b, d);
	bc = puissance(b, c, d);
	ca = puissance(c, a, d);
	//Si on est aligné on doit bien tester si on est dans les limites du segment formant le côté. 
	if((ab == ALIGNE) || (bc == ALIGNE) || (ca == ALIGNE))
	{
		if(ab == ALIGNE)
		{
			if( d->x > fmax(a->x, b->x) || 
				d->x < fmin(a->x, b->x) || 
				d->y > fmax(a->y, b->y) ||
				d->y < fmin(a->y, b->y))
					return DEHORS;
		}
		if(bc == ALIGNE)
		{
			if( d->x > fmax(c->x, b->x) || 
				d->x < fmin(c->x, b->x) || 
				d->y > fmax(c->y, b->y) ||
				d->y < fmin(c->y, b->y))
					return DEHORS;
		}
		if(ca == ALIGNE)
		{
			if( d->x > fmax(a->x, c->x) || 
				d->x < fmin(a->x, c->x) || 
				d->y > fmax(a->y, c->y) ||
				d->y < fmin(a->y, c->y))
					return DEHORS;
		}
		return SUR;
	}
	else if((ab == ADROITE) && (bc == ADROITE) && (ca == ADROITE))
		return DEDANS; 
	else
		return DEHORS; 
}
