//
//  Cone.h
//  FEMCloth2D
//
//  Created by Jingyi Fang on 3/6/2012.
//  Copyright 2012 UCLA. All rights reserved.
//

#ifndef FEMCloth2D_Cone_h
#define FEMCloth2D_Cone_h

#include "Utility.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"

class Cone:public Object{
    
protected:
   
    GLuint m_vertexArrayObject;                      
    GLuint m_vertexBufferObject;  
    GLuint m_shader;
    Eigen::Vector4f *m_Vertices;
    Eigen::Vector3f *m_Normals;
	Eigen::Vector2f *m_CirclePoints;

	int m_Index;

	void makeCircle();
	void makeConeWall(float z1, float z2, int dir);
    void GenerateCone();
   
public:
    Eigen::Vector3f m_Center;//For generating translation Angel::matrix
    Eigen::Vector3f m_Size;
	Eigen::Vector3f m_Color;

	Eigen::Affine3f m_Trans;//the transformation from object to world
    Eigen::Affine3f m_TransBack;//the tranformation from world to object

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW

public:
    Cone();//Default constructor create a unit cube in center of screen
	void Init(Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color);
	void InitDraw();
    void Draw(int type, const Camera& camera, const Light& light);//Update data on GPU's buffer and draw the vertexs, rotate clockwise around z with speed
	void UpdateAll(double dt){}
};



#endif
