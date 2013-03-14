
#include "RigidCube.h"
#include "Terrain.h"
#include "Eigen/Dense"
#include "World.h"

RigidCube::RigidCube(){
	m_nodes.clear();
	m_type = TypeRigidCube;
}

RigidCube::~RigidCube(){


}

void RigidCube::Init(double density, Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color){
	Cube::Init(center, size, color);
	m_density = density;
	m_mass = m_Size[0]*m_Size[1]*m_Size[2]*m_density;
	m_rotation.setIdentity();
	m_avelocity *= 0;
	m_velocity *= 0;
	m_force *= 0;
	m_torque *= 0;

	m_inertia.setIdentity();
	m_inertia(0,0) = m_Size[1]*m_Size[1] + m_Size[2]*m_Size[2];
	m_inertia(1,1) = m_Size[0]*m_Size[0] + m_Size[2]*m_Size[2];
	m_inertia(2,2) = m_Size[0]*m_Size[0] + m_Size[1]*m_Size[1];
	m_inertia *= m_mass/12.0;
	m_inertia_inverse = m_inertia.inverse();

	SetEdges();

	m_fixed = false;
}
void RigidCube::SetEdges(){
	
	Node* temp_node;
	temp_node = new Node;
	temp_node->m_Position = m_Center + Eigen::Vector3f(m_Size[0],m_Size[1],m_Size[2])*0.5;
	m_edges.push_back(temp_node);
	m_edge_r.push_back(m_TransBack.linear()*(temp_node->m_Position - m_Center));

	temp_node = new Node;
	temp_node->m_Position = m_Center + Eigen::Vector3f(m_Size[0],m_Size[1],-m_Size[2])*0.5;
	m_edges.push_back(temp_node);
	m_edge_r.push_back(m_TransBack.linear()*(temp_node->m_Position - m_Center));

	temp_node = new Node;
	temp_node->m_Position = m_Center + Eigen::Vector3f(m_Size[0],-m_Size[1],m_Size[2])*0.5;
	m_edges.push_back(temp_node);
	m_edge_r.push_back(m_TransBack.linear()*(temp_node->m_Position - m_Center));

	temp_node = new Node;
	temp_node->m_Position = m_Center + Eigen::Vector3f(m_Size[0],-m_Size[1],-m_Size[2])*0.5;
	m_edges.push_back(temp_node);
	m_edge_r.push_back(m_TransBack.linear()*(temp_node->m_Position - m_Center));

	temp_node = new Node;
	temp_node->m_Position = m_Center + Eigen::Vector3f(-m_Size[0],m_Size[1],m_Size[2])*0.5;
	m_edges.push_back(temp_node);
	m_edge_r.push_back(m_TransBack.linear()*(temp_node->m_Position - m_Center));

	temp_node = new Node;
	temp_node->m_Position = m_Center + Eigen::Vector3f(-m_Size[0],m_Size[1],-m_Size[2])*0.5;
	m_edges.push_back(temp_node);
	m_edge_r.push_back(m_TransBack.linear()*(temp_node->m_Position - m_Center));

	temp_node = new Node;
	temp_node->m_Position = m_Center + Eigen::Vector3f(-m_Size[0],-m_Size[1],m_Size[2])*0.5;
	m_edges.push_back(temp_node);
	m_edge_r.push_back(m_TransBack.linear()*(temp_node->m_Position - m_Center));

	temp_node = new Node;
	temp_node->m_Position = m_Center + Eigen::Vector3f(-m_Size[0],-m_Size[1],-m_Size[2])*0.5;
	m_edges.push_back(temp_node);
	m_edge_r.push_back(m_TransBack.linear()*(temp_node->m_Position - m_Center));

}
void RigidCube::UpdateAll(double dt){
		UpdatePhysics(dt);//actually the draw is also updated
}

