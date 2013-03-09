//
//  HeightFieldWater.h.
//
//  Created by Jingyi Fang on 3/1/13.
//  Copyright 2013 UCLA. All rights reserved.
//

#ifndef Millipede_Water_h
#define Millipede_Water_h

#include "Utility.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"

class Sphere;
class HFWater:public Object{
    
protected:
   
    GLuint m_vertexArrayObject;                      
    GLuint m_vertexBufferObject;  
    GLuint m_shader;

    Eigen::Vector4f* m_TrianglePoints;
	Eigen::Vector4f* m_TriangleColors;
    Eigen::Vector3f* m_TPointNormals;
    int m_NTrianglePoints;   
public:
	Eigen::Vector3f m_Color;//6 Face Colors
private:
	int m_res_x;
	int m_res_z;
	double m_size_x;
	double m_size_z;
	double m_dx;
	double m_depth;
	double* m_height_data;
	double* m_velocity_data;
    Eigen::Vector3f* m_normal_data;
	std::vector<Sphere*> m_spheres;
public:
    HFWater(Eigen::Vector2i res, double dx, double depth);//Default constructor create a unit cube in center of screen
	virtual ~HFWater(){delete[] m_height_data; delete[] m_velocity_data;
	delete[] m_normal_data;}
	virtual void InitWave();
    virtual void InitDraw();//Init the vertexs and color data on GPU, Init the shader program, link the shader program with the buffer data
    virtual void Draw(int type, const Camera& camera, const Light& light);//Update data on GPU's buffer and draw the vertexs, rotate clockwise around z with speed
	virtual void UpdateDraw();
	virtual void UpdateAll(double dt);
	virtual void UpdateNormal();
	virtual void UpdateSpheres(std::vector<Sphere*>& a_spheres){m_spheres = a_spheres;};
	virtual void Output2File(std::ofstream* filestream);
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};



#endif