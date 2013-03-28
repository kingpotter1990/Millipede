#ifndef MILLIPEDE_H_
#define MILLIPEDE_H_

#include <string>
#include "Utility.h" 
#include "Object.h"

class MillipedeRigidSection;
class MillipedeSoftSection;
class MillipedeHead;
class MillipedeTail;
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
	void Stop(){m_stopped = true;};
	void Start(){m_stopped = false;};
protected:
	void InitPhysics(Eigen::Vector3f a_position, int a_num_section, Eigen::Vector3f a_rigid_size, double a_soft_length);
	void InitPhysicsFromModel(Eigen::Vector3f a_position);
	void InitNeuroNet(Terrain* a_terrain);
	void UpdateBoundingBox();
	void UpdateTipSphere();
public:
	int m_num_section;
	Eigen::Vector3f m_size;
	Eigen::Vector4f m_bounding_box;//x_min, x_max, z_min, z_max
	Terrain* m_terrain;

	MillipedeHead* m_head;
	MillipedeRigidSection* m_last;
	MillipedeTail* m_tail_left;
	MillipedeTail* m_tail_right;
	
	std::vector<Sphere*> m_tip_spheres;//For interacting with water;
	Drawer* m_Drawer;

	//tail section padding
	Deformable3D* m_tail_soft;
	RigidCube* m_tail_rigid;
	bool m_stopped;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif