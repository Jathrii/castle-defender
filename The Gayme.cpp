#include "TextureBuilder.h"
#include "Model_3DS.h"
#include "GLTexture.h"
#include "Vector3f.h"
#include "Camera.h"
#include "Collidable.h"
#include "LinkedList.h"
#include "Node.h"
#include <glut.h>
#include <cmath>

#define ESCAPE 27
#define SPACEBAR 32

int WIDTH = 1280;
int HEIGHT = 720;

GLuint tex;

// Camera Instance
Camera camera;

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.05;
GLdouble zFar = 500;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_player;
Model_3DS model_skeleton;

// Collidable Variables
Collidable player;
LinkedList enemies;

// Textures
GLTexture tex_ground;

// Flow Control Variables
bool showBounds;

//=======================================================================
// Camera Setup Function
//=======================================================================
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspectRatio, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void initLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void initMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 96.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{
	glDisable(GL_LIGHTING);	// Disable lighting 

	glColor3f(0.6, 0.6, 0.6);	// Dim the ground texture a bit

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_ground.texture[0]);	// Bind the ground texture

	glPushMatrix();
	{
		glBegin(GL_QUADS);
		{
			glNormal3f(0, 1, 0);	// Set quad normal direction.
			glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
			glVertex3f(-20, 0, -20);
			glTexCoord2f(10, 0);
			glVertex3f(20, 0, -20);
			glTexCoord2f(10, 10);
			glVertex3f(20, 0, 20);
			glTexCoord2f(0, 10);
			glVertex3f(-20, 0, 20);
		}
		glEnd();
	}
	glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

	glColor3f(1, 1, 1);	// Set material back to white instead of grey used for the ground texture.
}

//=======================================================================
// Draw x y z axes
//=======================================================================
void axes(double length) {
	// x-axis
	glPushMatrix();
	{
		glColor3f(1.0, 0.0, 0.0);
		glLineWidth(5);
		glBegin(GL_LINES);
		{
			glVertex3d(0, 0, 0);
			glVertex3d(length, 0, 0); // along the x-axis
		}
		glEnd();

		glTranslated(length - 0.2, 0, 0);
		glScaled(5, 5, 5);
		glRotated(90, 0, 1, 0);
		glutSolidCone(0.04, 0.2, 12, 9);
	}
	glPopMatrix();

	// y-axis
	glPushMatrix();
	{
		glColor3f(0.0, 1.0, 0.0);
		glLineWidth(5);
		glBegin(GL_LINES);
		{
			glVertex3d(0, 0, 0);
			glVertex3d(0, length, 0); // along the y-axis
		}
		glEnd();

		glTranslated(0, length - 0.2, 0);
		glScaled(5, 5, 5);
		glRotated(-90, 1, 0, 0);
		glutSolidCone(0.04, 0.2, 12, 9);
	}
	glPopMatrix();

	// z-axis
	glPushMatrix();
	{
		glColor3f(0.0, 0.0, 1.0);
		glLineWidth(5);
		glBegin(GL_LINES);
		{
			glVertex3d(0, 0, 0);
			glVertex3d(0, 0, length); // along the z-axis 
		}
		glEnd();


		glTranslated(0, 0, length - 0.2);
		glScaled(5, 5, 5);
		glutSolidCone(0.04, 0.2, 12, 9);
	}
	glPopMatrix();
}

//=======================================================================
// Draw Crosshairs
//=======================================================================
void drawCrosshairs() {
	glPushMatrix();
	{
		glViewport(0, 0, WIDTH, HEIGHT);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glColor3f(0.0f, 0.0f, 0.0f);
		glLineWidth(3.0);
		//horizontal line
		glBegin(GL_LINES);
		{
			glVertex2i(WIDTH / 2 - 10, HEIGHT / 2);
			glVertex2i(WIDTH / 2 + 10, HEIGHT / 2);
		}
		glEnd();
		//vertical line
		glBegin(GL_LINES);
		{
			glVertex2i(WIDTH / 2, HEIGHT / 2 + 10);
			glVertex2i(WIDTH / 2, HEIGHT / 2 - 10);
		}
		glEnd();
		glColor3f(1.0f, 1.0f, 1.0f);
	}
	glPopMatrix();
}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void) {
	setupCamera();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat lightIntensity[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat lightPosition[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity);

	bool collision = false;
	Node* current = enemies.head;
	while (current) {
		collision |= (player & *(current->data));
		current = current->next;
	}

	if (collision) {
		glPushMatrix();
		{
			glTranslatef(0.0, 10.0, 0.0);
			glutSolidSphere(2, 10, 10);
		}
		glPopMatrix();
	}

	// Draw Center Sphere
	glutSolidSphere(0.5, 10, 10);

	// Draw Axes
	axes(30);

	// Draw Ground
	RenderGround();

	// Draw Tree Model
	glPushMatrix();
	{
		glTranslatef(10.0f, 0.0f, 0.0f);
		glScalef(0.7f, 0.7f, 0.7f);
		model_tree.Draw();
	}
	glPopMatrix();

	// Draw House Model
	glPushMatrix();
	{
		glTranslatef(0.0, 0.1, 0.0);
		glRotatef(90.0f, 1.0, 0.0, 0.0);
		model_house.Draw();
	}
	glPopMatrix();

	// Draw Player Model
	glPushMatrix();
	{
		player.draw();
	}
	glPopMatrix();

	// Draw Enemy Models
	current = enemies.head;
	while (current) {
		glPushMatrix();
		{
			(*(current->data)).draw();
		}
		glPopMatrix();

		current = current->next;
	}

	if (showBounds) {
		// Draw Player Bounding Sphere
		glColor4f(0.5, 0.0, 0.0, 0.5);

		glPushMatrix();
		{
			player.drawBounds();
		}
		glPopMatrix();

		// Draw Skeleton Bounding Sphere
		glColor4f(0.0, 0.0, 0.5, 0.5);

		current = enemies.head;
		while (current) {
			glPushMatrix();
			{
				(*(current->data)).drawBounds();
			}
			glPopMatrix();

			current = current->next;
		}

		glColor3f(1.0, 1.0, 1.0);
	}

	//Draw Sky Box
	glPushMatrix();
	{
		GLUquadricObj * qobj;
		qobj = gluNewQuadric();
		glTranslated(50, 0, 0);
		glRotated(90, 1, 0, 1);
		glBindTexture(GL_TEXTURE_2D, tex);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);
	}
	glPopMatrix();

	// Draw HUD Elements
	drawCrosshairs();

	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char key, int x, int y) {
	float d = 0.1;

	switch (key) {
	case 'w':
		camera.moveZ(d);
		break;
	case 's':
		camera.moveZ(-d);
		break;
	case 'a':
		camera.moveX(d);
		break;
	case 'd':
		camera.moveX(-d);
		break;
	case 'q':
		camera.moveY(-d);
		break;
	case 'e':
		camera.moveY(d);
		break;
	case SPACEBAR:
		showBounds = !showBounds;
		break;
	case ESCAPE:
		exit(EXIT_SUCCESS);
	}

	glutPostRedisplay();
}

