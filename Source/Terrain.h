#ifndef MILLI_TERRAIN_H_
#define MILLI_TERRAIN_H_

#include "Utility.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "World.h"

class Sphere;
class Millipede;

enum TerrainType{TERRAIN_RANDOM, TERRAIN_FLAT, TERRAIN_TEST};

class Terrain:public Object{

public:
	Terrain(Eigen::Vector2f a_size, Eigen::Vector2i a_res, int n_hill, TerrainType a_type, bool a_bostacle_on_off, bool a_food_on_off);
	double GetSizeX() const{return m_size_x;};
	double GetSizeY() const{return m_size_z;};
	int GetResX() const{return m_res_x;};
	int GetResY() const{return m_res_z;};
	Eigen::Vector2f GetSize() const{return Eigen::Vector2f(m_size_x,m_size_z);};
	Eigen::Vector2i GetRes() const{return Eigen::Vector2i(m_res_x, m_res_z);};
	double GetHeight(Eigen::Vector2f xz) const;
	double GetHeight(double x, double z) const{return GetHeight(Eigen::Vector2f(x,z));};
	Eigen::Vector3f GetNormal(Eigen::Vector2f xz) const;
	Eigen::Vector3f GetNormal(double x, double z) const{return GetNormal(Eigen::Vector2f(x,z));};
	~Terrain(){delete[] m_height_data; delete[] m_normal_data;};
	void Draw(int type, const Camera& camera, const Light& light);
	void UpdateAll(double dt){};
	bool TestIntersection(Millipede* a_bug, Eigen::Vector3f a_o, Eigen::Vector3f a_p);
	double GetFoodIntensity(Eigen::Vector3f a_pos);
	bool ReachFood(Eigen::Vector3f pos, double tol);
	void RegisterMillipede(Millipede* a_millipede);
public:
	double m_frictness;

protected:
	void InitBase(double a_size_x, double a_size_z, int a_res_x, int a_res_z, int n_hill, TerrainType a_type);
	void InitObstacles(TerrainType a_type);
	void InitFood(TerrainType a_type);
	void InitDraw();

protected:
	//drawing data
	int m_NTrianglePoints;
    GLuint m_vertexArrayObject;                      
    GLuint m_vertexBufferObject;  
    GLuint m_shader;
	Eigen::Vector4f *m_TrianglePoints;
    Eigen::Vector4f *m_TriangleColors;
    Eigen::Vector3f *m_TPointNormals;

	//simulation data
	double m_size_x;
	double m_size_z;
	int m_res_x;
	int m_res_z;
	double m_dx;
	double m_dz;
	double* m_height_data;//per node
	Eigen::Vector3f* m_normal_data;//per node
	std::vector<Object*> m_obstacles;
	std::vector<Sphere*> m_foods;
	std::vector<Millipede*> m_millipedes;//registered millipedes on the terrain
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
	

#endif
