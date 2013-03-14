//RigidCube.h
//Created By Franklin Fang @ 3/28/2012

#ifndef RIGID_CUBE_H
#define RIGID_CUBE_H

#include "Cube.h"
#include "Eigen/Dense"
#include <vector>

class RigidCube:public Cube{
    
public:
	RigidCube();
	virtual ~RigidCube();

	std::vector<Node*> m_nodes;//the list of nodes attached from deformable body
	std::vector<Eigen::Vector3f> m_fixed_r;
	std::vector<Node*> m_edges;
	std::vector<Eigen::Vector3f> m_edge_r;
	double m_density;
	double m_mass;
	bool m_fixed;

	Eigen::Matrix3f m_rotation;
	Eigen::Vector3f m_avelocity;
	Eigen::Matrix3f m_inertia;
	Eigen::Matrix3f m_inertia_inverse;
	Eigen::Vector3f m_velocity;

	Eigen::Vector3f m_force;
	Eigen::Vector3f m_torque;

	virtual void Init(double density, Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color);
	virtual void RotateY(double degree);
	virtual void UpdateAll(double dt);
	virtual void UpdatePhysics(double dt);
	virtual void UdpateDraw(){};
	virtual void SetEdges();
	virtual void AttachNodes(const std::vector<Node*>  a_list_nodes);
	virtual void AttachNode(Node* a_node);
	virtual void HandleCollision();
	virtual bool CheckCollision(const Eigen::Vector3f& a_point, Eigen::Vector3f& a_normal);
	virtual void UpdateFixed();
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif;
