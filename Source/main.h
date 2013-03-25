#include "Utility.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "3DDeformable.h"
#include "Cube.h"
#include "Cylinder.h"
#include "RigidCube.h"
#include "Sphere.h"
#include "MillipedeHeadTail.h"
#include "Millipede.h"
#include "HeightFieldWater.h"
#include "Terrain.h"
#include "World.h"
#include "Ball.h"
#include "Timer.h"
#include "Drawer.h"

bool FOOD_ON=true;
bool FOOD_OFF=false;
bool OBSTACLE_ON=true;
bool OBSTACLE_OFF=false;

//Global Variables
int Window_Width = 1600;
int Window_Height = 1080;

int Button = -1 ;
int PrevY = 0 ;


const int EscKey               = 27;
const int SpaceKey             = 32;

Camera  Pentax;
Light Lumia;
World * myWorld = NULL;
Terrain* myTerrain1 = NULL;
Terrain* myTerrain2 = NULL;
Terrain* myTerrain3 = NULL;
Millipede* myMillipedes;
Drawer* g_Drawer;
Sphere* mySphere = NULL;
RigidCube* myRigidCube;
Deformable3D* myJello;
std::ofstream*  TerrainOutput;
std::ofstream*  BugOutputPov;
std::ofstream*  BugOutputGraph;
std::ofstream*  BugOutputMaya;
std::ofstream*  WaterOutput;
std::ofstream*  RideOutput;

int STOP = -1;//stop the animation
int CONTROL = -1;//control the millipede
int OUTPUT = -1;//control output
int PICK = -1;//pick mode or not
int DRAW_TYPE = DRAW_PHONG;

TerrainType myTerrainType;
Eigen::Vector3f START_POSITION;
Timer TM ;
double TIME;
double TIME_LAST;
double DTIME;
double SIM_TIME = 0;
double FRAME_TIME = 0;
int FRAME_COUNT = 0;
GLfloat CursorX;
GLfloat CursorY;//Curosr Position

//declaration of function

void initScene();
void reinitScene();
void initRideScene1();
void initRideScene2();
void drawScene();
void keyboardCallback(unsigned char key, int x, int y);
void specialCallback(int key, int x, int y);
void displayCallback();
void reshapeCallback(int w, int h);
void cursorCallback(int x, int y);
void cursorCallback(int x, int y);
void mouseCallback(int button, int state, int x, int y);
void idleCallback();

void OUTPUT_ONE_FRAME();