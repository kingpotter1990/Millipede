//
//  Sphere.h
//
//  Created by 静一 方 on 11/22/11.
//  Copyright 2011 UCLA. All rights reserved.
//

#ifndef FEMCloth2D_Sphere_h
#define FEMCloth2D_Sphere_h

#include "Camera.h"
#include "Light.h"
#include "Object.h"

class Sphere:public Object{
    
protected:
   
    GLuint m_vertexArrayObject;                      
    GLuint m_vertexBufferObject;  
    GLuint m_shader;

	
    Eigen::Vector4f* m_Vertices;
    Eigen::Vector3f *m_Normals;
    int m_Index;
	int m_count;
    
public:    
    Eigen::Vector3f m_Center;//For generating translation Angel::matrix
    Eigen::Vector3f m_Size;//For generating scaling Angel::matrix
	Eigen::Vector3f m_Rotation;
    Eigen::Affine3f m_Trans;
    Eigen::Affine3f m_TransBack;
    Eigen::Vector3f m_Color;

public:
    Sphere();//Default constructor create a unit cube in center of screen
    Sphere(Eigen::Vector3f, Eigen::Vector3f,Eigen::Vector3f );//constructor creating a cube with size and center
    void GenerateSphere();
	void UpdateMatrix();
	void MoveUp(double amount);
	void MoveDown(double amount);
	Eigen::Vector4f Sphere::Unit(const Eigen::Vector4f &p);
	void DividTriangle(const Eigen::Vector4f& a,const Eigen::Vector4f& b, const Eigen::Vector4f& c, int n);//for iterative generation of sphere
    void Init(Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color);//Init the data
    void InitDraw();//Init the vertexs and color data on GPU, Init the shader program, link the shader program with the buffer data
    void Draw(int type, const Camera& camera,const Light& light);//Update data on GPU's buffer and draw the vertexs, rotate clockwise around z with speed
	virtual void UpdateAll(double dt){};
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};



#endif
