#ifndef MILLI_TERRAIN_H_
#define MILLI_TERRAIN_H_

#include "Utility.h"
#include "Camera.h"
#include "Light.h"
#include "Object.h"
#include "World.h"

class Sphere;
class Millipede;
class objLoader;
class MeshObject;
class HFWater;
enum TerrainType{TERRAIN_RANDOM, TERRAIN_FLAT, TERRAIN_TEST, TERRAIN_SPHERICAL, TERRAIN_WATER};
class Terrain:public Object{

public:
	Terrain(Eigen::Vector2f a_size, Eigen::Vector2i a_res, int n_hill, TerrainType a_type, bool a_bostacle_on_off, bool a_food_on_off);
	double GetSizeX() const{return m_size_x;};
	double GetSizeY() const{return m_size_z;};
	int GetResX() const{return m_res_x;};
	int GetResY() const{return m_res_z;};
	Eigen::Vector2f GetSize() const{return Eigen::Vector2f(m_size_x,m_size_z);};
	Eigen::Vector2i GetRes() const{return Eigen::Vector2i(m_res_x, m_res_z);};
	double GetHeight(const Eigen::Vector3f& xyz) const;
	bool TestInside(const Eigen::Vector3f& point);
	bool TestInsideObstacle(const Eigen::Vector3f& point);
	Eigen::Vector3f GetNormal(const Eigen::Vector3f& xyz);
	~Terrain(){delete[] m_height_data; delete[] m_normal_data;};
	void Draw(int type, const Camera& camera, const Light& light);
	void UpdateAll(double dt);
	double GetFoodIntensity(Eigen::Vector3f a_pos);
	bool ReachFood(Eigen::Vector3f pos, double tol);
	void RegisterMillipede(Millipede* a_millipede);
	void ClearMillipedes();
	void InitSurfaceObjects();
	void Output2File(std::ofstream* file);
public:
	double m_frictness;
	TerrainType m_terrain_type;
	HFWater* m_water;
protected:
	void InitBase(double a_size_x, double a_size_z, int a_res_x, int a_res_z, int n_hill);
	void InitObstacles(TerrainType a_type);
	void InitFood(TerrainType a_type);
	void InitDraw();

private:
	void GenerateNormals();
	
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

	SurfaceMesh* m_surface_mesh;//for spherical terrain 
protected:	

	std::vector<Object*> m_obstacles;
	std::vector<Object*> m_surface_objects;
	std::vector<Sphere*> m_foods;
	std::vector<Millipede*> m_millipedes;//registered millipedes on the terrain
public:
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};
	

#endif
