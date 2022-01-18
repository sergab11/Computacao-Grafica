#include "GL/glut.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "stdlib.h"
#include "math.h"

#define pi 3.14159265

static float viewer_pos[4] = {2.0, 3.0, 4.0, 1.0};
float coordsE[64*64*6];
float normalsE[64*64*6];

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
	glClearColor(0.5, 0.5, 0.5, 1.0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
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

void desenhaFolha(void){
	glBegin(GL_TRIANGLE_STRIP);
	glNormal3f(0.0, 1.0, 0.0);

	glVertex3f(-0.5, 0.0, -1.0);
	glVertex3f(-0.5, 0.0, 1.0);
	glVertex3f(0.5, 0.0, 1.0);
	glVertex3f(0.5, 0.0, -1.0);
	glVertex3f(-0.5, 0.0, -1.0);
	glVertex3f(-0.5, 0.0, 1.0);

	glEnd();
}

void desenhaCena(void){
	float amb[4] = {0.1, 0.1, 0.1, 1.0};
	float dif[4] = {0.8, 0.8, 0.8, 1.0};
	float spe[4] = {0.9, 0.9, 0.9, 0.8};

	glLightfv(GL_LIGHT0, GL_POSITION, viewer_pos);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spe);

	float spot_pos[4] = {0.0, 4.0, 0.0, 1.0};
	float spot_dir[3] = {0, -1, 0};
	float aux1[1] = {128};
	float aux2[1] = {90};
	glLightfv(GL_LIGHT1, GL_POSITION, spot_pos);	//posição do spotlight
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, dif);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spe);
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_dir);	//direção para onde aponta o spotlight
	glLightfv(GL_LIGHT1, GL_SPOT_EXPONENT, aux1);	//intensidade da luz do spotlight
	glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, aux2);	//abertura do spotlight

	//determina cor da mesa
	float amb_bro[4] = {0.21, 0.13, 0.05, 1};
	float dif_bro[4] = {0.91, 0.43, 0.18, 1};
	float spec_bro[4] = {0.39, 0.27, 0.17, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_bro);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_bro);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_bro);

	//desenha "tampo" (parte de cima da mesa)
	glPushMatrix();
	glScalef(2.0f, 0.2f, 2.0f);
	glTranslatef(0.0, -0.5, 0.0);
	desenhaCubo();
	glPopMatrix();
	
	//desenha os 4 "pés" da mesa
	float amb_bro_pes[4] = {0.21, 0.13, 0.05, 1};
	float dif_bro_pes[4] = {0.51, 0.43, 0.18, 1};
	float spec_bro_pes[4] = {0.39, 0.27, 0.17, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_bro_pes);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_bro_pes);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_bro_pes);

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

	//desenha 3 cubos vermelhos em cima da mesa 
	float blue[] = {0.3, 0.3, 1.0, 1.0};
	glScalef(0.25f, 0.25f, 0.25f);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, blue);

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
	
	//desenha esfera preta em cima da mesa
	float amb_plas[4] = {0, 0, 0, 1};
	float dif_plas[4] = {0, 0, 0, 1};
	float spec_plas[4] = {0.5, 0.5, 0.5, 1};
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb_plas);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif_plas);
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec_plas);

	glPushMatrix();
	glScalef(0.5f, 0.5f, 0.5f);
	glTranslatef(0.0, 1.0, 0.0);
	desenhaEsfera(64, 64);
	glPopMatrix();

	//desenha folha de papel em cima da mesa
	float white[4] = {1.0f, 1.0f, 1.0f, 1.0f};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, white);
	
	glPushMatrix();
	glTranslatef(0.0, 1.0, 3.0);
	desenhaFolha();
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
	glutCreateWindow("Projeto 1");
	
	glutDisplayFunc(DisplayCB);
	inicializa();

	glutMainLoop();
	return 0;
}
