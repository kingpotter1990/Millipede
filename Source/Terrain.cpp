#include "Terrain.h"
#include "Drawer.h"
#include "Object.h"
#include "Cube.h"
#include "Sphere.h"
#include "Cylinder.h"
#include "Millipede.h"
#include <algorithm>

extern Drawer* g_Drawer;

Terrain::Terrain(Eigen::Vector2f a_size, Eigen::Vector2i a_res, int n_hill, TerrainType a_type, bool a_obstacle_on_off, bool a_food_on_off){
	m_type = TypeTerrain;
	m_frictness = 10;
	
	m_terrain_type = a_type;

	if(m_terrain_type == TERRAIN_SPHERICAL){
		char objFile[100] = "../Model/armadillo.obj";

		//create a Mesh Object for drawing
		m_surface_mesh = new SurfaceMesh;

		m_surface_mesh->LoadObjFile(objFile);

	}else{
		InitBase(a_size.x(), a_size.y(), a_res.x(), a_res.y(), n_hill);
	
		if(a_obstacle_on_off)
			InitObstacles(a_type);
	
		if(a_food_on_off)
			InitFood(a_type);
	}

	InitDraw();
}


void Terrain::InitBase(double a_size_x, double a_size_z, int a_res_x, int a_res_z, int n_hill)
{
	
	m_size_x = a_size_x;
	m_size_z = a_size_z;
	m_res_x = a_res_x;
	m_res_z = a_res_z;
	m_dx = m_size_x/m_res_x;
	m_dz = m_size_z/m_res_z;

	m_height_data = new double[(a_res_x+1)*(a_res_z+1)];
	m_normal_data = new Eigen::Vector3f[(a_res_x+1)*(a_res_z+1)];
	for(int i = 0; i < (a_res_x+1)*(a_res_z+1); i++)
		m_height_data[i] = 0.0;//clear to 0
	
	if(m_terrain_type == TERRAIN_FLAT){
		for(int ix = 0; ix < (a_res_x+1); ix++)
			for(int iz = 0; iz < (a_res_z+1); iz++){
				m_height_data[ix*(a_res_z+1) + iz] = 0.0;
			}
	}

	else if(m_terrain_type == TERRAIN_RANDOM){
		double hill_height_max = 10;
		double hill_center_x, hill_center_z, hill_height, hill_narrowness_x, hill_narrowness_z;
		double dev_x, dev_z;

		for(int i = 0; i < n_hill; i++){	

			hill_center_x = (Util::getRand()-0.5)*a_size_x;
			hill_center_z = (Util::getRand()-0.5)*a_size_z;
			hill_height = Util::getRand()*(hill_height_max - 5) + 5;
			hill_narrowness_x = Util::getRand()*5 + 1; //20~40
			hill_narrowness_z = Util::getRand()*5 + 1; //20~40

			//add the hill to current height map
			for(int ix = 0; ix < (a_res_x+1); ix++)
				for(int iz = 0; iz < (a_res_z+1); iz++){
					dev_x = ix*m_dx - 0.5*a_size_x - hill_center_x;
					dev_z = iz*m_dz - 0.5*a_size_z - hill_center_z;
					m_height_data[ix*(a_res_z+1) + iz] += //guassian hill
						hill_height*exp(-dev_x*dev_x/(2*hill_narrowness_x*hill_narrowness_x)-dev_z*dev_z/(2*hill_narrowness_z*hill_narrowness_z));
				}
		}
	
		//normalize
		double temp_largest = m_height_data[0];
		double temp_smallest = m_height_data[0];
		for(int i = 0; i < (a_res_x+1)*(a_res_z+1); i++)
		{
			if(m_height_data[i] > temp_largest)
				temp_largest = m_height_data[i];
			if(m_height_data[i] < temp_smallest)
				temp_smallest = m_height_data[i];
		}

		for(int i = 0; i < (a_res_x+1)*(a_res_z+1); i++)
		{
			//eventually the range would be from 0~m_hill_height_max
			m_height_data[i] = hill_height_max*(m_height_data[i] - temp_smallest)/temp_largest;
		}


	}else if(m_terrain_type == TERRAIN_TEST){
		
		for(int ix = 0; ix < (a_res_x+1); ix++)
			for(int iz = 0; iz < (a_res_z+1); iz++){
				m_height_data[ix*(a_res_z+1) + iz] = 0.0;
			}

		InitSurfaceObjects();
	}

	GenerateNormals();
	
}

