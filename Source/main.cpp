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
	myTerrainType = TERRAIN_RANDOM;
	myTerrain = new Terrain(Eigen::Vector2f(2000,2000), Eigen::Vector2i(500,500), 50, myTerrainType
		, OBSTACLE_OFF, FOOD_OFF);

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

	myMillipedes = new Millipede[5];
	
	START_POSITION = Eigen::Vector3f(-80,15,0);
	for(int i = 0; i < 5; i++){
		myMillipedes[i].Init(START_POSITION, 12,Eigen::Vector3f(0.5,1.39,2.422),0.707895, myTerrain);
		START_POSITION[0] += 30;	
		myWorld->Add_Object(myMillipedes + i);
	}


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

	if( key == '4')
		myTerrain->SetFoodKey(1);
	if( key == '5')
		myTerrain->SetFoodKey(2);
	if( key == '6')
		myTerrain->SetFoodKey(3);
	if( key == '7')
		myTerrain->SetFoodKey(4);
	if( key == '8')
		myTerrain->SetFoodKey(5);
	if( key == '9')
		myTerrain->SetFoodKey(6);
	if( key == 'n')
		myTerrain->SetFoodKey(7);
	if( key == 'p')
		myTerrain->SetFoodKey(8);
	
	if( key == EscKey || key == 'q' || key == 'Q' ) 
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
	#pragma omp parallel for
	for (int i = 0; i<myWorld->List_of_Object.size(); i++)
		for(int j =0 ;j<num_division; j++)
		{
	      myWorld->List_of_Object[i]->UpdateAll(physics_time_step);

		}

	SIM_TIME += dt;
	if(OUTPUT == 1){
		OUTPUT_ONE_FRAME();//output one frame data
	}
	
	if(SIM_TIME > 295.0)
		exit(1);
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
		HackAnimation(0.01);
	}
	glutPostRedisplay() ; //draw new frame, the display is not real physics time
}

void OUTPUT_ONE_FRAME(){
	//Water
	if(myTerrainType == TERRAIN_WATER){
		//millipede
		std::string filename = "WATER_";
		filename += std::to_string(FRAME_COUNT);
		filename += ".inc";
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
	Eigen::Vector3f center;double a,d;
	
	for(int i = 0; i < myTerrain->m_foods.size(); i++){
		center = myTerrain->m_foods[i]->m_Center;
		(*BugOutputPov)<<"//BEGIN SPHERE "<<std::endl;
		(*BugOutputPov)<<"sphere{<"<<center[0]<<","<<center[1]<<","<<center[2]<<">,"<<myTerrain->m_foods[i]->m_Size[0]*2<<"}"<<std::endl;
		(*BugOutputPov)<<"//END SPHERE "<<std::endl;
		
	}
	(*BugOutputPov)<<"}\n"<<std::endl;

	(*BugOutputPov)<<"//End Food"<<std::endl;
	myMillipedes[0].Output2File(BugOutputPov,1);//0 is for maya model, 1 is for physics
	myMillipedes[0].Output2File(BugOutputPov,2);//0 is for maya model, 1 is for physics, 2 diagram of leg state
	
		//Surface Obstacles
	//(*BugOutputPov)<<"//Begin Surface Obstacles\n"<<std::endl;
	//(*BugOutputPov)<<"#declare SurfaceObstacle = merge {\n"<<std::endl;
	//Eigen::Vector3f point_a, point_b;
	//Cylinder* temp_cylinder;
	//double radius;
	//for(int i = 0; i < myTerrain->m_surface_objects.size(); i ++){
	//	switch (myTerrain->m_surface_objects[i]->m_type)
	//	{
	//	case TypeCube:
	//		(*BugOutputPov)<<"//Cube\n"<<std::endl;
	//		break;
	//	case TypeSphere:
	//		(*BugOutputPov)<<"//Sphere\n"<<std::endl;
	//		break;
	//	case TypeCylinder:
	//		(*BugOutputPov)<<"//Cylinder\n"<<std::endl;
	//		temp_cylinder = dynamic_cast<Cylinder*>(myTerrain->m_surface_objects[i]);
	//		point_a = temp_cylinder->m_Center;
	//		point_a[2] += 0.5*temp_cylinder->m_Size[2];
	//		point_b = temp_cylinder->m_Center;
	//		point_b[2] -= 0.5*temp_cylinder->m_Size[2];
	//		radius = temp_cylinder->m_Size[0]/2;
	//		(*BugOutputPov)<<"cylinder{"<<"<"<<point_a[0]<<","<<point_a[1]<<","<<point_a[2]<<">,<"<<point_b[0]<<","<<point_b[1]<<","<<point_b[2]<<">,"<<radius<<"}"<<std::endl;
	//		break;
	//	default:
	//		break;
	//	}
	//}
	//(*BugOutputPov)<<"}\n"<<std::endl;

/*
	//mel script file
	(*BugOutputMaya)<<"currentTime "<<FRAME_COUNT<<";"<<std::endl;
	myMillipedes[0].Output2File(BugOutputMaya,0);//0 is for maya model, 1 is for physics
	
	(*BugOutputMaya)<<"//save to obj"<<std::endl;
	(*BugOutputMaya)<<"file -force -options \"groups=1;ptgroups=1;materials=1;smoothing=1;normals=1\" -type \"OBJexport\" -pr" 
		"-ea \"D:/TEMP/Frame"<<FRAME_COUNT<<".obj\";"<<std::endl;
*/
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


