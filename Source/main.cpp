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
	myTerrainType = TERRAIN_FLAT;
	myTerrain = new Terrain(Eigen::Vector2f(900,900), Eigen::Vector2i(20,20), 50, myTerrainType
		, OBSTACLE_OFF, FOOD_ON);

	TerrainOutput = new std::ofstream;
	BugOutputPov = new std::ofstream;
	BugOutputGraph = new std::ofstream;
	BugOutputMaya = new std::ofstream;
	WaterOutput = new std::ofstream;
	RideOutput = new std::ofstream;
	//output static terrain mesh
	TerrainOutput->open("Terrain.inc");
	myTerrain->Output2File(TerrainOutput);
	TerrainOutput->close();

	BugOutputMaya->open("Bug.mel");
	RideOutput->open("Ride.inc");
	
	(*BugOutputMaya)<<"string $path = \"D:/TEMP/\";"<<std::endl<<
	"string $filelist[] = `getFileList -folder $path -filespec \"*.obj\"`;"<<std::endl<<
	"for($i=0; $i <= (`size $filelist` - 1); $i++)"<<std::endl
		<<"sysFile -delete ($path+$filelist[$i]);"<<std::endl<<std::endl;

	reinitScene();

	//initRideScene1();

	std::cout<<"Starting Animation..."<<std::endl;


}

