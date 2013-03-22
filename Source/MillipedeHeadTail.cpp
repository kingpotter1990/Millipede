#include "MillipedeHeadTail.h"
#include "Millipede.h"
#include "MillipedeLeg.h"
#include "Drawer.h"
#include "Terrain.h"

void ReadAntennaDataFromModel(double* readin){
	//generated by script of maya
	/*
string $name;
int $i;
float $vec[3];
float $prevpos[3] = `xform -ws -q -t al0`;
for($i = 1; $i < 28; $i++){
    $name = "al" + string($i);
    //print $name;
    //print "\n";
    float $positions[3] = `xform -ws -q -t $name`;
    $vec[0] = $positions[0] - $prevpos[0];
    $vec[1] = $positions[1] - $prevpos[1];
    $vec[2] = $positions[2] - $prevpos[2];
    print "\t readin[";
    int $j = $i -1;
    print $j;
    print "] = ";
    float $result= sqrt($vec[0]*$vec[0] + $vec[1]*$vec[1] + $vec[2]*$vec[2]);
    print $result;
    print ";\n";
    $prevpos = $positions;
}

	*/

	 readin[0] = 0.317766;
	 readin[1] = 0.41564;
	 readin[2] = 0.452685;
	 readin[3] = 0.418591;
	 readin[4] = 0.339678;
	 readin[5] = 0.417748;
	 readin[6] = 0.362788;
	 readin[7] = 0.356316;
	 readin[8] = 0.341148;
	 readin[9] = 0.352533;
	 readin[10] = 0.345662;
	 readin[11] = 0.359771;
	 readin[12] = 0.303336;
	 readin[13] = 0.289228;
	 readin[14] = 0.296282;
	 readin[15] = 0.275119;
	 readin[16] = 0.303336;
	 readin[17] = 0.293143;
	 readin[18] = 0.248309;
	 readin[19] = 0.258656;
	 readin[20] = 0.251758;
	 readin[21] = 0.244861;
	 readin[22] = 0.229435;
	 readin[23] = 0.219162;
	 readin[24] = 0.208889;
	 readin[25] = 0.229435;
	 readin[26] = 0.195191;
}

void MillipedeAntenna::InitNeuroNet(MillipedeHead* a_head, double a_length, int a_l_r){
	m_l_r = a_l_r;
	m_displacement = Eigen::Vector3f(-2.386, 0.4807, a_l_r*0.233);
	m_head = a_head;
	m_Drawer = m_head->m_Drawer;
	m_n_segment = 27;
	m_segment_length = new double[m_n_segment]; 
	m_omega1 = 300;
	m_omega2 = 300;
	m_theta1 = new double[m_n_segment];
	m_theta2 = new double[m_n_segment];
	m_sensor_pos = new Eigen::Vector3f[m_n_segment];
	m_timer = 0;
	for(int i = 0;i<m_n_segment; i++)
	{
		m_segment_length[i] = 0.2*i/(m_n_segment-1) + 0.3*(m_n_segment - 1 - i)/(m_n_segment - 1);//from 0.2 to 0.4
		m_theta1[i] = 0;
		m_theta2[i] = 0;
	}
	m_target_phi = 30;
	m_target_alpha = 0;
	m_rot_v_alpha = 10;
	m_rot_v_phi = 10;
	m_phi = 30;//base rotation
	m_alpha = 0;//base rotation
	m_food_sense_threshold = 0.0001;

	ReadAntennaDataFromModel(m_segment_length);
	UpdateTipRootPosition();
}

void MillipedeAntenna::UpdateTipRootPosition(){

	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_head->m_Center);
	m_Drawer->Rotate(m_head->m_rotation);
	m_Drawer->Translate(m_displacement);
	m_Drawer->RotateY(m_l_r*m_phi);
	m_Drawer->RotateZ(-m_alpha);

	m_root_position = m_Drawer->CurrentOrigin(); //update root position
	for(int i = 0; i < m_n_segment; i++){
		m_sensor_pos[i] = m_Drawer->CurrentOrigin();
		m_Drawer->RotateY(m_l_r*m_theta1[i]);
		m_Drawer->RotateZ(m_theta2[i]);
		m_Drawer->Translate(Eigen::Vector3f(-m_segment_length[i],0,0));
	}
	m_tip_position = m_Drawer->CurrentOrigin();//update tip position

}

