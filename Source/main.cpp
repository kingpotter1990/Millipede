//
//  main.cpp
//  FEMCloth2D
//
//  Created by Jingyi Fang on 2/10/11.
//  Copyright 2011 Jingyi Fang. All rights reserved.
//

//TODO:
//1. make FEM Cloth 3D
//4. Collision detection and resolution for self-collision 
//5. Rapture of FEMCloth

#include "main.h"

void initScene(){

	std::cout<<"Initiallizing The System...."<<std::endl;

	std::cout<<"Setting up Light, Camera and Clock...."<<std::endl;
    //set up the camera
    Pentax.Init(Eigen::Vector4f(0,0,25.0,1.0),Eigen::Vector4f(0,0,0.0,1.0), Eigen::Vector4f(0,1.0,0,0), 
		60, Window_Width/Window_Height , 1.0, 1000);
    
    //set up the light
    Lumia.m_position = Pentax.m_position;
    Lumia.m_color = Eigen::Vector4f(1.0,1.0,1.0,1.0);//white light

	//set up the world
	myWorld = new World(50000);
	myWorld->Clear();

	//set up the drawer
	myDrawer = new Drawer;
	myDrawer->PushMatrix();
	//set up the clock
	TIME_LAST = TM.GetElapsedTime() ;
	DTIME = 0.0;
	FRAME_TIME = 0.0;

    std::cout<<"Setting up the World..."<<std::endl;

	Millipede* myMillipede = new Millipede;
	myMillipede->Init(Eigen::Vector3f(-10,6,0),10,Eigen::Vector3f(1,1,2),1);
	
	RigidCube* myRigidCube = new RigidCube;
	myRigidCube->Init(1.0,Eigen::Vector3f(0,5,0),Eigen::Vector3f(5,5,5),Eigen::Vector3f(1,0,0));;

	Cube* myGround = new Cube;
	myGround->Init(Eigen::Vector3f(0,-5,0),Eigen::Vector3f(100,10,100), Eigen::Vector3f(0.5,0.5,0.5));

	//myMillipede->m_head->m_fixed = true;
	//myMillipede->m_tail->m_fixed = true;

//TODO: millipede sense ground not by y = 0, but from map
    
	myWorld->Add_Object(myMillipede);
	//myWorld->Add_Object(myRigidCube);
	myWorld->Add_Object(myGround);

	std::cout<<"Starting Animation..."<<std::endl;
}

void drawScene(){
    
    glEnable( GL_DEPTH_TEST );
    glClearColor(0.0, 0.0, 0.0, 0.0);//Black background

	Pentax.Update();

    Lumia.m_position = Pentax.m_position;//the light is attached to the camera
	
	myWorld->Update(DTIME);
	myWorld->Draw(DRAW_TYPE, Pentax, Lumia);

}

void keyboardCallback(unsigned char key, int x, int y){
    
    if ( key == EscKey || key == 'q' || key == 'Q' ) 
    {
        exit(0);
    }
    if( key == 's'|| key == 'S')
    {
        STOP *= -1; 
    }
	if( key == 'p'|| key == 'P' )
	{
		PICK *= -1;
		if(PICK == 1)
			std::cout<<"Picking Mode"<<std::endl;
		if(PICK == -1)
			std::cout<<"Rotating Mode"<<std::endl;
	}
    
    if( key == '1')
        DRAW_TYPE = DRAW_MESH;
    if( key == '2')
        DRAW_TYPE = DRAW_PHONG;
    if( key == '3')
        DRAW_TYPE = DRAW_TEXTURE;

        
    //reset the scene and camera
    if ( key == SpaceKey) {
        initScene();
        glutSwapBuffers();
    }
}

void displayCallback(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    drawScene();
    glutSwapBuffers();

}

void reshapeCallback(int w, int h){
    Window_Width = w;
    Window_Height = h;

    glViewport(0, 0, w, h);
	Pentax.m_aspect = (float)w/(float)h;
	glutPostRedisplay() ;

}

void motionCallback(int x, int y){
     
    if( Button == GLUT_LEFT_BUTTON )
    {
		CursorX = double(2*x-Window_Width)/(Window_Width);
		CursorY = double(Window_Height-2*y)/(Window_Height);
        
		Pentax.MouseDrag(CursorX, CursorY);
		
        glutPostRedisplay() ;
    }
    else if( Button == GLUT_RIGHT_BUTTON )
    {
        if( y - PrevY > 0 )
            Pentax.m_zoom  = Pentax.m_zoom  * 1.03 ;
        else 
            Pentax.m_zoom   = Pentax.m_zoom  * 0.97 ;
        PrevY = y ;
        glutPostRedisplay() ;
    }
}

void mouseCallback(int button, int state, int x, int y){

	CursorX = double(2*x-Window_Width)/(Window_Width);
	CursorY = double(Window_Height-2*y)/(Window_Height);

	Button = button;
	if( Button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
	{
		if(PICK == -1){
			Pentax.MouseLeftDown(CursorX,CursorY);
		}

	}
	if( Button == GLUT_LEFT_BUTTON && state == GLUT_UP )
	{
		if(PICK == -1){
			Pentax.MouseLeftUp(CursorX,CursorY);
			Button = -1 ;
		}
 
		
	}
	if( button == GLUT_RIGHT_BUTTON && state == GLUT_UP )
	{
		if(PICK == 1){
			//World.Pick(x,y);
		}
	}
    
    if( Button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
        PrevY = y ;
    }
	glutPostRedisplay() ;
}

void cursorCallback(int x, int y){

	CursorX = double(2*x-Window_Width)/(Window_Width);
	CursorY = double(Window_Height-2*y)/(Window_Height);
}

void idleCallback(){
    
	TIME = TM.GetElapsedTime() ;
        
	DTIME = TIME - TIME_LAST;
	TIME_LAST = TIME;
	FRAME_TIME += DTIME;

	if(DTIME > 1/2000.0)
		DTIME = 1/2000.0;
	if(STOP == -1){
	//only update physics
		myWorld->Update(DTIME);
	}

	if(FRAME_TIME > 0.02)//50 frames per second
	{
		glutPostRedisplay() ; //draw new frame
		FRAME_TIME = 0;
	}
	
	//printf("Physics Rate %f\r", 1.0/DTIME) ;
}

int main (int argc, char ** argv){
    // init GLUT
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	glutInitWindowSize(Window_Width,Window_Height);
    glutCreateWindow("Rigid-Deformable Coupling - Franklin Fang");
    // init GLUT callbacks
    glutIdleFunc(idleCallback);
	glutReshapeFunc (reshapeCallback);
    glutKeyboardFunc(keyboardCallback);
    glutMouseFunc(mouseCallback) ;
    glutMotionFunc(motionCallback);
	glutPassiveMotionFunc(cursorCallback);
    glutDisplayFunc(displayCallback);
#ifdef _WIN32
	glewInit();
#endif
    initScene();
    glutMainLoop();
    
    return 0;
}

