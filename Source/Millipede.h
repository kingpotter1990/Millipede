#ifndef MILLIPEDE_H_
#define MILLIPEDE_H_

#include <string>

#include "Utility.h" 
#include "Object.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Cone.h"
#include "RigidCube.h"
#include "3DDeformable.h"

class MillipedeSoftSection;
class MillipedeRigidSection;
class MillipedeLeg;
class Millipede;

enum LEG_STATUS {LEG_STANCE, LEG_SWAY_FORWARD_1, LEG_SWAY_FORWARD_2, LEG_SWAY_BACKWARD_1,LEG_SWAY_BACKWARD_2};

class MillipedeLeg:public Object{
    
public:
	MillipedeLeg(int a_l_r);//1 left, -1 right
	~MillipedeLeg(){}
	void InitPhysics(Eigen::Vector3f a_body_size);
 	void InitNeuroNet(MillipedeRigidSection* a_root);
	void UpdateNeuronNet(double dt);
	void UpdateAll(double dt);
	void Draw(int type, const Camera& camera, const Light& light);
	void UpdateTipPosition();//the root is fixed
	void UpdateRootPosition();//the tip is fixed


	void EnterStance();
	void EnterSwayForward();
	void EnterSwayBackward();
protected:
	void UpdateSwitchNet(double dt);
	void UpdateSFowardNet(double dt);
	void UpdateSBackwardNet(double dt);
	void UpdateStanceNet(double dt);
	void UpdateTargetNet(int a_code);
	void UpdateSpeedNet();

	bool InverseKinematics(Eigen::Vector3f a_tip, Eigen::Vector3f a_root, Eigen::Vector3f& a_result);//for leg


public:
	double m_alpha;
	double m_beta;
	double m_gamma;
	double m_phi;

	double m_target_alpha;
	double m_target_beta;
	double m_target_phi;
    
	Eigen::Vector3f m_segment_0_size;
	Eigen::Vector3f m_segment_1_size;
	Eigen::Vector3f m_segment_2_size;
	Eigen::Vector3f m_segment_3_size;

	double m_extreme;
	double m_dif_phi_tolerance;
	double m_alpha_velocity;
	double m_phi_velocity;
	double m_beta_velocity;

	double m_leg_rotation_velocity;

	int m_l_r;//left or right

	MillipedeRigidSection* m_root;//where the leg is attached to
	Eigen::Vector3f m_tip_position;//
	Eigen::Vector3f m_root_position;//root position of the leg, in the rigid cube's cos(after translation and rotation)

	LEG_STATUS m_leg_state;

	MillipedeLeg* m_prev;
	MillipedeLeg* m_next;
	MillipedeLeg* m_neig;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class MillipedeRigidSection:public RigidCube{
	//rigid section is rigid in physics
	//has two pairs of leg for millipede

public:
	MillipedeRigidSection(){};
	~MillipedeRigidSection(){};
	void InitPhysics(double density, Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color);
	void InitNeuroNet(Millipede* a_root);
	void SetWorld(World* a_world);
	void Draw(int type, const Camera& camera, const Light& light);
	void UpdateNeuronNet(double dt);
	void UpdatePBD(double dt);
	void UpdateAll(double dt);
public:
	MillipedeLeg* m_left_leg;
	MillipedeLeg* m_right_leg;

	LEG_STATUS m_leg_state;

	MillipedeSoftSection* m_next;
	MillipedeSoftSection* m_prev;

	Millipede* m_root;

	double m_height_obj;

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

enum MILLIPEDE_STATUS {STATIC, MOVE};

class Millipede:public Object{

public:
	Millipede();
	virtual ~Millipede();
	void Init(Eigen::Vector3f a_position, int a_num_section, Eigen::Vector3f a_rigid_size, double a_soft_length);
	void Draw(int type, const Camera& camera, const Light& light);
	void Update(double a_dt);
    void SetWorld(World* a_world);
    void UpdateAll(double dt);
protected:
	void UpdateNueroNet(double a_dt);
	void InitPhysics(Eigen::Vector3f a_position, int a_num_section, Eigen::Vector3f a_rigid_size, double a_soft_length);
	void InitDraw();
	void InitNeuroNet();
	void UpdatePhysics(double a_dt);
	void UpdateDraw();
	void UpdateNeuroNet(double a_dt);
	//void UpdateLegTipPosition(MillipedeRigidSection* a_section, int l_or_r);
	//void UpdateLegRootPosition(MillipedeRigidSection* a_section, int l_or_r);

public:
    
	int m_num_section;
	Eigen::Vector3f m_size;
	double m_link_length;

	MillipedeRigidSection* m_head;
	MillipedeRigidSection* m_tail;

	MILLIPEDE_STATUS m_mode;
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif