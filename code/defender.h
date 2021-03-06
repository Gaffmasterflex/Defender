//defender.h

// ===============================================
// @file   defender.h
// @author Dean Gaffney
// @student 20067423
// @practical defender
// @brief  Header file for PSP+GLFW(OpenGL) defender game
// ===============================================

#ifndef DEFENDER_H
#define DEFENDER_H

// uncomment next line for PSP executable
// #define PSP 1

// ===============================================
// Headers
// ===============================================
#ifdef PSP								// system specific headers
#else 									  
	#ifdef __APPLE__   						
		#include <GLFW/glfw3.h> 
	#else          						
		#include <GL/glfw.h> 
	#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "Vector2f.h"
#include "Pool.h"

// ===============================================
// Global identifiers
// ===============================================

// setup screen (on both the PSP and OpenGL) with 
// coordinates (0,0) on lower left to (ASPECT,1) on upper right

#ifdef PSP								// PSP specific headers
	const int WINDOW_WIDTH = SCEGU_SCR_WIDTH;  // 480
	const int WINDOW_HEIGHT = SCEGU_SCR_HEIGHT; // 272
#else
	const float SCALE = 2;
	const int WINDOW_WIDTH = (int)(480*SCALE);
	const int WINDOW_HEIGHT = (int)(272*SCALE);
	const float ASPECT = float(WINDOW_WIDTH)/float(WINDOW_HEIGHT);
	const float RADAR_HEIGHT = 0.1;
	const float RADAR_WIDTH = ASPECT * 0.8;		//80% of screen
	const float HEALTH_BAR_WIDTH = 0.5;
	const float RADAR_X = (ASPECT - RADAR_WIDTH) / 2;
	const float RADAR_Y = 1.0 - RADAR_HEIGHT;
#endif

// state for main game loop
enum GameState {GAME_INTRO, GAME_START, LEVEL_START, LEVEL_PLAY, LEVEL_OVER, GAME_OVER, GAME_QUIT};
extern GameState gameState;

using namespace std;

//list of levels
extern string levels [];
extern int currentlevel;
extern const int NUMBER_OF_LEVELS;
// Basic games object wth state, position and velocity
class Entity {
public:
	enum State {ASLEEP, AWAKE, DEAD};
	
	State state;
	Vector2f position;
	Vector2f velocity;
	Vector2f maxPoints;		//maxPoints holds the max X and Y for the rectangle hit box of entities.
	Vector2f minPoints;		//minPoints holds the min X and Y for the rectangle hit box of entities.
	unsigned int frame;		//holds animation frame for enemies
	virtual void render () const = 0;

	virtual void update (float dt) = 0;

	virtual std::string toString() const = 0;

	friend std::ostream& operator <<(std::ostream& outputStream, const Entity& entity) {
		outputStream <<entity.toString();
		return outputStream;
	}		
	
	//make vectors for bounding box
	
	//
};


class Ship : public Entity {
	
public:	
	int health;
	int lives;
	double damageRecoveryTime;
	enum DamageState {RECOVERING,RECOVERED};
	DamageState damageState;
	Ship() {
		state = AWAKE;
		position = Vector2f(0.02f, 0.5f);
		velocity = Vector2f(0,0);
		health = 100;
		lives = 3;
		damageState = RECOVERED;
		damageRecoveryTime = 0.0;
	}

	void render () const;
	
	void update (float dt);

	void takeDamage(const int amount){
		if(damageRecoveryTime > 0)return;
		health -= amount;
		damageRecoveryTime = 0.2;
		damageState = RECOVERING;
	}

	void removeLife(){
		lives--;
		health = 100;
	}
	
	void resetLives(){
		lives = 3;
		health = 100;
	}
	
	std::string toString() const {
		std::ostringstream outs;
		outs <<"Position = " <<position <<"\t Velocity = " <<velocity;
		return outs.str();
	};
	
};
extern Ship ship;


class Enemy : public Entity {
	
public:	
	enum Type {SIMPLE, TRACKING};

	Type type;
	Vector2f center;
	float radius;
	
	Enemy () {
		type = SIMPLE;
		state = ASLEEP;
		position = Vector2f::ZERO;
		velocity = Vector2f(0,-0.1);
	} 
	
	void render () const;
	
	void update (float dt);
	
	std::string toString() const {
		std::ostringstream outs;
		outs <<"Enemy Type =" <<type <<"\t Position = " <<position <<"\t Velocity = " <<velocity;
		return outs.str();
	}
};


class Bullet : public Entity {
public:
	Bullet() { 
		state = AWAKE;
		position = Vector2f(0,0);
		velocity = Vector2f(0,0);
	}

	void render () const;
	
	void update (float dt);

	void reset(){
		position = Vector2f::ZERO;
		velocity = Vector2f::ZERO;
		state = AWAKE;
	}

	std::string toString() const {
		std::ostringstream outs;
		outs <<"Bullet " <<"\t Position = " <<position <<"\t Velocity = " <<velocity;
		return outs.str();
	}
};
typedef Pool<Bullet> BulletPool;
extern BulletPool shipBullets;
extern BulletPool enemyBullets;


class Bomb : public Entity {
	
public:
	float angle;
	float initalVelocity;
	Bomb() {
		angle = 45;
		initalVelocity = 2;
		state = AWAKE;
		position = Vector2f(0,0);
		velocity = Vector2f(initalVelocity*cos(angle*PI/180),initalVelocity*sin(angle*PI/180));
	}

	void reset(){
		angle = 45;
		initalVelocity = 2;
		state = AWAKE;
		position = Vector2f::ZERO;
		velocity = Vector2f(initalVelocity*cos(angle*PI/180),initalVelocity*sin(angle*PI/180));
	}

	void render () const;
	
	void update (float dt);

	std::string toString() const {
		std::ostringstream outs;
		outs <<"Bomb " <<"\t Position = " <<position <<"\t Velocity = " <<velocity;
		return outs.str();
	}
	
};
typedef Pool<Bomb> BombPool;
extern BombPool shipBombs;


#include "Level.h"

// frame timing information
extern double startTime, currentTime, previousTime, dt;
extern double previousBulletTime, previousBombTime, enemyFireTime;

extern bool fire, fireBomb;
extern float gravity;
// ===============================================
// Function prototypes
// ===============================================
int initGraphics();
int deinitGraphics();
void renderRadar();
void drawRectangle(Vector2f position, Vector2f size);
void drawRectangle(Vector2f position, float width, float height);
void drawElipse(Vector2f position, float width, float height);
void drawElipse(Vector2f position, Vector2f size);
void renderHealthBar();
void render();
void update();
void getInput();
void checkCollisions();
bool checkPointCollisionWithLevel(const Vector2f point, bool ceilingCheck);
float distancePointFromLevel(const Vector2f point, bool ceilingCheck);
bool isPointInsideRectangle(Vector2f point);
bool isPointInsideCircle(Vector2f point,Vector2f center, float radius);
void cullObjects();
bool isInScreen(Entity &gameObject);
bool isPointInsideRectangle(Vector2f point,Vector2f maxPoint, Vector2f minPoint);
#endif