void Terrain::GenerateNormals(){
//generate normal

	Eigen::Vector3f v1,v2;
	Eigen::Vector3f current_normal, temp_face_normal;
	if(m_terrain_type == TERRAIN_SPHERICAL){
		for(int i = 0; i < m_surface_mesh->m_Num_f; i++)
		{
			v1 = m_surface_mesh->m_Trias[i]->m_node_2->m_Position - m_surface_mesh->m_Trias[i]->m_node_1->m_Position;
			v2 = m_surface_mesh->m_Trias[i]->m_node_3->m_Position - m_surface_mesh->m_Trias[i]->m_node_1->m_Position;
			temp_face_normal = v1.cross(v2);
			temp_face_normal.normalize();
			m_surface_mesh->m_Trias[i]->m_face_normal = temp_face_normal;
			m_surface_mesh->m_Trias[i]->m_node_1->m_Normal += temp_face_normal;
			m_surface_mesh->m_Trias[i]->m_node_2->m_Normal += temp_face_normal;
			m_surface_mesh->m_Trias[i]->m_node_3->m_Normal += temp_face_normal;
		}
		for(int i = 0; i < m_surface_mesh->m_Num_v; i++)
		{//normalize
			m_surface_mesh->m_Nodes[i]->m_Normal.normalize();
		}
	}
	else{
	for(int ix = 0; ix < m_res_x + 1; ix++)
		for(int iz = 0; iz < m_res_z + 1; iz++){
			current_normal *= 0.0;
			//upper left face normal
			if(ix > 0 && iz < m_res_z)
			{
				v1.x() = 0;
				v1.z() = m_dz;
				v1.y() = m_height_data[ix*(m_res_z+1) + iz + 1] - m_height_data[ix*(m_res_z+1) + iz];
				v1.normalize();

				v2.x() = -m_dx;
				v2.z() = 0;
				v2.y() = m_height_data[(ix-1)*(m_res_z+1) + iz] - m_height_data[ix*(m_res_z+1) + iz];
				v2.normalize();

				temp_face_normal = v1.cross(v2);
				current_normal += temp_face_normal;
			}
			//upper right face normal
			if(ix < m_res_x - 1 && iz < m_res_z)
			{
				v1.x() = 0;
				v1.z() = m_dz;
				v1.y() = m_height_data[ix*(m_res_z+1) + iz + 1] - m_height_data[ix*(m_res_z+1) + iz];
				v1.normalize();

				v2.x() = m_dx;
				v2.z() = 0;
				v2.y() = m_height_data[(ix+1)*(m_res_z+1) + iz] - m_height_data[ix*(m_res_z+1) + iz];
				v2.normalize();

				temp_face_normal = v2.cross(v1);
				current_normal += temp_face_normal;
			}
			//lower left face normal
			if(ix > 0 && iz > 0)
			{
				v1.x() = 0;
				v1.z() = -m_dz;
				v1.y() = m_height_data[ix*(m_res_z+1) + iz - 1] - m_height_data[ix*(m_res_z+1) + iz];
				v1.normalize();

				v2.x() = -m_dx;
				v2.z() = 0;
				v2.y() = m_height_data[(ix-1)*(m_res_z+1) + iz] - m_height_data[ix*(m_res_z+1) + iz];
				v2.normalize();

				temp_face_normal = v2.cross(v1);
				current_normal += temp_face_normal;
			}
			//lower right face normal
			if(ix < m_res_x  && iz > 0)
			{
				v1.x() = 0;
				v1.z() = -m_dz;
				v1.y() = m_height_data[ix*(m_res_z+1) + iz - 1] - m_height_data[ix*(m_res_z+1) + iz];
				v1.normalize();

				v2.x() = m_dx;
				v2.z() = 0;
				v2.y() = m_height_data[(ix+1)*(m_res_z+1) + iz] - m_height_data[ix*(m_res_z+1) + iz];
				v2.normalize();

				temp_face_normal = v1.cross(v2);
				current_normal += temp_face_normal;
			}

			m_normal_data[ix*(m_res_z+1) + iz] = -current_normal.normalized();//average
		}
	}

}

void Terrain::InitSurfaceObjects(){

	Cube* temp_cube;
	Cylinder* temp_cylinder;
	Sphere* temp_sphere;
	
	Eigen::Vector3f temp_center, temp_scale, temp_color;
	double dx = 2, dy, dz = 2;
	temp_color = Eigen::Vector3f(0.3,0.3,0.1);
	
	/*
	for(int i = 0; i < 20; i++)
		for(int j = 0; j < 20; j++){
						
			dy = Util::getRand()*1;
			temp_center = Eigen::Vector3f(-i*dx, dy/2, -10 + j*dz);			
			temp_scale = Eigen::Vector3f(dx,dy,dz);
			temp_cube = new Cube(temp_center, temp_scale, temp_color);

			m_surface_objects.push_back(temp_cube);

		}
	*/

	for(int i = 0; i < 30; i++){
		temp_center = Eigen::Vector3f(-i*2, 0, 0);			
		temp_scale = Eigen::Vector3f(2,2,10);
		temp_cylinder = new Cylinder(temp_center, temp_scale, temp_color);

		m_surface_objects.push_back(temp_cylinder);
	
	}
}