bool MillipedeAntenna::SenseObstacle(){
	static double store_rot_v_phi;
	//test anttena line intersection with terrain;
	for(int i = 0; i < m_n_segment; i++){
		if(m_head->m_terrain->TestInsideObstacle(m_sensor_pos[i])){
			store_rot_v_phi = m_rot_v_phi;
			m_rot_v_phi = 0;	
			return true;
			break;
		}
	}
	if(m_rot_v_phi == 0)
		m_rot_v_phi = store_rot_v_phi;
	return false;

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

	m_Drawer->Translate(m_displacement);
	m_Drawer->RotateY(m_l_r*m_phi);
	m_Drawer->RotateZ(-m_alpha);

	for(int i = 0; i < m_n_segment; i++){
		m_Drawer->PushMatrix();
		m_Drawer->Scale(m_segment_length[i]);
		m_Drawer->SetColor(Eigen::Vector3f(1,0,0));
		m_Drawer->DrawSphere(type,camera,light);
		m_Drawer->PopMatrix();

		m_Drawer->RotateY(m_l_r*m_theta1[i]);
		m_Drawer->RotateZ(m_theta2[i]);
		m_Drawer->Translate(Eigen::Vector3f(-m_segment_length[i],0,0));
		
	}

}

void MillipedeAntenna::UpdateAll(double a_dt){
	//update antennae angles
	m_timer += a_dt;

	double k1 = 6, k2 = 6;
	double theta1, theta2;

	for(int i = 0; i < m_n_segment; i++){
		theta1 = m_timer*m_omega1 - k1*i;
		theta2 = m_timer*m_omega2 - k2*i;
		m_theta1[i] = 2*sin(theta1*DegreesToRadians);
		m_theta2[i] = 1*sin(theta2*DegreesToRadians) + 1;
	}

	if(m_timer*m_omega1 > 360*3){
		m_omega1 = Util::getRand()*100 + 300;
		m_omega2 = Util::getRand()*100 + 300;
		m_timer = 0.0;
	}


	m_alpha += m_rot_v_alpha*a_dt; 
	m_phi += m_rot_v_phi*a_dt;

	if((m_rot_v_phi > 0 && m_phi > m_target_phi) || (m_rot_v_phi <0 && m_phi < m_target_phi)){
		m_target_phi = (Util::getRand() - 0.5)*60 + 30;
		m_rot_v_phi = 60*(m_phi < m_target_phi ? 1:-1);
	}

	if((m_rot_v_alpha > 0 && m_alpha > m_target_alpha) || (m_rot_v_alpha <0 && m_alpha < m_target_alpha)){
		m_target_alpha = (Util::getRand() - 0.5)*30;
		m_rot_v_alpha = 60*(m_alpha < m_target_alpha ? 1:-1);
	}

	//test antenna penetration with surface 
	Eigen::Vector3f test_point;
	for(int i = 0; i < m_n_segment; i++){
		if(m_head->m_terrain->TestInside(m_sensor_pos[i])){
			//move up the anttena immdeidately
			m_target_alpha = m_alpha + 20;
			m_rot_v_alpha = 200*(m_alpha < m_target_alpha ? 1:-1);
			break;
		}
	}

	UpdateTipRootPosition();
	
}