void RigidCube::UpdatePhysics(double dt){
	//first apply forces from connected nodes, plus gravity
	//update orientation and position of the rigid cube (in the draw, the matrix is generated based on the center, orientation and )
	//update the position of the fixed nodes using rigid body dynamics
	//update the velocity of the fixed nodes 
	if(m_fixed)
	{
		m_velocity *= 0;
		m_avelocity *= 0;
		return;
	}

	HandleCollision();
	//udpate the force and torque on this cube
	m_force = Eigen::Vector3f(0,-1.0,0.0)*m_mass*GRAVITY_CONSTANT;
	m_torque = Eigen::Vector3f(0,0.0,0.0);
	Eigen::Vector3f temp_elastic_force;
	for(unsigned int i = 0; i < m_nodes.size(); i++){
	//loop through the fixed nodes
		//add elastic force from the node
		temp_elastic_force = m_nodes[i]->m_Force - Eigen::Vector3f(0,-1.0,0.0)*m_nodes[i]->m_Mass*GRAVITY_CONSTANT;
		m_force += temp_elastic_force;
		m_torque += (m_Trans.linear()*m_fixed_r[i]).cross(temp_elastic_force);
	}

	
	//forward euler update states
	m_velocity += m_force*dt/m_mass;
	m_Center += m_velocity*dt;
	Eigen::Matrix3f avelocity_star;
	avelocity_star<< 0, -m_avelocity[2], m_avelocity[1],
						m_avelocity[2],0,-m_avelocity[0],
						-m_avelocity[1],m_avelocity[0],0;
	m_rotation += avelocity_star*m_rotation*dt;
	m_avelocity += dt*(m_rotation*m_inertia_inverse*m_rotation.transpose())*m_torque;
	//now update the matrixes
	m_Trans.setIdentity();
	m_Trans.translate(m_Center);
	m_Trans.rotate(m_rotation);
	m_Trans.scale(m_Size);

	m_TransBack = m_Trans.inverse();


	//feedback from rigid body dynamics to the elastic body nodes
	//udpate the position and velocity of the fixed nodes
	UpdateFixed();

}

void RigidCube::UpdateFixed(){
	//update the position and velocity of the fixed points
	for(unsigned int i = 0; i < m_nodes.size(); i++){
		m_nodes[i]->m_Position = m_Trans.linear()*m_fixed_r[i] + m_Center;
		m_nodes[i]->m_Velocity = m_avelocity.cross(m_Trans.linear()*m_fixed_r[i]) + m_velocity;
	}
	for(unsigned int i = 0; i < m_edges.size(); i++){
		m_edges[i]->m_Position =  m_Trans.linear()*m_edge_r[i] + m_Center;
		m_edges[i]->m_Velocity = m_avelocity.cross(m_Trans.linear()*m_edge_r[i]) + m_velocity;
	}

}

void RigidCube::AttachNodes(const std::vector<Node*> a_list_nodes){
    
    std::vector<Node*>::const_iterator iter;
    for(iter = a_list_nodes.begin(); iter != a_list_nodes.end(); iter++)
    {
        (*iter)->m_Fixed = true;
        m_nodes.push_back(*iter);
        m_fixed_r.push_back(m_TransBack.linear()*((*iter)->m_Position - m_Center));
    }

}

void RigidCube::AttachNode(Node* a_node){
	a_node->m_Fixed = true;
	m_nodes.push_back(a_node);
	m_fixed_r.push_back(m_TransBack.linear()*(a_node->m_Position - m_Center));
}

void RigidCube::HandleCollision(){
//for each of the edges, check against the objects in the world
	
	Eigen::Vector3f collision_surface_normal;
	Eigen::Vector3f edge_velocity;
	double IMPULSE;
	double epsilon = 0.6;//the impulse coeeficient between 0~1
	Eigen::Vector3f temp;
	Eigen::Vector3f temp_r;
	Eigen::Matrix3f temp_I;
	for(unsigned int id = 0; id < m_edges.size(); id++)
	{
	//check each edge against some of the objects in the world
		if(CheckCollision(m_edges[id]->m_Position,collision_surface_normal)){
			//detected a point-face collision(assume collision against fixed objects)
			edge_velocity = m_edges[id]->m_Velocity;
			if(edge_velocity.dot(collision_surface_normal) < 0){
				temp_r = m_Trans.linear()*m_edge_r[id];
				temp_I = m_rotation*m_inertia_inverse*m_rotation.transpose();
				temp = temp_I*(temp_r.cross(collision_surface_normal));

				IMPULSE = -(1+epsilon)*collision_surface_normal.dot(edge_velocity)/(1/m_mass + 
					collision_surface_normal.dot(temp.cross(temp_r)));

				//update the velocity and avelocity of the cube
				m_velocity += IMPULSE*collision_surface_normal/m_mass;
				m_avelocity += IMPULSE*temp_I*temp_r.cross(collision_surface_normal);
	
			}
		}
	}//end for

}
	
