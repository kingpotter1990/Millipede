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

	//set up the drawer
	g_Drawer = new Drawer;
	g_Drawer->SetIdentity();

	//set up the world
	myWorld = new World(51000);
    std::cout<<"Setting up the World..."<<std::endl;
	myTerrainType = TERRAIN_TEST;
	myTerrain = new Terrain(Eigen::Vector2f(900,900), Eigen::Vector2i(20,20), 50, myTerrainType
		, OBSTACLE_ON, FOOD_ON);

	TerrainOutput = new std::ofstream;
	BugOutputPov = new std::ofstream;
	BugOutputMaya = new std::ofstream;
	WaterOutput = new std::ofstream;
	//output static terrain mesh
	TerrainOutput->open("Terrain.inc");
	myTerrain->Output2File(TerrainOutput);
	TerrainOutput->close();

	BugOutputMaya->open("Bug.mel");
	WaterOutput->open("Water.inc");
	
	(*BugOutputMaya)<<"string $path = \"D:/TEMP/\";"<<std::endl<<
	"string $filelist[] = `getFileList -folder $path -filespec \"*.obj\"`;"<<std::endl<<
	"for($i=0; $i <= (`size $filelist` - 1); $i++)"<<std::endl
		<<"sysFile -delete ($path+$filelist[$i]);"<<std::endl<<std::endl;

	reinitScene();

	std::cout<<"Starting Animation..."<<std::endl;


}

void reinitScene(){

	FRAME_COUNT = 0;
	SIM_TIME = 0.0;

	//press SpaceBar to trigger

	myWorld->Clear();

	myWorld->Add_Object(myTerrain);
	myTerrain->ClearMillipedes();	

	if(myMillipedes)
		delete[] myMillipedes;
/*
	int m = 4, n = 4;
	myMillipedes = new Millipede[m*n];
	for(int i = 0; i < m; i++)
		for(int j = 0; j < n; j++)
		{
			myMillipedes[i*n +j].Init(Eigen::Vector3f(30*i,15,-20*j),6,Eigen::Vector3f(1,1,2),1, myTerrain);
			myWorld->Add_Object(&myMillipedes[i*n + j]);
		}
*/
	myMillipedes = new Millipede;
	START_POSITION = Eigen::Vector3f(40,6,0);
	myMillipedes->Init(START_POSITION, 18,Eigen::Vector3f(0.3,1.39,2.422),0.707895, myTerrain);
	//myMillipedes->Init(START_POSITION, 12,Eigen::Vector3f(0.8,1.0,2.0),0.8, myTerrain);
	myMillipedes->FixHead();
	myMillipedes->FixTail();
	myWorld->Add_Object(myMillipedes);
	
	//mySphere= new Sphere(Eigen::Vector3f(0,15,0), Eigen::Vector3f(3,3,3), Eigen::Vector3f(1,0,1));
	//myWorld->Add_Object(mySphere);
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
			case '7'://forward
			//myMillipedes[0].ReceiveControllKey(1);
			break;
			case '8'://left
			//myMillipedes[0].ReceiveControllKey(2);
			break;
			/*case '9'://right
			//myMillipedes[0].ReceiveControllKey(3);
			break;
			*/
		}
	}
    
	if ( key == EscKey || key == 'q' || key == 'Q' ) 
    {
		(*BugOutputMaya)<<"playbackOptions -min 1 -max "<< FRAME_COUNT - 1<<";"<<std::endl;
		BugOutputMaya->close();
        exit(0);
    }
    if( key == 'o'|| key == 'O')
    {
		OUTPUT *= -1;
    }
    if( key == 's'|| key == 'S')
    {
        STOP *= -1; 
    }
	if( key == 'h' || key == 'H'){
		myMillipedes->ReleaseHead();
	}
	if( key == 't' || key == 'T'){
		myMillipedes->ReleaseTail();
	}
	if( key == 'c'|| key == 'C')
	{
		CONTROL *= -1;
		//myMillipedes[0].SetControl(CONTROL == 1?true:false);
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

void HackAnimation(double dt){
	if(myMillipedes == NULL)
		return;
	if(myMillipedes->IsTailFixed()){
		//sway the tail, for demonstration of physics
		if(SIM_TIME > 2.0 && SIM_TIME < 4.0)
			myMillipedes->m_tail->m_Center[2] =  START_POSITION[2] + 2*sin(180*SIM_TIME*DegreesToRadians); 	

		if(SIM_TIME > 6.0 && SIM_TIME < 8.0)
			myMillipedes->m_tail->m_Center[1] =  START_POSITION[1] + 2*sin(180*SIM_TIME*DegreesToRadians); 	
	}

}
void idleCallback(){
    
	TIME = TM.GetElapsedTime() ;
        /*
	DTIME = TIME - TIME_LAST;
	TIME_LAST = TIME;

	FRAME_TIME += DTIME;
	*/
	if(STOP == -1){
	//only update physics
		SIM_TIME += 0.02;
		HackAnimation(0.02);
		myWorld->Update(0.02);//The real physics time step is much smaller

		glutPostRedisplay() ; //draw new frame, the display is not real physics time

		if(OUTPUT == 1){
			OUTPUT_ONE_FRAME();//output one frame data
		}
	}

}

void OUTPUT_ONE_FRAME(){
	//Water
	if(myTerrainType == TERRAIN_WATER){
		//millipede
		std::string filename = "FRAME_";
		filename += std::to_string(FRAME_COUNT);
		filename += "Water.inc";
		(*WaterOutput)<<"//Frame "<<FRAME_COUNT<<std::endl;
		myTerrain->m_water->Output2File(WaterOutput);
		WaterOutput->close();
	}
	//millipede
	//Physics Model for Pov
	/*
	std::string filename = "FRAME_";
	filename += std::to_string(FRAME_COUNT);
	filename += "Bug.inc";
	BugOutputPov->open(filename);
	(*BugOutputPov)<<"//Frame "<<FRAME_COUNT<<std::endl;
	myMillipedes[0].Output2File(BugOutputPov,1);//0 is for maya model, 1 is for physics
	BugOutputPov->close();
	*/
	//mel script file
	(*BugOutputMaya)<<"currentTime "<<FRAME_COUNT<<";"<<std::endl;
	myMillipedes[0].Output2File(BugOutputMaya,0);//0 is for maya model, 1 is for physics
	(*BugOutputMaya) <<"//End Frame"<<FRAME_COUNT<<";"<<std::endl;
	FRAME_COUNT++;
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


