#ifndef MILLIPEDE_H_
#define MILLIPEDE_H_

#include <string>
#include "Utility.h" 
#include "Object.h"

class MillipedeRigidSection;
class MillipedeSoftSection;
class MillipedeHead;
class Terrain;

class Millipede:public Object{

public:
	Millipede();
	virtual ~Millipede();
	void Init(Eigen::Vector3f a_position, int a_num_section, Eigen::Vector3f a_rigid_size, double a_soft_length, Terrain* a_terrain);
	void Draw(int type, const Camera& camera, const Light& light);
	void Update(double a_dt);
    void SetWorld(World* a_world);
    void UpdateAll(double dt);
	void ReceiveControllKey(int key);
	void SetControl(bool ctr);
	void FixHead();
	void FixTail();
protected:
	void InitPhysics(Eigen::Vector3f a_position, int a_num_section, Eigen::Vector3f a_rigid_size, double a_soft_length);
	void InitNeuroNet(Terrain* a_terrain);
	//void UpdateLegTipPosition(MillipedeRigidSection* a_section, int l_or_r);
	//void UpdateLegRootPosition(MillipedeRigidSection* a_section, int l_or_r);

public:
	int m_num_section;
	Eigen::Vector3f m_size;
	double m_link_length;

	Terrain* m_terrain;

	MillipedeHead* m_head;
	MillipedeRigidSection* m_tail;
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif