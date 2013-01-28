
#include "MillipedeSection.h"
#include "Millipede.h"
#include "MillipedeLeg.h"
#include "Drawer.h"
#include "Terrain.h"

extern Drawer* myDrawer;

void MillipedeRigidSection::InitPhysics(double density, Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color){
	
	RigidCube::Init(density, center, size, color);

	m_left_leg = new MillipedeLeg(1);//left
	m_left_leg->InitPhysics(size);

	m_right_leg = new MillipedeLeg(-1);//right
	m_right_leg->InitPhysics(size);

	m_next = NULL;
	m_prev = NULL;
	
}

void MillipedeRigidSection::InitNeuroNet(Millipede* a_root, int a_id){

	//determine the leg rotation speed from the global speed parameter
	m_master = a_root;
	m_terrain = m_master->m_terrain;
	m_section_id = a_id;
	m_body_state = NOT_SUPPORTED;

	m_left_leg->InitNeuroNet(this);
	m_right_leg->InitNeuroNet(this);
	
	m_prev_dis_obj = m_master->m_link_length;

	m_height_obj = m_Size[1]*0.5 + m_left_leg->GetBalanceHeight();
}

void MillipedeRigidSection::SetWorld(World* a_world){
	
	Cube::SetWorld(a_world);

	//add the legs into the world
	m_left_leg->SetWorld(a_world);
	m_right_leg->SetWorld(a_world);
}

void MillipedeRigidSection::Draw(int type, const Camera& camera, const Light& light){
	/*
	myDrawer->SetIdentity();
	myDrawer->Translate(this->m_Center);
	myDrawer->Scale(4);
	myDrawer->SetColor(Eigen::Vector3f(1,1,1));
	myDrawer->DrawSphere(type,camera,light);
	*/
	Cube::Draw(type, camera, light);
	
	m_left_leg->Draw(type, camera, light);
	m_right_leg->Draw(type, camera, light);
	
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

	if(m_left_leg->m_leg_state == LEG_STANCE || m_right_leg->m_leg_state == LEG_STANCE)
		m_body_state = LEG_SUPPORTED;
	else
		m_body_state = NOT_SUPPORTED;

	switch(m_body_state){
	case NOT_SUPPORTED:
		{
			//based on the two legs current configuration
			//assign different rotational speed such that
			//they will reach the target rotation
			//at the same time

			/*
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
		}
		break;
	case LEG_SUPPORTED:{
			m_velocity *= 0;
			m_avelocity *= 0;
			//m_height_obj = m_Size[1]*0.5 + 0.5*(m_left_leg->GetBalanceHeight() + m_right_leg->GetBalanceHeight());
		}
		break;
	}


}

void MillipedeRigidSection::UpdateAll(double dt){

	//update nueronet

	UpdateNeuronNet(dt);

	m_left_leg->UpdateAll(dt);
	m_right_leg->UpdateAll(dt);

	//update the rigid section's physics
	switch(m_body_state){
		case LEG_SUPPORTED:
			UpdatePBD(dt);
			break;
		case NOT_SUPPORTED:
			RigidCube::UpdateAll(dt);//based on the leg states, infer the cube position and rotation
			break;
	}
}

void MillipedeRigidSection::UpdatePBD(double dt){
	//position based dynamics
	//the tip is fixed, the root nodal point change position

	Eigen::Vector3f prev_link_vector; double turn_angle;
	prev_link_vector = m_prev->m_prev->m_Center - m_Center;

	m_Center[1] += 10*dt*(m_height_obj - (m_Center[1] - m_terrain->GetHeight(m_Center[0],m_Center[2])));//balance of height	
	//Minimize the distance to previous section to minimum;
	m_Center += 50*dt*prev_link_vector.normalized()*(prev_link_vector.norm() - m_prev_dis_obj);

	//keep the balance of lean in x rotation
	double zaxisdify = (m_rotation*Eigen::Vector3f(0,0,1)).dot(m_terrain->GetNormal(m_Center[0], m_Center[2]));
	turn_angle = (zaxisdify > 0? 1:-1)*60*dt;
	Eigen::AngleAxis<float> turnx(DegreesToRadians*turn_angle, Eigen::Vector3f(1,0,0));
	m_rotation = m_rotation*turnx.toRotationMatrix();

	//keep the balance of lean in z rotation
	double xaxisdify = (m_rotation*Eigen::Vector3f(-1,0,0)).dot(m_terrain->GetNormal(m_Center[0], m_Center[2]));
	turn_angle = (xaxisdify > 0? 1:-1)*60*dt;
	Eigen::AngleAxis<float> turnz(DegreesToRadians*turn_angle,Eigen::Vector3f(0,0,1));
	m_rotation = m_rotation*turnz.toRotationMatrix();

	//minimize the angle to orient the rigid section with the previous one.
	//Rotation in y axis;
	double diforient = prev_link_vector.normalized().dot(m_rotation*Eigen::Vector3f(0,0,1));
	turn_angle = (diforient > 0? 1:-1)*60*dt;//y axis anti clockwise
	Eigen::AngleAxis<float> turny(DegreesToRadians*turn_angle, Eigen::Vector3f(0,1,0));
	m_rotation = m_rotation*turny.toRotationMatrix();

	//now update the matrixes
	m_Trans.setIdentity();
	m_Trans.translate(m_Center);
	m_Trans.rotate(m_rotation);
	m_Trans.scale(m_Size);

	m_TransBack = m_Trans.inverse();
	
	UpdateFixed();
}

void MillipedeRigidSection::Output2File(std::ofstream* filestream){
	
	(*filestream)<<"//BEGIN RIGID ID"<<m_section_id<<std::endl;

	Cube::Output2File(filestream);
	m_left_leg->Output2File(filestream);
	m_right_leg->Output2File(filestream);

	(*filestream)<<"//END RIGID ID"<<m_section_id<<std::endl<<std::endl;
	
}