bool Terrain::TestInside(const Eigen::Vector3f& point){

	double* p = new double[3];
	p[0] = point[0];p[1] = point[1];p[2] = point[2];
	switch(m_terrain_type){
	case TERRAIN_SPHERICAL:
		//return point.norm() < 20; //test with sphere
        return m_surface_mesh->PointInsideMesh(point);
		break;
	case TERRAIN_TEST:
	case TERRAIN_FLAT:
	case TERRAIN_RANDOM:
	{//those terrain never reverse

		if(GetHeight(point) < 0)
			return true;
		else
			return false;
	}
	break;
	default:
	return false;
	}

}

void Terrain::InitObstacles(TerrainType a_type){

	//test cases
	//case 1: just cubes
	
	Cube* temp_cube;
	Cylinder* temp_cylinder;
	Sphere* temp_sphere;
	
	Eigen::Vector3f temp_center, temp_scale, temp_color;

	temp_cube = new Cube;
	temp_color = Eigen::Vector3f(0.3,0.3,0.1);
	temp_cube->m_Color = temp_color;

	g_Drawer->SetIdentity();
	temp_center = Eigen::Vector3f(0, 25, -100);
	g_Drawer->Translate(temp_center);
	g_Drawer->RotateY(0);
	temp_scale = Eigen::Vector3f(200,50,10);
	g_Drawer->Scale(temp_scale);

	temp_cube->m_Trans = g_Drawer->m_transformation;
	temp_cube->m_TransBack = temp_cube->m_Trans.inverse();

	m_obstacles.push_back(temp_cube);

	/*
	//cube 2
	temp_cube = new Cube;
	temp_color = Eigen::Vector3f(0.3,0.3,0.1);
	temp_cube->m_Color = temp_color;

	m_Drawer->SetIdentity();
	temp_center = Eigen::Vector3f(0, 25, 100);
	m_Drawer->Translate(temp_center);
	m_Drawer->RotateY(0);
	temp_scale = Eigen::Vector3f(200,50,10);
	m_Drawer->Scale(temp_scale);

	temp_cube->m_Trans = m_Drawer->m_transformation;
	temp_cube->m_TransBack = temp_cube->m_Trans.inverse();

	m_obstacles.push_back(temp_cube);

	//cube 3
	temp_cube = new Cube;
	temp_color = Eigen::Vector3f(0.3,0.3,0.1);
	temp_cube->m_Color = temp_color;

	m_Drawer->SetIdentity();
	temp_center = Eigen::Vector3f(100, 25, 0);
	m_Drawer->Translate(temp_center);
	m_Drawer->RotateY(0);
	temp_scale = Eigen::Vector3f(10,50,200);
	m_Drawer->Scale(temp_scale);

	temp_cube->m_Trans = m_Drawer->m_transformation;
	temp_cube->m_TransBack = temp_cube->m_Trans.inverse();

	m_obstacles.push_back(temp_cube);

	//cube 4
	temp_cube = new Cube;
	temp_color = Eigen::Vector3f(0.3,0.3,0.1);
	temp_cube->m_Color = temp_color;

	m_Drawer->SetIdentity();
	temp_center = Eigen::Vector3f(-100, 25, 0);
	m_Drawer->Translate(temp_center);
	m_Drawer->RotateY(0);
	temp_scale = Eigen::Vector3f(10,50,200);
	m_Drawer->Scale(temp_scale);

	temp_cube->m_Trans = m_Drawer->m_transformation;
	temp_cube->m_TransBack = temp_cube->m_Trans.inverse();

	m_obstacles.push_back(temp_cube);

	*/

	//case 2: cylinders

	temp_cylinder = new Cylinder;
	temp_color = Eigen::Vector3f(0.3,0.3,0.1);
	temp_cylinder->m_Color = temp_color;

	g_Drawer->SetIdentity();
	temp_center = Eigen::Vector3f(-100, 25, 0);
	g_Drawer->Translate(temp_center);
	g_Drawer->RotateX(90);
	temp_scale = Eigen::Vector3f(20,20,50);
	g_Drawer->Scale(temp_scale);

	temp_cylinder->m_Trans = g_Drawer->m_transformation;
	temp_cylinder->m_TransBack = temp_cylinder->m_Trans.inverse();

	m_obstacles.push_back(temp_cylinder);

	//case 3: spheres

	temp_sphere = new Sphere;
	temp_color = Eigen::Vector3f(0.3,0.3,0.1);
	temp_sphere->m_Color = temp_color;

	g_Drawer->SetIdentity();
	temp_center = Eigen::Vector3f(-90, 10, 0);
	g_Drawer->Translate(temp_center);
	temp_scale = Eigen::Vector3f(20,20,20);
	g_Drawer->Scale(temp_scale);

	temp_sphere->m_Trans = g_Drawer->m_transformation;
	temp_sphere->m_TransBack = temp_sphere->m_Trans.inverse();

	m_obstacles.push_back(temp_sphere);
}

