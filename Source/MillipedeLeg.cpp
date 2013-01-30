#include "MillipedeLeg.h"
#include "MillipedeSection.h"
#include "MillipedeHead.h"
#include "Millipede.h"
#include "Drawer.h"
#include "Terrain.h"

extern Drawer* myDrawer;

MillipedeLeg::MillipedeLeg(int a_l_r):m_l_r(a_l_r){

}

void MillipedeLeg::InitPhysics(Eigen::Vector3f a_body_size){
	
	m_segment_0_size = a_body_size[0]*Eigen::Vector3f(0.4,0.4,0.5);
	m_segment_1_size = a_body_size[0]*Eigen::Vector3f(0.4,0.4,2.0);
	m_segment_2_size = a_body_size[0]*Eigen::Vector3f(0.25,0.25,1.5);
	m_segment_3_size = a_body_size[0]*Eigen::Vector3f(0.2,0.2,0.5);

}

void MillipedeLeg::InitNeuroNet(MillipedeRigidSection* a_root){

	m_root = a_root;

	m_extreme_phi = 30;
	m_extreme_alpha = 30;
	m_extreme_beta = 30;

	double dif_phase = 30;//12 legs per cycle

	m_phi = m_extreme_phi* cos(DegreesToRadians*m_root->m_section_id*dif_phase);
    m_alpha = m_extreme_alpha * sin(DegreesToRadians*m_root->m_section_id*dif_phase);
	m_beta = m_extreme_beta/2;

	m_gamma = 10;//fixed

	m_leg_rotation_velocity = 1000;
	m_dif_phi_tolerance = 5000;//disabled by setting it very big
	
	
	//init the previous and next and neighbor legs
	m_prev = NULL;
	m_next = NULL;
	m_neig = NULL;

	switch(m_l_r){
	case 1:
		if(m_root->m_prev)
			m_prev = m_root->m_prev->m_prev->m_left_leg;
		if(m_root->m_next)
			m_next = m_root->m_next->m_next->m_left_leg;
		m_neig = m_root->m_right_leg;
		break;
	case -1:
		if(m_root->m_prev)
			m_prev = m_root->m_prev->m_prev->m_right_leg;
		if(m_root->m_next)
			m_next = m_root->m_next->m_next->m_right_leg;
		m_neig = m_root->m_left_leg;
		break;
	}

	//set leg initial states
	if(m_phi < 0 && m_alpha >= 0){
		EnterSwayForward1();
	}
	else if(m_phi < 0 && m_alpha < 0){
		EnterSwayBackward2();
	}
	else if(m_phi >= 0 && m_alpha >= 0){
		EnterSwayForward2();
	}
	else if(m_phi >= 0 && m_alpha < 0){
		EnterSwayBackward1();
	}

	UpdateRootPosition();
	UpdateTipPosition();//the order is important;
}

double MillipedeLeg::GetBalanceHeight(){
//the height root when at balanced static position;
	return m_segment_0_size[2] + m_segment_2_size[2]*sin(m_extreme_beta*0.5) 
		+ m_segment_3_size[2]*sin(m_extreme_beta*0.5 + m_gamma);
}

void MillipedeLeg::UpdateTipPosition(){
	
	if(m_leg_state == LEG_STANCE)
		return;//no update
	 
	//go to root position
	myDrawer->SetIdentity();
	myDrawer->Translate(m_root->m_Center);
	myDrawer->Rotate(m_root->m_rotation);
	switch(m_l_r){
	case 1:
		myDrawer->Translate(Eigen::Vector3f(0, -m_root->m_Size[1]/2, m_root->m_Size[2]/4));
		break;
	case -1:
		myDrawer->Translate(Eigen::Vector3f(0, -m_root->m_Size[1]/2, -m_root->m_Size[2]/4));
		break;
	}


	myDrawer->Translate(Eigen::Vector3f(0.0,-m_segment_0_size[2],0.0));
	myDrawer->RotateY(-m_l_r*m_phi);
	myDrawer->RotateX(-m_l_r*m_alpha);
	myDrawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*m_segment_1_size[2]));
	myDrawer->RotateX(m_l_r*m_beta);
	myDrawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*m_segment_2_size[2]));
	myDrawer->RotateX(m_l_r*m_gamma);
	myDrawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*m_segment_3_size[2]));

	m_tip_position = myDrawer->CurrentOrigin();
}

