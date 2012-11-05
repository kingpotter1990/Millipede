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
#include <vector>

#define GRAVITY_CONSTANT 9.8

#ifndef M_PI
#  define M_PI  3.14159265358979323846
#endif

#ifndef DegreesToRadians
# define DegreesToRadians 0.01745329251994329576922222222222
#endif


//list of types of objects
#ifndef Object_Types_

#define NoType 0
#define TypeCube 1
#define TypeSphere 2
#define TypeCloth 3
#define TypeDeformable3D 4
#define TypeRide 5
#define TypeRigidCube 6
#define TypeCone 7
#define TypeCylinder 8
#define TypeMixed 9

#endif

typedef int ObjectType;

class Node{
    //the Node class contains the mass, position, velocity and neighbor of the node
    //currently one Dimension
public:
    float m_Mass;
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

class Mesh3D{
//store pure gemoetry
public:
    Mesh3D(Eigen::Vector3i a_num, Eigen::Vector3f a_size, Eigen::Vector3f a_position);
    virtual ~Mesh3D(){}
    void Meshialize(Eigen::Vector3i a_num, Eigen::Vector3f a_size, Eigen::Vector3f a_position);
    
	std::vector<Node*> GetTopNodes();//return the vector nodes on the +y face of the 3D cubic mesh
	std::vector<Node*> GetBottomNodes();//-y
    std::vector<Node*> GetFrontNodes();//+z
    std::vector<Node*> GetBackNodes();//-z
    std::vector<Node*> GetLeftNodes();//-x
    std::vector<Node*> GetRightNodes();//+x
    
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
protected:
    std::string m_name;
    ObjectType m_type;
};

#endif
