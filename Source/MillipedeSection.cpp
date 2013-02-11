#include "Millipede.h"
#include "MillipedeHead.h"
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
	m_linear_speed *= 0;
	m_height_obj = m_Size[1]*0.5 + m_left_leg->GetBalanceHeight();
	m_orient_obj = Eigen::Vector3f(0,1,0);
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


	if(m_left_leg->m_leg_state == LEG_STANCE && m_right_leg->m_leg_state == LEG_STANCE){
		m_body_state = LEG_SUPPORTED;
		m_velocity = m_linear_speed;
		m_avelocity *= 0;

		//generating obj height and orientation

		Eigen::Vector3f i,j,k;//the local cs determined by the two leg tip
		i = (m_right_leg->m_tip_position -  m_left_leg->m_tip_position).normalized();
		j = (m_master->m_terrain->GetNormal(m_left_leg->m_tip_position) + 
			m_master->m_terrain->GetNormal(m_right_leg->m_tip_position)).normalized();
		//k = i.cross(j);
		
		m_height_obj = m_Size[1]*0.5 + m_left_leg->GetBalanceHeight();
		m_orient_obj = j;

		m_current_height = (m_Center - 0.5*m_left_leg->m_tip_position - 0.5*m_right_leg->m_tip_position).dot(m_orient_obj);
		
	}
	else{
		m_body_state = NOT_SUPPORTED;
		m_linear_speed = m_velocity;
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
	if(m_section_id == 1)
		prev_link_vector = m_master->m_head->m_Center - m_Center;
	else
		prev_link_vector = m_prev->m_prev->m_Center - m_Center;

	m_Center += 10*dt*(m_height_obj - m_current_height)*m_orient_obj;//balance of height	
	//Minimize the distance to previous section to minimum;
	m_linear_speed = 50*prev_link_vector.normalized()*(prev_link_vector.norm() - m_prev_dis_obj);
	m_Center += dt*m_linear_speed;

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
