#ifndef MILLIPEDE_HEAD_H_
#define MILLIPEDE_HEAD_H_

#include "Utility.h" 
#include "Object.h"
#include "MillipedeSection.h"

class MillipedeHead;
class MillipedeAntenna;
class Millipede;
class Drawer;

enum MILLIPEDE_STATUS {CONTROLLED, ADJUSTING, RANDOM_WALK, AVOID_OBSTACLE_LEFT,
						AVOID_OBSTACLE_RIGHT, PREDATING_FOOD_LEFT, PREDATING_FOOD_RIGHT,ESCAPING_PREDATOR};
enum MILLIPEDE_TURNING {TURN_LEFT, TURN_RIGHT, GO_STRAIGHT};

class MillipedeAntenna:public Object{
public:
	MillipedeAntenna(){m_type = NoType;};
	~MillipedeAntenna(){};
	void InitNeuroNet(MillipedeHead* a_head, double a_length, int a_l_r);
	void UpdateAll(double dt);
	bool SenseObstacle();
	bool SenseFood(double& intensity);
	void Draw(int type, const Camera& camera, const Light& light);
	void Output2File(std::ofstream* filestream, int type);
private:
	MillipedeHead* m_head;
	int m_n_segment;
	double* m_segment_length;//length of a segment
	double* m_theta1;
	double* m_theta2;
	Eigen::Vector3f* m_sensor_pos;
	double m_phi;
	double m_alpha;
	double m_target_phi;
	double m_target_alpha;
	double m_rot_v_phi;
	double m_rot_v_alpha;
	int m_l_r;//1 left, -1 right
	double m_timer;
	double m_omega1;
	double m_omega2;
public:
	Eigen::Vector3f m_root_position;
	Eigen::Vector3f m_tip_position;
	double m_food_sense_threshold;
	Drawer* m_Drawer;
protected:
	void UpdateTipRootPosition();
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class MillipedeHead:public RigidCube{
public:
	MillipedeHead(){};
	~MillipedeHead(){};
	void InitPhysics(double density, Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color);
	void InitNeuroNet(Millipede* a_root);
	void Draw(int type, const Camera& camera, const Light& light);
	void UpdateAll(double dt);
	void EnterMode(MILLIPEDE_STATUS a_mode);
	void ExitControl(){EnterMode(m_prev_mode);};//exit current mode
	double GetLinearSpeed(){return m_linear_speed;};
	void Output2File(std::ofstream* filestream, int type);

protected:
	void KeepHeadBalance(double a_dt);
	void UpdateNeuroNet(double a_dt);
	void UpdatePhysics(double a_dt);
protected:
    MILLIPEDE_STATUS m_mode;
    MILLIPEDE_STATUS m_prev_mode;
	MILLIPEDE_TURNING m_turning_direction;
	double m_linear_speed;
	double m_turning_speed;//degree per seconds
	double m_turning_obj;
	double m_current_turning_accum;
	double m_fake_leg_angle;
	double m_mouth_angle;
public:

	MillipedeAntenna* m_left_antenna;
	MillipedeAntenna* m_right_antenna;

	MillipedeSoftSection* m_next;

	Millipede* m_master;
	Terrain* m_terrain;
	int m_section_id;
	double m_height_obj;
	Eigen::Vector3f m_orient_obj;
	double m_prev_dis_obj;
	double m_current_height;
	Drawer* m_Drawer;
	double m_time;
};

#endif