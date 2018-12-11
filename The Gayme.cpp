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
#include <iostream>
#include <ctime>

using namespace std;

#define DEG2RAD(a) (a * 0.0174532925)
#define RAD2DEG(r) (r * 57.29577951)
#define ESCAPE 27
#define SPACEBAR 32

int WIDTH = 1280;
int HEIGHT = 720;

GLuint day;
GLuint night;

// Camera Instance
Camera freeCamera;
Camera firstPersonCamera;
Camera thirdPersonCamera;

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
bool freeView;
bool firstPerson;

// Lighting

GLfloat lightPosition2[] = { 0.0f, 10.0f, 0.0f, 1.0f };

bool nightTime;

//=======================================================================
// Camera Setup Function
//=======================================================================
void setupCamera() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, aspectRatio, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (freeView)
		freeCamera.look();
	else {
		if (firstPerson)
			firstPersonCamera.look();
		else
			thirdPersonCamera.look();
}
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
	// Skylight
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient0[] = { 0.02f, 0.02f, 0.02f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);

	// Define Light source 0 diffuse light
	GLfloat diffuse0[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);

	// Define Light source 0 Specular light
	GLfloat specular0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

	// Finally, define light source 0 position in World Space
	GLfloat lightPosition0[] = { 0.0f, 10.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);


	// Repeat for light 1: Lantern
	glEnable(GL_LIGHT1);

	GLfloat ambient1[] = { 0.0878f, 0.0616f, 0.0216f, 1.0f };
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient1);

	GLfloat diffuse1[] = { 0.878f, 0.616f, 0.216f, 1.0f };
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse1);

	GLfloat specular1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular1);

	GLfloat lightPosition1[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);

	// Light2 variables

	GLfloat ambient2[] = { 0.0878f, 0.0616f, 0.0216f, 1.0f };
	GLfloat diffuse2[] = { 0.878f, 0.616f, 0.216f, 1.0f };
	GLfloat specular2[] = { 1.0f, 1.0f, 1.0f, 1.0f };

	// Repeat for light 2: Disco ball
	//glEnable(GL_LIGHT2);

	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient2);

	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse2);

	glLightfv(GL_LIGHT2, GL_SPECULAR, specular2);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition2);

	// Attenuation
	glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 0.9f);
	glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.02f);
	glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, 0.9f);
	glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, 0.02f);
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

void toggleDayCycle() {
	if (nightTime) {
		nightTime = false;
		glEnable(GL_LIGHT0);
	}
	else {
		nightTime = true;
		glDisable(GL_LIGHT0);
	}
}

//=======================================================================
// Render Ground Function
//=======================================================================
void RenderGround()
{

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

void randomizeLight2() {
	GLfloat ambient2[] = { ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), 1.0f };
	GLfloat diffuse2[] = { ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), 1.0f };
	GLfloat specular2[] = { ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), ((double) rand() / (RAND_MAX)), 1.0f };
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient2);

	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse2);

	glLightfv(GL_LIGHT2, GL_SPECULAR, specular2);

}

