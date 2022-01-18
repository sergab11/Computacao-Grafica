#include "GL/glut.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "math.h"

#define pi 3.14159265

static float viewer_pos[4] = {2.0, 3.0, 4.0, 1.0};

static float M[16] = {
	1.0, 0, 0, 0,
	0, 1.0, 0, 0,
	0, 0, 1.0, 0,
	0, 0, 0, 1.0
};

void inicializa(void){
	glClearColor(1.0, 1.0, 1.0, 1.0);
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

void desenhaCubo(){
	glBegin(GL_QUADS);
	glNormal3f(1.0, 0.0, 0.0);

	glVertex3f(0.5, 0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(0.5, 0.5, 0.5);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0, 0.0, -1.0);	

	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glVertex3f(0.5, 0.5, -0.5);
	glEnd();
	
	glBegin(GL_QUADS);
	glNormal3f(-1.0, 0.0, 0.0);

	glVertex3f(-0.5, 0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(-0.5, 0.5, -0.5);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, 1.0);

	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glVertex3f(-0.5, 0.5, 0.5);
	glEnd();

	glBegin(GL_QUADS);	
	glNormal3f(0.0, 1.0, 0.0);

	glVertex3f(-0.5, 0.5, -0.5);
	glVertex3f(0.5, 0.5, -0.5);
	glVertex3f(0.5, 0.5, 0.5);
	glVertex3f(-0.5, 0.5, 0.5);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0.0, -1.0, 0.0);

	glVertex3f(-0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, -0.5);
	glVertex3f(0.5, -0.5, 0.5);
	glVertex3f(-0.5, -0.5, 0.5);
	glEnd();	
}

void desenhaCena(void){
	float amb[4] = {0.1, 0.1, 0.1, 1.0};
	float dif[4] = {0.7, 0.7, 0.7, 1.0};
	float spe[4] = {1.0, 1.0, 1.0, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, viewer_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spe);
	
	//determina cor da mesa
	float gray[] = {0.8, 0.8, 0.8, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gray);

	//desenha "tampo" (parte de cima da mesa)
	glPushMatrix();
	glScalef(2.0f, 0.2f, 2.0f);
	glTranslatef(0.0, -0.5, 0.0);
	desenhaCubo();
	glPopMatrix();

	//desenha os 4 "pés" da mesa
	glPushMatrix();
	glScalef(0.2f, 2.0f, 0.2f);
	glTranslatef(4.0, -0.5, 4.0);
	desenhaCubo();
	glPopMatrix();

	glPushMatrix();
	glScalef(0.2f, 2.0f, 0.2f);
	glTranslatef(4.0, -0.5, -4.0);
	desenhaCubo();
	glPopMatrix();

	glPushMatrix();
	glScalef(0.2f, 2.0f, 0.2f);
	glTranslatef(-4.0, -0.5, -4.0);
	desenhaCubo();
	glPopMatrix();

	glPushMatrix();
	glScalef(0.2f, 2.0f, 0.2f);
	glTranslatef(-4.0, -0.5, 4.0);
	desenhaCubo();
	glPopMatrix();

	//desenha 4 cubos vermelhos em cima da mesa 
	float red[] = {1.0, 0.0, 0.0, 1.0};
	glScalef(0.25f, 0.25f, 0.25f);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, red);

	glPushMatrix();
	glTranslatef(2.0, 0.5, 0.0);
	desenhaCubo();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.5, -2.0);
	desenhaCubo();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-2.0, 0.5, 0.0);
	desenhaCubo();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0, 0.5, 2.0);
	desenhaCubo();
	glPopMatrix();

	//desenha esfera verde em cima da mesa
	float white[] = {1.0, 1.0, 1.0, 1.0};
	float green[] = {0.0, 1.0, 0.0, 1.0};
	glScalef(0.5f, 0.5f, 0.5f);
	glTranslatef(0.0, 1.0, 0.0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
	desenhaEsfera(64, 64);
}

void setCamera(){
	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);
	float ratio = (float)vp[2]/vp[3];

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, ratio, 0.5, 50);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(M);
	gluLookAt(
		viewer_pos[0], viewer_pos[1], viewer_pos[2],
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0); 
}

void DisplayCB(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setCamera();
	desenhaCena();
	glutSwapBuffers();
}

int main(int argc, char* argv[]){
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Tarefa 1 - Computacao Grafica");
	
	glutDisplayFunc(DisplayCB);
	inicializa();

	glutMainLoop();
	return 0;
}
