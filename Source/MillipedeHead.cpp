#include "MillipedeHead.h"
#include "Millipede.h"
#include "MillipedeLeg.h"
#include "Drawer.h"
#include "Terrain.h"

void MillipedeAntenna::InitNeuroNet(MillipedeHead* a_head, double a_length, int a_l_r){
	m_l_r = a_l_r;
	m_head = a_head;
	m_Drawer = m_head->m_Drawer;
	m_length = a_length;
	m_phi = 30;
	m_alpha = 0;
	m_food_sense_threshold = 0.2;

	UpdateTipRootPosition();
}

void MillipedeAntenna::UpdateTipRootPosition(){

	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_head->m_Center);
	m_Drawer->Rotate(m_head->m_rotation);
	m_Drawer->Translate(Eigen::Vector3f(-0.5*m_head->m_Size[0], 0.5*m_head->m_Size[1], 0.5*m_l_r*m_head->m_Size[2]));
	m_Drawer->RotateY(m_l_r*m_phi);
	m_Drawer->RotateZ(-m_alpha);

	m_root_position = m_Drawer->CurrentOrigin(); //update root position
	m_Drawer->Translate(Eigen::Vector3f(-m_length,0,0));
	m_tip_position = m_Drawer->CurrentOrigin();//update tip position

}

bool MillipedeAntenna::SenseObstacle(){

	//test anttena line intersection with terrain;
	return m_head->m_terrain->TestIntersection(m_head->m_master, m_root_position, m_tip_position);

}

bool MillipedeAntenna::SenseFood(double& intensity){
	
	intensity = m_head->m_terrain->GetFoodIntensity(m_tip_position);
	if(intensity > m_food_sense_threshold)
		return true;
	return false;
}

void MillipedeAntenna::Draw(int type, const Camera& camera, const Light& light){

	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_head->m_Center);
	m_Drawer->Rotate(m_head->m_rotation);
	m_Drawer->Translate(Eigen::Vector3f(-0.5*m_head->m_Size[0], 0.5*m_head->m_Size[1], 0.5*m_l_r*m_head->m_Size[2]));
	m_Drawer->RotateY(m_l_r*m_phi);
	m_Drawer->RotateZ(-m_alpha);

	m_Drawer->PushMatrix();
	m_Drawer->Scale(Eigen::Vector3f(0.2,0.2,0.2));
	m_Drawer->SetColor(Eigen::Vector3f(1,1,1));
	m_Drawer->DrawSphere(type,camera,light);
	m_Drawer->PopMatrix();

	m_Drawer->PushMatrix();
	m_Drawer->Translate(Eigen::Vector3f(-0.5*m_length,0,0));
	m_Drawer->RotateY(90);
	m_Drawer->Scale(Eigen::Vector3f(0.2,0.2,m_length));
	m_Drawer->DrawCylinder(type, camera, light);
	m_Drawer->PopMatrix();

	m_Drawer->Translate(Eigen::Vector3f(-m_length,0,0));

	m_Drawer->Scale(Eigen::Vector3f(0.3,0.3,0.3));
	m_Drawer->SetColor(Eigen::Vector3f(1,0,0));
	m_Drawer->DrawSphere(type,camera,light);

}

void MillipedeAntenna::UpdateAll(double a_dt){
	
	UpdateTipRootPosition();
	
}