//=======================================================================
// Display Function
//=======================================================================
void myDisplay(void) {
	randomizeLight2();
	setupCamera();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLfloat lightIntensity0[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, lightIntensity0);

	GLfloat lightPosition0[] = { 0.0f, 100.0f, 0.0f, 0.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition0);

	GLfloat lightPosition1[] = { player.pos.x, player.pos.y, player.pos.z, 1.0f };

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);


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
	
	// Draw Camera Eye & Center
	/*
	// Eye
	glColor3f(0.0f, 0.0f, 1.0f);
	glPushMatrix();
	{
		glTranslatef(firstPersonCamera.eye.x, firstPersonCamera.eye.y, firstPersonCamera.eye.z);
		glutSolidSphere(0.3, 10, 10);
	}
	glPopMatrix();
	glPushMatrix();
	{
		glTranslatef(thirdPersonCamera.eye.x, thirdPersonCamera.eye.y, thirdPersonCamera.eye.z);
		glutSolidSphere(0.3, 10, 10);
	}
	glPopMatrix();

	// Center
	glColor3f(1.0f, 0.0f, 0.0f);
	glPushMatrix();
	{
		glTranslatef(firstPersonCamera.center.x, firstPersonCamera.center.y, firstPersonCamera.center.z);
		glutSolidSphere(0.3, 10, 10);
	}
	glPopMatrix();
	glPushMatrix();
	{
		glTranslatef(thirdPersonCamera.center.x, thirdPersonCamera.center.y, thirdPersonCamera.center.z);
		glutSolidSphere(0.3, 10, 10);
	}
	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);
	*/

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
		if (!firstPerson || freeView)
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
		if (!nightTime)
			glBindTexture(GL_TEXTURE_2D, day);
		else
			glBindTexture(GL_TEXTURE_2D, night);
		gluQuadricTexture(qobj, true);
		gluQuadricNormals(qobj, GL_SMOOTH);
		gluSphere(qobj, 100, 100, 100);
		gluDeleteQuadric(qobj);
	}
	glPopMatrix();

	// Draw HUD Elements
	drawCrosshairs();

	glutSwapBuffers();

	glutPostRedisplay();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void myKeyboard(unsigned char key, int x, int y) {
	float d = 0.1;

	switch (key) {
	case 'z':
		toggleDayCycle();
		break;
	case 'w':
		if (freeView)
			freeCamera.moveZ(d);
		else {
			firstPersonCamera.moveZ(d);
			thirdPersonCamera.moveZ(d);
			Vector3f view;
			if (firstPerson)
				view = (firstPersonCamera.center - firstPersonCamera.eye).unit();
			else
				view = (thirdPersonCamera.center - thirdPersonCamera.eye).unit();
			player.pos.x += view.x * d;
			player.pos.z += view.z * d;
		}
		break;
	case 's':
		if (freeView)
			freeCamera.moveZ(-d);
		if (!freeView) {
			firstPersonCamera.moveZ(-d);
			thirdPersonCamera.moveZ(-d);
			Vector3f view;
			if (firstPerson)
				view = (firstPersonCamera.center - firstPersonCamera.eye).unit();
			else
				view = (thirdPersonCamera.center - thirdPersonCamera.eye).unit();
			player.pos.x += view.x * -d;
			player.pos.z += view.z * -d;
		}
		break;
	case 'a':
		if (freeView)
			freeCamera.moveX(d);
		else {
			firstPersonCamera.moveX(d);
			thirdPersonCamera.moveX(d);
			Vector3f right;
			if (firstPerson)
				right = firstPersonCamera.up.cross(firstPersonCamera.center - firstPersonCamera.eye).unit();
			else
				right = thirdPersonCamera.up.cross(thirdPersonCamera.center - thirdPersonCamera.eye).unit();
			player.pos = player.pos + right * d;
		}
		break;
	case 'd':
		if (freeView)
			freeCamera.moveX(-d);
		else {
			firstPersonCamera.moveX(-d);
			thirdPersonCamera.moveX(-d);
			Vector3f right;
			if (firstPerson)
				right = firstPersonCamera.up.cross(firstPersonCamera.center - firstPersonCamera.eye).unit();
			else
				right = thirdPersonCamera.up.cross(thirdPersonCamera.center - thirdPersonCamera.eye).unit();
			player.pos = player.pos + right * -d;
		}
		break;
	case 'q':
		if (freeView)
			freeCamera.moveY(-d);
		break;
	case 'e':
		if (freeView)
			freeCamera.moveY(d);
		break;
	case '\/':
		if (!freeView) {
			if (firstPerson) {
				freeCamera.eye = firstPersonCamera.eye;
				freeCamera.center = firstPersonCamera.center;
			}
			else {
				freeCamera.eye = thirdPersonCamera.eye;
				freeCamera.center = thirdPersonCamera.center;
			}
		}
		freeView = !freeView;
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
		;
		break;
	case GLUT_KEY_DOWN:
		;
		break;
	case GLUT_KEY_LEFT:
		;
		break;
	case GLUT_KEY_RIGHT:
		;
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
void myPassiveMotion(int x, int y) {
	if (just_warped) {
		just_warped = false;
		return;
	}

	y = HEIGHT - y;

	int diffx = x - WIDTH / 2;
	int diffy = y - HEIGHT / 2;

	if (freeView) {
		freeCamera.rotateX(diffy);
		freeCamera.rotateY(-diffx * 0.5);
		/*
		freeCamera.center.y += diffy * 0.01;
		if (freeCamera.center.y < 0)
			freeCamera.center.y = 0;
		else if (freeCamera.center.y > 2 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1))
			freeCamera.center.y = 2 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1);

		Vector3f rotationCenter = player.pos + Vector3f(0, player.bound_height * player.scale * 1.5 + 1, 0);
		float camera_speed = 0.5;
		freeCamera.rotateAroundY(-diffx * camera_speed, (freeCamera.eye + freeCamera.center) / 2);
		*/
	}
	else {
		firstPersonCamera.center.y += diffy * 0.01;
		thirdPersonCamera.center.y += diffy * 0.01;

		if (firstPersonCamera.center.y < 0)
			firstPersonCamera.center.y = 0;
		else if (firstPersonCamera.center.y > 2 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1))
			firstPersonCamera.center.y = 2 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1);
		if (thirdPersonCamera.center.y < 0)
			thirdPersonCamera.center.y = 0;
		else if (thirdPersonCamera.center.y > 2 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1))
			thirdPersonCamera.center.y = 2 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1);

		float camera_speed = 0.5;

		Vector3f rotationCenter = player.pos + Vector3f(0, player.bound_height * player.scale * 1.5 + 1, 0);
		firstPersonCamera.rotateAroundY(-diffx * camera_speed, rotationCenter);
		thirdPersonCamera.rotateAroundY(-diffx * camera_speed, rotationCenter);

		float angle;
		if (firstPerson)
			angle = RAD2DEG(atan2(firstPersonCamera.center.z - firstPersonCamera.eye.z, firstPersonCamera.center.x - firstPersonCamera.eye.x));
		else
			angle = RAD2DEG(atan2(thirdPersonCamera.center.z - thirdPersonCamera.eye.z, thirdPersonCamera.center.x - thirdPersonCamera.eye.x));
		player.rot.y = 90 - angle;
	}


	glutWarpPointer(WIDTH / 2, HEIGHT / 2);
	just_warped = true;

	glutPostRedisplay();	//Re-draw scene 
}

