#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <math.h>  
#include <assert.h>
//#include "two.h"
#ifdef __linux
#include <GL/glut.h>
#elif __APPLE__
#include <GLUT/glut.h>
#endif

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

extern char *optarg;
extern int opterr;

const double minX = 0,
maxX = 500,
minY = 0,
maxY = 500,
margin = 10;

//int displayChoice = 0;
//char *lineOption[] = { "GL_POINTS", "GL_LINES", "GL_LINE_STRIP", "GL_LINE_LOOP", "GL_POLYGON"};
char *prg_chosen[] = { "", "", "", "", "" };
long prg_choice = 0;
vertex** polygon = NULL; 
int nb_points = 0;
int is_polygon_closed = 0;
char* ifilename = NULL; 
char* ofilename = NULL;



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


void winInit (void)
{
	gluOrtho2D(0.0, maxX, 0.0, maxY);
}


void define_points(int x, int y)
{
	if(nb_points == 0)
	{
		vertex* new_vertex = malloc(sizeof(vertex));
		polygon = malloc(sizeof(vertex*));
		new_vertex->x = x; 
		new_vertex->y = y;
		new_vertex->next = NULL; 
		new_vertex->previous = NULL;
		polygon[0] = new_vertex;  
		++nb_points;
	}
	else
	{
		double distance = sqrt((x-polygon[0]->x)*(x-polygon[0]->x)+(y-polygon[0]->y)*(y-polygon[0]->y));
		if(distance > 10.0)
		{
			distance = sqrt((x-polygon[nb_points-1]->x)*(x-polygon[nb_points-1]->x)+(y-polygon[nb_points-1]->y)*(y-polygon[nb_points-1]->y));
			if(x != polygon[nb_points-1]->x && y != polygon[nb_points-1]->y && distance > 5.0)
			{
				++nb_points;
				vertex* new_vertex = malloc(sizeof(vertex));
				polygon = realloc(polygon, nb_points*sizeof(vertex*));
				new_vertex->x = x; 
				new_vertex->y = y;
				new_vertex->next = NULL; 
				new_vertex->previous = NULL;
				polygon[nb_points-1] = new_vertex;
			}
		}
		else if(nb_points > 1)
		{
			is_polygon_closed = 1;
			FILE* f; 
			f =	fopen(ofilename, "w");
			int i;
			fprintf(f, "%d\n", nb_points);
			for(i=0; i<nb_points; ++i)
				fprintf(f, "%f %f\n", polygon[i]->x, polygon[i]->y);
			fclose(f);
			f = NULL;
		}
	}
}


//check if the current polygon is simple, that is without sides intersecting. return 1 if simple, 0 otherwise
int is_polygon_simple()
{
	return 0; 
}


//return 0 if the polygon is not degenerate, 1 otherwise
int is_polygon_degenerate()
{
	if(nb_points > 2 && puissance(polygon[0], polygon[1], polygon[2]) != ALIGNE)
		return 0;
	else
		return 1;
}


//remove duplicate vertices
/*int clean_polygon()
{
	int current = 0;
	for(current=0; current < nb_points; ++current)
	{
		int i = 1;
		for(i=1; current+i < nb_points; ++i)
		{
			if(polygon[current].x == polygon[current+i].x && polygon[current].y == polygon[current+i].y)
			{
				
			}
		}
	}
	return 0; 
	//comment
}*/

/*void rmdup(int *array, int length)
{
    int *current , *end = array + length - 1;

    for ( current = array + 1; array < end; array++, current = array + 1 )
    {
        while ( current < end )
        {
            if ( *current == *array )
            {
                *current = *end--;
            }
            else
            {
                current++;
            }
        }
    }
}*/


