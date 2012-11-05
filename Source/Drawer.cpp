
#include <math.h>
#include "Eigen/Dense"
#include "Drawer.h"
#include "Cube.h"
#include "Sphere.h"
#include "Cone.h"
#include "Cylinder.h"

MatrixStack::MatrixStack(){

	m_size = 100;
	m_stack = new Eigen::Affine3f[m_size];
	m_top = 0;
}

MatrixStack::~MatrixStack(){

}

bool MatrixStack::Empty(){
	return (m_top == 0)? true: false;
}

void MatrixStack::Push(const Eigen::Affine3f& a_transformation){
	
	assert(m_top < 100);

	m_stack[m_top] = a_transformation;
	m_top++;
	
}

void MatrixStack::Pop(){

	m_top--;

}

Eigen::Affine3f MatrixStack::Top(){
	
	return m_stack[m_top-1];

}

unsigned int MatrixStack::Size(){
	return m_top+1;
}

void MatrixStack::Clear(){
	m_top = 0;
}

Drawer::Drawer(){

	SetIdentity();
	m_cube = new Cube;
	m_cone = new Cone;
	m_sphere = new Sphere;
	m_cylinder = new Cylinder;
	m_color = Eigen::Vector3f(1.0,0,0);//red
}

Drawer::~Drawer(){
	//do nothing
}

void Drawer::RotateX(double a_angle){
	m_transformation.rotate(Eigen::AngleAxisf(a_angle*DegreesToRadians, Eigen::Vector3f::UnitX()));
}

void Drawer::RotateY(double a_angle){
    m_transformation.rotate(Eigen::AngleAxisf(a_angle*DegreesToRadians, Eigen::Vector3f::UnitY()));

}

void Drawer::RotateZ(double a_angle){
	m_transformation.rotate(Eigen::AngleAxisf(a_angle*DegreesToRadians, Eigen::Vector3f::UnitZ()));

}

void Drawer::Rotate(const Eigen::Matrix3f& a_rotation){
	m_transformation.rotate(a_rotation);
}

void Drawer::Translate(const Eigen::Vector3f& a_translation){
	m_transformation.translate(a_translation);
}

void Drawer::Scale(const Eigen::Vector3f& a_scale){
	m_transformation.scale(a_scale);
}

void Drawer::Scale(double a_scale){
	m_transformation.scale(a_scale);
}
	
void Drawer::SetIdentity(){
	m_transformation.setIdentity();
}

void Drawer::PushMatrix(){
	m_transformation_stack.Push(m_transformation);
}

void Drawer::PopMatrix(){
	if(!m_transformation_stack.Empty()){
		m_transformation = m_transformation_stack.Top();
		m_transformation_stack.Pop();
	}

}

Eigen::Vector3f Drawer::CurrentOrigin(void){

	return Eigen::Vector3f(m_transformation(0,3),m_transformation(1,3),m_transformation(2,3));

}

void Drawer::DrawCube(int type, const Camera& camera,const Light& light){
	m_cube->m_Color = m_color;
	m_cube->m_Trans = m_transformation;
	m_cube->Draw(type, camera, light);
}

void Drawer::DrawSphere(int type, const Camera& camera,const Light& light){
	m_sphere->m_Color = m_color;
	m_sphere->m_Trans = m_transformation;
	m_sphere->Draw(type, camera, light);
}

void Drawer::DrawCone(int type, const Camera& camera,const Light& light){
	m_cone->m_Color = m_color;
	m_cone->m_Trans = m_transformation;
	m_cone->Draw(type, camera, light);
}

void Drawer::DrawCylinder(int type, const Camera& camera,const Light& light){
	m_cylinder->m_Color = m_color;
	m_cylinder->m_Trans = m_transformation;
	m_cylinder->Draw(type, camera, light);
}