void MillipedeAntenna::Output2File(std::ofstream* filestream, int type){
	(*filestream)<<"//BEGIN ANTENNA "<<m_l_r<<std::endl;
	if(type == 0){//for maya
		
		std::string alr = (m_l_r == 1)?"al":"ar";

		(*filestream)<<"setAttr \""<<alr<<0<<".rotate\" 0 0 0;"<<std::endl;
		(*filestream)<<"select -r "<<alr<<0<<" ;"<<std::endl;
		(*filestream)<<"rotate -r -os 0 "<<m_l_r*m_phi<<" 0 ;"<<std::endl;
		(*filestream)<<"rotate -r -os "<<m_alpha<<" 0 0 ;"<<std::endl;

		for(int i=0; i < m_n_segment; i++){
			if(i != 0){
			(*filestream)<<"setAttr \""<<alr<<i<<".rotate\" 0 0 0;"<<std::endl;
			}
			(*filestream)<<"select -r "<<alr<<i<<" ;"<<std::endl;
			(*filestream)<<"rotate -r -os 0 "<<m_l_r*m_theta1[i]<<" 0 ;"<<std::endl;
			(*filestream)<<"rotate -r -os "<<-m_theta2[i]<<" 0 0 ;"<<std::endl;
		}
		
	}
	else{//for povray
	Eigen::Vector3f point_a, point_b, center; double radius;

	(*filestream)<<"merge{"<<std::endl;
	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_head->m_Center);
	m_Drawer->Rotate(m_head->m_rotation);
	m_Drawer->Translate(Eigen::Vector3f(-0.5*m_head->m_Size[0], 0.5*m_head->m_Size[1], 0.5*m_l_r*m_head->m_Size[2]));
	m_Drawer->RotateY(m_l_r*m_phi);
	m_Drawer->RotateZ(-m_alpha);

	for(int i = 0; i < m_n_segment; i++){
	center = m_Drawer->CurrentOrigin();
	radius = m_segment_length[i]/1.2;
	(*filestream)<<"//BEGIN SPHERE "<<std::endl;
	(*filestream)<<"sphere{"<<"<"<<center[0]<<","<<center[1]<<","<<center[2]<<">"<<","<<radius<<"}"<<std::endl;
	(*filestream)<<"//END SPHERE "<<std::endl;
	m_Drawer->RotateY(m_l_r*m_theta1[i]);
	m_Drawer->RotateZ(m_theta2[i]);
	m_Drawer->Translate(Eigen::Vector3f(-m_segment_length[i],0,0));
	}
	(*filestream)<<"}"<<std::endl;
	
	}
	(*filestream)<<"//END ANTENNA "<<m_l_r<<std::endl;
}

void MillipedeTail::InitNeuroNet(MillipedeHead* a_head, RigidCube* a_tail, int a_l_r){
	
	m_head = a_head;
	m_Drawer = m_head->m_Drawer;
	m_tail_section = a_tail;
	m_l_r = a_l_r;

	m_alpha = 10;
	m_phi = 15;
	m_target_alpha = m_alpha;
	m_timer = 0.0;
	m_omega = 60;

	m_displacement = Eigen::Vector3f(0.414,0,m_l_r*0.643);
	
	m_l0 = Eigen::Vector3f(0.702,0.454,1.573).norm();
	m_l1 = Eigen::Vector3f(0.372,0.369,0.822).norm();
	m_l2 = Eigen::Vector3f(1.167,1.481,2.077).norm();
}

void MillipedeTail::UpdateAll(double dt){

	m_timer += dt;
	UpdateTipRootPosition();
	//update tail movement;
	m_alpha += m_omega*(m_alpha < m_target_alpha ? 1: -1)*dt; 

	if(fabs(m_alpha - m_target_alpha) < 2*dt*m_omega){
		m_target_alpha = Util::getRand()*30;
		m_omega = 60;
	}

	//collision detection
	if(m_head->m_terrain->TestInside(m_tip_position)){
		m_target_alpha = m_alpha + 10;
		m_omega *=2;
	}

}


void MillipedeTail::UpdateTipRootPosition(){
	//root
	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_tail_section->m_Center);
	m_Drawer->Rotate(m_tail_section->m_rotation);

	m_Drawer->Translate(m_displacement);
	m_root_position = m_Drawer->CurrentOrigin();
	//tip	
	m_Drawer->RotateY(-m_phi*m_l_r);
	m_Drawer->RotateZ(m_alpha);

	m_Drawer->Translate(Eigen::Vector3f(m_l0,0,0));
	m_Drawer->RotateZ(-15);
	m_Drawer->Translate(Eigen::Vector3f(m_l1,0,0));
	m_Drawer->RotateZ(-20);
	m_Drawer->Translate(Eigen::Vector3f(m_l2,0,0));
	
	m_tip_position = m_Drawer->CurrentOrigin();


}

