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
	FRAME_COUNT = 0;
    std::cout<<"Setting up the World..."<<std::endl;

	myTerrain = new Terrain(Eigen::Vector2f(500,500), Eigen::Vector2i(200,200), 500, TERRAIN_WATER
		, OBSTACLE_OFF, FOOD_OFF);

	myOutputFile = new std::ofstream;
	//myOutputFile->open("millipede.mel");
	myOutputFile->open("water.inc");
	/*
	(*myOutputFile)<<"string $path = \"D:/TEMP/\";"<<std::endl<<
	"string $filelist[] = `getFileList -folder $path -filespec \"*.obj\"`;"<<std::endl<<
	"for($i=0; $i <= (`size $filelist` - 1); $i++)"<<std::endl
		<<"sysFile -delete ($path+$filelist[$i]);"<<std::endl<<std::endl;
	*/
	reinitScene();

	std::cout<<"Starting Animation..."<<std::endl;


}

void reinitScene(){

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
	myMillipedes->Init(Eigen::Vector3f(10,3,0),18,Eigen::Vector3f(0.2,1.0,2.422),0.807895, myTerrain);
	//myMillipedes->FixHead();
	//myMillipedes->FixTail();
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
		myOutputFile->close();
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

void idleCallback(){
    
	TIME = TM.GetElapsedTime() ;
        /*
	DTIME = TIME - TIME_LAST;
	TIME_LAST = TIME;

	FRAME_TIME += DTIME;
	*/
	if(STOP == -1){
	//only update physics
		myWorld->Update(0.02);//The real physics time step is much smaller

		glutPostRedisplay() ; //draw new frame, the display is not real physics time

		if(OUTPUT == 1){
			OUTPUT_ONE_FRAME();//output one frame data
		}
	}

}

void OUTPUT_ONE_FRAME(){

	FRAME_COUNT++;
	(*myOutputFile)<<"//Frame "<<FRAME_COUNT<<std::endl;
	//Terrain
	myTerrain->Output2File(myOutputFile);
	OUTPUT *= -1;//output only one frame
	//millipede
/*	

	(*myOutputFile)<<"currentTime "<<FRAME_COUNT<<" ;"<<std::endl;
	myMillipedes[0].Output2File(myOutputFile);
	(*myOutputFile)<<"//set one key frame"<<std::endl;
	(*myOutputFile)<<"select -r "
		"s1 s1l1 s1l2 s1r1 s1r2 s2 s2l1 s2l2 s2r1 s2r2 s3 s3l1 s3l2 s3r1 s3r2 s4 s4l1 s4l2 s4r1 s4r2 "
		"s5 s5l1 s5l2 s5r1 s5r2 s6 s6l1 s6l2 s6r1 s6r2 s7 s7l1 s7l2 s7r1 s7r2 s8 s8l1 s8l2 s8r1 s8r2 "
		"s9 s9l1 s9l2 s9r1 s9r2 s10 s10l1 s10l2 s10r1 s10r2 s11 s11l1 s11l2 s11r1 s11r2 s12 s12l1 s12l2 s12r1 s12r2 "
		"s13 s13l1 s13l2 s13r1 s13r2 s14 s14l1 s14l2 s14r1 s14r2 s15 s15l1 s15l2 s15r1 s15r2 s16 s16l1 s16l2 s16r1 s16r2 "
		"s17 s17l1 s17l2 s17r1 s17r2 s18 s18l1 s18l2 s18r1 s18r2 s19 s19l1 s19l2 s19r1 s19r2 head headl headr mouthl mouthr tail;"<<std::endl;

	(*myOutputFile)<<"setKeyframe -breakdown 0 -hierarchy none -controlPoints 0 -shape 0 {"
		"\"s1\",\"s1l1\",\"s1l2\",\"s1r1\",\"s1r2\",\"s2\",\"s2l1\",\"s2l2\",\"s2r1\",\"s2r2\",\"s3\",\"s3l1\",\"s3l2\",\"s3r1\",\"s3r2\",\"s4\",\"s4l1\",\"s4l2\",\"s4r1\",\"s4r2\","
		"\"s5\",\"s5l1\",\"s5l2\",\"s5r1\",\"s5r2\",\"s6\",\"s6l1\",\"s6l2\",\"s6r1\",\"s6r2\",\"s7\",\"s7l1\",\"s7l2\",\"s7r1\",\"s7r2\",\"s8\",\"s8l1\",\"s8l2\",\"s8r1\",\"s8r2\","
		"\"s9\",\"s9l1\",\"s9l2\",\"s9r1\",\"s9r2\",\"s10\",\"s10l1\",\"s10l2\",\"s10r1\",\"s10r2\",\"s11\",\"s11l1\",\"s11l2\",\"s11r1\",\"s11r2\",\"s12\",\"s12l1\",\"s12l2\",\"s12r1\",\"s12r2\","
		"\"s13\",\"s13l1\",\"s13l2\",\"s13r1\",\"s13r2\",\"s14\",\"s14l1\",\"s14l2\",\"s14r1\",\"s14r2\",\"s15\",\"s15l1\",\"s15l2\",\"s15r1\",\"s15r2\",\"s16\",\"s16l1\",\"s16l2\",\"s16r1\",\"s16r2\","
		"\"s17\",\"s17l1\",\"s17l2\",\"s17r1\",\"s17r2\",\"s18\",\"s18l1\",\"s18l2\",\"s18r1\",\"s18r2\",\"s19\",\"s19l1\",\"s19l2\",\"s19r1\",\"s19r2\",\"head\",\"headl\",\"headr\",\"mouthl\",\"mouthr\",\"tail\"};"<<std::endl;
	/*
	(*myOutputFile)<<"//save to obj"<<std::endl;
	(*myOutputFile)<<"file -force -options \"groups=1;ptgroups=1;materials=1;smoothing=1;normals=1\" -type \"OBJexport\" -pr" 
		"-ea \"D:/TEMP/MillipedeFrame"<<FRAME_COUNT<<".obj\";"<<std::endl;
		*/
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


