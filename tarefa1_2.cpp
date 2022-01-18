#include "GL/glut.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "math.h"

#define pi 3.141592

static float viewer_pos[4] = {8.0, 8.0, 8.0, 1.0};
static float theta = 0.0f;

void inicializa(void){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
}

void desenhaEsfera(int nx, int ny){
	float alfa, beta;
	float x1, y1, z1, x2, y2, z2;

	alfa = (2*pi)/nx;
	beta = pi/ny;

	glPolygonMode(GL_FRONT, GL_LINE);
	
	for(int j=0; j<=ny; j++){
		glBegin(GL_TRIANGLE_STRIP);
		for(int i=0; i<=nx; i++){
			x1 = cos(alfa*i)*sin(beta*j);
			y1 = cos(beta*j);
			z1 = sin(alfa*i)*sin(beta*j);

			x2 = cos(alfa*i)*sin(beta*(j+1));
			y2 = cos(beta*(j+1));
			z2 = sin(alfa*i)*sin(beta*(j+1));

			glNormal3f(x1, y1, z1);
			glVertex3f(x1, y1, z1);
			glNormal3f(x2, y2, z2);
			glVertex3f(x2, y2, z2);
		}
		glEnd();
	}	
}

void desenhaCena(void){
	float amb[4] = {0.1, 0.1, 0.1, 1.0};
	float dif[4] = {0.7, 0.7, 0.7, 1.0};
	float spe[4] = {1.0, 1.0, 1.0, 1.0};
	float zeros[4] = {0.0f, 0.0f, 0.0f, 1.0f};

	float white[] = {1.0, 1.0, 1.0, 1.0};
	float yellow[] = {1.0f, 1.0f, 0.0f, 1.0f};
	float blue[] = {0.0f, 0.0f, 1.0f, 1.0f};
	float pink[] = {1.0f, 0.5f, 0.55f , 1.0f};

	glLightfv(GL_LIGHT0, GL_POSITION, zeros);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spe);

	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);

	//Sol
	glPushMatrix();
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, yellow);
	desenhaEsfera(64, 64);
	
	//Terra
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);
	glRotatef(theta/1.0f, 0.0f, -1.0f, 0.0f);
	glTranslatef(5.0f, 0.0f, 0.0f);
	glScalef(0.5f, 0.5f, 0.5f);
	desenhaEsfera(64, 64);

	//Lua
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pink);
	glRotatef(theta/1.0f, 0.0f, -1.0f, 0.0f);
	glTranslatef(5.5f, 0.0f, 0.0f);
	glScalef(0.25f, 0.25f, 0.25f);
	desenhaEsfera(64, 64);
	glPopMatrix();
}

void setCamera(){
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	float ratio = (float)vp[2]/vp[3];

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, ratio, 0.5, 50);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(
		viewer_pos[0], viewer_pos[1], viewer_pos[2],
		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f); 
}

void DisplayCB(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setCamera();
	desenhaCena();
	glutSwapBuffers();
}

static void idle(void){
	theta += 1.0f;
	glutPostRedisplay();
}

int main(int argc, char* argv[]){
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Tarefa 2 - CGraf");
	
	glutDisplayFunc(DisplayCB);
	inicializa();

	glutIdleFunc(idle);	
	glutMainLoop();
	return 0;
}