void MillipedeTail::Draw(int type, const Camera& camera, const Light& light){
	m_Drawer->SetColor(Eigen::Vector3f(1,0,0));
	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_tail_section->m_Center);
	m_Drawer->Rotate(m_tail_section->m_rotation);


	m_Drawer->Translate(m_displacement);
	m_root_position = m_Drawer->CurrentOrigin();
	
	m_Drawer->PushMatrix();
	m_Drawer->Scale(0.3);
	m_Drawer->DrawSphere(type, camera, light);
	m_Drawer->PopMatrix();


	m_Drawer->RotateY(-m_phi*m_l_r);
	m_Drawer->RotateZ(m_alpha);

	m_Drawer->Translate(Eigen::Vector3f(0.5*m_l0,0,0));
	m_Drawer->PushMatrix();
	m_Drawer->RotateY(90);
	m_Drawer->Scale(Eigen::Vector3f(0.6,0.6,m_l0));
	m_Drawer->DrawCylinder(type, camera, light);
	m_Drawer->PopMatrix();

	m_Drawer->Translate(Eigen::Vector3f(0.5*m_l0,0,0));
	
	m_Drawer->PushMatrix();
	m_Drawer->Scale(0.3);
	m_Drawer->DrawSphere(type, camera, light);
	m_Drawer->PopMatrix();

	m_Drawer->RotateZ(-15);

	m_Drawer->Translate(Eigen::Vector3f(0.5*m_l1,0,0));
	m_Drawer->PushMatrix();
	m_Drawer->RotateY(90);
	m_Drawer->Scale(Eigen::Vector3f(0.4,0.4,m_l1));
	m_Drawer->DrawCylinder(type, camera, light);
	m_Drawer->PopMatrix();

	m_Drawer->Translate(Eigen::Vector3f(0.5*m_l1,0,0));
	m_Drawer->PushMatrix();
	m_Drawer->Scale(0.2);
	m_Drawer->DrawSphere(type, camera, light);
	m_Drawer->PopMatrix();

	m_Drawer->RotateZ(-20);
	m_Drawer->Translate(Eigen::Vector3f(0.5*m_l2,0,0));
	m_Drawer->PushMatrix();
	m_Drawer->RotateY(-90);
	m_Drawer->Scale(Eigen::Vector3f(0.3,0.3,m_l2));
	m_Drawer->DrawCone(type, camera, light);
	m_Drawer->PopMatrix();

}

