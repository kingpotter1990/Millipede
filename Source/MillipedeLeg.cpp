#include "MillipedeLeg.h"
#include "MillipedeSection.h"
#include "MillipedeHead.h"
#include "Millipede.h"
#include "Drawer.h"
#include "Terrain.h"


MillipedeLeg::MillipedeLeg(int a_l_r):m_l_r(a_l_r){

}

void MillipedeLeg::InitPhysics(Eigen::Vector3f a_body_size){
	
	double l0 = 0.039;double l1 = Eigen::Vector2f(1.411,0.275).norm();
	double l2 = 0.8*Eigen::Vector2f(1.49,0.682).norm(); double l3 = l2/4;
	m_segment_0_size = Eigen::Vector3f(0.2,0.2,l0);
	m_segment_1_size = Eigen::Vector3f(0.2,0.2,l1);
	m_segment_2_size = Eigen::Vector3f(0.15,0.15,l2);
	m_segment_3_size = Eigen::Vector3f(0.1,0.1,l3);

}

void MillipedeLeg::InitNeuroNet(MillipedeRigidSection* a_root){

	m_root = a_root;
	m_Drawer = m_root->m_Drawer;

	m_extreme_phi = 30;
	m_extreme_alpha = 30;
	m_extreme_beta = 30;

	double dif_phase = 30;//12 legs per cycle

	m_phi = m_extreme_phi* cos(DegreesToRadians*m_root->m_section_id*dif_phase);
    m_alpha = m_extreme_alpha * sin(DegreesToRadians*m_root->m_section_id*dif_phase);
	m_beta = m_extreme_beta/2;

	m_gamma = 0;//fixed

	m_leg_rotation_velocity = 1000;
	
	//init the previous and next and neighbor legs
	m_prev = NULL;
	m_next = NULL;
	m_neig = NULL;

	if(m_root->m_section_id == 1){
		//first section, no previous leg
		switch(m_l_r){
		case 1:
			m_prev = NULL;
			if(m_root->m_next)
				m_next = m_root->m_next->m_next->m_left_leg;
			m_neig = m_root->m_right_leg;
			break;
		case -1:
			m_prev = NULL;
			if(m_root->m_next)
				m_next = m_root->m_next->m_next->m_right_leg;
			m_neig = m_root->m_left_leg;
			break;
		}
	}
	else{
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
	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_root->m_Center);
	m_Drawer->Rotate(m_root->m_rotation);
	switch(m_l_r){
	case 1:
		m_Drawer->Translate(Eigen::Vector3f(0, -m_root->m_Size[1]/2, m_root->m_Size[2]/2));
		break;
	case -1:
		m_Drawer->Translate(Eigen::Vector3f(0, -m_root->m_Size[1]/2, -m_root->m_Size[2]/2));
		break;
	}


	m_Drawer->Translate(Eigen::Vector3f(0.0,-m_segment_0_size[2],0.0));
	m_Drawer->RotateY(-m_l_r*m_phi);
	m_Drawer->RotateX(-m_l_r*m_alpha);
	m_Drawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*m_segment_1_size[2]));
	m_Drawer->RotateX(m_l_r*m_beta);
	m_Drawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*m_segment_2_size[2]));
	m_Drawer->RotateX(m_l_r*m_gamma);
	m_Drawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*m_segment_3_size[2]));

	m_tip_position = m_Drawer->CurrentOrigin();
}

void MillipedeLeg::UpdateRootPosition(){
	//follow the rigid cube
	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_root->m_Center);
	m_Drawer->Rotate(m_root->m_rotation);
	switch(m_l_r){
	case 1:
		m_Drawer->Translate(Eigen::Vector3f(0, -m_root->m_Size[1]/2, m_root->m_Size[2]/2));
		break;
	case -1:
		m_Drawer->Translate(Eigen::Vector3f(0, -m_root->m_Size[1]/2, -m_root->m_Size[2]/2));
		break;
	}
	m_root_position = m_Drawer->CurrentOrigin();

}