void MillipedeAntenna::Output2File(std::ofstream* filestream){
	
	Eigen::Vector3f point_a, point_b, center; double radius;

	(*filestream)<<"//BEGIN ANTENNA "<<m_l_r<<std::endl;
	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_head->m_Center);
	m_Drawer->Rotate(m_head->m_rotation);
	m_Drawer->Translate(Eigen::Vector3f(-0.5*m_head->m_Size[0], 0.5*m_head->m_Size[1], 0.5*m_l_r*m_head->m_Size[2]));
	m_Drawer->RotateY(m_l_r*m_phi);
	m_Drawer->RotateZ(-m_alpha);

	center = m_Drawer->CurrentOrigin();
	radius = 0.2;
	(*filestream)<<"//BEGIN SPHERE "<<std::endl;
	(*filestream)<<"sphere{"<<"<"<<center[0]<<","<<center[1]<<","<<center[2]<<">"<<","<<radius<<"}"<<std::endl;
	(*filestream)<<"//END SPHERE "<<std::endl;

	m_Drawer->Translate(Eigen::Vector3f(-m_length,0,0));
	point_a = center;
	point_b = m_Drawer->CurrentOrigin();
	radius = 0.2;
	(*filestream)<<"//BEGIN CYLINDER "<<std::endl;
	(*filestream)<<"cylinder{"<<"<"<<point_a[0]<<","<<point_a[1]<<","<<point_a[2]<<">,<"<<point_b[0]<<","<<point_b[1]<<","<<point_b[2]<<">,"<<radius/2.0<<"}"<<std::endl;
	(*filestream)<<"//END CYLINDER "<<std::endl;

	center = point_b;
	radius = 0.2;
	(*filestream)<<"//BEGIN SPHERE "<<std::endl;
	(*filestream)<<"sphere{"<<"<"<<center[0]<<","<<center[1]<<","<<center[2]<<">"<<","<<radius<<"}"<<std::endl;
	(*filestream)<<"//END SPHERE "<<std::endl;

	(*filestream)<<"//END ANTENNA "<<m_l_r<<std::endl;
}


void MillipedeHead::InitPhysics(double density, Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color){
	
	RigidCube::Init(density, center, size, color);
	m_next = NULL;
	
}


void MillipedeHead::InitNeuroNet(Millipede* a_root){
	m_master = a_root;
	m_terrain = m_master->m_terrain;
	m_section_id = 0;
	m_Drawer = m_master->m_Drawer;
	m_left_antenna = new MillipedeAntenna();
	m_right_antenna = new MillipedeAntenna();
	m_left_antenna->InitNeuroNet(this, 8,1);
	m_right_antenna->InitNeuroNet(this, 8,-1);

	EnterMode(ADJUSTING);
}

void MillipedeHead::KeepHeadBalance(double dt){
	
	m_height_obj = m_next->m_next->m_height_obj;
	m_orient_obj = m_master->m_terrain->GetNormal(m_Center);
	m_current_height = m_master->m_terrain->GetHeight(m_Center);

	double turn_angle;
	//the head's height need to be balanced
	m_Center += 10*dt*(m_height_obj - m_current_height)*m_orient_obj;//balance of height	

	//keep the balance of lean in x rotation
	double zaxisdify = (m_rotation*Eigen::Vector3f(0,0,1)).dot(m_orient_obj);
	turn_angle = (zaxisdify > 0? 1:-1)*60*dt;
	Eigen::AngleAxis<float> turnx(DegreesToRadians*turn_angle, Eigen::Vector3f(1,0,0));
	m_rotation = m_rotation*turnx.toRotationMatrix();

	//keep the balance of lean in z rotation
	double xaxisdify = (m_rotation*Eigen::Vector3f(-1,0,0)).dot(m_orient_obj);
	turn_angle = (xaxisdify > 0? 1:-1)*60*dt;
	Eigen::AngleAxis<float> turnz(DegreesToRadians*turn_angle,Eigen::Vector3f(0,0,1));
	m_rotation = m_rotation*turnz.toRotationMatrix();

	//now update the matrixes and fixed points for soft part update
	m_Trans.setIdentity();
	m_Trans.translate(m_Center);
	m_Trans.rotate(m_rotation);
	m_Trans.scale(m_Size);
	m_TransBack = m_Trans.inverse();
	UpdateFixed();
}