void MillipedeLeg::UpdateRootPosition(){

	//follow the rigid cube
	myDrawer->SetIdentity();
	myDrawer->Translate(m_root->m_Center);
	myDrawer->Rotate(m_root->m_rotation);
	switch(m_l_r){
	case 1:
		myDrawer->Translate(Eigen::Vector3f(0, -m_root->m_Size[1]/2, m_root->m_Size[2]/4));
		break;
	case -1:
		myDrawer->Translate(Eigen::Vector3f(0, -m_root->m_Size[1]/2, -m_root->m_Size[2]/4));
		break;
	}
	m_root_position = myDrawer->CurrentOrigin();

}

void MillipedeLeg::Draw(int type, const Camera& camera, const Light& light){
	//assign different color for different state
	switch(m_leg_state){
	case LEG_SWAY_FORWARD_1:
	case LEG_SWAY_FORWARD_2:
		myDrawer->SetColor(Eigen::Vector3f(0,1,0));
		break;
	case LEG_SWAY_BACKWARD_1:
	case LEG_SWAY_BACKWARD_2:
		myDrawer->SetColor(Eigen::Vector3f(0,0,1));
		break;
	case LEG_STANCE:
		myDrawer->SetColor(Eigen::Vector3f(1.0,0,0));
		break;
	}

  //  myDrawer->SetColor(Eigen::Vector3f(1,1,1));
    
	//change the matrixes for all the primes based on the parameters
	myDrawer->SetIdentity();
	myDrawer->Translate(m_root_position);
	myDrawer->Rotate(m_root->m_rotation);

	myDrawer->PushMatrix();
	myDrawer->Scale(m_segment_0_size[0]*0.6);
	myDrawer->DrawSphere(type, camera,  light);
	myDrawer->PopMatrix();
	
	//cylinder 0
	myDrawer->Translate(Eigen::Vector3f(0.0,-0.5*m_segment_0_size[2],0.0));
	myDrawer->PushMatrix();
	myDrawer->RotateX(90);
	myDrawer->Scale(m_segment_0_size);
	myDrawer->DrawCylinder(type, camera, light);
	myDrawer->PopMatrix();

	//sphere 1
	myDrawer->Translate(Eigen::Vector3f(0.0,-0.5*m_segment_0_size[2],0.0));
	myDrawer->PushMatrix();
	myDrawer->Scale(m_segment_0_size[0]*0.5);
	myDrawer->DrawSphere(type, camera,  light);
	myDrawer->PopMatrix();

	//cylinder 1
	myDrawer->RotateY(-m_l_r*m_phi);
	myDrawer->RotateX(-m_l_r*m_alpha);
	myDrawer->Translate(Eigen::Vector3f(0.0,0.0,m_l_r*0.5*m_segment_1_size[2]));
	myDrawer->PushMatrix();
	myDrawer->Scale(m_segment_1_size);
	myDrawer->DrawCylinder(type, camera, light);
	myDrawer->PopMatrix();

	//sphere 2
	myDrawer->Translate(Eigen::Vector3f(0.0,0.0, m_l_r*0.5*m_segment_1_size[2]));
	myDrawer->PushMatrix();
	myDrawer->Scale(m_segment_1_size[0]*0.5);
	myDrawer->DrawSphere(type, camera,  light);
	myDrawer->PopMatrix();

	//cylinder 2
	myDrawer->RotateX(m_l_r*m_beta);
	myDrawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*0.5*m_segment_2_size[2]));
	myDrawer->PushMatrix();
	myDrawer->Scale(m_segment_2_size);
	myDrawer->DrawCylinder(type, camera, light);
	myDrawer->PopMatrix();

	//sphere 3
	myDrawer->Translate(Eigen::Vector3f(0.0,0.0, m_l_r*0.5*m_segment_2_size[2]));
	myDrawer->PushMatrix();
	myDrawer->Scale(m_segment_2_size[0]*0.5);
	myDrawer->DrawSphere(type, camera,  light);
	myDrawer->PopMatrix();

	//cone 0
	myDrawer->RotateX(m_l_r*m_gamma);
	myDrawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*0.5*m_segment_3_size[2]));
	myDrawer->PushMatrix();
	myDrawer->Scale(m_segment_3_size);
	if(m_l_r == 1)//left
		myDrawer->RotateX(180);
	myDrawer->DrawCone(type, camera, light);
	myDrawer->PopMatrix();
}