void Terrain::InitFood(TerrainType a_type){
	
	int num_food;
	double x,y,z;
	double sphere_radius;
	Sphere* temp_sphere;
	
	if(a_type == TERRAIN_FLAT||a_type == TERRAIN_RANDOM){
		num_food = 10;
		sphere_radius = 0.5+Util::getRand();
		for(int i = 0; i < num_food; i++){
		
			x = m_size_x*(Util::getRand() - 0.5);
			z = m_size_z*(Util::getRand() - 0.5);
			y = GetHeight(Eigen::Vector3f(x,0,z)) + sphere_radius;

			temp_sphere = new Sphere;
			temp_sphere->Init(Eigen::Vector3f(x,y,z),2*sphere_radius*Eigen::Vector3f(1,1,1),Eigen::Vector3f(0,1,0));
			m_foods.push_back(temp_sphere);
		}
	}
	else if(a_type == TERRAIN_TEST){
		sphere_radius = 5;
		x = -300;
		z = 0;
		y = GetHeight(Eigen::Vector3f(x,0,z)) + 1.8*sphere_radius;
		temp_sphere = new Sphere;
		temp_sphere->Init(Eigen::Vector3f(x,y,z),2*sphere_radius*Eigen::Vector3f(1,1,1),Eigen::Vector3f(0,1,0));
		m_foods.push_back(temp_sphere);
	
	}
}

void Terrain::RegisterMillipede(Millipede* a_bug){
	
	assert(a_bug);

	m_millipedes.push_back(a_bug);

}

void Terrain::ClearMillipedes(){
	
	m_millipedes.clear();

}

bool Terrain::ReachFood(Eigen::Vector3f pos, double tol){
	
	for(int i = 0; i < m_foods.size(); i++)
	{
		if((m_foods[i]->m_Center - pos).norm() < tol){
			double sphere_radius =  0.5+Util::getRand();
			double x = m_size_x*(Util::getRand() - 0.5);
			double z = m_size_z*(Util::getRand() - 0.5);
			double y = GetHeight(Eigen::Vector3f(x,0,z)) + sphere_radius;

			m_foods[i]->Init(Eigen::Vector3f(x,y,z),2*sphere_radius*Eigen::Vector3f(1,1,1),Eigen::Vector3f(0,1,0));

			return true;
		}
	}

	return false;

}

