//
//  main.cpp
//  Millipede
//
//  Created by Jingyi Fang on 2/10/11.
//  Copyright 2011 Jingyi Fang. All rights reserved.
//

#include "main.h"

void initScene(){

	std::cout<<"Initiallizing The System...."<<std::endl;

	std::cout<<"Setting up Light, Camera and Clock...."<<std::endl;
    //set up the camera
    Pentax.Init(Eigen::Vector4f(0,250,250.0,1.0),Eigen::Vector4f(0,0,0.0,1.0), Eigen::Vector4f(0,1.0,0,0), 
		60, Window_Width/Window_Height , 1.0, 1000);
    
    //set up the light
    Lumia.m_position = Pentax.m_position;
    Lumia.m_color = Eigen::Vector4f(1.0,1.0,1.0,1.0);//white light

	//set up the world
	myWorld = new World(50000);

	//set up the drawer
	myDrawer = new Drawer;
	myDrawer->PushMatrix();

    std::cout<<"Setting up the World..."<<std::endl;

	myTerrain = new Terrain(Eigen::Vector2f(500,500), Eigen::Vector2i(200,200), 15, TERRAIN_RANDOM, OBSTACLE_OFF, FOOD_OFF);

	reinitScene();

	std::cout<<"Starting Animation..."<<std::endl;


}

void reinitScene(){

	myWorld->Clear();
	
	myWorld->Add_Object(myTerrain);
	
	int m = 5, n = 5;
	myMillipedes = new Millipede[m*n];
	for(int i = 0; i < m; i++)
		for(int j = 0; j < n; j++)
		{
			myMillipedes[i*n +j].Init(Eigen::Vector3f(30*i,15,-20*j),6,Eigen::Vector3f(1,1,2),1, myTerrain);
			myWorld->Add_Object(&myMillipedes[i*n +j]);
		}


	//set up the clock
	TIME_LAST = TM.GetElapsedTime() ;
	DTIME = 0.0;
	FRAME_TIME = 0.0;

}

void drawScene(){
    
    glEnable( GL_DEPTH_TEST );
    glClearColor(0.0, 0.0, 0.0, 0.0);//Black background

	Pentax.Update();

    Lumia.m_position = Pentax.m_position;//the light is attached to the camera
	
	myWorld->Draw(DRAW_TYPE, Pentax, Lumia);

}

void specialCallback(int key, int x, int y){

	if(CONTROL == -1)
		return;

	switch(key)
	{
		case GLUT_KEY_UP:
		myMillipedes[0].ReceiveControllKey(0);
		break;	
		case GLUT_KEY_DOWN:
		myMillipedes[0].ReceiveControllKey(1);
		break;
		case GLUT_KEY_LEFT:
		myMillipedes[0].ReceiveControllKey(2);
		break;
		case GLUT_KEY_RIGHT:
		myMillipedes[0].ReceiveControllKey(3);
		break;
	}
	
		
}

void keyboardCallback(unsigned char key, int x, int y){

	if(CONTROL == 1){
		switch(key)
		{
			case '7'://down
			myMillipedes[0].ReceiveControllKey(1);
			break;
			case '8'://left
			myMillipedes[0].ReceiveControllKey(2);
			break;
			case '9'://right
			myMillipedes[0].ReceiveControllKey(3);
			break;
		}
	}
    
	if ( key == EscKey || key == 'q' || key == 'Q' ) 
    {
        exit(0);
    }
    if( key == 's'|| key == 'S')
    {
        STOP *= -1; 
    }
	if( key == 'c'|| key == 'C')
	{
		CONTROL *= -1;
		myMillipedes[0].SetControl(CONTROL == 1?true:false);
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
        reinitScene();
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

	if(DTIME > 1/1000.0)
		DTIME = 1/1000.0;
	
	DTIME = 1/2000.0;//fixed dt

	if(STOP == -1){
	//only update physics
		myWorld->Update(DTIME);
	}

	if(FRAME_TIME > 0.03)//33 frames per second
	{
		glutPostRedisplay() ; //draw new frame
		FRAME_TIME = 0;	
		FRAME_COUNT++;
		//OUTPUT_ONE_FRAME();
	}
	


	//printf("Physics Rate %f\r", 1.0/DTIME) ;
}

void OUTPUT_ONE_FRAME(){

	myOutputFile = new std::ofstream;
	std::string filename;
	filename = "Output/FRAME_"+ std::to_string(FRAME_COUNT) + ".txt";
	myOutputFile->open(filename);

	//Terrain


	//millipede
	myMillipedes[0].Output2File(myOutputFile);

	myOutputFile->close();
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
	glutSpecialFunc(specialCallback);
    glutMouseFunc(mouseCallback) ;
    glutMotionFunc(motionCallback);
	glutPassiveMotionFunc(cursorCallback);
    glutDisplayFunc(displayCallback);
	glewInit();
    initScene();
    glutMainLoop();
    
    return 0;
}

