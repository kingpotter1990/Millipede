#include "Camera.h"

Camera::Camera()
{
	m_arcball = NULL;

}

void Camera::Init(const Eigen::Vector4f& a_position,const Eigen::Vector4f& a_lookat, 
		const Eigen::Vector4f& a_up, double a_fovy, double a_aspect, double a_near, double a_far)
{
	m_position = a_position;
    m_lookat = a_lookat;
    m_up = a_up;
    m_fovy = a_fovy;
    m_aspect = a_aspect;
    m_znear = a_near;
    m_zfar = a_far;
	m_zoom = 1.00;
	m_arcball = new BallData;
	Ball_Init(m_arcball);
	Ball_Place(m_arcball,qOne,0.75);

}

void Camera::MouseLeftDown(double a_x, double a_y){

	HVect arcball_coords;
	arcball_coords.x = a_x;
	arcball_coords.y = a_y;
	Ball_Mouse(m_arcball, arcball_coords) ;
	Ball_Update(m_arcball);
	Ball_BeginDrag(m_arcball);

}

void Camera::MouseLeftUp(double a_x, double a_y){

	Ball_EndDrag(m_arcball);

}

void Camera::MouseDrag(double a_x, double a_y){

	HVect arcball_coords;
    arcball_coords.x =  a_x;
    arcball_coords.y =  a_y;
    Ball_Mouse(m_arcball,arcball_coords);
    Ball_Update(m_arcball);
}

void Camera::Update(){
	//recompute the world to camera matrix in the camera
	m_cMw.setIdentity();
	m_cMw.translate(Eigen::Vector3f(0.0f, 0.0f, -25.0f));
	HMatrix r;
    Ball_Value(m_arcball,r);

    Eigen::Matrix4f arcball_rot;
	arcball_rot<<
        r[0][0], r[0][1], r[0][2], r[0][3],
        r[1][0], r[1][1], r[1][2], r[1][3],
        r[2][0], r[2][1], r[2][2], r[2][3],
        r[3][0], r[3][1], r[3][2], r[3][3];

    m_cMw *= arcball_rot.transpose();
	m_cMw.scale(m_zoom);
	//get the camera position in the world
	m_position = m_cMw.inverse()*Eigen::Vector4f(0.0,0.0,0.0,1.0);

}