double Terrain::GetHeight(const Eigen::Vector3f& xyz) const{
	
	if(m_terrain_type == TERRAIN_SPHERICAL){
		
		Triangle closest_triangle;
		assert(m_surface_mesh->ClosestTriangle(xyz, closest_triangle));
		return (xyz - closest_triangle.m_node_1->m_Position).dot(closest_triangle.m_face_normal);
	}
	else{
		Cube* temp_cube;
		Cylinder* temp_cylinder;
		Sphere* temp_sphere;

		double x = xyz.x();
		double y = xyz.y();
		double z = xyz.z();
		//check if x y lands on a surface object
		for(int i = 0; i < m_surface_objects.size(); i++){
		
			switch (m_surface_objects[i]->m_type)
			{
			case TypeCube:
				temp_cube = dynamic_cast<Cube*>(m_surface_objects[i]);
				if(x < temp_cube->m_Center[0] + temp_cube->m_Size[0]*0.5
					&& x > temp_cube->m_Center[0] - temp_cube->m_Size[0]*0.5
					&& z < temp_cube->m_Center[2] + temp_cube->m_Size[2]*0.5
					&& z > temp_cube->m_Center[2] - temp_cube->m_Size[2]*0.5)
					return y - temp_cube->m_Size[1];
				break;
			case TypeCylinder:
				temp_cylinder = dynamic_cast<Cylinder*>(m_surface_objects[i]);
				if(x < temp_cylinder->m_Center[0] + temp_cylinder->m_Size[0]*0.5
					&&x > temp_cylinder->m_Center[0] - temp_cylinder->m_Size[0]*0.5
					&&z < temp_cylinder->m_Center[2] + temp_cylinder->m_Size[2]*0.5
					&&z > temp_cylinder->m_Center[0] - temp_cylinder->m_Size[2]*0.5)
					return y - sqrt(0.25*temp_cylinder->m_Size[1]*temp_cylinder->m_Size[1] - 
					(x - temp_cylinder->m_Center[0])*(x - temp_cylinder->m_Center[0]));
				break;
			default:
				break;
			}
		
	
		}


		int idx, idz;
		idx = (xyz.x() + 0.5*m_size_x)/m_dx;
		idz = (xyz.z() + 0.5*m_size_z)/m_dz;

		//interpolation
		double upleft, upright, downleft, downright;
		downleft = m_height_data[idx*(m_res_z+1) + idz];
		downright = m_height_data[(idx+1)*(m_res_z+1) + idz];
		upleft = m_height_data[idx*(m_res_z+1) + idz+1];
		upright = m_height_data[(idx+1)*(m_res_z+1) + idz+1];

		double alpha, beta;
		alpha = 1 - (xyz.x() + 0.5*m_size_x - idx*m_dx)/m_dx;
		beta = 1 - (xyz.z() + 0.5*m_size_z - idz*m_dz)/m_dz;

		return y - alpha*beta*downleft + (1-alpha)*beta*downright + (1-beta)*alpha*upleft + (1-beta)*(1-alpha)*upright;
	}
}

Eigen::Vector3f Terrain::GetNormal(const Eigen::Vector3f& xyz){

	if(m_terrain_type == TERRAIN_SPHERICAL){
		Triangle closest_triangle;
		assert(m_surface_mesh->ClosestTriangle(xyz, closest_triangle));
		return closest_triangle.m_face_normal;
	}//end if Terrain Spherical
	else{

		//first deal with surface objects if any
		Cube* temp_cube;
		Cylinder* temp_cylinder;
		Sphere* temp_sphere;

		double x = xyz.x();
		double z = xyz.z();
		//check if x y lands on a surface object
		for(int i = 0; i < m_surface_objects.size(); i++){
		
			switch (m_surface_objects[i]->m_type)
			{
			case TypeCube:
				temp_cube = dynamic_cast<Cube*>(m_surface_objects[i]);
				if(x < temp_cube->m_Center[0] + temp_cube->m_Size[0]*0.5
					&& x > temp_cube->m_Center[0] - temp_cube->m_Size[0]*0.5
					&& z < temp_cube->m_Center[2] + temp_cube->m_Size[2]*0.5
					&& z > temp_cube->m_Center[2] - temp_cube->m_Size[2]*0.5)
					return Eigen::Vector3f(0,1,0);
				break;
			case TypeCylinder:
				temp_cylinder = dynamic_cast<Cylinder*>(m_surface_objects[i]);
				if(x < temp_cylinder->m_Center[0] + temp_cylinder->m_Size[0]*0.5
					&&x > temp_cylinder->m_Center[0] - temp_cylinder->m_Size[0]*0.5
					&&z < temp_cylinder->m_Center[2] + temp_cylinder->m_Size[2]*0.5
					&&z > temp_cylinder->m_Center[0] - temp_cylinder->m_Size[2]*0.5)
					return (Eigen::Vector3f(x, GetHeight(xyz), z) - 
							Eigen::Vector3f(temp_cylinder->m_Center[0], 0, z)).normalized();//something 	
				break;
			default:
				break;
			}
		}

		//check terrain base
		int idx, idz;
		idx = (xyz.x() + 0.5*m_size_x)/m_dx;
		idz = (xyz.z() + 0.5*m_size_z)/m_dz;

		//interpolation
		Eigen::Vector3f upleft, upright, downleft, downright;
		downleft = m_normal_data[idx*(m_res_z+1) + idz];
		downright = m_normal_data[(idx+1)*(m_res_z+1) + idz];
		upleft = m_normal_data[idx*(m_res_z+1) + idz+1];
		upright = m_normal_data[(idx+1)*(m_res_z+1) + idz+1];

		double alpha, beta;
		alpha = 1 - (xyz.x() + 0.5*m_size_x - idx*m_dx)/m_dx;
		beta = 1 - (xyz.z() + 0.5*m_size_z - idz*m_dz)/m_dz;

		return alpha*beta*downleft + (1-alpha)*beta*downright + (1-beta)*alpha*upleft + (1-beta)*(1-alpha)*upright;
	}
}

