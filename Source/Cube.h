//
//  Cube.h
//  FEMCloth2D
//
//  Created by 静一 方 on 11/22/11.
//  Copyright 2011 UCLA. All rights reserved.
//

#ifndef FEMCloth2D_Cube_h
#define FEMCloth2D_Cube_h

#include "Utility.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"

class Cube:public Object{
    
protected:
   
    GLuint m_vertexArrayObject;                      
    GLuint m_vertexBufferObject;  
    GLuint m_shader;
    Eigen::Vector4f m_Points[8];//8 Vertices

    Eigen::Vector4f* m_Vertices;
    Eigen::Vector3f *m_Normals;
    int m_Index;
    
public:
    Eigen::Vector3f m_Center;//For generating translation Angel::matrix
    Eigen::Vector3f m_Size;//For generating scaling Angel::matrix
	Eigen::Vector3f m_Color;//6 Face Colors

    Eigen::Affine3f m_Trans;//the transformation from object to world
    Eigen::Affine3f m_TransBack;//the tranformation from world to object
    
public:
    Cube();//Default constructor create a unit cube in center of screen
	virtual ~Cube(){}
    Cube(Eigen::Vector3f, Eigen::Vector3f,Eigen::Vector3f );//constructor creating a cube with size and center
    void GenFace(int colorid, int a, int b, int c, int d);
    virtual void Init(Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color);//Init the data
    virtual void InitDraw();//Init the vertexs and color data on GPU, Init the shader program, link the shader program with the buffer data
    virtual void Draw(int type, const Camera& camera, const Light& light);//Update data on GPU's buffer and draw the vertexs, rotate clockwise around z with speed
	virtual void UpdateAll(double dt){};

	void SetColor(const Eigen::Vector3f& a_color){m_Color = a_color;}
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};



#endif