int load_file()
{
	if(ifilename == NULL)
		return -1;
	FILE* f;
	f = fopen(ifilename, "r");
	if(f == NULL)
		return -1;
	
	char * line = NULL;
	char * token1 = NULL;
	char * token2 = NULL;
	size_t len = 0;
	ssize_t read;
	
	if(getline(&line, &len, f) != 0)
		nb_points = strtol(line, NULL, 0);
	
	if(nb_points > 0)
		polygon = realloc(polygon, nb_points*sizeof(vertex*));
	
	int i; 
	for(i=0; i<nb_points; ++i)
	{
		read = getline(&line, &len, f);
		if(read != -1)
		{
			vertex* new_vertex = malloc(sizeof(vertex));
			token1 = strtok(line, " ");
			token2 = strtok(NULL, "\n");
			new_vertex->x = strtol(token1, NULL, 0);
			new_vertex->y = strtol(token2, NULL, 0);
			new_vertex->next = NULL; 
			new_vertex->previous = NULL;
			polygon[i] = new_vertex;
			//printf("%f %f\n", polygon[i].x, polygon[i].y);
		}
		else
		{
			printf("Problem reading the file. Check if the file structure is correct.\n");
			exit(EXIT_FAILURE);
		}
	}
	is_polygon_closed = 1; 
	
	if(line)
		free(line);
	
	return nb_points; 
}

void on_mouse_button(int button, int state, int x, int y)
{
	//int b;
	if(button == GLUT_LEFT_BUTTON || button == GLUT_MIDDLE_BUTTON)
		if(is_polygon_closed == 0)
			define_points(x, maxY - y);

	glutPostRedisplay();
}


void display (void)
{
	glColor3f(0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);
		
	if(is_polygon_closed)
		glBegin(GL_LINE_LOOP);
	else
		glBegin(GL_LINE_STRIP);

	if(polygon != NULL)
	{
		int i;
		for(i=0; i<nb_points; ++i)
		{
			glVertex2f(polygon[i]->x, polygon[i]->y);
		}
	}

	glEnd();

	glFlush();
}


int main(int argc, char **argv)  
{  
	int option;

	opterr = 0; /* set off std error messages in case wrong option is chosen */
	while ((option = getopt(argc, argv, "p:o:i:")) != EOF)
	{
		switch (option)
		{
			case 'o': 
			ofilename = strdup(optarg);
			if(ofilename == NULL)
			{
				printf("You have to specify a filename for the o flag\n");
				exit(EXIT_FAILURE);
			}
			break;
			case 'i':
			ifilename = strdup(optarg);
			if(ifilename == NULL)
			
			{
				printf("You have to specify a filename for the i flag\n");
				exit(EXIT_FAILURE);
			}
			break;
			case 'p': 
			prg_choice = strtol(optarg, NULL, 0);
			if(prg_choice<1 || prg_choice > 5)
			{
				printf("Usage: %s -pX with X: 1,2,3,4,5.\n", argv[0]);
				exit(EXIT_FAILURE);
			}
			break;
			case '?': /* value returned by getopt if invalid option chosen */
			default : printf("Usage: %s -X with X: 1,2,3,4 or 5.\n", argv[0]);
			break;
		}
	}
	glutInit(&argc, argv);  
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);  

	glutInitWindowPosition(5,5);
	glutInitWindowSize(maxX,maxY);

	glutCreateWindow("My first OpenGL window...");  
  
	winInit();

	if(prg_choice == 1)
	{
		glutDisplayFunc(display);
		glutMouseFunc(on_mouse_button);  
	}
	
	if(prg_choice == 2)
	{
		load_file();
		if(is_polygon_degenerate() == 1)
		{
			printf("The current polygon is degenerated. Please choose another polygon for computations\n");
			exit(EXIT_SUCCESS);
		}
		is_polygon_simple();
		glutDisplayFunc(display);
	}

	glutMainLoop();
	
	if(ofilename != NULL)
		free(ofilename);
	if(ifilename != NULL)
		free(ifilename);  
	if(polygon != NULL)
	{
		int i;
		for(i=0; i<nb_points; ++i)
		{
			free(polygon[i]);
		}
	}
	free(polygon);
	polygon = NULL;
  
	return EXIT_SUCCESS;  
}  