void MillipedeHead::EnterMode(MILLIPEDE_STATUS a_mode){

	m_prev_mode = m_mode;
	m_mode = a_mode;
	switch (a_mode)
	{
	case CONTROLLED:
		m_linear_speed = 15.0;
		m_turning_speed = 0.0;
		m_turning_obj = 0.0;
		m_current_turning_accum = 0;
		m_turning_direction = GO_STRAIGHT;
		break;
	case AVOID_OBSTACLE_LEFT:
		m_linear_speed = 5;
		m_turning_speed = 100;
		m_turning_obj = 2;
		m_current_turning_accum = 0;
		m_turning_direction = TURN_RIGHT;
		break;
	case AVOID_OBSTACLE_RIGHT:
		m_linear_speed = 5;
		m_turning_speed = 80;
		m_turning_obj = 2;
		m_current_turning_accum = 0;
		m_turning_direction = TURN_LEFT;
		break;
	case ADJUSTING:
		m_linear_speed = 10.0;
		m_turning_speed = 0.0;
		m_turning_obj = 0.0;
		m_current_turning_accum = 0;
		m_turning_direction = GO_STRAIGHT;
		break;
	case PREDATING_FOOD_LEFT:
		m_linear_speed = 15;
		m_turning_speed = 40;
		m_turning_obj = 2;
		m_current_turning_accum = 0;
		m_turning_direction = TURN_LEFT;
		break;
	case PREDATING_FOOD_RIGHT:
		m_linear_speed = 15;
		m_turning_speed = 40;
		m_turning_obj = 2;
		m_current_turning_accum = 0;
		m_turning_direction = TURN_RIGHT;
		break;
	case RANDOM_WALK:
		m_linear_speed = 10;
		m_turning_speed = 10 + 30*Util::getRand();
		m_current_turning_accum = 0;
		m_turning_direction = (Util::getRand() > 0.5 ? TURN_LEFT:TURN_RIGHT); //set a new turning direction
		m_current_turning_accum = 0.0; //clear to 0
		m_turning_obj = Util::getRand()*180; //set a new turning angle
		//m_turning_direction = GO_STRAIGHT;//for testing
		break;
	default:
		break;
	}

}


void MillipedeHead::UpdateNeuroNet(double dt){

	//sense the environment and update the turning direction, speed, etc
	//ACTION SELECTION: food?escape?follow

	if(m_mode == ADJUSTING){
		MillipedeRigidSection *neck;
		neck = m_master->m_head->m_next->m_next;

		if(neck->m_body_state == LEG_SUPPORTED)
			EnterMode(RANDOM_WALK);
		else
			return;
	}
		/*
		//In adjusting mode only when all sections get supported will it start walking
		while(1){
			if(temp_rigid_section->m_body_state != LEG_SUPPORTED){
				return;//remain in Adjusting mode
			}
			temp_soft_section = temp_rigid_section->m_next;
			if(temp_soft_section){
				temp_rigid_section = temp_soft_section->m_next;
			}
			else{
				EnterMode(RANDOM_WALK);//All sections are in Leg_supported mode now
				break;
			}
		}
		*/
	

	//left antenna has a higher priority when both are hit
	//avoid swaying 

	if(m_left_antenna->SenseObstacle()){
		EnterMode(AVOID_OBSTACLE_LEFT);
		return;
	}
	else if(m_right_antenna->SenseObstacle()){
		EnterMode(AVOID_OBSTACLE_RIGHT);
		return;
	}
		
	double food_intensity_left, food_intensity_right;
	if(m_left_antenna->SenseFood(food_intensity_left))
	{
		if(m_right_antenna->SenseFood(food_intensity_right)){
			if(food_intensity_left > food_intensity_right){
				//Turn Left
				EnterMode(PREDATING_FOOD_LEFT);
			}
			else{
				//Turn right
				EnterMode(PREDATING_FOOD_RIGHT);
			}
		}
		else{
			//Turn Left
			EnterMode(PREDATING_FOOD_LEFT);
		}
	}

	if(m_mode == PREDATING_FOOD_LEFT|| m_mode == PREDATING_FOOD_RIGHT){
		if(m_terrain->ReachFood(m_Center,3))
			EnterMode(RANDOM_WALK);
	}

	if(m_mode == RANDOM_WALK || m_mode == AVOID_OBSTACLE_LEFT || m_mode == AVOID_OBSTACLE_RIGHT
		||m_mode == PREDATING_FOOD_LEFT || m_mode ==PREDATING_FOOD_RIGHT){
		m_current_turning_accum += m_turning_speed*dt;
		if(m_current_turning_accum >= m_turning_obj)
			EnterMode(RANDOM_WALK);
	}
	
}