void MillipedeLeg::UpdateSwitchNet(double a_dt){

	Eigen::Vector3f target_rotation(m_target_phi, m_target_alpha, m_target_beta);
	Eigen::Vector3f dif;
	dif = target_rotation - Eigen::Vector3f(m_phi, m_alpha, m_beta);
	
	switch(m_leg_state){
	
		//check leg status, update the leg status when reach Extreme position or leg tip touch ground
		case LEG_SWAY_FORWARD_1:
			{
			if(dif.norm() < 2*m_leg_rotation_velocity*a_dt){
				EnterSwayForward2();
				//m_neig->EnterSwayForward2();
			}
			break;
			}

		case LEG_SWAY_FORWARD_2:
			{
			if(m_tip_position[1] < m_root->m_terrain->GetHeight(m_tip_position[0],m_tip_position[2])){//tip tapping the ground, TODO test against terrain
				EnterStance();
			}
			else if(dif.norm() < 2*m_leg_rotation_velocity*a_dt)
					EnterSwayBackward1();
			else if(m_prev)
				if((m_phi - m_prev->m_phi) > m_dif_phi_tolerance){//collision avoidance
					
					EnterSwayBackward1();

					if(m_prev->m_leg_state == LEG_SWAY_BACKWARD_1||m_prev->m_leg_state == LEG_SWAY_BACKWARD_2){
						m_prev->EnterSwayForward1();
						//m_prev->m_neig->EnterSwayForward1();
					}
				}
			break;
			}
        case LEG_SWAY_BACKWARD_1:{
			if(dif.norm() < 2*m_leg_rotation_velocity*a_dt)
			{
				EnterSwayBackward2();
			}
			else if(m_tip_position[1] < m_root->m_terrain->GetHeight(m_tip_position[0],m_tip_position[2])){//tip tapping the ground TODO test against terrain
				EnterStance();
			}
								 }
			break;
		case LEG_SWAY_BACKWARD_2:{

			if(m_tip_position[1] < m_root->m_terrain->GetHeight(m_tip_position[0],m_tip_position[2])){//tip tapping the ground TODO test against terrain
				EnterStance();
			}
			else if(dif.norm() < 2*m_leg_rotation_velocity*a_dt){//reach extreme position
				EnterSwayForward1();
				//m_neig->EnterSwayForward1();
			}
			else if(m_next)
				if((m_next->m_phi - m_phi) > m_dif_phi_tolerance){//collision avoidance
	
					EnterSwayForward1();
					m_neig->EnterSwayForward1();

					if(m_next->m_leg_state == LEG_SWAY_FORWARD_1||m_next->m_leg_state == LEG_SWAY_FORWARD_2){
						m_next->EnterSwayBackward1();
					}
				}
			
			break;
								 }
                
		case LEG_STANCE:{

			}
			break;
	}
}

void MillipedeLeg::UpdateSwingNet(double a_dt){

	//swing to the target rotation

	if(m_target_phi > m_phi)
		m_phi += m_phi_velocity*a_dt;
	else
		m_phi -= m_phi_velocity*a_dt;
	
	if(m_target_alpha > m_alpha)
		m_alpha += m_alpha_velocity*a_dt;
	else
		m_alpha -= m_alpha_velocity*a_dt;
	
	if(m_target_beta > m_beta)
		m_beta += m_beta_velocity*a_dt;
	else
		m_beta -= m_beta_velocity*a_dt;
}

void MillipedeLeg::UpdateSpeedNet(){

	//adjust the speed of rotation
	Eigen::Vector3f target_rotation(m_target_phi, m_target_alpha, m_target_beta);
	Eigen::Vector3f dif;
	dif = target_rotation - Eigen::Vector3f(m_phi, m_alpha, m_beta);
	m_phi_velocity = m_leg_rotation_velocity*fabs(dif[0])/dif.norm();
	m_alpha_velocity = m_leg_rotation_velocity*fabs(dif[1])/dif.norm();
	m_beta_velocity = m_leg_rotation_velocity*fabs(dif[2])/dif.norm();
}

