#include "Millipede.h"
#include "MillipedeHead.h"
#include "MillipedeLeg.h"
#include "MillipedeSection.h"
#include "Terrain.h"
#include "Drawer.h"

extern Drawer* myDrawer;

Millipede::Millipede(){
	m_type = TypeMixed;
}

Millipede::~Millipede(){


}

void Millipede::Init(Eigen::Vector3f a_position, int a_num_section, Eigen::Vector3f a_rigid_size, double a_soft_length, Terrain* a_terrain){

	InitPhysics(a_position,a_num_section,a_rigid_size,a_soft_length);
	InitNeuroNet(a_terrain);
}

void Millipede::InitPhysics(Eigen::Vector3f a_position, int a_num_section, Eigen::Vector3f a_rigid_size, double a_soft_length){

	assert(a_num_section >=3);//Need to be at least this lenght

	m_num_section = a_num_section;
	m_link_length = a_rigid_size[0] + a_soft_length;

	MillipedeRigidSection* previous_rigid_section, *current_rigid_section;
	MillipedeSoftSection* previous_soft_section, *current_soft_section;

	//create head section
	m_head = new MillipedeHead;
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
		current_soft_section->Init(Eigen::Vector3i(3,3,3),1.0,3000,0.4,100.0,temp_position,
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

void Millipede::InitNeuroNet(Terrain* a_terrain){

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
		temp_rigid_section->InitNeuroNet(this,id++);
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

	//draw the bounding box
	myDrawer->SetIdentity();
	myDrawer->SetColor(Eigen::Vector3f(1.0,1.0,0.0));
	myDrawer->Translate(Eigen::Vector3f((m_bounding_box[0] + m_bounding_box[1])/2.0, 0,
		(m_bounding_box[2] + m_bounding_box[3])/2.0));
	myDrawer->Scale(Eigen::Vector3f(m_bounding_box[1] - m_bounding_box[0],1,m_bounding_box[3] - m_bounding_box[2]));
	myDrawer->DrawCube(type, camera, light);

}

void Millipede::UpdateAll(double dt){

	UpdateBoundingBox();

	//update head
	m_head->UpdateAll(dt);

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

void Millipede::FixHead(){
	m_head->m_fixed = true;
}

void Millipede::FixTail(){
	m_tail->m_fixed = true;
}

void Millipede::Output2File(std::ofstream* filestream){
	
	(*filestream)<<"//BEGIN MILLIPEDE \n"<<std::endl;
	//output head
	m_head->Output2File(filestream);

	//output each body section
	MillipedeRigidSection *temp_rigid_section;
	MillipedeSoftSection *temp_soft_section;
	temp_rigid_section = m_head->m_next->m_next;
	//rigid phase
	while(1){
		temp_rigid_section->Output2File(filestream);
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
			temp_soft_section->Output2File(filestream);
			temp_rigid_section = temp_soft_section->m_next;
		}
		else
			break;
	}

	(*filestream)<<"//END MILLIPEDE \n"<<std::endl;

}