void myMotion(int x, int y) {
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
	if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
		/*
		if (firstPerson) {
			firstPersonEye = camera.eye;
			firstPersonCenter = camera.center;
			camera.eye = thirdPersonEye;
			camera.center = thirdPersonCenter;
		}
		else {
			thirdPersonEye = camera.eye;
			thirdPersonCenter = camera.center;
			camera.eye = firstPersonEye;
			camera.center = firstPersonCenter;
		}
		*/
		firstPerson = !firstPerson;
	}
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
	if (freeView)
		freeCamera.look();
	else {
		if (firstPerson)
			firstPersonCamera.look();
		else
			thirdPersonCamera.look();
	}
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

	// Initialize Player Model
	player.model = model_player;

	// Initialize Player Transformations
	player.pos = Vector3f(8.0, 0.01, 5.0);
	player.rot = Vector3f(0.0, 0.0, 0.0);
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
	freeView = false;
	firstPerson = false;

	// Initialize Camera & Cursor Positions
	glutWarpPointer(WIDTH / 2, HEIGHT / 2);

	// First Person View
	firstPersonCamera.eye = player.pos + Vector3f(0.0, player.bound_height * player.scale * 2, -1.5);
	firstPersonCamera.center = player.pos + Vector3f(0.0, player.bound_height * player.scale * 2, 2);
	firstPersonCamera.up = Vector3f(0, 1, 0);

	// Third Person View
	thirdPersonCamera.eye = player.pos + Vector3f(-0.8, player.bound_height * player.scale * 2, -1.5);
	thirdPersonCamera.center = player.pos + Vector3f(-0.8, player.bound_height * player.scale * 2, 2);
	thirdPersonCamera.up = Vector3f(0, 1, 0);

	// Free Roaming
	freeCamera.center = Vector3f(0, 0, 0);
	freeCamera.eye = Vector3f(20, 10, 20);
	freeCamera.up = Vector3f(0, 1, 0);

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
	loadBMP(&day, "Textures/blu-sky-3.bmp", true);
	loadBMP(&night, "Textures/night.bmp", true);
}

//=======================================================================
// Main Function
//=======================================================================
void main(int argc, char** argv)
{
	srand(time(NULL));
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
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glShadeModel(GL_SMOOTH);

	glutMainLoop();
}