bool Terrain::TestIntersection(Millipede* a_bug, Eigen::Vector3f a_o, Eigen::Vector3f a_p){

	Cube* temp_cube;
	Sphere* temp_sphere;
	Cylinder* temp_cylinder;
	//generate sample points on the line segment for intersection test
	int num_sample_points = 5; 
	assert(num_sample_points > 2);
	double d_alpha = 1.0/(num_sample_points-1);
	std::vector<Eigen::Vector3f> sample_points;
	for(int i = 0; i < num_sample_points; i++){
		sample_points.push_back(a_o*i*d_alpha+ a_p*(1 - i*d_alpha));
	}
	
	Eigen::Vector4f temp_point;
	//the intersection test of line agains geometrical primitives are now 
	//implemented with point-inside-outside test

	for(int i = 0; i < m_obstacles.size(); i++)
	{
		//test if part of the line segment inside the object
		switch (m_obstacles[i]->m_type)
		{
		case TypeCube:
			temp_cube = dynamic_cast<Cube*>(m_obstacles[i]);
			for(int j = 0; j < num_sample_points; j++){
				temp_point = Eigen::Vector4f(sample_points[j][0],sample_points[j][1],sample_points[j][2],1.0);
				temp_point = temp_cube->m_TransBack*temp_point;
				if(fabs(temp_point[0]) < 0.5&&fabs(temp_point[1]) < 0.5 && fabs(temp_point[2]) < 0.5){
					return true; 
					
				}
			}
		break;
		case TypeSphere:
			temp_sphere = dynamic_cast<Sphere*>(m_obstacles[i]);
			for(int j = 0; j < num_sample_points; j++){
				temp_point = Eigen::Vector4f(sample_points[j][0],sample_points[j][1],sample_points[j][2],1.0);
				temp_point = temp_sphere->m_TransBack*temp_point;
				temp_point[3] = 0.0;
				if(temp_point.norm() < 1.0){
					return true; 
					
				}
			}
		break;
		case TypeCylinder:
			temp_cylinder = dynamic_cast<Cylinder*>(m_obstacles[i]);
			for(int j = 0; j < num_sample_points; j++){
				temp_point = Eigen::Vector4f(sample_points[j][0],sample_points[j][1],sample_points[j][2],1.0);
				temp_point = temp_cylinder->m_TransBack*temp_point;
				if(temp_point[0]*temp_point[0] + temp_point[1]*temp_point[1] < 0.5*0.5){
					return true; 
				}
			}

		break;
		default:
			break;
		}
	}

	//TEst agains the list of other millipedes
	for(int i = 0; i < m_millipedes.size(); i++){
		if(m_millipedes[i] == a_bug)
			continue;

		double x_min, z_min, x_max, z_max;
		x_min = m_millipedes[i]->m_bounding_box[0];
		x_max = m_millipedes[i]->m_bounding_box[1];
		z_min = m_millipedes[i]->m_bounding_box[2];
		z_max = m_millipedes[i]->m_bounding_box[3];
		//treat the other millipede as a cube obstacle;
		for(int j = 0; j < num_sample_points; j++){
			if( sample_points[j][0] > x_min &&
				sample_points[j][0] < x_max &&
				sample_points[j][2] > z_min &&
				sample_points[j][2] < z_max)
				return true; 
		}

	}

	return false;
}

double Terrain::GetFoodIntensity(Eigen::Vector3f pos){
	double intensity = 0;
	double dist;
	for(int i = 0; i < m_foods.size(); i++)
	{
		dist = (m_foods[i]->m_Center - pos).norm();
		intensity += 100*m_foods[i]->m_Size.norm()/(dist*dist);//1/r*r spherical decay
	}

	return intensity;
}

