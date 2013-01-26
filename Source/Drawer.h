#ifndef Drawer_H_
#define Drawer_H_

#include <stack>

#include "Utility.h"

class Camera;
class Light;
class Cube;
class Sphere;
class Cylinder;
class Cone;

class MatrixStack{
public:
	MatrixStack();
	~MatrixStack();
	bool Empty();
	void Push(const Eigen::Affine3f& a_transformation);
	void Pop();
	Eigen::Affine3f Top();
	unsigned int Size();
	void Clear();
private:
	int m_size;
	int m_top;
	Eigen::Affine3f* m_stack;
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class Drawer{

public:
	Drawer();
	~Drawer();

	Eigen::Vector3f CurrentOrigin(void);//return the origin position of current object coordinate system

	void SetIdentity();
	void PushMatrix();
	void PopMatrix();
	void RotateX(double a_angle);
	void RotateY(double a_angle);
	void RotateZ(double a_angle);
	void Rotate(const Eigen::Matrix3f& a_rotation);
	void Translate(const Eigen::Vector3f& a_translation);
	void Scale(const Eigen::Vector3f& a_scale);
	void Scale(double a_scale);
	void SetColor(const Eigen::Vector3f a_color){m_color = a_color;}

	void DrawCube(int type, const Camera& camera,const Light& light);
	void DrawSphere(int type, const Camera& camera,const Light& light);
	void DrawCone(int type, const Camera& camera,const Light& light);
	void DrawCylinder(int type, const Camera& camera,const Light& light);
public:
	MatrixStack m_transformation_stack;
	Eigen::Affine3f m_transformation;
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
private:
	Cube* m_cube;
	Cone* m_cone;
	Sphere* m_sphere;
	Cylinder* m_cylinder;
	Eigen::Vector3f m_color;
};


#endif