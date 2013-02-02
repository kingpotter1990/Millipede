//
//  Cylinder.h
//  FEMCloth2D
//
//  Created by Jingyi Fang on 3/6/2012.
//  Copyright 2012 UCLA. All rights reserved.
//

#ifndef FEMCloth2D_Cylinder_h
#define FEMCloth2D_Cylinder_h

#include "Utility.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"

class Cylinder:public Object{

public:
    Cylinder();//Default constructor create a unit cube in center of screen
	Cylinder(Eigen::Vector3f, Eigen::Vector3f,Eigen::Vector3f );
	virtual ~Cylinder(){}
	void Init(Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color);
	void InitDraw();
    void Draw(int type, const Camera& camera, const Light& light);
	void UpdateAll(double dt){};
    
protected:
   
    GLuint m_vertexArrayObject;                      
    GLuint m_vertexBufferObject;  
    GLuint m_shader;
    Eigen::Vector4f *m_Vertices;
    Eigen::Vector3f *m_Normals;
	Eigen::Vector2f *m_CirclePoints;

	int m_Index;

	void makeCircle();
	void makeConeWall(double z1, double z2, int dir);
    void GenerateCylinder();
    
public:
	Eigen::Vector3f m_Center;//For generating translation Angel::matrix
    Eigen::Vector3f m_Size;
	Eigen::Vector3f m_Color;

	Eigen::Affine3f m_Trans;//the transformation from object to world
    Eigen::Affine3f m_TransBack;//the tranformation from world to object

   EIGEN_MAKE_ALIGNED_OPERATOR_NEW

};

#endif