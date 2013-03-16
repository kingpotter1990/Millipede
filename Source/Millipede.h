#ifndef MILLIPEDE_H_
#define MILLIPEDE_H_

#include <string>
#include "Utility.h" 
#include "Object.h"

class MillipedeRigidSection;
class MillipedeSoftSection;
class MillipedeHead;
class Terrain;
class Drawer;
class Sphere;
class Deformable3D;
class RigidCube;
class Millipede:public Object{

public:
	Millipede();
	virtual ~Millipede();
	void Init(Eigen::Vector3f a_position, int a_num_section, Eigen::Vector3f a_rigid_size, double a_soft_length, Terrain* a_terrain);
	void Draw(int type, const Camera& camera, const Light& light);
    void SetWorld(World* a_world);
    void UpdateAll(double dt);
	void ReceiveControllKey(int key);
	void SetControl(bool ctr);
	void FixHead();
	void FixTail();
	bool IsHeadFixed();
	bool IsTailFixed();
	void ReleaseHead();
	void ReleaseTail();
	void Output2File(std::ofstream* filestream, int type);
protected:
	void InitPhysics(Eigen::Vector3f a_position, int a_num_section, Eigen::Vector3f a_rigid_size, double a_soft_length);
	void InitNeuroNet(Terrain* a_terrain);
	void UpdateBoundingBox();
	void UpdateTipSphere();
public:
	int m_num_section;
	Eigen::Vector3f m_size;
	double m_link_length;
	Eigen::Vector4f m_bounding_box;//x_min, x_max, z_min, z_max
	Terrain* m_terrain;

	MillipedeHead* m_head;
	MillipedeRigidSection* m_tail;
	std::vector<Sphere*> m_tip_spheres;//For interacting with water;
	Drawer* m_Drawer;

	Deformable3D* m_tail_sl;
	Deformable3D* m_tail_sr;
	RigidCube* m_tail_cl;
	RigidCube* m_tail_cr;
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif