#include "Millipede.h"
#include "Drawer.h"

extern Drawer* myDrawer;

MillipedeLeg::MillipedeLeg(int a_l_r):m_l_r(a_l_r){

}

void MillipedeLeg::InitPhysics(Eigen::Vector3f a_body_size){
	
	m_segment_0_size = a_body_size[0]*Eigen::Vector3f(0.2,0.2,0.5);
	m_segment_1_size = a_body_size[0]*Eigen::Vector3f(0.2,0.2,1.5);
	m_segment_2_size = a_body_size[0]*Eigen::Vector3f(0.15,0.15,1.0);
	m_segment_3_size = a_body_size[0]*Eigen::Vector3f(0.12,0.12,0.5);

	m_extreme = 50;
	m_phi = 1*Util::getRand()*m_extreme - m_extreme;
    m_alpha = 10*(Util::getRand() - 0.5);
	m_beta = 20;
	m_gamma = 30;

	m_leg_rotation_velocity = 500;

	m_dif_phi_tolerance = 5000;//diabled

}

void MillipedeLeg::InitNeuroNet(MillipedeRigidSection* a_root){

	m_root = a_root;
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

	m_leg_state = m_root->m_leg_state;

	UpdateRootPosition();
	UpdateTipPosition();//the order is important;

	switch(m_leg_state){
	case LEG_SWAY_FORWARD_1:
		EnterSwayForward();
		break;
	case LEG_SWAY_BACKWARD_1:
		EnterSwayBackward();
		break;
	}
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

    myDrawer->SetColor(Eigen::Vector3f(1,1,1));
    
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

	double a_prev_phi;
	if(m_prev)
		a_prev_phi = m_prev->m_phi;
	else
		a_prev_phi = 100;

	double a_next_phi;
	if(m_next)
		a_next_phi = m_next->m_phi;
	else
		a_next_phi = -100;

	switch(m_leg_state){
			//check leg status, update the leg status when reach Extreme position or leg tip touch ground
		case LEG_SWAY_FORWARD_1:
			{
			Eigen::Vector3f target_rotation(m_target_phi, m_target_alpha, m_target_beta);
			Eigen::Vector3f dif;
			dif = target_rotation - Eigen::Vector3f(m_phi, m_alpha, m_beta);

			if(dif.norm() < 2*m_leg_rotation_velocity*a_dt){
				UpdateTargetNet(3);//enter phase 2
				m_leg_state = LEG_SWAY_FORWARD_2;
			}
			
			}
			break;
		case LEG_SWAY_FORWARD_2:
			if(m_tip_position[1] < 0){//tip tapping the ground
				EnterStance();
			}
			else if(m_phi > m_extreme){//reach the extreme position

				EnterSwayBackward();
			}
			else if((m_phi - a_prev_phi) > m_dif_phi_tolerance){//collision avoidance
				//this sometimes  result in wave pattern
				if(m_leg_state!=LEG_STANCE)
					EnterSwayBackward();
				if(m_prev){
					if(m_prev->m_leg_state == LEG_SWAY_BACKWARD_1||m_prev->m_leg_state == LEG_SWAY_BACKWARD_2)
						m_prev->EnterSwayForward();
				}
				
			}
			
			break;
        case LEG_SWAY_BACKWARD_1:
			{
			Eigen::Vector3f target_rotation(m_target_phi, m_target_alpha, m_target_beta);
			Eigen::Vector3f dif;
			dif = target_rotation - Eigen::Vector3f(m_phi, m_alpha, m_beta);

			if(dif.norm() < 2*m_leg_rotation_velocity*a_dt)
			{
				UpdateTargetNet(4);//enter phase 2
				m_leg_state = LEG_SWAY_BACKWARD_2;
			}
			}

		case LEG_SWAY_BACKWARD_2:
			if(m_tip_position[1] < 0){//tip tapping the ground
				EnterStance();
			}
			else if(m_phi < -m_extreme){//reach extreme position
				EnterSwayForward();
			}
			
			else if((a_next_phi - m_phi) > m_dif_phi_tolerance){//collision avoidance
				if(m_leg_state!=LEG_STANCE)
					EnterSwayForward();
				if(m_next){
					if(m_prev->m_leg_state == LEG_SWAY_FORWARD_1||m_prev->m_leg_state == LEG_SWAY_FORWARD_2)
						m_prev->EnterSwayForward();
					m_next->EnterSwayBackward();
				}
			} 
			
            break;
                
		case LEG_STANCE:

			if(m_phi  < -m_extreme){//reach extreme position
				EnterSwayForward();
			}
			if(m_phi > m_extreme){
				EnterSwayBackward();
			}
		}
}

void MillipedeLeg::UpdateSFowardNet(double a_dt){


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

void MillipedeLeg::UpdateSBackwardNet(double a_dt){

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

void MillipedeLeg::UpdateTargetNet(int a_code){
	//code : 1, Enter sway forward phase 1
	//code : 2, Enter sway backward phase 1
	//code : 3, Enter sway forward phase 2
	//code : 4, Enter sway backward phase 2

	switch(a_code){
	case 1:
		{

			//get the target middle point
			Eigen::Vector3f middle_point;
			//magical middle point
			middle_point = Eigen::Vector3f(m_tip_position[0],m_root_position[1],m_tip_position[2]);
		

			Eigen::Vector3f result;
			if(InverseKinematics(middle_point, m_root_position, result))
			{
				m_target_phi = result[0];
				m_target_alpha = result[1];
				m_target_beta = result[2];
			}
			else{
				m_target_phi = m_phi;
				m_target_alpha = m_alpha + 40;
				m_target_beta = m_beta;
			}
		}
		break;
	case 2:

		m_target_phi = (m_phi - m_extreme)/2;
		m_target_alpha = -20;
		m_target_beta = 20;
		break;
	case 3:

		m_target_phi = m_extreme;
		m_target_alpha = 0;
		m_target_beta = 20;
		break;
	case 4:


		m_target_phi = -m_extreme;
		m_target_alpha = 0;
		m_target_beta = 20;
		break;
	}

	UpdateSpeedNet();//update the speed of each rotation
}

void MillipedeLeg::UpdateStanceNet(double a_dt){
	
	Eigen::Vector3f result ;
	if(InverseKinematics(m_tip_position, m_root_position, result))
	{
		//update
		m_phi = result[0];
		m_alpha = result[1];
		m_beta = result[2];
	}
	else{
	//InverseKinematics can not resolve
		EnterSwayForward();
	}
	
	//the stance net results in the inverse kinematics of desired physics configuration
}

void MillipedeLeg::UpdateSpeedNet(){

	//sync the speed of rotations
	Eigen::Vector3f target_rotation(m_target_phi, m_target_alpha, m_target_beta);
	Eigen::Vector3f dif;
	dif = target_rotation - Eigen::Vector3f(m_phi, m_alpha, m_beta);
	m_phi_velocity = m_leg_rotation_velocity*fabs(dif[0])/dif.norm();
	m_alpha_velocity = m_leg_rotation_velocity*fabs(dif[1])/dif.norm();
	m_beta_velocity = m_leg_rotation_velocity*fabs(dif[2])/dif.norm();

}

void MillipedeLeg::UpdateNeuronNet(double a_dt){
	UpdateSwitchNet(a_dt);//manage the switch of different modes
	switch (m_leg_state) {
        case LEG_SWAY_FORWARD_1:
		case LEG_SWAY_FORWARD_2:
			UpdateSFowardNet(a_dt);
            break;
		case LEG_SWAY_BACKWARD_1:
		case LEG_SWAY_BACKWARD_2:
			UpdateSBackwardNet(a_dt);
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

	if(m_neig->m_leg_state == LEG_STANCE){
		m_root->m_leg_state = LEG_STANCE; //update the root's state
		//velocity lost to zero due to impulse on the ground when switch to stance mode
		m_root->m_velocity *= 0;
		m_root->m_avelocity *= 0;
	}
}

void MillipedeLeg::EnterSwayBackward(){
	UpdateTargetNet(2);
	m_root->m_leg_state = LEG_SWAY_BACKWARD_1;
	m_leg_state = LEG_SWAY_BACKWARD_1;
}

void MillipedeLeg::EnterSwayForward(){

	UpdateTargetNet(1);
	m_root->m_leg_state = LEG_SWAY_FORWARD_1;
	m_leg_state = LEG_SWAY_FORWARD_1;
} 

bool MillipedeLeg::InverseKinematics(Eigen::Vector3f a_tip, Eigen::Vector3f a_root, Eigen::Vector3f& a_result){
	//based on current rigid section configuration, get the rotations
	double phi;
	//first update the phi
	Eigen::Vector3f legv;
	legv = m_root->m_rotation*(a_tip - a_root);

	if(m_l_r == 1)
		phi = atan(-legv[0]/legv[2])/DegreesToRadians;//rotation in y
	else
		phi = atan(legv[0]/legv[2])/DegreesToRadians;//rotation in y

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
	Eigen::Vector3f H;
	myDrawer->SetIdentity();
	myDrawer->Rotate(m_root->m_rotation);
	myDrawer->RotateY(-m_l_r*phi);
	H = myDrawer->m_transformation.linear()*Eigen::Vector3f(0,0,m_l_r*1);
	H.normalize();
	
	double temp;
	if(Leg_Plane_N.dot(H.cross(M-P))>0){
		temp = H.dot(M-P)/(M-P).norm();
		if(temp>1)
			temp = 1.0;
		if(temp<-1)
			temp = -1.0;
		alpha = m_l_r*acos(temp)/DegreesToRadians;
	}
	else{
		temp = H.dot(M-P)/(M-P).norm();
		if(temp>1)
			temp = 1.0;
		if(temp<-1)
			temp = -1.0;
		alpha = -m_l_r*acos(temp)/DegreesToRadians;
	}

	myDrawer->SetIdentity();
	myDrawer->Rotate(m_root->m_rotation);
	myDrawer->RotateY(-m_l_r*phi);
	H = myDrawer->m_transformation.linear()*Eigen::Vector3f(0,0,-m_l_r*1);
	H.normalize();
	psi = acos(H.dot(M-O)/(M-O).norm())/DegreesToRadians;

	ksi = atan(l3*sin(DegreesToRadians*gamma)/(l3*cos(DegreesToRadians*gamma) + l2))/DegreesToRadians;

	beta = psi - ksi + alpha;


	a_result = Eigen::Vector3f(phi, alpha, beta);
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


void MillipedeRigidSection::InitPhysics(double density, Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color){
	
	RigidCube::Init(density, center, size, color);

	m_left_leg = new MillipedeLeg(1);
	m_left_leg->InitPhysics(size);

	m_right_leg = new MillipedeLeg(-1);
	m_right_leg->InitPhysics(size);

	m_next = NULL;
	m_prev = NULL;
	
}

void MillipedeRigidSection::InitNeuroNet(Millipede* a_root){

	//determine the leg rotation speed from the global speed parameter
	m_root = a_root;

	m_leg_state = Util::getRand()>0.5?LEG_SWAY_FORWARD_1:LEG_SWAY_BACKWARD_1;

	m_left_leg->InitNeuroNet(this);
	m_right_leg->InitNeuroNet(this);

	m_left_leg->UpdateRootPosition();
	m_left_leg->UpdateTipPosition();

	m_height_obj = m_Size[1]*0.5 + m_left_leg->m_segment_2_size[2]*sin(30*DegreesToRadians) 
		+ m_left_leg->m_segment_3_size[2]*sin(60*DegreesToRadians);
}

void MillipedeRigidSection::SetWorld(World* a_world){
	
	Cube::SetWorld(a_world);

	//add the legs into the world
	m_left_leg->SetWorld(a_world);
	m_right_leg->SetWorld(a_world);
}

void MillipedeRigidSection::Draw(int type, const Camera& camera, const Light& light){


//            myDrawer->SetColor(Eigen::Vector3f(0.5,0.25,0.08));
//            myDrawer->SetIdentity();
//            myDrawer->Translate(m_Center + Eigen::Vector3f(0,0.2*m_Size[1],0));
//            myDrawer->Scale(m_Size[1]);
//            myDrawer->DrawSphere(type, camera, light);

    Cube::Draw(type, camera, light);

	if(this != m_root->m_head){
		m_left_leg->Draw(type, camera, light);
		m_right_leg->Draw(type, camera, light);
	}
}

void MillipedeRigidSection::UpdateNeuronNet(double a_dt){
	
	//Sync the two legs' rotation
	//make sure the rotation state and process are syncronized
	//get the force on the rigid section
	//based on the force, adjust the speed of rotation
	//adjust the m_extreme on each leg
	//to minimize the force on the rigid section;
	//the leg would provide support to cancel the gravity
	//so only the elastic force from deformable sections will be considered

	switch(m_leg_state){
	case LEG_SWAY_FORWARD_1:
		{
			//sync the leg state
		

			break;
		}
	case LEG_SWAY_BACKWARD_1:
		{
			//sync the leg state
			
			/*
			//based on the two legs current configuration
			//assign different rotational speed such that
			//they will reach the Inverse Dynamics estimated target rotation
			//at the same time
			Eigen::Vector3f dif_l, dif_r;
			dif_l = Eigen::Vector3f(m_left_leg->m_target_phi - m_left_leg->m_phi
				,m_left_leg->m_target_alpha - m_left_leg->m_alpha
				,m_left_leg->m_target_alpha - m_left_leg->m_alpha);
			dif_r = Eigen::Vector3f(m_right_leg->m_target_phi - m_right_leg->m_phi
				,m_right_leg->m_target_alpha - m_right_leg->m_alpha
				,m_right_leg->m_target_alpha - m_right_leg->m_alpha);

			//the closer one moves slower
			if(dif_l.norm() > dif_r.norm())
			{
				m_right_leg->m_leg_rotation_velocity = (dif_r.norm()/dif_l.norm())*m_left_leg->m_leg_rotation_velocity;
			}else
			{
				m_left_leg->m_leg_rotation_velocity = (dif_l.norm()/dif_r.norm())*m_right_leg->m_leg_rotation_velocity;
			}
			*/
			break;
			
		}
	case LEG_STANCE:{
			//control the height using negative feedback net
		m_Center[1] += 10*(m_height_obj - m_Center[1])*a_dt;
		 //control the lean around x axis, keep balance of body
		}
	}


}

void MillipedeRigidSection::UpdateAll(double dt){
	//update nueronet

	UpdateNeuronNet(dt);

	m_left_leg->UpdateAll(dt);
	m_right_leg->UpdateAll(dt);

	//update the rigid section's physics
	switch(m_leg_state){
		case LEG_STANCE:
			UpdatePBD(dt);//based on the leg states, infer the cube position and rotation
			break;
		case LEG_SWAY_FORWARD_1:
		case LEG_SWAY_FORWARD_2:
		case LEG_SWAY_BACKWARD_1:
		case LEG_SWAY_BACKWARD_2:
			RigidCube::UpdateAll(dt);
			break;
	}
}

void MillipedeRigidSection::UpdatePBD(double dt){
	//position based dynamics
	//the tip is fixed, the root nodal point change position
	//let the physics resolve the best configuration
	//leg will accomplish what the physics wants

	HandleCollision();

	m_force = Eigen::Vector3f(0,-1.0,0.0)*m_mass*GRAVITY_CONSTANT;
	m_torque = Eigen::Vector3f(0,0.0,0.0);
	Eigen::Vector3f temp_elastic_force;

	for(unsigned int i = 0; i < m_nodes.size(); i++){
		temp_elastic_force = m_nodes[i]->m_Force - Eigen::Vector3f(0,-1.0,0.0)*m_nodes[i]->m_Mass*GRAVITY_CONSTANT;
		m_force += temp_elastic_force;
		m_torque += (m_Trans.linear()*m_fixed_r[i]).cross(temp_elastic_force);
	}

	//keep the balance of lean in x rotation
	double dify = m_left_leg->m_root_position[1] - m_right_leg->m_root_position[1];
	m_torque += (m_rotation*Eigen::Vector3f(0,0,m_Size[2]/2)).cross(-dify*m_rotation*Eigen::Vector3f(0,1,0));

	
	Eigen::Matrix3f avelocity_star;
	avelocity_star<< 0, -m_avelocity[2], m_avelocity[1],
					m_avelocity[2],0,-m_avelocity[0],
					-m_avelocity[1],m_avelocity[0],0;
	m_rotation += avelocity_star*m_rotation*dt;
	m_avelocity += dt*(m_rotation*m_inertia_inverse*m_rotation.transpose())*m_torque;
	
	if(m_root->m_mode != STATIC && m_fixed == false){
		//if in static mode, no advancement,also the rigid section need not be fixed
		//only the head will be fixed, cause it is controlled by the brain directly
	
		Eigen::Vector3f front_vector;
		front_vector = m_rotation*Eigen::Vector3f(-1,0,0);

		m_force = front_vector.dot(m_force)*front_vector;
		m_force[1] = 0;//supported by leg

		m_velocity += m_force*dt/m_mass;
		m_Center += m_velocity*dt;//the desired configuration, to be achieved by adjusting leg rotations
	}

	//now update the matrixes
	m_Trans.setIdentity();
	m_Trans.translate(m_Center);
	m_Trans.rotate(m_rotation);
	m_Trans.scale(m_Size);

	m_TransBack = m_Trans.inverse();
	
	UpdateFixed();
}

Millipede::Millipede(){
	m_type = TypeMixed;
}

Millipede::~Millipede(){


}

void Millipede::Init(Eigen::Vector3f a_position, int a_num_section, Eigen::Vector3f a_rigid_size, double a_soft_length){

	InitPhysics(a_position,a_num_section,a_rigid_size,a_soft_length);
	InitNeuroNet();
}

void Millipede::InitPhysics(Eigen::Vector3f a_position, int a_num_section, Eigen::Vector3f a_rigid_size, double a_soft_length){
		m_num_section = a_num_section;

	MillipedeRigidSection* previous_rigid_section, *current_rigid_section;
	MillipedeSoftSection* previous_soft_section, *current_soft_section;

	//create head section
	m_head = new MillipedeRigidSection;
	m_head->InitPhysics(1.0, a_position, a_rigid_size, Eigen::Vector3f(1.0,0,0));
	m_head->m_prev = NULL;
	//create body sections
	previous_rigid_section = m_head;
	Eigen::Vector3f temp_position;
	for(int iter = 0; iter < m_num_section + 1; iter++){//the last rigid section is tail
		//create a softsection after the previous rigid section
		current_soft_section = new MillipedeSoftSection;
		temp_position = previous_rigid_section->m_Center 
			+ Eigen::Vector3f(0.5*a_rigid_size[0], -0.5*a_rigid_size[1], -0.5*a_rigid_size[2]);
		current_soft_section->Init(Eigen::Vector3i(3,3,3),1.0,500,0.4,100.0,temp_position,
			Eigen::Vector3f(a_soft_length,a_rigid_size[1],a_rigid_size[2]),Eigen::Vector3f(1,1,1));
		//hook up with the previous rigid section
		previous_rigid_section->m_next = current_soft_section;
		current_soft_section->m_prev = previous_rigid_section;
		previous_soft_section = current_soft_section;
		//create a rigidsection after the previous soft section
		current_rigid_section = new MillipedeRigidSection;
		temp_position = previous_rigid_section->m_Center;
		temp_position[0] += a_rigid_size[0] + a_soft_length;//the millipede init heading -x direction
		current_rigid_section->InitPhysics(1,temp_position,a_rigid_size, Eigen::Vector3f(0.4,0.4,0.4));
		//hook up with previous soft section
		current_rigid_section->m_prev = previous_soft_section;
		previous_soft_section->m_next = current_rigid_section;
		previous_rigid_section = current_rigid_section;
	}
	//assign different color for tail section
	m_tail = current_rigid_section;
	m_tail->SetColor(Eigen::Vector3f(0,0,1));
	m_tail->m_next = NULL;

	//connect the rigid and soft sections
	MillipedeRigidSection *temp_rigid_section;
	MillipedeSoftSection *temp_soft_section;
	temp_rigid_section = m_head;
	std::vector<Node*> temp_nodes;
	//loop through the nodes, init force with gravity, add collision force with the ground
	while(1){
		//deal with the rigid section
		temp_soft_section = temp_rigid_section->m_next;

		if(temp_soft_section){
			//attach the left face with the previous rigid section
			temp_nodes = temp_soft_section->m_Mesh->GetLeftNodes();
			temp_rigid_section->AttachNodes(temp_nodes);

			temp_rigid_section = temp_soft_section->m_next;
			//attach the right face with the next rigid section
			temp_nodes = temp_soft_section->m_Mesh->GetRightNodes();
			temp_rigid_section->AttachNodes(temp_nodes);
		}
		else
			break;
	}

}

void Millipede::InitNeuroNet(){

	m_mode = STATIC;

	MillipedeRigidSection *temp_rigid_section;
	MillipedeSoftSection *temp_soft_section;
	temp_rigid_section = m_head;
	std::vector<Node*> temp_nodes;
	//loop through the nodes, init force with gravity, add collision force with the ground
	while(1){

		//deal with the rigid section
		temp_rigid_section->InitNeuroNet(this);
		temp_soft_section = temp_rigid_section->m_next;
		if(temp_soft_section){
			temp_rigid_section = temp_soft_section->m_next;
		}
		else
			break;
	}

}

void Millipede::SetWorld(World* a_world){
	m_world = a_world;
	
	//draw all the rigid and soft sections
	MillipedeRigidSection *temp_rigid_section;
	MillipedeSoftSection *temp_soft_section;
	temp_rigid_section = m_head;
	//loop through the nodes, init force with gravity, add collision force with the ground
	while(1){
		//deal with the rigid section
		temp_rigid_section->SetWorld(a_world);
		//end deal with the rigid section
		temp_soft_section = temp_rigid_section->m_next;
		if(temp_soft_section){
			//deal with the soft section
			temp_soft_section->SetWorld(a_world);
			//end deal with the soft section
			temp_rigid_section = temp_soft_section->m_next;
		}
		else
			break;
	}

}

void Millipede::Draw(int type, const Camera& camera, const Light& light){
	//draw all the rigid and soft sections
	MillipedeRigidSection *temp_rigid_section;
	MillipedeSoftSection *temp_soft_section;
	temp_rigid_section = m_head;
	//loop through the nodes, init force with gravity, add collision force with the ground
	while(1){
		//deal with the rigid section
		temp_rigid_section->Draw(type, camera, light);
		//end deal with the rigid section
		temp_soft_section = temp_rigid_section->m_next;
		if(temp_soft_section){
			//deal with the soft section
			temp_soft_section->Draw(type, camera, light);
			//end deal with the soft section
			temp_rigid_section = temp_soft_section->m_next;
		}
		else
			break;
	}

}

void Millipede::UpdateAll(double dt){

	//UpdateNeuroNet(dt);

	MillipedeRigidSection *temp_rigid_section;
	MillipedeSoftSection *temp_soft_section;
	temp_rigid_section = m_head;
	//rigid phase
	while(1){
		temp_rigid_section->UpdateAll(dt);
		temp_soft_section = temp_rigid_section->m_next;
		if(temp_soft_section){
			temp_rigid_section = temp_soft_section->m_next;
		}
		else
			break;
	}
	//soft phase
	temp_rigid_section = m_head;
	while(1){
		temp_soft_section = temp_rigid_section->m_next;
		if(temp_soft_section){
			temp_soft_section->UpdateAll(dt);
			temp_rigid_section = temp_soft_section->m_next;
		}
		else
			break;
	}

}

void Millipede::UpdateNeuroNet(double dt){

	//start to move when all legs are in stance mode
	if(m_mode == STATIC){
		MillipedeRigidSection *temp_rigid_section;
		MillipedeSoftSection *temp_soft_section;
		temp_rigid_section = m_head;
		while(1){
			if(temp_rigid_section->m_leg_state != LEG_STANCE)
				return;
			temp_soft_section = temp_rigid_section->m_next;
			if(temp_soft_section){
				temp_rigid_section = temp_soft_section->m_next;
			}
			else
				break;
		}
		m_mode = MOVE;
		m_head->m_fixed = true;//head now take control
	}

	//straight movement, move the head section
	if(m_mode == MOVE){
		
		m_head->m_Center += 3*dt*Eigen::Vector3f(-1,0,0);
	}

}