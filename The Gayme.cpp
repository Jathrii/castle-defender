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
#include <time.h>
#include <irrKlang.h>
using namespace std;
using namespace irrklang;
#pragma comment(lib, "irrKlang.lib") // link with irrKlang.dll


#define DEG2RAD(a) (a * 0.0174532925)
#define RAD2DEG(r) (r * 57.29577951)
#define ESCAPE 27
#define SPACEBAR 32

int WIDTH = 1280;
int HEIGHT = 720;
int X;
int Z;
int PlayerScore = 0;
float CastleHealth = 1000;
float RandomEnemyX;
float RandomEnemyY;
int CurrentEnemyNumber = 1;
int EnemySize = 0;
char* GameOverText = "";
bool GameOver = false;
bool Hit = false;
bool Level1 = true;




GLuint tex;



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
Model_3DS model_castle;
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS model_player;
Model_3DS model_skeleton;
Model_3DS model_coin;
Model_3DS model_stone;
Model_3DS model_knight;


// Collidable Variables
Collidable player;
Collidable Castle;
Collidable Stone;
LinkedList enemies = LinkedList();
LinkedList Collectibles = LinkedList();
LinkedList Stones = LinkedList();

// Textures
GLTexture tex_ground;

// Flow Control Variables
bool showBounds;
bool freeView;
bool firstPerson;

// IrrKlang init
ISoundEngine* engine = createIrrKlangDevice();
ISound *footsteps;
ISound *castleFall;

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
	GLfloat ambient0[] = { 0.6f, 0.6f, 0.6f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient0);

	// Define Light source 0 diffuse light
	GLfloat diffuse0[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse0);

	// Define Light source 0 Specular light
	GLfloat specular0[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular0);

	// Finally, define light source 0 position in World Space
	GLfloat lightPosition0[] = { 0.0f, 100.0f, 0.0f, 0.0f };
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
	GLfloat specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
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
//Draw Castle Health .. decreases with monster hits

//=======================================================================
void UpdateCastleHealth() {

	glPushMatrix();
	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(CastleHealth, 10, 0);
	glVertex3f(1270, 10, 0);
	glVertex3f(1270, 35, 0);
	glVertex3f(CastleHealth, 35, 0);
	glEnd();
	glPopMatrix();


}
//Print UI
//=======================================================================
void print(int x, int y, char *string)
{
	int len, i;

	//set the position of the text in the window using the x and y coordinates
	glRasterPos2f(x, y);

	//get the length of the string to display
	len = (int)strlen(string);

	//loop to display character by character
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}
// Draw Crosshairs - Has drawing of UI and Castle health
//=======================================================================
void drawCrosshairs() {
	glPushMatrix();
	{
		if (nightTime)
			glColor3f(1.0f, 1.0f, 1.0f);
		else
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

void drawHUD() {
	glBindTexture(GL_TEXTURE_2D, NULL);
	glDisable(GL_LIGHTING);

	glPushMatrix();
	glViewport(0, 0, WIDTH, HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, WIDTH, HEIGHT, 0, -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	drawCrosshairs();

	char* p0s[20];
	sprintf((char *)p0s, "Your Score = %d ", PlayerScore);
	print(10, 30, (char *)p0s);

	print(500, 30, GameOverText);
	UpdateCastleHealth();

	glPopMatrix();

	glEnable(GL_LIGHTING);
}

void randomizeLight2() {
	GLfloat ambient2[] = { ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), 1.0f };
	GLfloat diffuse2[] = { ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), 1.0f };
	GLfloat specular2[] = { ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), ((double)rand() / (RAND_MAX)), 1.0f };
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

	GLfloat lightPosition1[] = { player.pos.x, player.pos.y, player.pos.z, 1.0f };

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition1);


	if (enemies.length == 0 && !GameOver) {

		if ((Level1 && nightTime) || (!Level1 && !nightTime)) {
			toggleDayCycle();
		}

		if (Level1) {
			for (int i = 0; i < 10; i++)
			{
				float RandZ = (std::rand() % (20));
				float RandX = -3.3 + (std::rand() % (7));
				Collidable* enemy = new Collidable();
				enemy->model = model_knight;
				enemy->pos = Vector3f(RandX, 0.0f, RandZ);;
				enemy->rot = Vector3f(0, 180, 0);
				enemy->scale = 2;
				enemy->bound_radius = 0.6;
				enemy->bound_height = 1;
				enemies.add(enemy);
			}

			Node* current = enemies.head;
			while (current) {
				cout << (current->data) << endl;
				current = current->next;
			}
		}
		else {

			for (int i = 0; i < 10; i++)
			{
				float RandZ = (std::rand() % (20));
				float RandX = -3.3 + (std::rand() % (7));
				Collidable* enemy = new Collidable();
				enemy->model = model_skeleton;
				enemy->pos = Vector3f(RandX, 1.8f, RandZ);;
				enemy->rot = Vector3f(90, 180, 0);
				enemy->scale = 0.04;
				enemy->bound_radius = 20;
				enemy->bound_height = 0;
				enemies.add(enemy);
			}


		}
	}

	// Add New collectibles 
	if (Collectibles.length == 0) {
		//cout << Collectibles.length << " " << "Length " << endl;

		for (int i = 0; i < 10; i++) {
			Z = -18 + (std::rand() % (36));
			X = (std::rand() % (30)) + -10;
			Collidable* c = new Collidable();
			c->model = model_coin;
			c->pos = Vector3f(X, 0.7, Z);
			c->rot = Vector3f(90.0, 0, 0);
			c->scale = 0.5;
			c->bound_radius = 1;
			c->bound_height = 0;
			Collectibles.add(c);
		}





	}
	//create new stone



	bool HitCastle = false;

	// Check if the Castle was hit
	Node*current = enemies.head;
	int i = 0;
	while (current) {

		HitCastle |= (Castle & *((current)->data));
		if (HitCastle) {
			//cout << i << " " << "Hit Castle " << endl;

			enemies.remove(current->data);

			if (Level1) {
				CastleHealth += 22;
			}
			else {
				CastleHealth += 27; // More Damage
			}

			engine->play2D("./sounds/castleHit.wav");
			HitCastle = false;



		}
		if (CastleHealth >= 1270) {
			CastleHealth = 1270;
			castleFall->setIsPaused(false);
			GameOver = true; // Either the Health of the castle is finished or All enemies die 
			GameOverText = "Game Over and goodbye";
		}
		
		current = current->next;
	}
	if (enemies.head == NULL) {
		if (Level1) {
			enemies = LinkedList();
			Level1 = false;
		}
		else {
			if (CastleHealth < 1270) {
				GameOver = true; // The Player Won
				GameOverText = "GoodJob Sucka!";
			}
		}
	}
	bool HitCollectible = false;

	// Check if a Collectible was collected
	current = Collectibles.head;

	while (current) {
		cout << current  << endl;
		HitCollectible |= (player & *((current)->data));
		if (HitCollectible) {
			Collectibles.remove(current->data);
			CastleHealth -= 10;
		}
		HitCollectible = false;
		current = current->next;
	}
	if (Collectibles.head == NULL)
	{
		Collectibles = LinkedList();
	}

	current = Stones.head;
	int i1 = 0;
	int j = 0;
	while (current) {

		Node* Enemy = enemies.head;
		Node* lastEnemy = NULL;
		//cout << stonehit << " " << "first loop " << endl;
		while (Enemy) {
			cout << i1 << endl;
			cout << j << endl;
			cout << "abc" << endl;
			cout << Enemy << endl;
			//			cout << current << endl;
			bool stonehit = (*((Enemy)->data) & *((current)->data));
			//cout << stonehit << " " << "stone hit " << endl;
			if (stonehit) {
				Stones.remove(current->data);

				enemies.remove(Enemy->data);

				PlayerScore += 10;
				engine->play2D("./sounds/hit.wav");

				if (enemies.head == NULL) {
					if (Level1) {
						enemies = LinkedList();
						Level1 = false;
					}
					else {
						if (CastleHealth < 1270) {
							glEnable(GL_LIGHT2);
							GameOver = true; // The Player Won
							GameOverText = "GoodJob Sucka!";
						}
					}
				}

			}
			Enemy = Enemy->next;
			j++;
		}

		i1++;

		Collidable Stone = *(current->data);
		if (Stone.pos.x > 18 || Stone.pos.x < -18 || Stone.pos.z>20 || Stone.pos.z < -20) {
			Stones.remove(current->data);
		}
		HitCollectible = false;

		current = current->next;
	}

	if (Stones.head == NULL)
	{
		Stones = LinkedList();
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
	glutSolidSphere(0.05, 10, 10);
	}
	glPopMatrix();
	glPushMatrix();
	{
	glTranslatef(thirdPersonCamera.eye.x, thirdPersonCamera.eye.y, thirdPersonCamera.eye.z);
	glutSolidSphere(0.05, 10, 10);
	}
	glPopMatrix();

	// Center
	glColor3f(1.0f, 0.0f, 0.0f);
	glPushMatrix();
	{
	glTranslatef(firstPersonCamera.center.x, firstPersonCamera.center.y, firstPersonCamera.center.z);
	glutSolidSphere(0.05, 10, 10);
	}
	glPopMatrix();
	glPushMatrix();
	{
	glTranslatef(thirdPersonCamera.center.x, thirdPersonCamera.center.y, thirdPersonCamera.center.z);
	glutSolidSphere(0.05, 10, 10);
	}
	glPopMatrix();

	glColor3f(1.0f, 1.0f, 1.0f);
	*/

	// Draw Ground
	RenderGround();



	// Draw Tree Model
	glPushMatrix();
	{
		glTranslatef(-15.0f, 0.0f, 0.0f);
		glScalef(0.7f, 0.7f, 0.7f);
		model_tree.Draw();
	}
	glPopMatrix();

	//draw coin/collectiable model
	glPushMatrix();
	{
		glTranslatef(18.0f, 6.0f, 3.0f);
		glScalef(0.8f, 0.8f, 0.8f);
		//glColor3f(0.8f,0.7f,0.4f);
		glRotatef(90, 10, 0, 0);
		//model_coin.Draw();
	}
	glPopMatrix();
	//draw stone
	glPushMatrix();
	{
		//glTranslatef(0.0f, 6.0f, 0.0f);
		//glScalef(2.8f, 2.8f, 2.8f);
		Stone.draw();

	}
	glPopMatrix();


	// Draw House Model


	glPushMatrix();
	{

		//model_house.Draw();
		Castle.draw();

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
	//draw Collectibles
	current = Collectibles.head;
	while (current) {

		glPushMatrix();
		{
			(*(current->data)).draw();
		}
		glPopMatrix();

		current = current->next;
	}
	//draw stone
	current = Stones.head;
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
			Stone.drawBounds();
			//Castle.drawBounds();

		}
		glPopMatrix();


		glPushMatrix();
		{
			glColor4f(0.4, 0.1, 0.0, 0.5);

			Castle.drawBounds();

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

		// Draw Collectibles
		glColor4f(0.0, 0.0, 0.5, 0.5);

		current = Collectibles.head;
		while (current) {
			glPushMatrix();
			{
				(*(current->data)).drawBounds();
			}
			glPopMatrix();

			current = current->next;
		}
		//stone bounds
		glColor4f(0.0, 0.0, 0.5, 0.5);

		current = Stones.head;
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
	drawHUD();

	glutSwapBuffers();

	//glutPostRedisplay();
}

void addStone() {
	cout << PlayerScore << " " << "Player is Hitting " << endl;
	//Hit = true;
	Collidable* c = new Collidable();
	c->model = model_stone;
	c->pos.x = player.pos.x;
	if (firstPerson)
		c->pos.y = firstPersonCamera.eye.y * 0.7;
	else
		c->pos.y = thirdPersonCamera.eye.y * 0.7;
	c->pos.z = player.pos.z;
	Vector3f view;
	if (firstPerson) {
		view = firstPersonCamera.getView();
	}

	else {
		view = thirdPersonCamera.getView();
	}
	c->momentum = view;
	c->rot = Vector3f(90.0, 0, 0);
	c->scale = 0.1;
	c->bound_radius = 1;
	c->bound_height = 0;
	Stones.add(c);
	engine->play2D("./sounds/throw.wav");
}

//=======================================================================
// Timer Functions
//=======================================================================
void ShootEnemy(int extravar) {
	Node* current = Stones.head;
	while (current) {
		Collidable Stone = *(current->data);

		//Stone.scale = 0.08;
		Stone.pos.x += Stone.momentum.x * 0.8;
		Stone.pos.z += Stone.momentum.z * 0.8;
		//cout << Stone.pos.z << " " << "here " << Stone.momentum.z << endl;
		*(current->data) = Stone;


		current = current->next;
	}



	//glutPostRedisplay();

	glutTimerFunc(1000.0 / 60.0, ShootEnemy, 0);

}
//=======================================================================
// Move Enemy
//=======================================================================
void MoveEnemy(int extravar) {

	Node* current = enemies.head;
	while (current) {
		(current->data)->pos.z -= 0.01;


		current = current->next;
	}
	glutTimerFunc(1000.0 / 60.0, MoveEnemy, 0);

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
			footsteps->setIsPaused(false);
			Vector3f view;
			if (firstPerson)
				view = (firstPersonCamera.center - firstPersonCamera.eye).unit();
			else
				view = (thirdPersonCamera.center - thirdPersonCamera.eye).unit();
			view = view * d;
			player.pos = player.pos + Vector3f(view.x, 0, view.z);
			if (player & Castle) {
				player.pos = player.pos - Vector3f(view.x, 0, view.z);
				break;
			}
			firstPersonCamera.eye = firstPersonCamera.eye + Vector3f(view.x, 0, view.z);
			firstPersonCamera.center = firstPersonCamera.center + Vector3f(view.x, 0, view.z);
			thirdPersonCamera.eye = thirdPersonCamera.eye + Vector3f(view.x, 0, view.z);
			thirdPersonCamera.center = thirdPersonCamera.center + Vector3f(view.x, 0, view.z);
		}
		break;
	case 's':
		if (freeView)
			freeCamera.moveZ(-d);
		if (!freeView) {
			footsteps->setIsPaused(false);
			Vector3f view;
			if (firstPerson)
				view = (firstPersonCamera.center - firstPersonCamera.eye).unit();
			else
				view = (thirdPersonCamera.center - thirdPersonCamera.eye).unit();
			view = view * -d;
			player.pos = player.pos + Vector3f(view.x, 0, view.z);
			if (player & Castle) {
				player.pos = player.pos - Vector3f(view.x, 0, view.z);
				break;
			}
			firstPersonCamera.eye = firstPersonCamera.eye + Vector3f(view.x, 0, view.z);
			firstPersonCamera.center = firstPersonCamera.center + Vector3f(view.x, 0, view.z);
			thirdPersonCamera.eye = thirdPersonCamera.eye + Vector3f(view.x, 0, view.z);
			thirdPersonCamera.center = thirdPersonCamera.center + Vector3f(view.x, 0, view.z);
		}
		break;
	case 'a':
		if (freeView)
			freeCamera.moveX(d);
		else {
			footsteps->setIsPaused(false);
			Vector3f right;
			if (firstPerson)
				right = firstPersonCamera.up.cross(firstPersonCamera.center - firstPersonCamera.eye).unit();
			else
				right = thirdPersonCamera.up.cross(thirdPersonCamera.center - thirdPersonCamera.eye).unit();
			right = right * d;
			player.pos = player.pos + right;
			if (player & Castle) {
				player.pos = player.pos - right;
				break;
			}
			firstPersonCamera.eye = firstPersonCamera.eye + Vector3f(right.x, 0, right.z);
			firstPersonCamera.center = firstPersonCamera.center + Vector3f(right.x, 0, right.z);
			thirdPersonCamera.eye = thirdPersonCamera.eye + Vector3f(right.x, 0, right.z);
			thirdPersonCamera.center = thirdPersonCamera.center + Vector3f(right.x, 0, right.z);
		}
		break;
	case 'd':
		if (freeView)
			freeCamera.moveX(-d);
		else {
			footsteps->setIsPaused(false);
			Vector3f right;
			if (firstPerson)
				right = firstPersonCamera.up.cross(firstPersonCamera.center - firstPersonCamera.eye).unit();
			else
				right = thirdPersonCamera.up.cross(thirdPersonCamera.center - thirdPersonCamera.eye).unit();
			right = right * -d;
			player.pos = player.pos + right;
			if (player & Castle) {
				player.pos = player.pos - right;
				break;
			}
			firstPersonCamera.eye = firstPersonCamera.eye + Vector3f(right.x, 0, right.z);
			firstPersonCamera.center = firstPersonCamera.center + Vector3f(right.x, 0, right.z);
			thirdPersonCamera.eye = thirdPersonCamera.eye + Vector3f(right.x, 0, right.z);
			thirdPersonCamera.center = thirdPersonCamera.center + Vector3f(right.x, 0, right.z);
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
	case '\\':
		firstPerson = !firstPerson;
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
	case 'h':
	{
		

		break;
	}
	case SPACEBAR:
		showBounds = !showBounds;
		break;
	case ESCAPE:
		exit(EXIT_SUCCESS);
	}

	//glutPostRedisplay();
}

void myKeyboardUp(unsigned char key, int x, int y) {
	switch (key) {
	case 'w':
		footsteps->setIsPaused(true);
		break;
	case 's':
		footsteps->setIsPaused(true);
		break;
	case 'a':
		footsteps->setIsPaused(true);
		break;
	case 'd':
		footsteps->setIsPaused(true);
		break;
	}

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

	//glutPostRedisplay();
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
		firstPersonCamera.center.y += diffy * 0.05;
		thirdPersonCamera.center.y += diffy * 0.05;

		if (firstPersonCamera.center.y < -5 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1))
			firstPersonCamera.center.y = -5 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1);
		else if (firstPersonCamera.center.y > 5 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1))
			firstPersonCamera.center.y = 5 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1);
		if (thirdPersonCamera.center.y < -5 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1))
			thirdPersonCamera.center.y = -5 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1);
		else if (thirdPersonCamera.center.y > 5 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1))
			thirdPersonCamera.center.y = 5 * (player.pos.y + player.bound_height * player.scale * 1.5 + 1);

		float camera_speed = 0.5;

		Vector3f rotationCenter = player.pos + Vector3f(0, player.bound_height * player.scale * 1.5 + 1, 0);
		firstPersonCamera.rotateAroundY(-diffx * camera_speed, rotationCenter);
		thirdPersonCamera.rotateAroundY(-diffx * camera_speed, rotationCenter);

		float angle = RAD2DEG(atan2(firstPersonCamera.center.z - firstPersonCamera.eye.z, firstPersonCamera.center.x - firstPersonCamera.eye.x));
		player.rot.y = 90 - angle;
	}


	glutWarpPointer(WIDTH / 2, HEIGHT / 2);
	just_warped = true;

	//glutPostRedisplay();	//Re-draw scene 
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
		addStone();
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
	player.bound_height = 80;

	//Initialize Castle bounds
	Castle.model = model_castle;
	//Castle.model = model_house;
	Castle.bound_radius = 70;
	Castle.bound_height = -10;
	Castle.scale = 0.1;
	Castle.pos = Vector3f(0, 0.0, -16);
	Castle.rot = Vector3f(0.0f,  90.0f, 0.0f);

	Stone.model = model_stone;
	Stone.bound_radius = 4;
	Stone.bound_height = 2;
	Stone.scale = 0.08;
	Stone.pos = Vector3f(player.pos.x, 0, player.pos.z);
	Stone.rot = Vector3f(90.0f, 0.0, 0.0);




	Collidable* tree;
	// Spawn at first


	// Initialize Flow Control Variables
	showBounds = false;
	freeView = false;
	firstPerson = false;

	// Initialize Camera & Cursor Positions
	glutWarpPointer(WIDTH / 2, HEIGHT / 2);

	// First Person Camera
	firstPersonCamera.eye = player.pos + Vector3f(0.0, player.bound_height * player.scale * 2, 0.0);
	firstPersonCamera.center = player.pos + Vector3f(0.0, player.bound_height * player.scale * 2, 20);
	firstPersonCamera.up = Vector3f(0, 1, 0);

	// Third Person Camera
	thirdPersonCamera.eye = player.pos + Vector3f(-0.8, player.bound_height * player.scale * 2, -1.5);
	thirdPersonCamera.center = player.pos + Vector3f(0.0, player.bound_height * player.scale * 2, 20);
	thirdPersonCamera.up = Vector3f(0, 1, 0);

	// Free View Camera
	freeCamera.center = Vector3f(0, 0, 0);
	freeCamera.eye = Vector3f(20, 10, 20);
	freeCamera.up = Vector3f(0, 1, 0);

	ShootEnemy(1);

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
	model_castle.Load("Models/castle/castle.3DS");
	model_house.Load("Models/house/house.3DS");
	model_tree.Load("Models/tree/Tree1.3ds");
	model_player.Load("Models/player/player.3ds");
	model_skeleton.Load("Models/skeleton/skeleton.3ds");
	model_coin.Load("Models/coin/coin.3ds");
	model_stone.Load("models/rock/rock.3DS");
	model_knight.Load("models/chevalier/chevalier.3DS");

	// Loading texture files
	tex_ground.Load("Textures/ground.bmp");
	loadBMP(&day, "Textures/blu-sky-3.bmp", true);
	loadBMP(&night, "Textures/night.bmp", true);
}
void Redisplay() {

	glutPostRedisplay();
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

	glutKeyboardUpFunc(myKeyboardUp);

	glutSpecialFunc(mySpecial);

	glutMotionFunc(myMotion);

	glutPassiveMotionFunc(myPassiveMotion);

	glutMouseFunc(myMouse);

	glutSetCursor(GLUT_CURSOR_NONE);
	glutIdleFunc(Redisplay);
	glutTimerFunc(100, MoveEnemy, 0);
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
	footsteps = engine->play2D("./sounds/footstep.wav", true, false, false, ESM_AUTO_DETECT, true);
	footsteps->setIsPaused(true);

	castleFall = engine->play2D("./sounds/castleFall.wav", false, false, false, ESM_AUTO_DETECT, true);
	castleFall->setIsPaused(true);


	glutMainLoop();
}