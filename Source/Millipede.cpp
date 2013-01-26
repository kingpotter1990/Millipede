#include "Millipede.h"
#include "MillipedeHead.h"
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
		current_soft_section->Init(Eigen::Vector3i(3,3,3),1.0,2000,0.4,100.0,temp_position,
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

}

void Millipede::UpdateAll(double dt){

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