#include "GL/glut.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#define pi 3.14159265

static float viewer_pos[4] = {5.0, 5.0, 5.0, 1.0};

float coordsE[64*64*6];
float normalsE[64*64*6];

static float px0;
static float py0;

int w = 600;
int h = 600;

float M[16] = {
	1, 0, 0, 0, 
	0, 1, 0, 0, 
	0, 0, 1, 0, 
	0, 0, 0, 1
};

static float coords [] = {
	0.5, 0.5, -0.5,
	0.5, -0.5, -0.5,
	0.5, -0.5, 0.5,
	0.5, 0.5, 0.5,

	-0.5, 0.5, -0.5,
	-0.5, -0.5, -0.5,
	0.5, -0.5, -0.5,
	0.5, 0.5, -0.5,

	-0.5, 0.5, 0.5,
	-0.5, -0.5, 0.5,
	-0.5, -0.5, -0.5,
	-0.5, 0.5, -0.5,

	0.5, 0.5, 0.5,
	0.5, -0.5, 0.5,
	-0.5, -0.5, 0.5,
	-0.5, 0.5, 0.5,

	-0.5, 0.5, -0.5,
	0.5, 0.5, -0.5,
	0.5, 0.5, 0.5,
	-0.5, 0.5, 0.5,

	-0.5, -0.5, -0.5,
	0.5, -0.5, -0.5,
	0.5, -0.5, 0.5,
	-0.5, -0.5, 0.5
};

static float normals [] = {
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,
	1.0, 0.0, 0.0,

	0.0, 0.0, -1.0,
	0.0, 0.0, -1.0,
	0.0, 0.0, -1.0,
	0.0, 0.0, -1.0,

	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,
	-1.0, 0.0, 0.0,

	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,
	0.0, 0.0, 1.0,

	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,
	0.0, 1.0, 0.0,

	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0,
	0.0, -1.0, 0.0	
};

void inicializa(void){
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_VERTEX_ARRAY);
}

void desenhaEsfera(int nx, int ny){
	int k;	
	float alfa, beta;
	float x1, y1, z1, x2, y2, z2;

	alfa = (2*pi)/nx;
	beta = pi/ny;
	k = 0;

	glPolygonMode(GL_FRONT, GL_LINE);
	
	for(int j=0; j<=ny; j++){
		for(int i=0; i<=nx; i++){
			x1 = cos(alfa*i)*sin(beta*j);
			y1 = cos(beta*j);
			z1 = sin(alfa*i)*sin(beta*j);

			x2 = cos(alfa*i)*sin(beta*(j+1));
			y2 = cos(beta*(j+1));
			z2 = sin(alfa*i)*sin(beta*(j+1));
			
			normalsE[k] = x1;
            normalsE[k+1] = y1;
            normalsE[k+2] = z1;
            coordsE[k] = x1;
            coordsE[k+1] = y1;
            coordsE[k+2] = z1;
            k += 3;

            normalsE[k] = x2;
            normalsE[k+1] = y2;
            normalsE[k+2] = z2;
            coordsE[k] = x2;
            coordsE[k+1] = y2;
            coordsE[k+2] = z2;
            k += 3;
		}
	}

	glVertexPointer(3, GL_FLOAT, 0, coordsE);
	glNormalPointer(GL_FLOAT, 0, normalsE);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	for (int i = 0; i <= ny; i++) {
        int pos = i*(nx+1)*2;
        glDrawArrays(GL_TRIANGLE_STRIP, pos, (nx+1)*2);
    }

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);	
}

void desenhaCubo(void){
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	glVertexPointer(3, GL_FLOAT, 0, coords);
	glNormalPointer(GL_FLOAT, 0, normals);

	for(int i=0; i<6; i++){
		glDrawArrays(GL_QUADS, i*4, 4);
	}

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);	
}

void desenhaCena(void){
	float amb[4] = {0.1, 0.1, 0.1, 1.0};
	float dif[4] = {0.7, 0.7, 0.7, 1.0};
	float spe[4] = {0.8, 0.8, 0.8, 0.8};

	glLightfv(GL_LIGHT0, GL_POSITION, viewer_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spe);

	//determina cor da mesa
	float gray[] = {0.6, 0.6, 0.6, 1.0};
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
	float white[] = {0.8, 0.8, 0.8, 0.8};
	float green[] = {0.0, 1.0, 0.0, 1.0};
	
	glPushMatrix();
	glScalef(0.5f, 0.5f, 0.5f);
	glTranslatef(0.0, 1.0, 0.0);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
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
	glLoadMatrixf(M);
	gluLookAt(
		viewer_pos[0], viewer_pos[1], viewer_pos[2],
		0.0, 0.0, 0.0,
		0.0, 1.0, 0.0); 
}

static void motionCB(int x, int y){
	float X0, Y0, Z0, X1, Y1, Z1, l0, l1, r;
	float mod_a, ax, ay, az, theta, theta_rad;
	r = w/2;

	X0 = (px0-(w/2))/r;
	Y0 = (py0-(h/2))/r;
	l0 = sqrt((X0*X0)+(Y0*Y0));

	X1 = (x-(w/2))/r;
	Y1 = (y-(h/2))/r;
	l1 = sqrt((X1*X1)+(Y1*Y1));

	if(l0 > 1.0){
		X0 = X0/l0;
		Y0 = Y0/l0;
		Z0 = 0.0f;
	}
	else if(l0 <= 1.0){
		Z0 = sqrt(1-(l0*l0));
	}

	if(l1 > 1.0){
		X1 = X1/l1;
		Y1 = Y1/l1;
		Z1 = 0.0f;
	}
	else if(l1 <= 1.0){
		Z1 = sqrt(1-(l1*l1));
	}

	ax = (Y0*Z1 - Y1*Z0);
	ay = (Z0*X1 - Z1*X0);
	az = (X0*Y1 - X1*Y0);
	mod_a = sqrt((ax*ax) + (ay*ay) + (az*az));
	theta_rad = 2*asin(mod_a);
	theta = (360*theta_rad)/(2*pi);
	
	glPushMatrix();
	glLoadIdentity();	
	glTranslatef(0.0, 0.0, -Z1);
	glRotatef(theta, ax, ay, az);
	glTranslatef(0.0, 0.0, Z1);
	glMultMatrixf(M); //multiplica a matriz do modelview com M

	glGetFloatv(GL_MODELVIEW_MATRIX, M); //"joga" a modelview em M
	
	glPopMatrix();
	px0 = x;
	py0 = y;
	glutPostRedisplay();
}

static void mouseCB(int button, int state, int x, int y){
	if(button != GLUT_LEFT_BUTTON){
		return;
	}
	if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		px0 = float(x);
		py0 = float(h-y);
		glutMotionFunc(motionCB);
	}
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_UP){
		glutMotionFunc(NULL);
	}
}


void displayCB(void){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	setCamera();
	desenhaCena();
	glutSwapBuffers();
}

int main(int argc, char* argv[]){
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(w, h);
	glutCreateWindow("Tarefa 3");
	
	glutDisplayFunc(displayCB);
	inicializa();
	glutMouseFunc(mouseCB);

	glutMainLoop();
	return 0;
}