void MillipedeTail::Output2File(std::ofstream* filestream, int type){
	
	if(type == 1){
	//povray
		Eigen::Vector3f point_a, point_b, center; double radius;
	(*filestream)<<"//BEGIN Tail Left "<<std::endl;

	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_tail_section->m_Center);
	m_Drawer->Rotate(m_tail_section->m_rotation);
	m_Drawer->Translate(m_displacement);

	center = m_Drawer->CurrentOrigin();
	radius = 0.2;
	(*filestream)<<"sphere{<"<<center[0]<<","<<center[1]<<","<<center[2]<<">,"<<radius<<"}"<<std::endl;
	point_a = center;

	m_Drawer->RotateY(-m_l_r*m_phi);
	m_Drawer->RotateZ(m_alpha);

	m_Drawer->Translate(Eigen::Vector3f(m_l0,0,0));
	center = m_Drawer->CurrentOrigin();
	point_b = center;
	radius = 0.2;
	(*filestream)<<"cylinder{"<<"<"<<point_a[0]<<","<<point_a[1]<<","<<point_a[2]<<">,<"<<point_b[0]<<","<<point_b[1]<<","<<point_b[2]<<">,"<<radius<<"}"<<std::endl;
	(*filestream)<<"sphere{<"<<center[0]<<","<<center[1]<<","<<center[2]<<">,"<<radius<<"}"<<std::endl;
	
	point_a = point_b;
	m_Drawer->RotateZ(-15);
	m_Drawer->Translate(Eigen::Vector3f(m_l1,0,0));
	center = m_Drawer->CurrentOrigin();
	point_b = center;
	radius = 0.15;
	(*filestream)<<"cylinder{"<<"<"<<point_a[0]<<","<<point_a[1]<<","<<point_a[2]<<">,<"<<point_b[0]<<","<<point_b[1]<<","<<point_b[2]<<">,"<<radius<<"}"<<std::endl;
	(*filestream)<<"sphere{<"<<center[0]<<","<<center[1]<<","<<center[2]<<">,"<<radius<<"}"<<std::endl;
	
	point_a = point_b;
	m_Drawer->RotateZ(-20);
	m_Drawer->Translate(Eigen::Vector3f(m_l2,0,0));
	point_b = m_Drawer->CurrentOrigin();
	(*filestream)<<"cone{"<<"<"<<point_a[0]<<","<<point_a[1]<<","<<point_a[2]<<">,"<<radius<<std::endl;
	(*filestream)<<"<"<<point_b[0]<<","<<point_b[1]<<","<<point_b[2]<<">,0}"<<std::endl;
	}
	if(type == 0){
	//maya
		if(m_l_r == 1){
		(*filestream)<<"setAttr \"tail_l0"<<".rotate\" 0 0 0;"<<std::endl;
		(*filestream)<<"select -r tail_l0 ;"<<std::endl;
		(*filestream)<<"rotate -r -os 0 "<<-m_phi<<" 0 ;"<<std::endl;
		(*filestream)<<"rotate -r -os "<<-m_alpha<<" 0 0 ;"<<std::endl;
		}
		else{
		(*filestream)<<"setAttr \"tail_r0"<<".rotate\" 0 0 0;"<<std::endl;
		(*filestream)<<"select -r tail_r0 ;"<<std::endl;
		(*filestream)<<"rotate -r -os 0 "<<m_phi<<" 0 ;"<<std::endl;
		(*filestream)<<"rotate -r -os "<<-m_alpha<<" 0 0 ;"<<std::endl;
		}
	}

}

void MillipedeHead::InitPhysics(double density, Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color){
	
	RigidCube::Init(density, center, size, color);
	m_next = NULL;
	
}


void MillipedeHead::InitNeuroNet(Millipede* a_root){
	m_master = a_root;
	m_time = 0;
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
		m_linear_speed = 8;
		m_turning_speed = 60;
		m_turning_obj = 10;
		m_current_turning_accum = 0;
		m_turning_direction = TURN_RIGHT;
		break;
	case AVOID_OBSTACLE_RIGHT:
		m_linear_speed = 8;
		m_turning_speed = 60;
		m_turning_obj = 10;
		m_current_turning_accum = 0;
		m_turning_direction = TURN_LEFT;
		break;
	case ADJUSTING:
		m_linear_speed = 8.0;
		m_turning_speed = 0.0;
		m_turning_obj = 0.0;
		m_current_turning_accum = 0;
		m_turning_direction = GO_STRAIGHT;
		break;
	case PREDATING_FOOD_LEFT:
		m_linear_speed = 8;
		m_turning_speed = 60;
		m_turning_obj = 2;
		m_current_turning_accum = 0;
		m_turning_direction = TURN_LEFT;
		break;
	case PREDATING_FOOD_RIGHT:
		m_linear_speed = 8;
		m_turning_speed = 60;
		m_turning_obj = 2;
		m_current_turning_accum = 0;
		m_turning_direction = TURN_RIGHT;
		break;
	case RANDOM_WALK:
		m_linear_speed = 8;
		m_turning_speed = 60 + 10*Util::getRand();
		m_current_turning_accum = 0;
		m_turning_direction = (m_turning_direction != TURN_LEFT?TURN_LEFT:TURN_RIGHT);
		//m_turning_direction = (Util::getRand() > 0.5 ? TURN_LEFT:TURN_RIGHT); //set a new turning direction
		m_current_turning_accum = 0.0; //clear to 0
		m_turning_obj = 120 + Util::getRand()*10; //set a new turning angle
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
		if(m_terrain->ReachFood(m_Center,2))
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
	m_next->m_next->m_left_leg->m_leg_rotation_velocity =  80*m_linear_speed;
	m_next->m_next->m_right_leg->m_leg_rotation_velocity =  80*m_linear_speed;

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

	    m_time += dt;
		//central controller for the head movement, which determine the movement of the entire millipede
	
		m_left_antenna->UpdateAll(dt);
		m_right_antenna->UpdateAll(dt);

		//update the fake leg and mouth motion parameters;
		//from 10 to 30
		m_fake_leg_angle = 20 + 10*sin(50*m_time);
		m_mouth_angle = 5*sin(20*m_time);
		//Head Target Net: sense/think then set all the parameters:
		//turning direction, turning speed and linear speed, etc;
		UpdateNeuroNet(dt);

		//Apply the Turning
		UpdatePhysics(dt);
		
	
}

