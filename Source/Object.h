//
//  Object.h
//  FEMCloth2D-FEM
//
//  Created by 静一 方 on 2/15/12.
//  Copyright 2012 UCLA. All rights reserved.
//

#ifndef FEMCloth2D_FEM_Object_h
#define FEMCloth2D_FEM_Object_h

#include"Eigen/Dense"
#include "ObjLoader/objLoader.h"
#include "MeshQuery/mesh_query.h"
#include "Boost/boost/unordered_map.hpp"
#include "Boost/numeric/ublas/vector.hpp"

#include <vector>
#include <windows.h>


#define GRAVITY_CONSTANT 9.8

#ifndef M_PI
#  define M_PI  3.14159265358979323846
#endif

#ifndef DegreesToRadians
# define DegreesToRadians 0.01745329251994329576922222222222
#endif

enum ObjectType{NoType, TypeCube, TypeSphere, TypeCone, TypeCylinder, TypeCloth, TypeDeformable3D,
				TypeRide, TypeRigidCube, TypeMixed, TypeTerrain };

class Node{
    //the Node class contains the mass, position, velocity and neighbor of the node
    //currently one Dimension
public:
    double m_Mass;
    bool m_Fixed;
    int m_Updated; //-1 or 1; alternate
    Eigen::Vector3f m_Velocity;
    Eigen::Vector3f m_Position;
    Eigen::Vector3f m_Force;
    Eigen::Vector3f m_Normal;//the normal is averaged over the neighboring face

public:
    Node()
    {m_Mass = 0.0; m_Fixed = false; m_Updated  = -1; m_Velocity = Eigen::Vector3f(0.0,0.0,0.0); 
        m_Position = Eigen::Vector3f(0.0,0.0,0.0); m_Force = Eigen::Vector3f(0.0,-m_Mass*GRAVITY_CONSTANT,0.0);
    }//inline Default Constructor
    ~Node(){};

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class Triangle{
    
public:
    Triangle(){}
    ~Triangle(){}
    
    Node* m_node_1;
    Node* m_node_2;
    Node* m_node_3;
    
    Eigen::Vector3f m_face_normal;
    
    double m_area;
    double m_density;
    Eigen::Matrix2f m_D_m_Inverse;
    Eigen::Matrix2f m_D_m_Inverse_Transpose;
    
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class Tetrahedron{
    
public:
    Tetrahedron(){m_node_1 = 0; m_node_2 = 0; m_node_3 = 0; m_node_4 = 0 ;}
	~Tetrahedron(){}
    
    Node* m_node_1;
    Node* m_node_2;
    Node* m_node_3;
	Node* m_node_4;
    
    double m_volume;
    double m_density;
    
	Eigen::Vector3f m_face_normal_123;
	Eigen::Vector3f m_face_normal_124;
	Eigen::Vector3f m_face_normal_134;
	Eigen::Vector3f m_face_normal_234;
    
    
    Eigen::Matrix3f m_D_m_Inverse;
    Eigen::Matrix3f m_D_m_Inverse_Transpose;
    
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

class CubicTetraMesh{
//store pure gemoetry
public:
    CubicTetraMesh(Eigen::Vector3i a_num, Eigen::Vector3f a_size, Eigen::Vector3f a_position);
    virtual ~CubicTetraMesh(){}
    void Meshialize(Eigen::Vector3i a_num, Eigen::Vector3f a_size, Eigen::Vector3f a_position);
    
	std::vector<Node*> GetTopNodes();//return the vector nodes on the +y face of the 3D cubic mesh
	std::vector<Node*> GetBottomNodes();//-y
    std::vector<Node*> GetFrontNodes();//+z
    std::vector<Node*> GetBackNodes();//-z
    std::vector<Node*> GetLeftNodes();//-x
    std::vector<Node*> GetRightNodes();//+x
    
	void GetTopSurface(std::vector<Node*>& nodes, std::vector<int>& faces);
	void GetBottomSurface(std::vector<Node*>& nodes, std::vector<int>& faces);
	void GetBackSurface(std::vector<Node*>& nodes, std::vector<int>& faces);
	void GetFrontSurface(std::vector<Node*>& nodes, std::vector<int>& faces);
	void GetLeftSurface(std::vector<Node*>& nodes, std::vector<int>& faces);
	void GetRightSurface(std::vector<Node*>& nodes, std::vector<int>& faces);
	void GetSurface(std::vector<Node*>& nodes, std::vector<int>& faces);

    Eigen::Vector3f m_Size;
    Eigen::Vector3i m_Num;
    
    Node* m_Nodes;
    Triangle* m_Surface;
    Tetrahedron* m_Tetras;
    
    int m_Num_Node;
    int m_Num_Trian;
    int m_Num_Tetra;
    
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


class SpaceGrid{
public:

	int m_res_x;
	int m_res_y;
	int m_res_z;

	double m_dx;
	double m_dy;
	double m_dz;

	double m_start_x;
	double m_start_y;
	double m_start_z;

	double m_end_x;
	double m_end_y;
	double m_end_z;

	int QueryGridIDX(double ps_x){
		if(ps_x < m_start_x || ps_x > m_end_x)
			return -1;
		return int((ps_x - m_start_x)/m_dx);//from 0 to m_res_x - 1;
	};

	int QueryGridIDY(double ps_y){
		if(ps_y < m_start_y || ps_y > m_end_y)
			return -1;
		return int((ps_y - m_start_y)/m_dy);//from 0 to m_res_x - 1
	};

	int QueryGridIDZ(double ps_z){
		if(ps_z < m_start_z || ps_z > m_end_z)
			return -1;
		return int((ps_z - m_start_z)/m_dz);//from 0 to m_res_x - 1
	};

};

class SurfaceMesh{
//store pure gemoetry
public:
	SurfaceMesh(){m_Num_v = 0; m_Num_f = 0;};
	virtual ~SurfaceMesh(){};
    int m_Num_v;
	int m_Num_f;
	bool LoadObjFile(char * filename);
	bool PointInsideMesh(Eigen::Vector3f point);
	bool ClosestTriangle(const Eigen::Vector3f& xyz, Triangle& triangle);

    std::vector<Node*> m_Nodes;
    std::vector<Triangle*> m_Trias;
    
	boost::unordered_map<int,std::vector<int> > m_spatial_hash;
	SpaceGrid m_space_grid;

	objLoader* m_objdata;
	MeshObject* m_mesh_obj;

    EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};


class World;
class Camera;
class Light;
class Object{
public:
    Object();
    virtual ~Object(){};
    virtual void SetName(std::string name){m_name = name;}
    virtual void SetType(ObjectType type){m_type = type;}
    std::string GetName(){return m_name;}
    virtual ObjectType GetType(){return m_type;}
	virtual void SetWorld(World *a_world);
	virtual void Draw(int type, const Camera& camera, const Light& light) = 0;
	virtual void UpdateAll(double dt) = 0;
    World* m_world;

public:
    std::string m_name;
    ObjectType m_type;
};

#endif