void Terrain::InitDraw(){

	// Initialize the data array on CPU
	if(m_terrain_type == TERRAIN_SPHERICAL){
		m_NTrianglePoints = m_surface_mesh->m_Num_f*3;

		m_TrianglePoints = new Eigen::Vector4f[m_NTrianglePoints];
		m_TPointNormals = new Eigen::Vector3f[m_NTrianglePoints];
		m_TriangleColors = new Eigen::Vector4f[m_NTrianglePoints];

		Eigen::Vector3f pos;
		for(int i = 0; i < m_surface_mesh->m_Num_f; i++){
			pos = m_surface_mesh->m_Trias[i]->m_node_1->m_Position;
			m_TrianglePoints[3*i] = Eigen::Vector4f(pos[0],pos[1],pos[2],1);
			pos = m_surface_mesh->m_Trias[i]->m_node_2->m_Position;
			m_TrianglePoints[3*i + 1] = Eigen::Vector4f(pos[0],pos[1],pos[2],1);
			pos = m_surface_mesh->m_Trias[i]->m_node_3->m_Position;
			m_TrianglePoints[3*i + 2] = Eigen::Vector4f(pos[0],pos[1],pos[2],1);
			m_TPointNormals[3*i] = m_surface_mesh->m_Trias[i]->m_node_1->m_Normal;
			m_TPointNormals[3*i + 1] = m_surface_mesh->m_Trias[i]->m_node_2->m_Normal;
			m_TPointNormals[3*i + 2] = m_surface_mesh->m_Trias[i]->m_node_3->m_Normal;
			m_TriangleColors[3*i] = Eigen::Vector4f(1.0,0,0,1.0);
			m_TriangleColors[3*i + 1] = Eigen::Vector4f(1.0,0,0,1.0);
			m_TriangleColors[3*i + 2] = Eigen::Vector4f(1.0,0,0,1.0);
		}
	}
	else{
		m_NTrianglePoints = 3*2*m_res_x*m_res_z;
		m_TrianglePoints = new Eigen::Vector4f[m_NTrianglePoints];
		m_TPointNormals = new Eigen::Vector3f[m_NTrianglePoints];
		m_TriangleColors = new Eigen::Vector4f[m_NTrianglePoints];

		int box_id;
		Eigen::Vector4f a,b,c,d;
		Eigen::Vector3f na,nb,nc,nd;
		for(int ix = 0; ix < m_res_x; ix++)
			for(int iz = 0; iz < m_res_z; iz++){

			box_id = ix*m_res_z + iz;
		
			a.x() = m_dx*ix - 0.5*m_size_x;a.z() = m_dz*iz - 0.5*m_size_z;a.y() = m_height_data[ix*(m_res_z+1) + iz];a.w() = 1.0;
			b.x() = m_dx*(ix+1) - 0.5*m_size_x;b.z() = m_dz*iz - 0.5*m_size_z;b.y() = m_height_data[(ix+1)*(m_res_z+1) + iz];b.w() = 1.0;
			c.x() = m_dx*(ix+1) - 0.5*m_size_x;c.z() = m_dz*(iz+1) - 0.5*m_size_z;c.y() = m_height_data[(ix+1)*(m_res_z+1) + iz + 1];c.w() = 1.0;
			d.x() = m_dx*ix - 0.5*m_size_x;d.z() = m_dz*(iz+1) - 0.5*m_size_z;d.y() = m_height_data[ix*(m_res_z+1) + iz + 1];d.w() = 1.0;
		
			m_TrianglePoints[6*box_id] = a;
			m_TrianglePoints[6*box_id + 1] = b;
			m_TrianglePoints[6*box_id + 2] = c;
			m_TrianglePoints[6*box_id + 3] = c;
			m_TrianglePoints[6*box_id + 4] = d;
			m_TrianglePoints[6*box_id + 5] = a;

			na = m_normal_data[ix*(m_res_z+1) + iz];
			nb = m_normal_data[(ix+1)*(m_res_z+1) + iz];
			nc = m_normal_data[(ix+1)*(m_res_z+1) + iz+1];
			nd = m_normal_data[ix*(m_res_z+1) + iz+1];
		
			m_TPointNormals[6*box_id] = na;
			m_TPointNormals[6*box_id + 1] = nb;
			m_TPointNormals[6*box_id + 2] = nc;
			m_TPointNormals[6*box_id + 3] = nc;
			m_TPointNormals[6*box_id + 4] = nd;
			m_TPointNormals[6*box_id + 5] = na;
		
			for(int ii = 0; ii < 6; ii++)
				m_TriangleColors[box_id*6+ii] = Eigen::Vector4f(0,0.8,1.0,1.0);	
		}
	}
	
    //Create the Vertex Array and Buffers, bind them
#ifdef __APPLE__
	glGenVertexArraysAPPLE(1, &m_vertexArrayObject);
	glBindVertexArrayAPPLE(m_vertexArrayObject);//use as current vertex array   
#else
	 glGenVertexArrays(1, &m_vertexArrayObject);
	 glBindVertexArray(m_vertexArrayObject);//use as current vertex array 
#endif
   
    glGenBuffers(1, &m_vertexBufferObject);//generate buffer for current vertex array
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
    
	 //send the updated data to buffer
    glBufferData(GL_ARRAY_BUFFER, (sizeof(m_TrianglePoints[0]) + sizeof(m_TriangleColors[0]) + sizeof(m_TPointNormals[0]))*m_NTrianglePoints, NULL, GL_STATIC_DRAW);//send data to current buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_TrianglePoints[0])*m_NTrianglePoints, m_TrianglePoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(m_TrianglePoints[0])*m_NTrianglePoints, sizeof(m_TriangleColors[0])*m_NTrianglePoints, m_TriangleColors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(m_TriangleColors[0])*m_NTrianglePoints+sizeof(m_TrianglePoints[0])*m_NTrianglePoints,sizeof(m_TPointNormals[0])*m_NTrianglePoints, m_TPointNormals);
    
    //load and compile shaders on GPU, use current shader program
    m_shader = Util::InitShader( "vSmoothPhong.glsl", "fSmoothPhong.glsl" );
    glUseProgram(m_shader);
   
	 
    GLuint position;
    GLuint color;
    GLuint normal;
    

	position = glGetAttribLocation( m_shader, "vPosition" );
    glEnableVertexAttribArray( position );
    glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); 
    
    color = glGetAttribLocation(m_shader, "vColor");
    glEnableVertexAttribArray(color);
    glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE,0, BUFFER_OFFSET(sizeof(m_TrianglePoints[0])*m_NTrianglePoints));
    
    normal = glGetAttribLocation(m_shader, "vNormal");
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE,0, BUFFER_OFFSET((sizeof(m_TriangleColors[0])+sizeof(m_TrianglePoints[0]))*m_NTrianglePoints));
	
}