void MillipedeLeg::UpdateStanceNet(double a_dt){
	
	Eigen::Vector3f result ;
	//TODO: narrow the solution space for inversekinematics
	if(InverseKinematics(m_tip_position, m_root_position, result))
	{
		/*
		if((Eigen::Vector3f(m_phi,m_alpha,m_beta) - result).norm() > a_dt*1000)
		{
			if(m_root->m_section_id == 1&& m_l_r == 1)
			std::cout<<'\n'<<m_root->m_section_id<<"\n OLD: "<<m_phi<<","<<m_alpha<<","<<m_beta<<
			"\n NEW"<<result[0]<<","<<result[1]<<","<<result[2]<<std::endl;
		}
		*/
		//update
		m_phi = result[0];
		m_alpha = result[1];
		m_beta = result[2];
	}
	else{
	//InverseKinematics can not resolve
		EnterSwayForward1();
	}
	
	//the stance net results in the inverse kinematics of desired physics configuration
}

void MillipedeLeg::UpdateNeuronNet(double a_dt){
	//central part for leg locomotion
	
	//leg rotation speed synced with linear speed
	//maintain the time ratio of body section in supported/free mode;

	m_leg_rotation_velocity = 70*m_root->m_master->m_head->GetLinearSpeed();

	UpdateSwitchNet(a_dt);//manage the switch of different modes
	switch (m_leg_state) {
        case LEG_SWAY_FORWARD_1:
		case LEG_SWAY_FORWARD_2:
		case LEG_SWAY_BACKWARD_1:
		case LEG_SWAY_BACKWARD_2:
			UpdateSwingNet(a_dt);
			break;
        case LEG_STANCE:
			UpdateStanceNet(a_dt);
			break;
	}
}

void MillipedeLeg::UpdateAll(double a_dt){	
	
	UpdateRootPosition();
	UpdateTipPosition();//the order is important;
	UpdateNeuronNet(a_dt);
}

void MillipedeLeg::EnterStance(){
	//ground contact on the left leg
	m_leg_state = LEG_STANCE;
}

void MillipedeLeg::EnterSwayBackward1(){
	if(m_leg_state == LEG_SWAY_BACKWARD_1)
		return;
	//lifting Down
	m_target_phi = 0;
	m_target_alpha = - m_extreme_alpha;
	m_target_beta = m_extreme_beta/2;

	m_leg_state = LEG_SWAY_BACKWARD_1;

	UpdateSpeedNet();
}

void MillipedeLeg::EnterSwayForward1(){
	//both leg should leave the stance mode and enter sway forward mode at the same time

  	if(m_leg_state == LEG_SWAY_FORWARD_1)
		return;
	
	//Lifting Up
	m_target_phi = m_phi;
	m_target_alpha = m_extreme_alpha;
	m_target_beta = m_extreme_beta/2;

	m_leg_state = LEG_SWAY_FORWARD_1;

	UpdateSpeedNet();
} 

void MillipedeLeg::EnterSwayBackward2(){
	if(m_leg_state == LEG_SWAY_BACKWARD_2)
		return;

	m_target_phi = -m_extreme_phi;
	m_target_alpha = 0;
	m_target_beta = m_extreme_beta/2;

	m_leg_state = LEG_SWAY_BACKWARD_2;

	UpdateSpeedNet();
}

void MillipedeLeg::EnterSwayForward2(){

	if(m_leg_state == LEG_SWAY_FORWARD_2)
		return;
		
	double phase_lag = 20;
	//forward Down
	m_target_phi = m_extreme_phi;
	//query previous leg, if previous leg in stance mode, then target the previous leg's landing point
	if(m_prev)
		if(m_prev->m_leg_state == LEG_STANCE)
		{
#ifdef __linux__
                    m_target_phi = std::min(m_prev->m_phi + phase_lag, m_extreme_phi);
#else
			m_target_phi = min(m_prev->m_phi + phase_lag, m_extreme_phi);
#endif
		}
	m_target_alpha = 0;
	m_target_beta = m_extreme_beta/2;
	
	m_leg_state = LEG_SWAY_FORWARD_2;

	UpdateSpeedNet();
} 