void MillipedeLeg::Draw(int type, const Camera& camera, const Light& light){
	//assign different color for different state
	switch(m_leg_state){
	case LEG_SWAY_FORWARD_1:
	case LEG_SWAY_FORWARD_2:
		m_Drawer->SetColor(Eigen::Vector3f(0,1,0));
		break;
	case LEG_SWAY_BACKWARD_1:
	case LEG_SWAY_BACKWARD_2:
		m_Drawer->SetColor(Eigen::Vector3f(0,0,1));
		break;
	case LEG_STANCE:
		m_Drawer->SetColor(Eigen::Vector3f(1.0,0,0));
		break;
	case LEG_ADJUST:
		m_Drawer->SetColor(Eigen::Vector3f(1.0,1.0,0));
		break;
	}

  //  m_Drawer->SetColor(Eigen::Vector3f(1,1,1));
    
	//change the matrixes for all the primes based on the parameters
	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_root_position);
	m_Drawer->Rotate(m_root->m_rotation);

	m_Drawer->PushMatrix();
	m_Drawer->Scale(m_segment_0_size[0]*0.6);
	m_Drawer->DrawSphere(type, camera,  light);
	m_Drawer->PopMatrix();
	
	//cylinder 0
	m_Drawer->Translate(Eigen::Vector3f(0.0,-0.5*m_segment_0_size[2],0.0));
	m_Drawer->PushMatrix();
	m_Drawer->RotateX(90);
	m_Drawer->Scale(m_segment_0_size);
	m_Drawer->DrawCylinder(type, camera, light);
	m_Drawer->PopMatrix();

	//sphere 1
	m_Drawer->Translate(Eigen::Vector3f(0.0,-0.5*m_segment_0_size[2],0.0));
	m_Drawer->PushMatrix();
	m_Drawer->Scale(m_segment_0_size[0]*0.5);
	m_Drawer->DrawSphere(type, camera,  light);
	m_Drawer->PopMatrix();

	//cylinder 1
	m_Drawer->RotateY(-m_l_r*m_phi);
	m_Drawer->RotateX(-m_l_r*m_alpha);
	m_Drawer->Translate(Eigen::Vector3f(0.0,0.0,m_l_r*0.5*m_segment_1_size[2]));
	m_Drawer->PushMatrix();
	m_Drawer->Scale(m_segment_1_size);
	m_Drawer->DrawCylinder(type, camera, light);
	m_Drawer->PopMatrix();

	//sphere 2
	m_Drawer->Translate(Eigen::Vector3f(0.0,0.0, m_l_r*0.5*m_segment_1_size[2]));
	m_Drawer->PushMatrix();
	m_Drawer->Scale(m_segment_1_size[0]*0.5);
	m_Drawer->DrawSphere(type, camera,  light);
	m_Drawer->PopMatrix();

	//cylinder 2
	m_Drawer->RotateX(m_l_r*m_beta);
	m_Drawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*0.5*m_segment_2_size[2]));
	m_Drawer->PushMatrix();
	m_Drawer->Scale(m_segment_2_size);
	m_Drawer->DrawCylinder(type, camera, light);
	m_Drawer->PopMatrix();

	//sphere 3
	m_Drawer->Translate(Eigen::Vector3f(0.0,0.0, m_l_r*0.5*m_segment_2_size[2]));
	m_Drawer->PushMatrix();
	m_Drawer->Scale(m_segment_2_size[0]*0.5);
	m_Drawer->DrawSphere(type, camera,  light);
	m_Drawer->PopMatrix();

	//cone 0
	m_Drawer->RotateX(m_l_r*m_gamma);
	m_Drawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*0.5*m_segment_3_size[2]));
	m_Drawer->PushMatrix();
	m_Drawer->Scale(m_segment_3_size);
	if(m_l_r == 1)//left
		m_Drawer->RotateX(180);
	m_Drawer->DrawCone(type, camera, light);
	m_Drawer->PopMatrix();
}

void MillipedeLeg::UpdateSwitchNet(double a_dt){

	Eigen::Vector3f target_rotation(m_target_phi, m_target_alpha, m_target_beta);
	Eigen::Vector3f dif;
	dif = target_rotation - Eigen::Vector3f(m_phi, m_alpha, m_beta);
	
	switch(m_leg_state){
	
		//check leg status, update the leg status when reach Extreme position or leg tip touch ground
		case LEG_SWAY_FORWARD_1:
		
			if(dif.norm() < 2*m_leg_rotation_velocity*a_dt){
				EnterSwayForward2();
				//m_neig->EnterSwayForward2();
			}
			break;
			

		case LEG_SWAY_FORWARD_2:
			
			if(m_root->m_terrain->TestInside(m_tip_position)){//tip tapping the ground, TODO test against terrain
				EnterStance();
			}
			else if(dif.norm() < 2*m_leg_rotation_velocity*a_dt)
					EnterSwayBackward1();
			break;
			
        case LEG_SWAY_BACKWARD_1:
			if(dif.norm() < 2*m_leg_rotation_velocity*a_dt)
			{
				EnterSwayBackward2();
			}
			else if(m_root->m_terrain->TestInside(m_tip_position)){//tip tapping the ground TODO test against terrain
				EnterStance();
			}
			
			break;
		case LEG_SWAY_BACKWARD_2:

			if(m_root->m_terrain->TestInside(m_tip_position)){//tip tapping the ground TODO test against terrain
				EnterStance();
			}
			else if(dif.norm() < 2*m_leg_rotation_velocity*a_dt){//reach extreme position
				EnterSwayForward1();
				//m_neig->EnterSwayForward1();
			}

			break;                

		case LEG_ADJUST:
			if(dif.norm() < 2*m_leg_rotation_velocity*a_dt)
				EnterSwayForward2();
			
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

	if(m_prev)
		m_leg_rotation_velocity = m_prev->m_leg_rotation_velocity;//speed is defined by head and propogated down

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
		//update
		m_phi = result[0];
		m_alpha = result[1];
		m_beta = result[2];
	}
	else{
	//InverseKinematics can not resolve
		EnterSwayForward1();
		//sync the neigbor leg
		m_neig->EnterSwayForward1();
	}
	
	//the stance net results in the inverse kinematics of desired physics configuration
}

void MillipedeLeg::UpdateNeuronNet(double a_dt){
	//central part for leg locomotion
	
	//leg rotation speed synced with linear speed
	//maintain the time ratio of body section in supported/free mode;

	UpdateSwitchNet(a_dt);//manage the switch of different modes
	switch (m_leg_state) {
        case LEG_SWAY_FORWARD_1:
		case LEG_SWAY_FORWARD_2:
		case LEG_SWAY_BACKWARD_1:
		case LEG_SWAY_BACKWARD_2:
		case LEG_ADJUST:
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

	if(m_next){
		m_next->EnterAdjust();
	}

}

void MillipedeLeg::EnterAdjust(){
	//adjust the leg so that the phase lag between previous leg is maintained

	//ajustable to form different size wave
	//change the m_target_phi to change the dif phase of stanced legs
	m_target_phi = m_extreme_phi*0.8;
	m_target_alpha = m_extreme_alpha*0.2;
	m_target_beta = m_extreme_beta/2;

	m_leg_state = LEG_ADJUST;

	UpdateSpeedNet();


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
		
	//forward Down
	m_target_phi = m_extreme_phi;
	
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
	m_Drawer->SetIdentity();
	m_Drawer->Rotate(m_root->m_rotation);
	m_Drawer->RotateY(-m_l_r*phi);
	Leg_Plane_N = (m_Drawer->m_transformation).linear()*Eigen::Vector3f(-1,0,0);
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
	m_Drawer->SetIdentity();
	m_Drawer->Rotate(m_root->m_rotation);
	m_Drawer->RotateY(-m_l_r*phi);
	H = m_Drawer->m_transformation.linear()*Eigen::Vector3f(0,0,-m_l_r*1);
	H.normalize();
	psi = acos(H.dot(M-O)/(M-O).norm())/DegreesToRadians;

	ksi = atan(l3*sin(DegreesToRadians*gamma)/(l3*cos(DegreesToRadians*gamma) + l2))/DegreesToRadians;

	beta = psi - ksi + alpha;

	
	if(beta < 0)
		return false;

	a_result = Eigen::Vector3f(phi, alpha, beta);

	return true;

}

void MillipedeLeg::Output2File(std::ofstream* filestream){
	

	Eigen::Vector3f point_a, point_b, center; double radius;
	(*filestream)<<"//BEGIN LEG "<<m_l_r<<std::endl;

	m_Drawer->SetIdentity();
	m_Drawer->Translate(m_root_position);
	m_Drawer->Rotate(m_root->m_rotation);

	//segment 0
	center = m_root_position;
	radius = m_segment_0_size[0]*0.6;
	(*filestream)<<"//BEGIN SPHERE "<<std::endl;
	(*filestream)<<"sphere{<"<<center[0]<<","<<center[1]<<","<<center[2]<<">,"<<radius<<"}"<<std::endl;
	(*filestream)<<"//END SPHERE "<<std::endl;

	m_Drawer->Translate(Eigen::Vector3f(0.0,-m_segment_0_size[2],0.0));

	point_a = m_root_position;
	point_b = m_Drawer->CurrentOrigin();
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
	
	m_Drawer->RotateY(-m_l_r*m_phi);
	m_Drawer->RotateX(-m_l_r*m_alpha);
	m_Drawer->Translate(Eigen::Vector3f(0.0,0.0,m_l_r*m_segment_1_size[2]));
	point_a = point_b;
	point_b = m_Drawer->CurrentOrigin();
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

	m_Drawer->RotateX(m_l_r*m_beta);
	m_Drawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*m_segment_2_size[2]));
	point_a = point_b;
	point_b = m_Drawer->CurrentOrigin();
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

	m_Drawer->RotateX(m_l_r*m_gamma);
	m_Drawer->Translate(Eigen::Vector3f(0.0, 0.0, m_l_r*m_segment_3_size[2]));
	point_a = point_b;
	point_b = m_Drawer->CurrentOrigin();
	radius = m_segment_3_size[0]*0.5;
	(*filestream)<<"//BEGIN CONE "<<std::endl;
	(*filestream)<<"cone{"<<"<"<<point_a[0]<<","<<point_a[1]<<","<<point_a[2]<<">,"<<radius<<std::endl;
	(*filestream)<<"<"<<point_b[0]<<","<<point_b[1]<<","<<point_b[2]<<">,0}"<<std::endl;
	(*filestream)<<"//END CONE "<<std::endl;

	(*filestream)<<"//END LEG "<<m_l_r<<std::endl;
}