void Terrain::Draw(int type, const Camera& camera, const Light& light){

	//Get new position of the cube and update the model view matrix
    Eigen::Affine3f wMo;//object to world matrix
    Eigen::Affine3f cMw;
    Eigen::Affine3f proj;

    glUseProgram(m_shader);
#ifdef __APPLE__
    glBindVertexArrayAPPLE(m_vertexArrayObject); 
#else
	glBindVertexArray(m_vertexArrayObject);
#endif
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);//use as current buffer
    
    GLint world2camera = glGetUniformLocation(m_shader, "cMw"); 
	GLint projection = glGetUniformLocation(m_shader, "proj");
    GLint kAmbient = glGetUniformLocation(m_shader,"kAmbient");
    GLint kDiffuse = glGetUniformLocation(m_shader,"kDiffuse");
    GLint kSpecular = glGetUniformLocation(m_shader,"kSpecular");
    GLint shininess = glGetUniformLocation(m_shader,"shininess");
    GLint camera_position = glGetUniformLocation(m_shader, "cameraPosition");
    GLint light_position = glGetUniformLocation(m_shader, "lightPosition");
    
    //generate the Angel::Angel::Angel::matrixes
    proj = Util::Perspective( camera.m_fovy, camera.m_aspect, camera.m_znear, camera.m_zfar );
	cMw = camera.m_cMw;//LookAt(camera.position,camera.lookat, camera.up );
    
    Eigen::Vector4f v4color(0.55,0.25,0.08,1.0);
    Eigen::Vector4f Ambient;
    Ambient = 0.3*v4color;
    Eigen::Vector4f Diffuse;
    Diffuse = 0.5*v4color;
    Eigen::Vector4f Specular(0.3,0.3,0.3,1.0);
    
    glUniformMatrix4fv( world2camera, 1, GL_FALSE, cMw.data() );
    glUniformMatrix4fv( projection, 1, GL_FALSE, proj.data() );
    
    glUniform4fv(kAmbient, 1, Ambient.data());
    glUniform4fv(kDiffuse, 1, Diffuse.data()); 
    glUniform4fv(kSpecular, 1, Specular.data());
    glUniform4fv(camera_position, 1, camera.m_position.data());
    glUniform4fv(light_position, 1, light.m_position.data());
    glUniform1f(shininess, 10);

    switch (type) {
        case DRAW_MESH:
            glUniform1i(glGetUniformLocation(m_shader, "renderType"), 1);
            glDrawArrays(GL_LINES, 0, m_NTrianglePoints);
            break;
        case DRAW_PHONG:
            glUniform1i(glGetUniformLocation(m_shader, "renderType"), 2);
            glDrawArrays(GL_TRIANGLES, 0, m_NTrianglePoints);
            break;
    }

	//draw the obstacles
	for(int i = 0; i < m_obstacles.size(); i++)
	{
		m_obstacles[i]->Draw(type,camera, light);
	}

	for(int i = 0; i < m_foods.size(); i++)
	{
		m_foods[i]->Draw(type,camera, light);
	}

	//draw the obstacles
	for(int i = 0; i < m_surface_objects.size(); i++)
	{
		m_surface_objects[i]->Draw(type,camera, light);
	}

}