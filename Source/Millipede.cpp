#include <queue>
#include "Millipede.h"
#include "MillipedeHeadTail.h"
#include "MillipedeLeg.h"
#include "MillipedeSection.h"
#include "Terrain.h"
#include "HeightFieldWater.h"
#include "Drawer.h"
#include "3DDeformable.h"

Millipede::Millipede(){
	m_type = TypeMixed;
	m_stopped = false;
	m_Drawer = new Drawer;
}

Millipede::~Millipede(){


}

void Millipede::Init(int hack_type,Eigen::Vector3f a_position, int a_num_section, Eigen::Vector3f a_rigid_size, double a_soft_length, Terrain* a_terrain){
	
	m_hack_type = hack_type;
	if(m_hack_type!=2)
		InitPhysics(a_position,a_num_section,a_rigid_size,a_soft_length);
	else
		InitPhysicsFromModel(a_position);
	InitNeuroNet(a_terrain);
}

void ReadSectionDataFromModel(double *readin){

 readin[0] = 0.705102;//section 1 to head distance
 readin[1] = 1.374068;
 readin[2] = 1.086392;
 readin[3] = 1.228905;
 readin[4] = 1.086392;
 readin[5] = 1.229232;
 readin[6] = 1.002572;
 readin[7] = 1.224139;
 readin[8] = 1.087384;
 readin[9] = 1.231778;
 readin[10] = 1.089603;
 readin[11] = 1.234731;
 readin[12] = 1.085085;
 readin[13] = 1.234732;
 readin[14] = 1.092328;
 readin[15] = 1.219581;
 readin[16] = 1.099235;
 readin[17] = 1.220029;//section 18 to 17 distance
 readin[18] = 1.099657;//section 19 to 18 distance
 readin[19] = 0.838355;//tail to section 19 distance

}

