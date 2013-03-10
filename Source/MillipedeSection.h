
#ifndef MILLIPEDE_SECTION_H_
#define MILLIPEDE_SECTION_H_

#include "RigidCube.h"
#include "3DDeformable.h"

class MillipedeLeg;
class Millipede;
class MillipedeRigidSection;
class MillipedeSoftSection;
class Terrain;
class Drawer;

enum SECTION_STATUS {LEG_SUPPORTED, NOT_SUPPORTED};

class MillipedeRigidSection:public RigidCube{
	//rigid section is rigid in physics
	//has two pairs of leg for millipede

public:
	MillipedeRigidSection(){};
	~MillipedeRigidSection(){};
	virtual void InitPhysics(double density, Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color);
	virtual void InitNeuroNet(Millipede* a_root, int a_id);
	virtual void SetWorld(World* a_world);
	virtual void Draw(int type, const Camera& camera, const Light& light);
	virtual void UpdateNeuronNet(double dt);
	virtual void UpdatePBD(double dt);
	virtual void UpdateAll(double dt);
	virtual void Output2File(std::ofstream* filestream, int type);
public:
	MillipedeLeg* m_left_leg;
	MillipedeLeg* m_right_leg;

	SECTION_STATUS m_body_state;

	MillipedeSoftSection* m_next;
	MillipedeSoftSection* m_prev;

	Millipede* m_master;
	Terrain* m_terrain;
	int m_section_id;
	double m_height_obj;
	Eigen::Vector3f m_orient_obj;
	double m_prev_dis_obj;
	double m_current_height;
	Eigen::Vector3f m_linear_speed;

	Drawer* m_Drawer;
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class MillipedeSoftSection:public Deformable3D{
	//Finite Element Simulated Deformable Body Section

public:
	MillipedeSoftSection(){}
	~MillipedeSoftSection(){}
public:

	MillipedeRigidSection* m_next;
	MillipedeRigidSection* m_prev;

};

#endif