bool MillipedeLeg::InverseKinematics(Eigen::Vector3f a_tip, Eigen::Vector3f a_root, Eigen::Vector3f& a_result){
	//based on current rigid section configuration, get the rotations
	double phi;
	//first update the phi
	Eigen::Vector3f legv;
	legv = Eigen::Transpose<Eigen::Matrix3f>(m_root->m_rotation)*(a_tip - a_root);

	if(m_l_r == 1)
		phi = atan(-legv[0]/legv[2])/DegreesToRadians;//rotation in y
	else
		phi = atan(legv[0]/legv[2])/DegreesToRadians;//rotation in y

	
	if(fabs(phi) > m_extreme_phi*1.05){
		return false;
	}
	

	//second update the alpha and beta
	// to ensure solution, the length of the three leg segments need to have constraints
	double a,b, d, r1,r2, l0, l1, l2,l3, alpha, beta, gamma, psi, ksi;
	Eigen::Vector3f O,P,D,N,V,M;
	Eigen::Vector3f Leg_Plane_N;

	l0 = m_segment_0_size[2];
	l1 = m_segment_1_size[2];
	l2 = m_segment_2_size[2];
	l3 = m_segment_3_size[2];
	gamma = m_gamma;

	P = a_root + m_root->m_rotation*Eigen::Vector3f(0,- m_segment_0_size[2],0);//the position of the new root

	O = a_tip;
	D = P - O;	
	d = D.norm();
	D.normalize();
	//get the leg plane normal
	myDrawer->SetIdentity();
	myDrawer->Rotate(m_root->m_rotation);
	myDrawer->RotateY(-m_l_r*phi);
	Leg_Plane_N = (myDrawer->m_transformation).linear()*Eigen::Vector3f(-1,0,0);
	//get the vector normal to D on the plane
	N = m_l_r*D.cross(Leg_Plane_N);
	N.normalize();

	r1 = sqrt(l2*l2 + l3*l3 - 2*l3*l2*cos(DegreesToRadians*(180-gamma)));
	r2 = l1;

	if(r1+r2<d)
	{
		//inverse kinetmatics can not resolve!!!, leave stance state
		return false;
	}

	a = (d*d + r1*r1 - r2*r2)/(2*d);
	b = (d*d - r1*r1 + r2*r2)/(2*d);

	V = D*a + sqrt(r1*r1 - a*a)*N;
	M = O + V;//the joint position

	//get alpha
	Eigen::Vector3f UP;
	UP = m_root->m_rotation*Eigen::Vector3f(0,1,0);
	UP.normalize();

	double temp;
	temp = UP.dot(M-P)/(M-P).norm();
	alpha = asin(temp)/DegreesToRadians;
	
	//if(fabs(alpha) > m_extreme_alpha)
		//return false;
	Eigen::Vector3f H;
	myDrawer->SetIdentity();
	myDrawer->Rotate(m_root->m_rotation);
	myDrawer->RotateY(-m_l_r*phi);
	H = myDrawer->m_transformation.linear()*Eigen::Vector3f(0,0,-m_l_r*1);
	H.normalize();
	psi = acos(H.dot(M-O)/(M-O).norm())/DegreesToRadians;

	ksi = atan(l3*sin(DegreesToRadians*gamma)/(l3*cos(DegreesToRadians*gamma) + l2))/DegreesToRadians;

	beta = psi - ksi + alpha;

	
	if(beta < 0)
		return false;

	a_result = Eigen::Vector3f(phi, alpha, beta);

	/*
	if(fabs(phi - m_phi) > 5)
		if(m_root->m_section_id == 1&& m_l_r == 1){	
			std::cout<<"\n Phi: "<<m_phi<<"-> "<<phi<<std::endl;
			std::cout<<"\n Alpha: "<<m_alpha<<"-> "<<alpha<<std::endl;
			std::cout<<"\n Beta: "<<m_beta<<"-> "<<beta<<std::endl;

		}
	*/
	return true;
	//assign
	//check whether extreme, if reach extreme of leg rotations, give up, enter swayfroward state
	//if(fabs(alpha) < 40&& beta > 0 && beta<40){
		//m_alpha = alpha;
		//m_beta = beta;
	//}
	//else{
	//	EnterSwayForward();
	//	return;
	//}

}