void Millipede::InitPhysics(Eigen::Vector3f a_position, int a_num_section, Eigen::Vector3f a_rigid_size, double a_soft_length){

    Eigen::Vector3i soft_resolution(3,3,3);
    double youngs_modulus = 1500;

	assert(a_num_section >=3);//Need to be at least this lenght

	m_num_section = a_num_section;

	MillipedeRigidSection* previous_rigid_section, *current_rigid_section;
	MillipedeSoftSection* previous_soft_section, *current_soft_section;
	Eigen::Vector3f temp_position;
	
	//create head section
	m_head = new MillipedeHead;
	m_head->InitPhysics(1.0, a_position, a_rigid_size, Eigen::Vector3f(1.0,0,0));

	//create neck
	current_soft_section = new MillipedeSoftSection;
	temp_position = m_head->m_Center 
		+ Eigen::Vector3f(0.5*a_rigid_size[0], -0.5*a_rigid_size[1], -0.5*a_rigid_size[2]);
	current_soft_section->Init(soft_resolution,1.0,youngs_modulus,0.4,100.0,temp_position,
		Eigen::Vector3f(a_soft_length,a_rigid_size[1],a_rigid_size[2]),Eigen::Vector3f(1,1,1));
	//hook up with the previous rigid section
	m_head->m_next = current_soft_section;
	current_soft_section->m_prev = NULL;
	previous_soft_section = current_soft_section;
	//create a rigidsection after the previous soft section
	current_rigid_section = new MillipedeRigidSection;
	temp_position = m_head->m_Center;
	temp_position[0] += a_rigid_size[0] + a_soft_length;//the millipede init heading -x direction
	current_rigid_section->InitPhysics(1,temp_position,a_rigid_size, Eigen::Vector3f(0.4,0.4,0.4),a_rigid_size[0] + a_soft_length);
	//hook up with previous soft section
	current_rigid_section->m_prev = previous_soft_section;
	previous_soft_section->m_next = current_rigid_section;
	previous_rigid_section = current_rigid_section;

	for(int iter = 0; iter < m_num_section; iter++){//the last rigid section is tail
		//create a softsection after the previous rigid section
		current_soft_section = new MillipedeSoftSection;
		temp_position = previous_rigid_section->m_Center 
			+ Eigen::Vector3f(0.5*a_rigid_size[0], -0.5*a_rigid_size[1], -0.5*a_rigid_size[2]);
		current_soft_section->Init(soft_resolution,1.0,youngs_modulus,0.4,100.0,temp_position,
			Eigen::Vector3f(a_soft_length,a_rigid_size[1],a_rigid_size[2]),Eigen::Vector3f(1,1,1));
		//hook up with the previous rigid section
		previous_rigid_section->m_next = current_soft_section;
		current_soft_section->m_prev = previous_rigid_section;
		previous_soft_section = current_soft_section;
		//create a rigidsection after the previous soft section
		current_rigid_section = new MillipedeRigidSection;
		temp_position = previous_rigid_section->m_Center;
		temp_position[0] += a_rigid_size[0] + a_soft_length;//the millipede init heading -x direction
		current_rigid_section->InitPhysics(1,temp_position,a_rigid_size, Eigen::Vector3f(0.4,0.4,0.4), a_rigid_size[0] + a_soft_length);
		//hook up with previous soft section
		current_rigid_section->m_prev = previous_soft_section;
		previous_soft_section->m_next = current_rigid_section;
		previous_rigid_section = current_rigid_section;
	}
	//assign different color for tail section
	m_last = current_rigid_section;
	m_last->SetColor(Eigen::Vector3f(0,0,1));
	m_last->m_next = NULL;

	//Hack Tail parts
	m_tail_soft = new Deformable3D;
	Eigen::Vector3f tail_s_size = Eigen::Vector3f(a_rigid_size[0],a_rigid_size[1]*0.7,a_rigid_size[2]*0.7);
	temp_position[0] += 0.5*a_rigid_size[0];
	temp_position[1] -= 0.5*tail_s_size[1]; 
	temp_position[2] -= 0.5*tail_s_size[2];
	m_tail_soft->Init(Eigen::Vector3i(3,3,3),1.0,2000,0.4,80,temp_position,tail_s_size,Eigen::Vector3f(1,0,0));
	
	m_last->AttachNodes(m_tail_soft->m_Mesh->GetLeftNodes());

	Eigen::Vector3f tail_c_size = tail_s_size;
	tail_c_size[0] = 0.5*tail_c_size[1];
	temp_position[0] += tail_s_size[0] + tail_c_size[0]*0.5;
	temp_position[1] += tail_s_size[1]*0.5;
	temp_position[2] += 0.5*tail_s_size[2];
	m_tail_rigid = new RigidCube;
	m_tail_rigid->Init(2,temp_position,tail_c_size,Eigen::Vector3f(0,0,1));
	
	m_tail_rigid->AttachNodes(m_tail_soft->m_Mesh->GetRightNodes());
	//connect the rigid and soft sections
	//connect the rigid and soft sections
	MillipedeRigidSection *temp_rigid_section;
	MillipedeSoftSection *temp_soft_section;
	std::vector<Node*> temp_nodes;
	temp_soft_section = m_head->m_next;
	temp_nodes = temp_soft_section->m_Mesh->GetLeftNodes();
	m_head->AttachNodes(temp_nodes);

	temp_rigid_section = temp_soft_section->m_next;
	//attach the right face with the next rigid section
	temp_nodes = temp_soft_section->m_Mesh->GetRightNodes();
	temp_rigid_section->AttachNodes(temp_nodes);

	m_tail_left = new MillipedeTail;
	m_tail_left->InitNeuroNet(m_head,m_tail_rigid,1);

	m_tail_right = new MillipedeTail;
	m_tail_right->InitNeuroNet(m_head,m_tail_rigid,-1);
	

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

void Millipede::InitPhysicsFromModel(Eigen::Vector3f a_position){

	double* sectiondata;
	sectiondata = new double[20];
	ReadSectionDataFromModel(sectiondata);

	Eigen::Vector3i soft_resolution(3,3,3);
    double youngs_modulus = 1500;

	m_num_section = 19;

	MillipedeRigidSection* previous_rigid_section, *current_rigid_section;
	MillipedeSoftSection* previous_soft_section, *current_soft_section;
	Eigen::Vector3f temp_position;
	Eigen::Vector3f a_rigid_size = Eigen::Vector3f(0.2,2*0.539,2.422);
	double a_link_length;
	//create head section
	m_head = new MillipedeHead;
	m_head->InitPhysics(1.0, a_position, a_rigid_size, Eigen::Vector3f(1.0,0,0));

	//create neck
	current_soft_section = new MillipedeSoftSection;
	temp_position = m_head->m_Center 
		+ Eigen::Vector3f(0.5*a_rigid_size[0], -0.5*a_rigid_size[1], -0.5*a_rigid_size[2]);

	a_link_length = sectiondata[0];
	current_soft_section->Init(soft_resolution,1.0,youngs_modulus,0.4,100.0,temp_position,
		Eigen::Vector3f(a_link_length - a_rigid_size[0],a_rigid_size[1],a_rigid_size[2]),Eigen::Vector3f(1,1,1));
	//hook up with the previous rigid section
	m_head->m_next = current_soft_section;
	current_soft_section->m_prev = NULL;
	previous_soft_section = current_soft_section;
	//create first rigid section
	current_rigid_section = new MillipedeRigidSection;
	temp_position = m_head->m_Center;
	temp_position[0] += a_link_length;//the millipede init heading -x direction
	current_rigid_section->InitPhysics(1,temp_position,a_rigid_size, Eigen::Vector3f(0.4,0.4,0.4),a_link_length);
	//hook up with previous soft section
	current_rigid_section->m_prev = previous_soft_section;
	previous_soft_section->m_next = current_rigid_section;
	previous_rigid_section = current_rigid_section;
	
	for(int iter = 0; iter < m_num_section-1; iter++){//the last rigid section is tail
		//create a softsection after the previous rigid section
		current_soft_section = new MillipedeSoftSection;
		temp_position = previous_rigid_section->m_Center 
			+ Eigen::Vector3f(0.5*a_rigid_size[0], -0.5*a_rigid_size[1], -0.5*a_rigid_size[2]);
		a_link_length = sectiondata[iter+1];
		current_soft_section->Init(soft_resolution,1.0,youngs_modulus,0.4,100.0,temp_position,
			Eigen::Vector3f(a_link_length - a_rigid_size[0],a_rigid_size[1],a_rigid_size[2]),Eigen::Vector3f(1,1,1));
		//hook up with the previous rigid section
		previous_rigid_section->m_next = current_soft_section;
		current_soft_section->m_prev = previous_rigid_section;
		previous_soft_section = current_soft_section;
		//create a rigidsection after the previous soft section
		current_rigid_section = new MillipedeRigidSection;
		temp_position = previous_rigid_section->m_Center;
		temp_position[0] += a_link_length;//the millipede init heading -x direction
		current_rigid_section->InitPhysics(1,temp_position,a_rigid_size, Eigen::Vector3f(0.4,0.4,0.4), a_link_length);
		//hook up with previous soft section
		current_rigid_section->m_prev = previous_soft_section;
		previous_soft_section->m_next = current_rigid_section;
		previous_rigid_section = current_rigid_section;
	}
	//assign different color for tail section
	m_last = current_rigid_section;
	m_last->SetColor(Eigen::Vector3f(0,0,1));
	m_last->m_next = NULL;

	a_link_length = sectiondata[18];
	//Hack Tail parts
	m_tail_soft = new Deformable3D;
	Eigen::Vector3f tail_s_size = Eigen::Vector3f(a_link_length - a_rigid_size[0]*0.8,a_rigid_size[1]*0.8,a_rigid_size[2]*0.8);
	temp_position[0] += 0.5*a_rigid_size[0];
	temp_position[1] -= 0.5*tail_s_size[1]; 
	temp_position[2] -= 0.5*tail_s_size[2];
	m_tail_soft->Init(Eigen::Vector3i(3,3,3),1.0,2000,0.4,80,temp_position,tail_s_size,Eigen::Vector3f(1,0,0));
	
	m_last->AttachNodes(m_tail_soft->m_Mesh->GetLeftNodes());

	Eigen::Vector3f tail_c_size = tail_s_size;
	tail_c_size[0] = 0.8*a_rigid_size[0];
	temp_position[0] += tail_s_size[0] + tail_c_size[0]*0.5;
	temp_position[1] += tail_s_size[1]*0.5;
	temp_position[2] += 0.5*tail_s_size[2];
	m_tail_rigid = new RigidCube;
	m_tail_rigid->Init(2,temp_position,tail_c_size,Eigen::Vector3f(0,0,1));
	
	m_tail_rigid->AttachNodes(m_tail_soft->m_Mesh->GetRightNodes());
	//connect the rigid and soft sections
	MillipedeRigidSection *temp_rigid_section;
	MillipedeSoftSection *temp_soft_section;
	std::vector<Node*> temp_nodes;
	temp_soft_section = m_head->m_next;
	temp_nodes = temp_soft_section->m_Mesh->GetLeftNodes();
	m_head->AttachNodes(temp_nodes);

	temp_rigid_section = temp_soft_section->m_next;
	//attach the right face with the next rigid section
	temp_nodes = temp_soft_section->m_Mesh->GetRightNodes();
	temp_rigid_section->AttachNodes(temp_nodes);

	m_tail_left = new MillipedeTail;
	m_tail_right = new MillipedeTail;
	
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
void Millipede::InitNeuroNet(Terrain* a_terrain){

	assert(a_terrain != NULL);
	m_terrain = a_terrain;
	
	a_terrain->RegisterMillipede(this);

	MillipedeRigidSection *temp_rigid_section;
	MillipedeSoftSection *temp_soft_section;
	m_head->InitNeuroNet(this);
	temp_rigid_section = m_head->m_next->m_next;
	std::vector<Node*> temp_nodes;
	int id = 1;
	while(1){
		//deal with the rigid section
		temp_rigid_section->InitNeuroNet(this,id);
		id++;
		temp_soft_section = temp_rigid_section->m_next;
		if(temp_soft_section){
			temp_rigid_section = temp_soft_section->m_next;
		}
		else
			break;
	}

	m_tip_spheres.clear();
	//create tip sphere for water effect
	temp_rigid_section = m_head->m_next->m_next;
	//loop through the nodes, init force with gravity, add collision force with the ground
	while(1){
		m_tip_spheres.push_back(temp_rigid_section->m_left_leg->GetTipSphere());
		m_tip_spheres.push_back(temp_rigid_section->m_right_leg->GetTipSphere());
		temp_soft_section = temp_rigid_section->m_next;
		if(temp_soft_section){
			temp_rigid_section = temp_soft_section->m_next;
		}
		else
			break;
	}

	m_tail_left->InitNeuroNet(m_head,m_tail_rigid,1);
	m_tail_right->InitNeuroNet(m_head,m_tail_rigid,-1);

	if(m_terrain->m_terrain_type == TERRAIN_WATER){
	
		m_terrain->m_water->UpdateSpheres(m_tip_spheres);

	}

}

void Millipede::SetWorld(World* a_world){
	m_world = a_world;
	
	//draw all the rigid and soft sections
	MillipedeRigidSection *temp_rigid_section;
	MillipedeSoftSection *temp_soft_section;
	m_head->SetWorld(a_world);
	m_head->m_next->SetWorld(a_world);
	temp_rigid_section = m_head->m_next->m_next;
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
	m_tail_soft->SetWorld(a_world);
	m_tail_rigid->SetWorld(a_world);
	m_tail_left->SetWorld(a_world);
	m_tail_right->SetWorld(a_world);

}

void Millipede::Draw(int type, const Camera& camera, const Light& light){
	//draw all the rigid and soft sections
	MillipedeRigidSection *temp_rigid_section;
	MillipedeSoftSection *temp_soft_section;
	m_head->Draw(type, camera, light);//head
	m_head->m_next->Draw(type, camera, light);//neck
	temp_rigid_section = m_head->m_next->m_next;
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

	m_tail_soft->Draw(type, camera, light);
	m_tail_rigid->Draw(type, camera, light);

	m_tail_left->Draw(type, camera, light);
	m_tail_right->Draw(type, camera, light);
	//draw the bounding box
	/*
	m_Drawer->SetIdentity();
	m_Drawer->SetColor(Eigen::Vector3f(1.0,1.0,0.0));
	m_Drawer->Translate(Eigen::Vector3f((m_bounding_box[0] + m_bounding_box[1])/2.0, 0,
		(m_bounding_box[2] + m_bounding_box[3])/2.0));
	m_Drawer->Scale(Eigen::Vector3f(m_bounding_box[1] - m_bounding_box[0],1,m_bounding_box[3] - m_bounding_box[2]));
	m_Drawer->DrawCube(type, camera, light);
	*/
}

void Millipede::UpdateAll(double dt){

	UpdateBoundingBox();

	//update head
	m_head->UpdateAll(dt);

	m_tail_left->UpdateAll(dt);
	m_tail_right->UpdateAll(dt);

	if(m_stopped)
		return;

	//update each body section
	MillipedeRigidSection *temp_rigid_section;
	MillipedeSoftSection *temp_soft_section;
	temp_rigid_section = m_head->m_next->m_next;
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
	std::vector<MillipedeSoftSection*> temp_sections;
	temp_sections.push_back(m_head->m_next);
	temp_rigid_section = m_head->m_next->m_next;
	while(1){
		temp_soft_section = temp_rigid_section->m_next;
		if(temp_soft_section){
			temp_sections.push_back(temp_soft_section);
			temp_rigid_section = temp_soft_section->m_next;
		}
		else
			break;
	}

	for(int i = 0; i< temp_sections.size(); i++)
		temp_sections[i]->UpdateAll(dt);
	
	m_tail_soft->UpdateAll(dt);
	m_tail_rigid->UpdateAll(dt);


}


void Millipede::UpdateTipSphere(){
	



}
void Millipede::UpdateBoundingBox(){

	double x_min, z_min, x_max, z_max;

#ifdef __linux__
	x_min = std::min(m_head->m_left_antenna->m_tip_position[0],m_head->m_right_antenna->m_tip_position[0]);
	x_max = std::max(m_head->m_left_antenna->m_tip_position[0],m_head->m_right_antenna->m_tip_position[0]);
	z_min = std::min(m_head->m_left_antenna->m_tip_position[2],m_head->m_right_antenna->m_tip_position[2]);
	z_max = std::max(m_head->m_left_antenna->m_tip_position[2],m_head->m_right_antenna->m_tip_position[2]);
#else
	x_min = min(m_head->m_left_antenna->m_tip_position[0],m_head->m_right_antenna->m_tip_position[0]);
	x_max = max(m_head->m_left_antenna->m_tip_position[0],m_head->m_right_antenna->m_tip_position[0]);
	z_min = min(m_head->m_left_antenna->m_tip_position[2],m_head->m_right_antenna->m_tip_position[2]);
	z_max = max(m_head->m_left_antenna->m_tip_position[2],m_head->m_right_antenna->m_tip_position[2]);
#endif

	MillipedeRigidSection *temp_rigid_section;
	MillipedeSoftSection *temp_soft_section;
	temp_rigid_section = m_head->m_next->m_next;
	Eigen::Vector3f current_point;

	
	//rigid phase
	while(1){

		
		current_point = temp_rigid_section->m_left_leg->m_tip_position;

		if(x_min > current_point[0])
			x_min = current_point[0];

		if(x_max < current_point[0])
			x_max = current_point[0];
		
		if(z_min > current_point[2])
			z_min = current_point[2];
		
		if(z_max < current_point[2])
			z_max = current_point[2];

		
		current_point = temp_rigid_section->m_right_leg->m_tip_position;
		
		if(x_min > current_point[0])
			x_min = current_point[0];
		
		if(x_max < current_point[0])
			x_max = current_point[0];
		
		if(z_min > current_point[2])
			z_min = current_point[2];
		
		if(z_max < current_point[2])
			z_max = current_point[2];
	

		temp_soft_section = temp_rigid_section->m_next;
		if(temp_soft_section){
			temp_rigid_section = temp_soft_section->m_next;
		}
		else
			break;
	}
	
	double buffer_zone_size = 3.0;
	m_bounding_box = buffer_zone_size * Eigen::Vector4f(-1,1,-1,1) + Eigen::Vector4f(x_min, x_max, z_min, z_max);

}

void Millipede::ReceiveControllKey(int key){

	MILLIPEDE_TURNING turning_direction;
	double move_dist = 0.01;
	switch (key)
	{
	case 1:
		turning_direction = GO_STRAIGHT;
		break;
	case 2:
		turning_direction = TURN_LEFT;
		break;
	case 3:
		turning_direction = TURN_RIGHT;
		break;
	default:
		return;
	}

	Eigen::Vector3f front_vector; double turn_angle;
	front_vector = m_head->m_rotation*Eigen::Vector3f(-1,0,0);
	//turn to the desired direction
	if(turning_direction != GO_STRAIGHT){
		turn_angle =  (turning_direction == TURN_LEFT ? 1:-1)*0.05;//y axis anti clockwise
		Eigen::AngleAxis<float> turny(DegreesToRadians*turn_angle, Eigen::Vector3f(0,1,0));
		m_head->m_rotation = m_head->m_rotation*turny.toRotationMatrix();
	}
	//go straight in the front vector direction
	m_head->m_Center += move_dist*front_vector;

	//now update the matrixes and fixed points for soft part update
	m_head->m_Trans.setIdentity();
	m_head->m_Trans.translate(m_head->m_Center);
	m_head->m_Trans.rotate(m_head->m_rotation);
	m_head->m_Trans.scale(m_head->m_Size);
	m_head->m_TransBack = m_head->m_Trans.inverse();
	m_head->UpdateFixed();

}

void Millipede::SetControl(bool ctr){
	if(ctr) m_head->EnterMode(CONTROLLED);
	else m_head->ExitControl();
};

bool Millipede::IsHeadFixed(){
	return m_head->m_fixed;
}

bool Millipede::IsTailFixed(){
	return m_last->m_fixed;
}

void Millipede::FixHead(){
	m_head->m_fixed = true;
}

void Millipede::ReleaseHead(){

	m_head->m_fixed = false;
}

void Millipede::FixTail(){
	m_last->m_fixed = true;
}

void Millipede::ReleaseTail(){
	m_last->m_fixed = false;
}

struct leg_state_box{
	double l_x;
	double l_y;
	double r_x;
	double r_y;
}
;
typedef std::vector<leg_state_box> BoxVec;

struct StateTable{
	void Insert(LEG_STATUS state, leg_state_box box){
	  switch(state){
		  case LEG_SWAY_FORWARD_1:
		  case LEG_SWAY_FORWARD_2:
			  swf.push_back(box);
			  break;
		  case LEG_SWAY_BACKWARD_1:
		  case LEG_SWAY_BACKWARD_2:
			  swb.push_back(box);
			  break;
		  case LEG_STANCE:
			  sta.push_back(box);
			  break;
		  case LEG_ADJUST:
			  adj.push_back(box);
			  break;
		  }

	};
	BoxVec swf;
	BoxVec swb;
	BoxVec sta;
	BoxVec adj;
};

void Millipede::Output2File(std::ofstream* filestream, int type){

	(*filestream)<<"//Begin Millipede\n"<<std::endl;
if(type == 0){
	//output for the python script in maya
    //output head

    m_head->Output2File(filestream, 0);
	m_head->m_left_antenna->Output2File(filestream, 0);
	m_head->m_right_antenna->Output2File(filestream, 0);
    //output each body section
    MillipedeRigidSection *temp_rigid_section;
    MillipedeSoftSection *temp_soft_section;
    temp_rigid_section = m_head->m_next->m_next;

    //rigid phase
    while(1){
        temp_rigid_section->Output2File(filestream, 0);
        temp_soft_section = temp_rigid_section->m_next;
        if(temp_soft_section){
            temp_rigid_section = temp_soft_section->m_next;
        }
        else
            break;
    }

    // soft part end
	//tail
	
	(*filestream)<<"//BEGIN TAIL "<<std::endl;
	Eigen::Vector3f ea = m_tail_rigid->m_rotation.eulerAngles(0,1,2)/DegreesToRadians;
	(*filestream)<<"setAttr \"tail.translate\" "<< -m_tail_rigid->m_Center.z() <<" "<<m_tail_rigid->m_Center.y()<<" "<< m_tail_rigid->m_Center.x()<<";"<<std::endl;
	(*filestream)<<"setAttr \"tail.rotate\" "<<-ea.z()<<" "<<ea.y()<<" "<<ea.x()<<";"<<std::endl;

	m_tail_left->Output2File(filestream, 0);
	m_tail_right->Output2File(filestream, 0);
	
	(*filestream)<<"//END TAIL "<<std::endl;

	(*filestream)<<"//set one key frame"<<std::endl;
	(*filestream)<<"select -r "
		"s1 s1l1 s1l2 s1r1 s1r2 s2 s2l1 s2l2 s2r1 s2r2 s3 s3l1 s3l2 s3r1 s3r2 s4 s4l1 s4l2 s4r1 s4r2 "
		"s5 s5l1 s5l2 s5r1 s5r2 s6 s6l1 s6l2 s6r1 s6r2 s7 s7l1 s7l2 s7r1 s7r2 s8 s8l1 s8l2 s8r1 s8r2 "
		"s9 s9l1 s9l2 s9r1 s9r2 s10 s10l1 s10l2 s10r1 s10r2 s11 s11l1 s11l2 s11r1 s11r2 s12 s12l1 s12l2 s12r1 s12r2 "
		"s13 s13l1 s13l2 s13r1 s13r2 s14 s14l1 s14l2 s14r1 s14r2 s15 s15l1 s15l2 s15r1 s15r2 s16 s16l1 s16l2 s16r1 s16r2 "
		"s17 s17l1 s17l2 s17r1 s17r2 s18 s18l1 s18l2 s18r1 s18r2 s19 s19l1 s19l2 s19r1 s19r2 "
		"al0 al1 al2 al3 al4 al5 al6 al7 al8 al9 al10 al11 al12 al13 al14 al15 al16 al17 al18 al19 al20 al21 al22 al23 al24 al25 al26 al27 " 
		"ar0 ar1 ar2 ar3 ar4 ar5 ar6 ar7 ar8 ar9 ar10 ar11 ar12 ar13 ar14 ar15 ar16 ar17 ar18 ar19 ar20 ar21 ar22 ar23 ar24 ar25 ar26 al27 " 
		"head headl headr mouthl mouthr tail tail_l0 tail_r0;"<<std::endl;

	(*filestream)<<"setKeyframe -breakdown 0 -hierarchy none -controlPoints 0 -shape 0 {"
		"\"s1\",\"s1l1\",\"s1l2\",\"s1r1\",\"s1r2\",\"s2\",\"s2l1\",\"s2l2\",\"s2r1\",\"s2r2\",\"s3\",\"s3l1\",\"s3l2\",\"s3r1\",\"s3r2\",\"s4\",\"s4l1\",\"s4l2\",\"s4r1\",\"s4r2\","
		"\"s5\",\"s5l1\",\"s5l2\",\"s5r1\",\"s5r2\",\"s6\",\"s6l1\",\"s6l2\",\"s6r1\",\"s6r2\",\"s7\",\"s7l1\",\"s7l2\",\"s7r1\",\"s7r2\",\"s8\",\"s8l1\",\"s8l2\",\"s8r1\",\"s8r2\","
		"\"s9\",\"s9l1\",\"s9l2\",\"s9r1\",\"s9r2\",\"s10\",\"s10l1\",\"s10l2\",\"s10r1\",\"s10r2\",\"s11\",\"s11l1\",\"s11l2\",\"s11r1\",\"s11r2\",\"s12\",\"s12l1\",\"s12l2\",\"s12r1\",\"s12r2\","
		"\"s13\",\"s13l1\",\"s13l2\",\"s13r1\",\"s13r2\",\"s14\",\"s14l1\",\"s14l2\",\"s14r1\",\"s14r2\",\"s15\",\"s15l1\",\"s15l2\",\"s15r1\",\"s15r2\",\"s16\",\"s16l1\",\"s16l2\",\"s16r1\",\"s16r2\","
		"\"s17\",\"s17l1\",\"s17l2\",\"s17r1\",\"s17r2\",\"s18\",\"s18l1\",\"s18l2\",\"s18r1\",\"s18r2\",\"s19\",\"s19l1\",\"s19l2\",\"s19r1\",\"s19r2\","
		"\"al0\",\"al1\",\"al2\",\"al3\",\"al4\",\"al5\",\"al6\",\"al7\",\"al8\",\"al9\",\"al10\",\"al11\",\"al12\",\"al13\",\"al14\",\"al15\",\"al16\",\"al17\",\"al18\",\"al19\",\"al20\",\"al21\",\"al22\",\"al23\",\"al24\",\"al25\",\"al26\",\"al27\"," 
		"\"ar0\",\"ar1\",\"ar2\",\"ar3\",\"ar4\",\"ar5\",\"ar6\",\"ar7\",\"ar8\",\"ar9\",\"ar10\",\"ar11\",\"ar12\",\"ar13\",\"ar14\",\"ar15\",\"ar16\",\"ar17\",\"ar18\",\"ar19\",\"ar20\",\"ar21\",\"ar22\",\"ar23\",\"ar24\",\"ar25\",\"ar26\",\"ar27\"," 
		"\"head\",\"headl\",\"headr\",\"mouthl\",\"mouthr\",\"tail\",\"tail_l0\",\"tail_r0\"};"<<std::endl;
	

		
}
else if(type == 1){
// output for povray
    // rigid part start
    (*filestream)<<"#declare MillipedeRigidPart = union {\n"<<std::endl;

    //output head
    m_head->Output2File(filestream, 1);

    //output each body section
    MillipedeRigidSection *temp_rigid_section;
    MillipedeSoftSection *temp_soft_section;
    temp_rigid_section = m_head->m_next->m_next;

    //rigid phase
    while(1){
        temp_rigid_section->Output2File(filestream, 1);
        temp_soft_section = temp_rigid_section->m_next;
        if(temp_soft_section){
            temp_rigid_section = temp_soft_section->m_next;
        }
        else
            break;
    }


    // rigid part end
    (*filestream)<<"}\n"<<std::endl;

    (*filestream)<<"#declare MillipedeAntenneaPart = union { \n"<<std::endl; 
	m_head->m_left_antenna->Output2File(filestream,1);
	m_head->m_right_antenna->Output2File(filestream,1);
    (*filestream)<<"}\n"<<std::endl;

    (*filestream)<<"#declare MillipedeTailPart = union { \n"<<std::endl; 
	m_tail_rigid->Output2File(filestream);
	m_tail_left->Output2File(filestream,1);
	m_tail_right->Output2File(filestream,1);

	(*filestream)<<"}\n"<<std::endl;

    (*filestream)<<"#declare LegForward = union { \n"<<std::endl; 
	 temp_rigid_section = m_head->m_next->m_next;
    //rigid phase
    while(1){
		if(temp_rigid_section->m_left_leg->m_leg_state == LEG_SWAY_FORWARD_1 || temp_rigid_section->m_left_leg->m_leg_state == LEG_SWAY_FORWARD_2)
			temp_rigid_section->m_left_leg->Output2File(filestream);
		if(temp_rigid_section->m_right_leg->m_leg_state == LEG_SWAY_FORWARD_1 || temp_rigid_section->m_right_leg->m_leg_state == LEG_SWAY_FORWARD_2)
			temp_rigid_section->m_right_leg->Output2File(filestream);
        temp_soft_section = temp_rigid_section->m_next;
        if(temp_soft_section){
            temp_rigid_section = temp_soft_section->m_next;
        }
        else
            break;
    }
    (*filestream)<<"}\n"<<std::endl;

    (*filestream)<<"#declare LegBackward = union { \n"<<std::endl; 
	temp_rigid_section = m_head->m_next->m_next;
    //rigid phase
    while(1){
		if(temp_rigid_section->m_left_leg->m_leg_state == LEG_SWAY_BACKWARD_1 || temp_rigid_section->m_left_leg->m_leg_state == LEG_SWAY_BACKWARD_2)
			temp_rigid_section->m_left_leg->Output2File(filestream);
		if(temp_rigid_section->m_right_leg->m_leg_state == LEG_SWAY_BACKWARD_1 || temp_rigid_section->m_right_leg->m_leg_state == LEG_SWAY_BACKWARD_2)
			temp_rigid_section->m_right_leg->Output2File(filestream);
        temp_soft_section = temp_rigid_section->m_next;
        if(temp_soft_section){
            temp_rigid_section = temp_soft_section->m_next;
        }
        else
            break;
    }
    (*filestream)<<"}\n"<<std::endl;

    (*filestream)<<"#declare LegStance = union { \n"<<std::endl; 
    temp_rigid_section = m_head->m_next->m_next;
	  //rigid phase
    while(1){
		if(temp_rigid_section->m_left_leg->m_leg_state == LEG_STANCE)
			temp_rigid_section->m_left_leg->Output2File(filestream);
		if(temp_rigid_section->m_right_leg->m_leg_state == LEG_STANCE)
			temp_rigid_section->m_right_leg->Output2File(filestream);
        temp_soft_section = temp_rigid_section->m_next;
        if(temp_soft_section){
            temp_rigid_section = temp_soft_section->m_next;
        }
        else
            break;
    }
    (*filestream)<<"}\n"<<std::endl;

    (*filestream)<<"#declare LegAdjust = union { \n"<<std::endl;
    temp_rigid_section = m_head->m_next->m_next;
	  //rigid phase
     while(1){
		if(temp_rigid_section->m_left_leg->m_leg_state == LEG_ADJUST)
			temp_rigid_section->m_left_leg->Output2File(filestream);
		if(temp_rigid_section->m_right_leg->m_leg_state == LEG_ADJUST)
			temp_rigid_section->m_right_leg->Output2File(filestream);
        temp_soft_section = temp_rigid_section->m_next;
        if(temp_soft_section){
            temp_rigid_section = temp_soft_section->m_next;
        }
        else
            break;
    }
    (*filestream)<<"}\n"<<std::endl;

//	m_left_leg->Output2File(filestream);
//	m_right_leg->Output2File(filestream);

    // soft part start
    (*filestream)<<"#declare MillipedeSoftPart = union { \n"<<std::endl;
    //soft phase
	m_head->m_next->Output2File(filestream);//first soft
	//two antenna
    temp_rigid_section = m_head->m_next->m_next;
    while(1){
        temp_soft_section = temp_rigid_section->m_next;
        if(temp_soft_section){
            temp_soft_section->Output2File(filestream);
            temp_rigid_section = temp_soft_section->m_next;
        }
        else
            break;
    }

	m_tail_soft->Output2File(filestream);
    // soft part end
    (*filestream)<<"}\n"<<std::endl;
	
}
else if(type == 2){
    MillipedeRigidSection *temp_rigid_section;
    MillipedeSoftSection *temp_soft_section;
    temp_rigid_section = m_head->m_next->m_next;
	//for the graph
	 temp_rigid_section = m_head->m_next->m_next;
	 double start_time, end_time;
	 double ratio_a = 0, ratio_b = 0;
	 double shift_x = 1, shift_y = 20, bar_width = 3, bar_height = 0.4, bar_dist_x = 0.5, bar_dist_y = 0.2;
	 std::queue<LegStateTransition> copy_history_state;
	 LegStateTransition cur_trans;
	 StateTable statetable;
	 leg_state_box temp_box;
	 //rigid phase
    while(1){
		//output for each section, each leg
		ratio_a = 0; 
		ratio_b = 0;
		end_time = temp_rigid_section->m_timer;
		start_time = end_time - temp_rigid_section->m_left_leg->m_history_length;//record 2 second;
		copy_history_state = temp_rigid_section->m_left_leg->m_history_state;
		while(copy_history_state.size()>0){
			  cur_trans = copy_history_state.front();
			  copy_history_state.pop();
			  ratio_a = ratio_b;
			  ratio_b = (cur_trans.m_time_stamp - start_time)/(end_time - start_time);
			  //assert(ratio_b>=0);
			  //Output a box based on ratio_a and ratio_b;
			  temp_box.l_x = shift_x + bar_width*ratio_a;
			  temp_box.l_y = shift_y;
			  temp_box.r_x = shift_x + bar_width*ratio_b;
			  temp_box.r_y = shift_y + bar_height;
			
			  statetable.Insert(cur_trans.m_prev_state, temp_box);
		}
		ratio_a = ratio_b;
		ratio_b = 1;
		//Output the last box;
		temp_box.l_x = shift_x + bar_width*ratio_a;
		temp_box.l_y = shift_y;
		temp_box.r_x = shift_x + bar_width*ratio_b;
		temp_box.r_y = shift_y + bar_height;
			
		statetable.Insert(temp_rigid_section->m_left_leg->m_leg_state, temp_box);
		
		//right
		ratio_a = 0;
		ratio_b = 0;
		shift_x += bar_dist_x + bar_width;
		copy_history_state = temp_rigid_section->m_right_leg->m_history_state;
		while(!copy_history_state.empty()){
			  cur_trans = copy_history_state.front();
			  copy_history_state.pop();
			  ratio_a = ratio_b;
			  ratio_b = (cur_trans.m_time_stamp - start_time)/(end_time - start_time);
			  //assert(ratio_b>=0);
			  //Output a box based on ratio_a and ratio_b;
			  temp_box.l_x = shift_x + bar_width*ratio_a;
			  temp_box.l_y = shift_y;
			  temp_box.r_x = shift_x + bar_width*ratio_b;
			  temp_box.r_y = shift_y + bar_height;
			
			  statetable.Insert(cur_trans.m_prev_state, temp_box);
		}
		ratio_a = ratio_b;
		ratio_b = 1;
		//Output the last box;
		temp_box.l_x = shift_x + bar_width*ratio_a;
		temp_box.l_y = shift_y;
		temp_box.r_x = shift_x + bar_width*ratio_b;
		temp_box.r_y = shift_y + bar_height;
			
		statetable.Insert(temp_rigid_section->m_right_leg->m_leg_state, temp_box);
	
		//move to the next column
		shift_x -= bar_dist_x + bar_width;
		shift_y -= bar_dist_y + bar_height;

        temp_soft_section = temp_rigid_section->m_next;
        if(temp_soft_section){
            temp_rigid_section = temp_soft_section->m_next;
        }
        else
            break;
	
	}//end looping all rigid section

	//output the statetable;

	(*filestream)<<"//Output Diagram"<<std::endl;
	(*filestream)<<"#declare SWF = object{" <<std::endl<<"union{"<<std::endl;
	for(int i = 0;i < statetable.swf.size();i++){
		(*filestream)<<"box{"<<std::endl;
		(*filestream)<<"<"<<statetable.swf[i].l_x <<","<<statetable.swf[i].l_y<<","<<"0"<<">,"<<std::endl;
		(*filestream)<<"<"<<statetable.swf[i].r_x <<","<<statetable.swf[i].r_y<<","<<"0"<<">"<<std::endl;
		(*filestream)<<"}"<<std::endl;
	}
	(*filestream)<<"}"<<std::endl<<"}"<<std::endl;
	
	(*filestream)<<"#declare SWB = object{" <<std::endl<<"union{"<<std::endl;
	for(int i = 0;i < statetable.swb.size();i++){
		(*filestream)<<"box{"<<std::endl;
		(*filestream)<<"<"<<statetable.swb[i].l_x <<","<<statetable.swb[i].l_y<<","<<"0"<<">,"<<std::endl;
		(*filestream)<<"<"<<statetable.swb[i].r_x <<","<<statetable.swb[i].r_y<<","<<"0"<<">"<<std::endl;
		(*filestream)<<"}"<<std::endl;
	}
	(*filestream)<<"}"<<std::endl<<"}"<<std::endl;
	
	(*filestream)<<"#declare STA = object{" <<std::endl<<"union{"<<std::endl;
	for(int i = 0;i < statetable.sta.size();i++){
		(*filestream)<<"box{"<<std::endl;
		(*filestream)<<"<"<<statetable.sta[i].l_x <<","<<statetable.sta[i].l_y<<","<<"0"<<">,"<<std::endl;
		(*filestream)<<"<"<<statetable.sta[i].r_x <<","<<statetable.sta[i].r_y<<","<<"0"<<">"<<std::endl;
		(*filestream)<<"}"<<std::endl;
	}
	(*filestream)<<"}"<<std::endl<<"}"<<std::endl;
	
	(*filestream)<<"#declare ADJ = object{" <<std::endl<<"union{"<<std::endl;
	for(int i = 0;i < statetable.adj.size();i++){
		(*filestream)<<"box{"<<std::endl;
		(*filestream)<<"<"<<statetable.adj[i].l_x <<","<<statetable.adj[i].l_y<<","<<"0"<<">,"<<std::endl;
		(*filestream)<<"<"<<statetable.adj[i].r_x <<","<<statetable.adj[i].r_y<<","<<"0"<<">"<<std::endl;
		(*filestream)<<"}"<<std::endl;
	}
	(*filestream)<<"}"<<std::endl<<"}" <<std::endl;

}
else{
	assert(-1);
}
    (*filestream)<<std::endl<<"//End Millipede\n"<<std::endl;
}