//=======================================================================
// Special Key Function
//=======================================================================
void mySpecial(int key, int x, int y) {

	switch (key) {
	case GLUT_KEY_UP:
		player.pos.z = player.pos.z - 0.1f;
		break;
	case GLUT_KEY_DOWN:
		player.pos.z = player.pos.z + 0.1f;
		break;
	case GLUT_KEY_LEFT:
		player.pos.x = player.pos.x - 0.1f;
		break;
	case GLUT_KEY_RIGHT:
		player.pos.x = player.pos.x + 0.1f;
		break;
	}

	glutPostRedisplay();
}

// This variable is hack to stop glutWarpPointer from triggering an event callback to Mouse(...)
// This avoids it being called recursively and hanging up the event loop
bool just_warped = false;

//=======================================================================
// Motion Functions
//=======================================================================
void myPassiveMotion(int x, int y)
{
	if (just_warped) {
		just_warped = false;
		return;
	}

	y = HEIGHT - y;

	int diffx = x - WIDTH / 2;
	int diffy = y - HEIGHT / 2;

	camera.rotateX(diffy);
	camera.rotateY(-diffx * 0.5);

	glutWarpPointer(WIDTH / 2, HEIGHT / 2);
	just_warped = true;

	glutPostRedisplay();	//Re-draw scene 
}

void myMotion(int x, int y)
{
	myPassiveMotion(x, y);
}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	y = HEIGHT - y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		;
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN)
		;
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP)
		;
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
		;
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
		;
}

//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera.look();
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	// Initialize Camera & Cursor Positions
	glutWarpPointer(WIDTH / 2, HEIGHT / 2);
	camera.center = Vector3f(0, 0, 0);
	camera.eye = Vector3f(20, 10, 20);
	camera.up = Vector3f(0, 1, 0);

	// Initialize Player Model
	player.model = model_player;

	// Initialize Player Transformations
	player.pos = Vector3f(8.0, 0.01, 5.0);
	player.rot = Vector3f(0.0, 45.0, 0.0);
	player.scale = 0.015;

	// Initialize Player Bounds
	player.bound_radius = 60;
	player.bound_height = 90;

	Collidable* tree;
	// Initialize Enemies
	enemies = LinkedList();
	for (int i = 0; i < 3; i++) {
		Collidable* enemy = new Collidable();
		enemy->model = model_skeleton;
		enemy->pos = Vector3f(5.0, 2.149, 8.0 + i);
		enemy->rot = Vector3f(90.0, 45.0, 0);
		enemy->scale = 0.05;
		enemy->bound_radius = 20;
		enemy->bound_height = 0;
		enemies.add(enemy);
	}

	// Initialize Flow Control Variables
	showBounds = false;

	initLightSource();

	initMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_player.Load("Models/player/player.3ds");
	model_skeleton.Load("Models/skeleton/skeleton.3ds");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	loadBMP(&tex, "Textures/blu-sky-3.bmp", true);
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WIDTH) / 2, (glutGet(GLUT_SCREEN_HEIGHT) - HEIGHT) / 2);

	glutCreateWindow("The Gayme");

	glutDisplayFunc(myDisplay);

	glutKeyboardFunc(myKeyboard);

	glutSpecialFunc(mySpecial);

	glutMotionFunc(myMotion);

	glutPassiveMotionFunc(myPassiveMotion);

	glutMouseFunc(myMouse);

	glutSetCursor(GLUT_CURSOR_NONE);
	//glutSetCursor(GLUT_CURSOR_CROSSHAIR);

	glutReshapeFunc(myReshape);

	LoadAssets();

	myInit();

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}