void MillipedeHead::UpdatePhysics(double dt){

	if(m_mode == ADJUSTING){
		//Head in free fall state
		RigidCube::UpdatePhysics(dt);
		return;
	}

	//Head Balance Net: balance of height, orientation in x and z axis, flat to the ground.
	KeepHeadBalance(dt);

	//speed sync, head control the speed
	//the factor depends on the leg length
	//faster the leg rotation, more legs supportting body
	m_next->m_next->m_left_leg->m_leg_rotation_velocity =  100*m_linear_speed;
	m_next->m_next->m_right_leg->m_leg_rotation_velocity =  100*m_linear_speed;

	if(m_mode == CONTROLLED)
		return;

	Eigen::Vector3f front_vector; double turn_angle;
	front_vector = m_rotation*Eigen::Vector3f(-1,0,0);
	//turn to the desired direction
	if(m_turning_direction != GO_STRAIGHT){
		turn_angle =  (m_turning_direction == TURN_LEFT ? 1:-1)*m_turning_speed*dt;//y axis anti clockwise
		Eigen::AngleAxis<float> turny(DegreesToRadians*turn_angle, Eigen::Vector3f(0,1,0));
		m_rotation = m_rotation*turny.toRotationMatrix();
	}
	//go straight in the front vector direction
	m_Center += m_linear_speed*dt*front_vector;

	//now update the matrixes and fixed points for soft part update
	m_Trans.setIdentity();
	m_Trans.translate(m_Center);
	m_Trans.rotate(m_rotation);
	m_Trans.scale(m_Size);
	m_TransBack = m_Trans.inverse();
	UpdateFixed();

}

void MillipedeHead::UpdateAll(double dt){

		//central controller for the head movement, which determine the movement of the entire millipede
	
		m_left_antenna->UpdateAll(dt);
		m_right_antenna->UpdateAll(dt);

		//Head Target Net: sense/think then set all the parameters:
		//turning direction, turning speed and linear speed, etc;
		UpdateNeuroNet(dt);

		//Apply the Turning
		UpdatePhysics(dt);
		
	
}

void MillipedeHead::Draw(int type, const Camera& camera, const Light& light){
	 Cube::Draw(type, camera, light);
	 m_left_antenna->Draw(type, camera, light);
	 m_right_antenna->Draw(type, camera, light);
}

void MillipedeHead::Output2File(std::ofstream* filestream){
	
	//output for maya python script
	(*filestream)<<"//BEGIN HEAD"<<std::endl;
	
	Eigen::Vector3f ea = m_rotation.eulerAngles(0,1,2)/DegreesToRadians;
	(*filestream)<<"select -r head ;"<<std::endl;
	(*filestream)<<"setAttr \"head.translate\" "<<-m_Center.z() <<" "<<m_Center.y()<<" "<<m_Center.x()<<";"<<std::endl;
	(*filestream)<<"setAttr \"head.rotate\" "<<ea.x()<<" "<<ea.y()<<" "<<ea.z()<<";"<<std::endl;
	//no legs

	//two antenna
	//m_left_antenna->Output2File(filestream);
	//m_right_antenna->Output2File(filestream);
	(*filestream)<<"//END HEAD"<<std::endl;



/* output for povray
	(*filestream)<<"//BEGIN HEAD"<<std::endl;
	Cube::Output2File(filestream);
	//no legs
	//two antenna
	m_left_antenna->Output2File(filestream);
	m_right_antenna->Output2File(filestream);
	(*filestream)<<"//END HEAD"<<std::endl;
*/
}
