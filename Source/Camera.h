//
//  Camera.h

//  Created by 静一 方 on 11/22/11.
//  Copyright 2011 UCLA. All rights reserved.
//

#ifndef FEMCloth2D_Camera_h
#define FEMCloth2D_Camera_h
#include "Ball.h"
#include "Eigen/Dense"

//this class defines the camera's position, lookat point, rotation, etc
//it also defines the kind of projection the camera is using: orthographics or perspective
//currently, only perspective 

class Camera{
public:
    //define camera's aligment
    Eigen::Vector4f m_position;
    Eigen::Vector4f m_lookat;
    Eigen::Vector4f m_up;
    //define the perspective devision
    double m_fovy;
    double m_aspect;
    double m_znear;
    double m_zfar;

	double m_zoom;

	BallData *m_arcball;
	Eigen::Affine3f m_cMw;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
    
	Camera();//default constructor

	~Camera(){}
	void Init(const Eigen::Vector4f& a_position,const Eigen::Vector4f& a_lookat, 
		const Eigen::Vector4f& a_up, double a_fovy, double a_aspect, double a_near, double a_far);
	void MouseLeftDown(double a_x, double a_y);
	void MouseLeftUp(double a_x, double a_y);
	void MouseDrag(double a_x, double a_y);
	void Update();//update the m_cMw;
};


#endif
