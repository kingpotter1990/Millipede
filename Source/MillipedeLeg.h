
#ifndef MILLIPEDE_LEG_H_
#define MILLIPEDE_LEG_H_

#include <string>

#include "Utility.h" 
#include "Object.h"

class Terrain;
class MillipedeRigidSection;

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
	double GetBalanceHeight();
	void Output2File(std::ofstream *filestream);

protected:
	
	void UpdateSwitchNet(double dt);
	void UpdateSwingNet(double dt);
	void UpdateStanceNet(double dt);
	void UpdateSpeedNet();

	void EnterStance();
	void EnterSwayForward1();
	void EnterSwayBackward1();
	void EnterSwayForward2();
	void EnterSwayBackward2();

	bool InverseKinematics(Eigen::Vector3f a_tip, Eigen::Vector3f a_root, Eigen::Vector3f& a_result);//for leg


private:
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

	Eigen::Vector3f m_tip_position;//
	Eigen::Vector3f m_root_position;//root position of the leg, in the rigid cube's cos(after translation and rotation)


	double m_extreme_alpha;
	double m_extreme_beta;
	double m_extreme_phi;
	double m_dif_phi_tolerance;
	double m_alpha_velocity;
	double m_phi_velocity;
	double m_beta_velocity;

	double m_leg_rotation_velocity;

	int m_l_r;//left or right

	MillipedeLeg* m_prev;
	MillipedeLeg* m_next;
	MillipedeLeg* m_neig;

public:
	MillipedeRigidSection* m_root;//where the leg is attached to
	LEG_STATUS m_leg_state;

public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif