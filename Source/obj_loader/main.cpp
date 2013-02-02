#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include "fanfu_utilities/geometry.h"
#include "fanfu_utilities/fast_sweeping_3d.hpp"
#include "bridson_mesh_query/mesh_query.h"
#include "obj_loader/objLoader.h"

int main(){
	typedef double T;    

	// TEST: Point-Triangle distance
	//{
	//	metis::ALGEBRA::VECTOR_3D<T> A(1,0,0);
	//	metis::ALGEBRA::VECTOR_3D<T> B(0,1,0);
	//	metis::ALGEBRA::VECTOR_3D<T> C(0,0,1);    
	//	metis::GEOMETRY::TRIANGLE_3D<T> tri(A,B,C);
	//	std::cout<<"Triangle is "<<"("<<A(0)<<","<<A(1)<<","<<A(2)<<")"<<"("<<B(0)<<","<<B(1)<<","<<B(2)<<")"<<"("<<C(0)<<","<<C(1)<<","<<C(2)<<")"<<std::endl;    
	//	metis::ALGEBRA::VECTOR_3D<T> P;
	//	P(0)=1; P(1)=0; P(2)=0;
	//	std::cout << "Distance to "<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is "<<tri.Distance_To_Point(P)<<std::endl;
	//	P(0)=0; P(1)=0; P(2)=0;
	//	std::cout << "Distance to "<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is "<<tri.Distance_To_Point(P)<<std::endl;
	//	P(0)=0; P(1)=1; P(2)=0;
	//	std::cout << "Distance to "<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is "<<tri.Distance_To_Point(P)<<std::endl;
	//	P(0)=1; P(1)=1; P(2)=1;
	//	std::cout << "Distance to "<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is "<<tri.Distance_To_Point(P)<<std::endl;
	//	P(0)=3; P(1)=0; P(2)=0;
	//	std::cout << "Distance to "<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is "<<tri.Distance_To_Point(P)<<std::endl;
	//	P(0)=0; P(1)=2; P(2)=0;
	//	std::cout << "Distance to "<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is "<<tri.Distance_To_Point(P)<<std::endl;
	//	P(0)=0; P(1)=0; P(2)=5;
	//	std::cout << "Distance to "<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is "<<tri.Distance_To_Point(P)<<std::endl;
	//}	

	std::cout<<"-------------------------------------------------------\n"
		     <<"   metis - mesh to implicit surface                    \n"
			 <<"   Copyright Chenfanfu Jiang, 2012                     \n"
			 <<"-------------------------------------------------------\n"
			 <<" The program will convert 'test.obj' to 'test.vtk'.    \n"
			 <<"-------------------------------------------------------\n"
			 <<" Please input grid res (h=1/res): ";

	// input grid resolution
	int res;
	std::cin>>res;
	T h=1.0/res;

	// read obj file using obj loader
	std::cout<<"\nLoading test.obj raw data...";
	objLoader *objData=new objLoader();
	objData->load("test.obj");
	printf("- Number of vertices: %i\n",objData->vertexCount);
	printf("- Number of faces: %i\n",objData->faceCount);

	// convert obj loader data to triangle mesh
	std::cout<<"\nBuilding triangle mesh...\n";
	metis::GEOMETRY::TRIANGLE_MESH_3D<T> mesh;
    mesh.Clear();
	for(int i=0; i<objData->vertexCount; i++){
		metis::ALGEBRA::VECTOR_3D<T> v(objData->vertexList[i]->e[0],objData->vertexList[i]->e[1],objData->vertexList[i]->e[2]);
		mesh.nodes.push_back(v);}
	for(int i=0; i<objData->faceCount; i++){
		obj_face *o = objData->faceList[i];
		metis::ALGEBRA::VECTOR_3D<int> e(o->vertex_index[0],o->vertex_index[1],o->vertex_index[2]);
		mesh.tris.push_back(e);}

	// translate and scale mesh into an unit cube
	std::cout<<"\nTranslating and scaling mesh into an unit cube...\n";
	metis::ALGEBRA::VECTOR_3D<T> com=mesh.Compute_Center_Of_Mass();
	std::cout<<"- Old center of mass: ("<<com(0)<<","<<com(1)<<","<<com(2)<<")"<<std::endl;
	T bounding_cube_d=mesh.Compute_Buffless_Bounding_Cube_Side_Length();
	std::cout<<"- Old buffless bounding cube side length: "<<bounding_cube_d<<std::endl;
	mesh.Translate_And_Scale(-com,1.0/bounding_cube_d);
	com=mesh.Compute_Center_Of_Mass();
	std::cout<<"- New center of mass: ("<<com(0)<<","<<com(1)<<","<<com(2)<<")"<<std::endl;
	bounding_cube_d=mesh.Compute_Buffless_Bounding_Cube_Side_Length();
	std::cout<<"- New buffless bounding cube side length: "<<bounding_cube_d<<std::endl;

	// initialize the grid
	std::cout<<"\nInitializing gid...\n";
	metis::ALGEBRA::VECTOR_3D<T> box_min,box_max;
	mesh.Get_Mesh_Bounding_Box(box_min,box_max,4.0*h);
	metis::ALGEBRA::VECTOR_3D<T> origin(box_min(0),box_min(1),box_min(2));
	metis::ALGEBRA::VECTOR_3D<int> dimensionN(int((box_max(0)-box_min(0))/h)+1,int((box_max(1)-box_min(1))/h)+1,int((box_max(2)-box_min(2))/h)+1);
	metis::GEOMETRY::UNIFORM_GRID_3D<T> grid(origin,dimensionN,h);

	std::cout<<"- Grid information:\n"
		<<"  - origin: ("<<grid.origin(0)<<","<<grid.origin(1)<<","<<grid.origin(2)<<")\n"
		<<"  - max corner: ("<<grid.origin(0)+(grid.dimensionN(0)-1)*grid.h<<","<<grid.origin(1)+(grid.dimensionN(1)-1)*grid.h<<","<<grid.origin(2)+(grid.dimensionN(2)-1)*grid.h<<")\n"
		<<"  - dimension: "<<grid.dimensionN(0)<<" * "<<grid.dimensionN(1)<<" * "<<grid.dimensionN(2)<<"\n"
		<<"  - spacing: "<<grid.h<<"\n"
		<<"  - total nubmer of nodes: "<<grid.dimensionN(0)*grid.dimensionN(1)*grid.dimensionN(2)<<"\n";

	// build unsigned distance field near surface
	std::cout<<"\nBuilding exact unsigned distance field near the surface...\n";
	int Nx=dimensionN(0),Ny=dimensionN(1),Nz=dimensionN(2);
	std::vector<std::vector<std::vector<T> > > phi(Nx, std::vector<std::vector<T> >(Ny, std::vector<T>(Nz)));
	for(int i=0;i<Nx;i++)for(int j=0;j<Ny;j++)for(int k=0;k<Nz;k++)phi[i][j][k]=99999;
	for(int t=0;t<mesh.tris.size();t++){
		int a=mesh.tris[t](0),b=mesh.tris[t](1),c=mesh.tris[t](2);
		metis::GEOMETRY::TRIANGLE_3D<T> tri(mesh.nodes[a],mesh.nodes[b],mesh.nodes[c]);
		metis::ALGEBRA::VECTOR_3D<int> min_corner,max_corner;
		grid.Get_Triangle_Bounding_Box(tri,min_corner,max_corner,3);
		for(int i=min_corner(0);i<=max_corner(0);i++){
			for(int j=min_corner(1);j<=max_corner(1);j++){
				for(int k=min_corner(2);k<=max_corner(2);k++){
					metis::ALGEBRA::VECTOR_3D<T> P=grid(i,j,k);
					T dist=tri.Distance_To_Point(P);
					if(dist<phi[i][j][k])phi[i][j][k]=dist;}}}}

	// assign signs by doing inside/outside query
	std::cout<<"\nAssigning signs for all grid nodes...\n";
	int num_vertices=mesh.nodes.size();
	int num_triangles=mesh.tris.size();
	double *positions=new double[3*num_vertices];
	for(int v=0;v<mesh.nodes.size();v++){
		positions[3*v]=mesh.nodes[v][0];
		positions[3*v+1]=mesh.nodes[v][1];
		positions[3*v+2]=mesh.nodes[v][2];}
	int *triangles=new int[3*num_triangles];
	for(int t=0;t<mesh.tris.size();t++){
		triangles[3*t]=mesh.tris[t][0];
		triangles[3*t+1]=mesh.tris[t][1];
		triangles[3*t+2]=mesh.tris[t][2];}
	MeshObject* brison_mesh_object=construct_mesh_object(num_vertices,positions,num_triangles,triangles);
	for(int i=0;i<Nx;i++)for(int j=0;j<Ny;j++)for(int k=0;k<Nz;k++){
		metis::ALGEBRA::VECTOR_3D<T> P=grid(i,j,k);
		double point[3]={P(0),P(1),P(2)};
		if(point_inside_mesh(point,brison_mesh_object) && phi[i][j][k]>0.0)phi[i][j][k]*=(-1.0);}
	destroy_mesh_object(brison_mesh_object);
	delete[] positions;
	delete[] triangles;
	//for(int i=0;i<Nx;i++)for(int j=0;j<Ny;j++)for(int k=0;k<Nz;k++)if(phi[i][j][k]<-100)std::cout<<phi[i][j][k]<<"  ";
	
	// fast sweeping
	std::cout<<"\nFast sweeping...\n";
	metis::fast_sweeping_3d(phi,Nx,Ny,Nz,h);
	int num_nodes_negative=0,num_nodes_positive=0,num_nodes_exact_zero=0;
	for(int i=0;i<Nx;i++)for(int j=0;j<Ny;j++)for(int k=0;k<Nz;k++){
		if(phi[i][j][k]<0.0)num_nodes_negative++;
		else if(phi[i][j][k]>0.0)num_nodes_positive++;
		else num_nodes_exact_zero++;}
	std::cout<<"  - Number of nodes with negative phi: "<<num_nodes_negative<<"\n"
		<<"  - Number of nodes with positive phi: "<<num_nodes_positive<<"\n"
		<<"  - Number of nodes with zero phi: "<<num_nodes_exact_zero<<"\n";

	// write vtk file
	std::cout<<"\nWriting result to test.vtk\n";
	std::ofstream myfile;
	myfile.open("test.vtk");
	myfile<<"# vtk DataFile Version 1.0\nVector Field Dataset\nASCII\n\nDATASET STRUCTURED_POINTS\n";
	myfile<<"DIMENSIONS "<<grid.dimensionN(0)<<" "<<grid.dimensionN(1)<<" "<<grid.dimensionN(2)<<"\n";
	myfile<<"ORIGIN "<<std::fixed<<std::setprecision(15)<<grid.origin(0)<<" "<<grid.origin(1)<<" "<<grid.origin(2)<<"\n";
	myfile<<"SPACING "<<std::fixed<<std::setprecision(15)<<grid.h<<" "<<grid.h<<" "<<grid.h<<"\n\n";
	myfile<<"POINT_DATA "<<Nx*Ny*Nz<<"\n\n";
	myfile<<"SCALARS phi float\nLOOKUP_TABLE default\n";
	for(int k=0;k<Nz;k++)for(int j=0;j<Ny;j++)for(int i=0;i<Nx;i++)
		myfile<<std::fixed<<std::setprecision(15)<<phi[i][j][k]<<"\n";
	myfile.close();
	


    // TEST: grid
    {
        //metis::ALGEBRA::VECTOR_3D<T> origin(0,0,0);
        //metis::ALGEBRA::VECTOR_3D<int> dimensionN(4,4,4);
        //T h=1.0;
        //metis::GEOMETRY::UNIFORM_GRID_3D<T> grid(origin,dimensionN,h);

        //metis::ALGEBRA::VECTOR_3D<T> P;
        //metis::ALGEBRA::VECTOR_3D<int> min_corner,max_corner;

        //P(0)=0.5;P(1)=0.5;P(2)=0.5;
        //grid.Get_Vertex_Bounding_Box(P,min_corner,max_corner,1);
        //std::cout<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is bounded by ("<<min_corner(0)<<","<<min_corner(1)<<","<<min_corner(2)<<") - ("<<max_corner(0)<<","<<max_corner(1)<<","<<max_corner(2)<<")\n";
        //P(0)=1.5;P(1)=0.5;P(2)=0.5;
        //grid.Get_Vertex_Bounding_Box(P,min_corner,max_corner,1);
        //std::cout<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is bounded by ("<<min_corner(0)<<","<<min_corner(1)<<","<<min_corner(2)<<") - ("<<max_corner(0)<<","<<max_corner(1)<<","<<max_corner(2)<<")\n";
        //P(0)=1.9;P(1)=1.5;P(2)=0.5;
        //grid.Get_Vertex_Bounding_Box(P,min_corner,max_corner,1);
        //std::cout<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is bounded by ("<<min_corner(0)<<","<<min_corner(1)<<","<<min_corner(2)<<") - ("<<max_corner(0)<<","<<max_corner(1)<<","<<max_corner(2)<<")\n";
        //P(0)=0.5;P(1)=1.5;P(2)=0.5;
        //grid.Get_Vertex_Bounding_Box(P,min_corner,max_corner,1);
        //std::cout<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is bounded by ("<<min_corner(0)<<","<<min_corner(1)<<","<<min_corner(2)<<") - ("<<max_corner(0)<<","<<max_corner(1)<<","<<max_corner(2)<<")\n";
        //P(0)=0.5;P(1)=0.5;P(2)=1.5;
        //grid.Get_Vertex_Bounding_Box(P,min_corner,max_corner,1);
        //std::cout<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is bounded by ("<<min_corner(0)<<","<<min_corner(1)<<","<<min_corner(2)<<") - ("<<max_corner(0)<<","<<max_corner(1)<<","<<max_corner(2)<<")\n";
        //P(0)=0.5;P(1)=0.5;P(2)=2.5;
        //grid.Get_Vertex_Bounding_Box(P,min_corner,max_corner,1);
        //std::cout<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is bounded by ("<<min_corner(0)<<","<<min_corner(1)<<","<<min_corner(2)<<") - ("<<max_corner(0)<<","<<max_corner(1)<<","<<max_corner(2)<<")\n";
        //P(0)=1.5;P(1)=0.5;P(2)=2.5;
        //grid.Get_Vertex_Bounding_Box(P,min_corner,max_corner,1);
        //std::cout<<"("<<P(0)<<","<<P(1)<<","<<P(2)<<") is bounded by ("<<min_corner(0)<<","<<min_corner(1)<<","<<min_corner(2)<<") - ("<<max_corner(0)<<","<<max_corner(1)<<","<<max_corner(2)<<")\n";

        //metis::ALGEBRA::VECTOR_3D<T> A(1,1.2,0.2);
        //metis::ALGEBRA::VECTOR_3D<T> B(1.3,1,2.2);
        //metis::ALGEBRA::VECTOR_3D<T> C(1.1,2.2,1);    
        //metis::GEOMETRY::TRIANGLE_3D<T> tri(A,B,C);
        //grid.Get_Triangle_Bounding_Box(tri,min_corner,max_corner,1);
        //std::cout<<"triangle is bounded by ("<<min_corner(0)<<","<<min_corner(1)<<","<<min_corner(2)<<") - ("<<max_corner(0)<<","<<max_corner(1)<<","<<max_corner(2)<<")\n";
    }

	// Test multidimensional vector
	{
		//int Nx=2,Ny=3,Nz=5;
		//int counter=0;
		//std::vector<std::vector<std::vector<int> > > hello(Nx, std::vector<std::vector<int> >(Ny, std::vector<int>(Nz)));
		//for(int i=0;i<2;i++)for(int j=0;j<3;j++)for(int k=0;k<5;k++)hello[i][j][k]=counter++;
		//for(int i=0;i<2;i++)for(int j=0;j<3;j++)for(int k=0;k<5;k++)std::cout<<hello[i][j][k]<<std::endl;
	}

	return 0;
}