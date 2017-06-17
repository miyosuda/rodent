#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <GLUT/glut.h>

#include "play.h"
#include "Matrix4f.h"

#define DEFAULT_SCREEN_WIDTH   640
#define DEFAULT_SCREEN_HEIGHT  480

static int curButton = 0;

// 座標系をGL形式に直す為のマトリクス.
static Matrix4f crdFlipMat( 1.0f, 0.0f, 0.0f, 0.0f,
							0.0f,-1.0f, 0.0f, 0.0f,
							0.0f, 0.0f,-1.0f, 0.0f,
							0.0f, 0.0f, 0.0f, 1.0f );

/**
 * setProjection():
 */
static void setProjection(float width, float height) {
	float aspect = (float)DEFAULT_SCREEN_WIDTH / (float)DEFAULT_SCREEN_HEIGHT;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glFrustum(-0.5f*aspect * DEFAULT_SCREEN_HEIGHT * 0.1f, 
			   0.5f*aspect * DEFAULT_SCREEN_HEIGHT * 0.1f,
			  -0.5f        * DEFAULT_SCREEN_HEIGHT * 0.1f,
			   0.5f        * DEFAULT_SCREEN_HEIGHT* 0.1f,
			   512.0f * 0.1f,
			  12000.0f);

	glMatrixMode(GL_MODELVIEW);
}

/**
 * init(): 
 */
static void init() {
	setProjection(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);

	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
	glPolygonOffset(0.4, 0.5);
	glEnable(GL_POLYGON_OFFSET_FILL); 
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

	playInit();
}

/**
 * release():
 */
static void release() {
	playFinalize();
}

/**
 * paint(): 
 */
static void paint() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glMultMatrixf( (const float*)&crdFlipMat );

	playLoop();

	glPopMatrix();
	glutSwapBuffers();
	
	glutPostRedisplay();
}

/**
 * reshape():
 */
static void reshape(int width, int height) {
	glViewport(0, 0, width, height);
	setProjection(width, height);

	playReshape(width, height);
}

/**
 * keyDown(): 
 */
static void keyDown(unsigned char key, int x, int y) {
	switch(key) {
    case 0x1b:
		exit(0);
		break;
    }
}

/**
 * mouseDown(): 
 */
static void mouseDown(int button, int state, int x, int y) {
	if(button == GLUT_LEFT_BUTTON || 
	   button == GLUT_MIDDLE_BUTTON || 
	   button == GLUT_RIGHT_BUTTON) {

		int playButton;
		if(button == GLUT_LEFT_BUTTON) {
			playButton = MOUSE_LELFT_BUTTON;
		} else if(button == GLUT_RIGHT_BUTTON) {
			playButton = MOUSE_RIGHT_BUTTON;
		}

		if(state == GLUT_DOWN) {
			playMouseDown(x, y, playButton);
		}
		curButton = button;
    }
}

/**
 * mouseDrag(): 
 */
static void mouseDrag(int x, int y) {

	int playButton;
	if(curButton == GLUT_LEFT_BUTTON) {
		playButton = MOUSE_LELFT_BUTTON;
	} else if(curButton == GLUT_RIGHT_BUTTON) {
		playButton = MOUSE_RIGHT_BUTTON;
	}

	playMouseDrag(x, y, playButton);
}

/**
 * idle(): 
 */
static void idle() {
}

/**
 * main(): 
 */
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
	glutInitWindowSize(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT);
	glutInitWindowPosition(50, 50);
	glutCreateWindow( argv[0] );
	
	init();
  
	glutDisplayFunc( paint ); 
	glutReshapeFunc( reshape );
	glutKeyboardFunc( keyDown );
	glutMouseFunc( mouseDown );
	glutMotionFunc( mouseDrag );
	glutIdleFunc( idle );

	glutMainLoop();

	release();

	return 0;
}