bool RigidCube::CheckCollision(const Eigen::Vector3f& a_point, Eigen::Vector3f& a_normal){
	
	Cube * temp_cube;Terrain* temp_terrain;
	Eigen::Vector4f temp_point;
	Eigen::Vector4f temp_dist;
	double tempx, tempy, tempz;
	double dis_up, dis_down, dis_left, dis_right, dis_front, dis_back;

	for (unsigned int i = 0; i< m_world->List_of_Object.size(); i++) {
    //find the first intersection and calculate the force, return the force
		if(m_world->List_of_Object[i] == this)
			continue;//this one is itself, no self-collision

		switch (m_world->List_of_Object[i]->GetType()) {
			case TypeTerrain://Terrain
			{
				temp_terrain = dynamic_cast<Terrain*>(m_world->List_of_Object[i]);
				if(temp_terrain->TestInside(a_point)){
					 a_normal = temp_terrain->GetNormal(a_point);
					return true;
				}
			}
			break;
			case TypeCube://cube
			case TypeRigidCube://rigidcube
			{
				temp_cube = dynamic_cast<Cube*>(m_world->List_of_Object[i]);
				//first transform the position of the point to the cordinate system relative to the cube axis
                
				temp_point = Eigen::Vector4f(a_point[0],a_point[1],a_point[2],1.0);
				temp_dist = temp_cube->m_TransBack*temp_point;
				tempx = temp_dist[0];
				tempy = temp_dist[1];
				tempz = temp_dist[2];
                
				//check with plane -.5,.5 in x, y and z
				//if( point[0] > -0.5&&point[0] < 0.5 && point[1] > -0.5 && point[1] < 0.5 && point[2] > -0.5 && point[2] < 0.5 ){
				if( fabs(tempx) < 0.5&&fabs(tempy) < 0.5 && fabs(tempz) < 0.5){
					//the point is in the cube, determine the closet face
					dis_up = 0.50 - tempy;
					dis_down = tempy + 0.50;
					dis_left = tempx + 0.50;
					dis_right = 0.50 - tempx;
					dis_back = tempz + 0.50;
					dis_front = 0.50 - tempz;
                    
					//different collision cases
					if(dis_up < dis_down && dis_up < dis_back && dis_up < dis_left && dis_up < dis_right && dis_up < dis_front){
							a_normal = temp_cube->m_Trans*Eigen::Vector3f(0,1,0);
							return true;
					}
					if(dis_down < dis_up && dis_down < dis_back && dis_down < dis_left && dis_down < dis_right && dis_down < dis_front){
							a_normal = temp_cube->m_Trans*Eigen::Vector3f(0,-1,0);
							return true;
					}
					if(dis_left < dis_down && dis_left < dis_back && dis_left < dis_right && dis_left < dis_up && dis_left < dis_front){
							a_normal = temp_cube->m_Trans*Eigen::Vector3f(-1,0,0);
							return true;
					}
					if(dis_right < dis_down && dis_right < dis_back && dis_right < dis_left && dis_right < dis_up && dis_right < dis_front){
							a_normal = temp_cube->m_Trans*Eigen::Vector3f(1,0,0);
							return true;
					}
					if(dis_front < dis_down && dis_front < dis_back && dis_front < dis_left && dis_front < dis_right && dis_front < dis_up){
							a_normal = temp_cube->m_Trans*Eigen::Vector3f(0,0,1);
							return true;
					}
					if(dis_back < dis_down && dis_back < dis_up && dis_back < dis_left && dis_back < dis_right && dis_back < dis_front){
							a_normal = temp_cube->m_Trans*Eigen::Vector3f(0,0,-1);
							return true;
					}
				}//end if
			}//end case
				break;
		}//end switch
	}//end for
	return false;
	
}

void RigidCube::RotateY(double degree){
	

	//now update the matrixes
	m_Trans.rotate(Eigen::AngleAxisf(degree*DegreesToRadians, Eigen::Vector3f::UnitY()));
	m_TransBack = m_Trans.inverse();

	//feedback from rigid body dynamics to the elastic body nodes
	//udpate the position and velocity of the fixed nodes
	UpdateFixed();

}