void initRideScene1(){
	
	FRAME_COUNT = 0;
	SIM_TIME = 0.0;

	//press SpaceBar to trigger

	myWorld->Clear();
	myWorld->Add_Object(myTerrain);

	myJello = new Deformable3D[6];
	myRigidCube= new RigidCube[7];

	myJello[0].Init(Eigen::Vector3i(10,10,10),1,1000,0.4,20,Eigen::Vector3f(-2,15,-2),Eigen::Vector3f(4.0,2.0,4.0), Eigen::Vector3f(1.0,1,0));
	myJello[1].Init(Eigen::Vector3i(10,10,10),1,1000,0.4,20,Eigen::Vector3f(-1.5,11,-1.5),Eigen::Vector3f(3.0,2.0,3.0), Eigen::Vector3f(1.0,0,1));
	myJello[2].Init(Eigen::Vector3i(10,10,10),1,1000,0.4,20,Eigen::Vector3f(-1,7,-1),Eigen::Vector3f(2.0,2.0,2.0), Eigen::Vector3f(1.0,0,0));
	myJello[3].Init(Eigen::Vector3i(10,10,10),1,1000,0.4,20,Eigen::Vector3f(-1,27,-1),Eigen::Vector3f(2.0,2.0,2.0), Eigen::Vector3f(1.0,0,0));
	myJello[4].Init(Eigen::Vector3i(10,10,10),1,1000,0.4,20,Eigen::Vector3f(-1.5,23,-1.5),Eigen::Vector3f(3.0,2.0,3.0), Eigen::Vector3f(1.0,0,1));
	myJello[5].Init(Eigen::Vector3i(10,10,10),1,1000,0.4,20,Eigen::Vector3f(-2,19,-2),Eigen::Vector3f(4.0,2.0,4.0), Eigen::Vector3f(1.0,1,0));
    
	myRigidCube[0].Init(1, Eigen::Vector3f(0,18,0),Eigen::Vector3f(5.0,2.0,5.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[1].Init(1, Eigen::Vector3f(0,14,0),Eigen::Vector3f(4.0,2.0,4.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[2].Init(1, Eigen::Vector3f(0,10,0),Eigen::Vector3f(3.0,2.0,3.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[3].Init(1, Eigen::Vector3f(0,6,0),Eigen::Vector3f(2.0,2.0,2.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[4].Init(1, Eigen::Vector3f(0,26,0),Eigen::Vector3f(3.0,2.0,3.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[5].Init(1, Eigen::Vector3f(0,22,0),Eigen::Vector3f(4.0,2.0,4.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[6].Init(1, Eigen::Vector3f(0,30,0),Eigen::Vector3f(2.0,2.0,2.0),Eigen::Vector3f(0.0,1.0,0.0));
	//manuly couple up the deformable and rigid parts
        myRigidCube[0].AttachNodes(myJello[0].m_Mesh->GetTopNodes());
        myRigidCube[1].AttachNodes(myJello[0].m_Mesh->GetBottomNodes());
        myRigidCube[1].AttachNodes(myJello[1].m_Mesh->GetTopNodes());
        myRigidCube[2].AttachNodes(myJello[1].m_Mesh->GetBottomNodes());
        myRigidCube[2].AttachNodes(myJello[2].m_Mesh->GetTopNodes());
        myRigidCube[3].AttachNodes(myJello[2].m_Mesh->GetBottomNodes());

        myRigidCube[4].AttachNodes(myJello[3].m_Mesh->GetBottomNodes());
        myRigidCube[4].AttachNodes(myJello[4].m_Mesh->GetTopNodes());
        myRigidCube[5].AttachNodes(myJello[4].m_Mesh->GetBottomNodes());
        myRigidCube[5].AttachNodes(myJello[5].m_Mesh->GetTopNodes());
		myRigidCube[0].AttachNodes(myJello[5].m_Mesh->GetBottomNodes());
		myRigidCube[6].AttachNodes(myJello[3].m_Mesh->GetTopNodes());

    //set up the World
        myWorld->Add_Object(&myJello[0]);
        myWorld->Add_Object(&myJello[1]);
        myWorld->Add_Object(&myJello[2]);
        myWorld->Add_Object(&myJello[3]);
        myWorld->Add_Object(&myJello[4]);
        myWorld->Add_Object(&myJello[5]);
        myWorld->Add_Object(&myRigidCube[0]);
	myWorld->Add_Object(&myRigidCube[1]);
	myWorld->Add_Object(&myRigidCube[2]);
	myWorld->Add_Object(&myRigidCube[3]);
	myWorld->Add_Object(&myRigidCube[4]);
	myWorld->Add_Object(&myRigidCube[5]);
	myWorld->Add_Object(&myRigidCube[6]);

	myRigidCube[6].m_velocity = Eigen::Vector3f(1,0,0);
	TIME_LAST = TM.GetElapsedTime() ;
	DTIME = 0.0;
	FRAME_TIME = 0.0;


}

void initRideScene2(){
	
	FRAME_COUNT = 0;
	SIM_TIME = 0.0;

	//press SpaceBar to trigger

	myWorld->Clear();
	myWorld->Add_Object(myTerrain);

	if(myJello)
		delete[] myJello;
	if(myRigidCube)
		delete[] myRigidCube;

	myJello = new Deformable3D[12];
	myRigidCube= new RigidCube[13];
	//central strip
	myJello[0].Init(Eigen::Vector3i(8,8,8),1,300,0.4,10,Eigen::Vector3f(-2,25,-2),Eigen::Vector3f(4.0,2.0,4.0), Eigen::Vector3f(1.0,0,0));
	myJello[1].Init(Eigen::Vector3i(8,8,8),1,300,0.4,10,Eigen::Vector3f(-1.5,21,-1.5),Eigen::Vector3f(3.0,2.0,3.0), Eigen::Vector3f(1.0,0,0));
	myJello[2].Init(Eigen::Vector3i(8,8,8),1,300,0.4,10,Eigen::Vector3f(-1,17,-1),Eigen::Vector3f(2.0,2.0,2.0), Eigen::Vector3f(1.0,0,0));
	myJello[3].Init(Eigen::Vector3i(8,8,8),1,300,0.4,10,Eigen::Vector3f(-1,13,-1),Eigen::Vector3f(2.0,2.0,2.0), Eigen::Vector3f(1.0,0,0));
	//left
	myJello[4].Init(Eigen::Vector3i(8,8,8),1,1000,0.4,10,Eigen::Vector3f(-3.5,19,-1),Eigen::Vector3f(2.0,2.0,2.0), Eigen::Vector3f(1.0,0,0));
	myJello[5].Init(Eigen::Vector3i(8,8,8),1,1000,0.4,10,Eigen::Vector3f(-7.5,19,-1),Eigen::Vector3f(2.0,2.0,2.0), Eigen::Vector3f(1.0,0,0));
    //right
	myJello[6].Init(Eigen::Vector3i(8,8,8),1,1000,0.4,10,Eigen::Vector3f(1.5,19,-1),Eigen::Vector3f(2.0,2.0,2.0), Eigen::Vector3f(1.0,0,0));
	myJello[7].Init(Eigen::Vector3i(8,8,8),1,1000,0.4,10,Eigen::Vector3f(5.5,19,-1),Eigen::Vector3f(2.0,2.0,2.0), Eigen::Vector3f(1.0,0,0));
    //back
	myJello[8].Init(Eigen::Vector3i(8,8,8),1,1000,0.4,10,Eigen::Vector3f(-1,19,-3.5),Eigen::Vector3f(2.0,2.0,2.0), Eigen::Vector3f(1.0,0,0));
	myJello[9].Init(Eigen::Vector3i(8,8,8),1,1000,0.4,10,Eigen::Vector3f(-1,19,-7.5),Eigen::Vector3f(2.0,2.0,2.0), Eigen::Vector3f(1.0,0,0));
	//front
	myJello[10].Init(Eigen::Vector3i(8,8,8),1,1000,0.4,10,Eigen::Vector3f(-1,19,1.5),Eigen::Vector3f(2.0,2.0,2.0), Eigen::Vector3f(1.0,0,0));
	myJello[11].Init(Eigen::Vector3i(8,8,8),1,1000,0.4,10,Eigen::Vector3f(-1,19,5.5),Eigen::Vector3f(2.0,2.0,2.0), Eigen::Vector3f(1.0,0,0));

	myRigidCube[0].Init(1, Eigen::Vector3f(0,28,0),Eigen::Vector3f(5.0,2.0,5.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[1].Init(1, Eigen::Vector3f(0,24,0),Eigen::Vector3f(4.0,2.0,4.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[2].Init(1, Eigen::Vector3f(0,20,0),Eigen::Vector3f(3.0,2.0,3.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[3].Init(1, Eigen::Vector3f(0,16,0),Eigen::Vector3f(2.0,2.0,2.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[4].Init(1, Eigen::Vector3f(0,12,0),Eigen::Vector3f(2.0,2.0,2.0),Eigen::Vector3f(0.0,1.0,0.0));

	myRigidCube[5].Init(1, Eigen::Vector3f(-4.5,20,0),Eigen::Vector3f(2.0,2.0,2.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[6].Init(1, Eigen::Vector3f(-8.5,20,0),Eigen::Vector3f(2.0,2.0,2.0),Eigen::Vector3f(0.0,1.0,0.0));

	myRigidCube[7].Init(1, Eigen::Vector3f(4.5,20,0),Eigen::Vector3f(2.0,2.0,2.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[8].Init(1, Eigen::Vector3f(8.5,20,0),Eigen::Vector3f(2.0,2.0,2.0),Eigen::Vector3f(0.0,1.0,0.0));

	myRigidCube[9].Init(1, Eigen::Vector3f(0,20,-4.5),Eigen::Vector3f(2.0,2.0,2.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[10].Init(1, Eigen::Vector3f(0,20,-8.5),Eigen::Vector3f(2.0,2.0,2.0),Eigen::Vector3f(0.0,1.0,0.0));

	myRigidCube[11].Init(1, Eigen::Vector3f(0,20,4.5),Eigen::Vector3f(2.0,2.0,2.0),Eigen::Vector3f(0.0,1.0,0.0));
	myRigidCube[12].Init(1, Eigen::Vector3f(0,20,8.5),Eigen::Vector3f(2.0,2.0,2.0),Eigen::Vector3f(0.0,1.0,0.0));

	//manuly couple up the deformable and rigid parts
    myRigidCube[0].AttachNodes(myJello[0].m_Mesh->GetTopNodes());
    myRigidCube[1].AttachNodes(myJello[0].m_Mesh->GetBottomNodes());
    myRigidCube[1].AttachNodes(myJello[1].m_Mesh->GetTopNodes());
    myRigidCube[2].AttachNodes(myJello[1].m_Mesh->GetBottomNodes());
    myRigidCube[2].AttachNodes(myJello[2].m_Mesh->GetTopNodes());
    myRigidCube[3].AttachNodes(myJello[2].m_Mesh->GetBottomNodes());
    myRigidCube[3].AttachNodes(myJello[3].m_Mesh->GetTopNodes());
    myRigidCube[4].AttachNodes(myJello[3].m_Mesh->GetBottomNodes());

	myRigidCube[2].AttachNodes(myJello[4].m_Mesh->GetRightNodes());
	myRigidCube[5].AttachNodes(myJello[4].m_Mesh->GetLeftNodes());
	myRigidCube[5].AttachNodes(myJello[5].m_Mesh->GetRightNodes());
	myRigidCube[6].AttachNodes(myJello[5].m_Mesh->GetLeftNodes());

	myRigidCube[2].AttachNodes(myJello[6].m_Mesh->GetLeftNodes());
	myRigidCube[7].AttachNodes(myJello[6].m_Mesh->GetRightNodes());
	myRigidCube[7].AttachNodes(myJello[7].m_Mesh->GetLeftNodes());
	myRigidCube[8].AttachNodes(myJello[7].m_Mesh->GetRightNodes());


	myRigidCube[2].AttachNodes(myJello[8].m_Mesh->GetFrontNodes());
	myRigidCube[9].AttachNodes(myJello[8].m_Mesh->GetBackNodes());
	myRigidCube[9].AttachNodes(myJello[9].m_Mesh->GetFrontNodes());
	myRigidCube[10].AttachNodes(myJello[9].m_Mesh->GetBackNodes());

	myRigidCube[2].AttachNodes(myJello[10].m_Mesh->GetBackNodes());
	myRigidCube[11].AttachNodes(myJello[10].m_Mesh->GetFrontNodes());
	myRigidCube[11].AttachNodes(myJello[11].m_Mesh->GetBackNodes());
	myRigidCube[12].AttachNodes(myJello[11].m_Mesh->GetFrontNodes());

	myRigidCube[0].m_fixed = true;
    //set up the World
    myWorld->Add_Object(&myJello[0]);
    myWorld->Add_Object(&myJello[1]);
    myWorld->Add_Object(&myJello[2]);
    myWorld->Add_Object(&myJello[3]);
    myWorld->Add_Object(&myJello[4]);
    myWorld->Add_Object(&myJello[5]);
    myWorld->Add_Object(&myJello[6]);
    myWorld->Add_Object(&myJello[7]);
    myWorld->Add_Object(&myJello[8]);
    myWorld->Add_Object(&myJello[9]);
    myWorld->Add_Object(&myJello[10]);
    myWorld->Add_Object(&myJello[11]);
	myWorld->Add_Object(&myRigidCube[0]);
	myWorld->Add_Object(&myRigidCube[1]);
	myWorld->Add_Object(&myRigidCube[2]);
	myWorld->Add_Object(&myRigidCube[3]);
	myWorld->Add_Object(&myRigidCube[4]);
	myWorld->Add_Object(&myRigidCube[5]);
	myWorld->Add_Object(&myRigidCube[6]);
	myWorld->Add_Object(&myRigidCube[7]);
	myWorld->Add_Object(&myRigidCube[8]);
	myWorld->Add_Object(&myRigidCube[9]);
	myWorld->Add_Object(&myRigidCube[10]);
	myWorld->Add_Object(&myRigidCube[11]);
	myWorld->Add_Object(&myRigidCube[12]);

	TIME_LAST = TM.GetElapsedTime() ;
	DTIME = 0.0;
	FRAME_TIME = 0.0;


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
	START_POSITION = Eigen::Vector3f(0,8,0);
	//myMillipedes->Init(START_POSITION, 18,Eigen::Vector3f(0.2,1.39,2.422),0.707895, myTerrain);
	myMillipedes->Init(START_POSITION, 12,Eigen::Vector3f(0.4,1.0,2.0),0.8, myTerrain);
	myMillipedes->FixHead();
	myMillipedes->FixTail();
	myWorld->Add_Object(myMillipedes);
	
	//mySphere= new Sphere(Eigen::Vector3f(0,15,0), Eigen::Vector3f(3,3,3), Eigen::Vector3f(1,0,1));
	//myWorld->Add_Object(mySphere);
	//set up the clock
	TIME_LAST = TM.GetElapsedTime();
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
        //initRideScene1();
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
	double physics_time_step = 1/3000.0;
	int num_division = int (dt/physics_time_step);
	//#pragma omp parallel for
	for(int j =0 ;j<num_division; j++)
	{
		for (int i = 0; i<myWorld->List_of_Object.size(); i++)
	      myWorld->List_of_Object[i]->UpdateAll(physics_time_step);

		SIM_TIME += physics_time_step;
		if(myMillipedes->IsTailFixed()){
			//sway the tail, for demonstration of physics
			if(SIM_TIME > 2.0 && SIM_TIME < 4.0)
				myMillipedes->m_tail->m_Center[2] =  START_POSITION[2] + 3*sin(180*SIM_TIME*DegreesToRadians); 	

			if(SIM_TIME > 6.0 && SIM_TIME < 8.0)
				myMillipedes->m_tail->m_Center[1] =  START_POSITION[1] + 3*sin(180*SIM_TIME*DegreesToRadians); 	
		}
	}
	
	if(SIM_TIME > 10.0 && SIM_TIME < 12.0) 
		myMillipedes->m_tail->m_fixed = false;

	if(SIM_TIME > 14.0)
		myMillipedes->m_head->m_fixed = false;

	if(OUTPUT == 1){
		OUTPUT_ONE_FRAME();//output one frame data
	}
	

}

void HackAnimation2(double dt){

	double physics_time_step = 1/4000.0;
	int num_division = int (dt/physics_time_step);
	//#pragma omp parallel for
	for(int j =0 ;j<num_division; j++)
	{
		for (int i = 0; i<myWorld->List_of_Object.size(); i++)
	      myWorld->List_of_Object[i]->UpdateAll(physics_time_step);
	}

	if(OUTPUT==1){
		std::string filename = "FRAME_";
		filename += std::to_string(FRAME_COUNT);
		filename += ".inc";
		RideOutput->open(filename);
		(*RideOutput)<<"//Frame "<<FRAME_COUNT<<std::endl;
		(*RideOutput)<<"#declare SoftPart1 = union {\n"<<std::endl;
			myJello[0].Output2File(RideOutput);
			myJello[5].Output2File(RideOutput);
		(*RideOutput)<<"}\n"<<std::endl;
		(*RideOutput)<<"#declare SoftPart2 = union {\n"<<std::endl;
			myJello[1].Output2File(RideOutput);
			myJello[4].Output2File(RideOutput);
		(*RideOutput)<<"}\n"<<std::endl;
		(*RideOutput)<<"#declare SoftPart3 = union {\n"<<std::endl;
			myJello[2].Output2File(RideOutput);
			myJello[3].Output2File(RideOutput);
		(*RideOutput)<<"}\n"<<std::endl;

		(*RideOutput)<<"#declare RigidPart = union { \n"<<std::endl;
		for(int i = 0; i < 7; i++)
			myRigidCube[i].Output2File(RideOutput);
		(*RideOutput)<<"}\n"<<std::endl;

		RideOutput->close();
		FRAME_COUNT++;
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
		HackAnimation(0.05);
		//myWorld->Update(0.02);//The real physics time step is much smaller

		glutPostRedisplay() ; //draw new frame, the display is not real physics time
		
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
	
	std::string filename = "BUG_";
	filename += std::to_string(FRAME_COUNT);
	filename += ".inc";
	BugOutputPov->open(filename);
	(*BugOutputPov)<<"//Frame "<<FRAME_COUNT<<std::endl;
	(*BugOutputPov)<<"//Begin Food"<<std::endl;
	(*BugOutputPov)<<"#declare Food = union {\n"<<std::endl;
	Eigen::Vector3f tmp_center;double a,d;
	a = 2,d = a/sqrt(2);
	for(int i = 0; i < myTerrain->m_foods.size(); i++){
		tmp_center = myTerrain->m_foods[i]->m_Center;
		(*BugOutputPov)<<"mesh2{"<<std::endl;

		(*BugOutputPov)<<"vertex_vectors{"<<std::endl;
		(*BugOutputPov)<<"6,"<<std::endl;
		(*BugOutputPov)<<"<"<< tmp_center[0] - a/2.0 <<","<< tmp_center[1] <<","<< tmp_center[2] + a/2 <<">,"<<std::endl;
		(*BugOutputPov)<<"<"<< tmp_center[0] + a/2.0 <<","<< tmp_center[1] <<","<< tmp_center[2] + a/2 <<">,"<<std::endl;
		(*BugOutputPov)<<"<"<< tmp_center[0] + a/2.0 <<","<< tmp_center[1] <<","<< tmp_center[2] - a/2 <<">,"<<std::endl;
		(*BugOutputPov)<<"<"<< tmp_center[0] - a/2.0 <<","<< tmp_center[1] <<","<< tmp_center[2] - a/2 <<">,"<<std::endl;
		(*BugOutputPov)<<"<"<< tmp_center[0]<<","<< tmp_center[1] + d <<","<< tmp_center[2]<<">"<<std::endl;
		(*BugOutputPov)<<"<"<< tmp_center[0]<<","<< tmp_center[1] - d <<","<< tmp_center[2]<<">"<<std::endl;
		(*BugOutputPov)<<"}"<<std::endl; // end vertex_vectors

		(*BugOutputPov)<<"face_indices{"<<std::endl;
		(*BugOutputPov)<<"8,"<<std::endl;
        (*BugOutputPov)<<"<0,1,4>,<1,2,4>,<2,3,4>,<3,0,4>,<5,1,0>,<5,2,1>,<5,3,2>,<5,0,3>"<<std::endl;
		(*BugOutputPov)<<"}"<<std::endl; // end face indices 

		(*BugOutputPov)<<"}"<<std::endl<<std::endl; // end mesh2

	}
	(*BugOutputPov)<<"}\n"<<std::endl;

	(*BugOutputPov)<<"//End Food"<<std::endl;
	myMillipedes[0].Output2File(BugOutputPov,1);//0 is for maya model, 1 is for physics
	BugOutputPov->close();

	filename = "GRAPH_";
	filename += std::to_string(FRAME_COUNT);
	filename += ".inc";
	BugOutputGraph->open(filename);
	myMillipedes[0].Output2File(BugOutputGraph,2);//0 is for maya model, 1 is for physics, 2 is for diagram of leg state
	BugOutputGraph->close();
	//mel script file
/*	(*BugOutputMaya)<<"currentTime "<<FRAME_COUNT<<";"<<std::endl;
	myMillipedes[0].Output2File(BugOutputMaya,0);//0 is for maya model, 1 is for physics
	(*BugOutputMaya) <<"//End Frame"<<FRAME_COUNT<<";"<<std::endl;
	*/
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