void MillipedeHead::Draw(int type, const Camera& camera, const Light& light){
	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_Center);
	m_Drawer->Rotate(m_rotation);
	m_Drawer->Translate(Eigen::Vector3f(-1.0,0,0));
	m_Drawer->Scale(Eigen::Vector3f(1.2,0.8,1.5));
	m_Drawer->SetColor(Eigen::Vector3f(1,0,0));
	m_Drawer->DrawSphere(type, camera, light);

	 Cube::Draw(type, camera, light);
	 m_left_antenna->Draw(type, camera, light);
	 m_right_antenna->Draw(type, camera, light);
}

void MillipedeHead::Output2File(std::ofstream* filestream, int type){
	if(type == 0){	
	//output for maya python script
	(*filestream)<<"//BEGIN HEAD"<<std::endl;
	
	Eigen::Vector3f ea = m_rotation.eulerAngles(0,1,2)/DegreesToRadians;
	(*filestream)<<"setAttr \"head.translate\" "<<-m_Center.z() <<" "<<m_Center.y()<<" "<<m_Center.x()<<";"<<std::endl;
	(*filestream)<<"select -r head ;"<<std::endl;
	(*filestream)<<"setAttr \"head.rotate\" "<<-ea.z()<<" "<<ea.y()<<" "<<ea.x()<<";"<<std::endl;
	//the fake leg 

	(*filestream)<<"setAttr \"headl.rotate\" 0 55 0;"<<std::endl;
	(*filestream)<<"select -r headl ;"<<std::endl;
	(*filestream)<<"rotate -r -os 0 0 "<<m_fake_leg_angle<<";"<<std::endl;

	(*filestream)<<"setAttr \"headr.rotate\" 0 55 0;"<<std::endl;
	(*filestream)<<"select -r headr ;"<<std::endl;
	(*filestream)<<"rotate -r -os 0 0 "<<m_fake_leg_angle<<";"<<std::endl;
	
	//mouth
	(*filestream)<<"setAttr \"mouthl.rotate\" 0 "<<m_mouth_angle<<" 0;"<<std::endl;
	(*filestream)<<"setAttr \"mouthr.rotate\" 0 "<<m_mouth_angle<<" 0;"<<std::endl;
	(*filestream)<<"//END HEAD"<<std::endl;

	}
	else if(type == 1){
	// output for povray
	(*filestream)<<"#declare MillipedeHeadPart = union { \n"<<std::endl; 
	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_Center);
	m_Drawer->Rotate(m_rotation);
	m_Drawer->Translate(Eigen::Vector3f(-1.0,0,0));
	Eigen::Vector3f center;
	center = m_Drawer->CurrentOrigin();

	(*filestream)<<"sphere{<"<<center[0]<<","<<center[1]<<","<<center[2]<<">,"<<1<<"}"<<std::endl<<"scale<0.6,0.4,0.75>"<<std::endl;
	Cube::Output2File(filestream);
	(*filestream)<<"}\n"<<std::endl;

	}
}