void MillipedeLeg::Output2File(std::ofstream* filestream){


	Eigen::Vector3f point_a, point_b, center; double radius;
	(*filestream)<<"//BEGIN LEG "<<m_l_r<<std::endl;

	myDrawer->SetIdentity();
	myDrawer->Translate(m_root_position);
	myDrawer->Rotate(m_root->m_rotation);

	//segment 0
	center = m_root_position;
	radius = m_segment_0_size[0]*0.6;
	(*filestream)<<"//BEGIN SPHERE "<<std::endl;
	(*filestream)<<"sphere{<"<<center[0]<<","<<center[1]<<","<<center[2]<<">,"<<radius<<"}"<<std::endl;
	(*filestream)<<"//END SPHERE "<<std::endl;

	myDrawer->Translate(Eigen::Vector3f(0.0,m_segment_0_size[2],0.0));

	point_a = m_root_position;
	point_b = myDrawer->CurrentOrigin();
	radius = m_segment_0_size[0]*0.5;
	(*filestream)<<"//BEGIN CYLINDER "<<std::endl;
	(*filestream)<<"cylinder{"<<"<"<<point_a[0]<<","<<point_a[1]<<","<<point_a[2]<<">,<"<<point_b[0]<<","<<point_b[1]<<","<<point_b[2]<<">,"<<radius<<"}"<<std::endl;
	(*filestream)<<"//END CYLINDER "<<std::endl;

	//segment 1
	center = point_b;
	radius = m_segment_0_size[0]*0.5;
	(*filestream)<<"//BEGIN SPHERE "<<std::endl;
	(*filestream)<<"sphere{<"<<center[0]<<","<<center[1]<<","<<center[2]<<">,"<<radius<<"}"<<std::endl;
	(*filestream)<<"//END SPHERE "<<std::endl;
	
	myDrawer->RotateY(-m_l_r*m_phi);
	myDrawer->RotateX(-m_l_r*m_alpha);
	myDrawer->Translate(Eigen::Vector3f(0.0,0.0,m_l_r*m_segment_1_size[2]));
	point_a = point_b;
	point_b = myDrawer->CurrentOrigin();
	radius = m_segment_1_size[0]*0.5;
	(*filestream)<<"//BEGIN CYLINDER "<<std::endl;
	(*filestream)<<"cylinder{"<<"<"<<point_a[0]<<","<<point_a[1]<<","<<point_a[2]<<">,<"<<point_b[0]<<","<<point_b[1]<<","<<point_b[2]<<">,"<<radius<<"}"<<std::endl;
	(*filestream)<<"//END CYLINDER "<<std::endl;
	
	//segment 2
	center = point_b;
	radius = m_segment_1_size[0]*0.5;
	(*filestream)<<"//BEGIN SPHERE "<<std::endl;
	(*filestream)<<"sphere{<"<<center[0]<<","<<center[1]<<","<<center[2]<<">,"<<radius<<"}"<<std::endl;
	(*filestream)<<"//END SPHERE "<<std::endl;

	myDrawer->RotateX(m_l_r*m_beta);
	myDrawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*m_segment_2_size[2]));
	point_a = point_b;
	point_b = myDrawer->CurrentOrigin();
	radius = m_segment_2_size[0]*0.5;
	(*filestream)<<"//BEGIN CYLINDER "<<std::endl;
	(*filestream)<<"cylinder{"<<"<"<<point_a[0]<<","<<point_a[1]<<","<<point_a[2]<<">,<"<<point_b[0]<<","<<point_b[1]<<","<<point_b[2]<<">,"<<radius<<"}"<<std::endl;
	(*filestream)<<"//END CYLINDER "<<std::endl;

	//segment 3
	center = point_b;
	radius = m_segment_2_size[0]*0.5;
	(*filestream)<<"//BEGIN SPHERE "<<std::endl;
	(*filestream)<<"sphere{<"<<center[0]<<","<<center[1]<<","<<center[2]<<">,"<<radius<<"}"<<std::endl;
	(*filestream)<<"//END SPHERE "<<std::endl;

	myDrawer->RotateX(m_l_r*m_gamma);
	myDrawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*m_segment_3_size[2]));
	point_a = point_b;
	point_b = myDrawer->CurrentOrigin();
	radius = m_segment_3_size[0]*0.5;
	(*filestream)<<"//BEGIN CONE "<<std::endl;
	(*filestream)<<"cone{"<<"<"<<point_a[0]<<","<<point_a[1]<<","<<point_a[2]<<">,"<<radius<<std::endl;
	(*filestream)<<"<"<<point_b[0]<<","<<point_b[1]<<","<<point_b[2]<<">,0}"<<std::endl;
	(*filestream)<<"//END CONE "<<std::endl;

	(*filestream)<<"//END LEG "<<m_l_r<<std::endl;
}
