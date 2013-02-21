//
//  Object.cpp
//  FVM3D_Rigid_coupling
//
//  Created by 静一 方 on 4/2/12.
//  Copyright 2012 UCLA. All rights reserved.
//

#include "Object.h"
#include "World.h"

#include <algorithm>

CubicTetraMesh::CubicTetraMesh(Eigen::Vector3i a_num, Eigen::Vector3f a_size, Eigen::Vector3f a_position){
    Meshialize(a_num, a_size, a_position);
}

void CubicTetraMesh::Meshialize(Eigen::Vector3i a_num, Eigen::Vector3f a_size, Eigen::Vector3f a_position){
    m_Num = a_num;
    m_Size = a_size;
    
    m_Num_Node = (m_Num[0]+1)*(m_Num[1]+1)*(m_Num[2]+1);
    //m_Num_Trian;
    m_Num_Tetra = m_Num[0]*m_Num[1]*m_Num[2]*5;
    //create a 3D mesh of a_num and each cube of a_size
    
    m_Nodes = new Node[m_Num_Node];
 //   m_Surface = new Triangle[2*(m_Num[0])*(m_Num[1]) + 2*(m_Num[1])*(m_Num[2]) + 2*(m_Num[0])*(m_Num[2])];
    m_Tetras = new Tetrahedron[m_Num_Tetra];
    
    int idx,idy,idz;
    int idc;//id for current node
    
    //the array of nodes
	for(idx = 0; idx< (m_Num[0]+1); idx++){
        for(idy = 0; idy < (m_Num[1]+1); idy++){
            for(idz = 0; idz < (m_Num[2]+1); idz++){
                
                idc = idx*(m_Num[1]+1)*(m_Num[2]+1) + idy*(m_Num[2]+1) + idz;
                
                m_Nodes[idc].m_Position = a_position + Eigen::Vector3f(m_Size[0]*idx,m_Size[1]*idy,m_Size[2]*idz);
                m_Nodes[idc].m_Velocity = Eigen::Vector3f(0,0,0);
                m_Nodes[idc].m_Mass = 0;
                m_Nodes[idc].m_Fixed = false;
            } 
        }
	}

	Node *a, *b, *c, *d, *e, *f, *g, *h;
    //generate the tetrahedrons
	int i_dx, i_dy, i_dz;
	i_dx = (m_Num[1] + 1)*(m_Num[2] + 1);
	i_dy = (m_Num[2] + 1);
	i_dz = 1;
	int idn;
	for(idx = 0; idx< m_Num[0]; idx++){
        for(idy = 0; idy < m_Num[1]; idy++){
            for(idz = 0; idz < m_Num[2]; idz++){
                
                idc = idx*(m_Num[1])*(m_Num[2]) + idy*(m_Num[2]) + idz;//the id of cube cell
				
                //locate the eight nodes wrapping the cube cell
				idn = idx*i_dx + idy*i_dy + idz*i_dz;//the id of a node on the cell
                a = &m_Nodes[idn + i_dz];
                b = &m_Nodes[idn + i_dx + i_dz]; 
                c = &m_Nodes[idn + i_dx]; 
                d = &m_Nodes[idn];
                e = &m_Nodes[idn + i_dz + i_dy]; 
                f = &m_Nodes[idn + i_dx + i_dz + i_dy]; 
                g = &m_Nodes[idn + i_dx + i_dy]; 
                h = &m_Nodes[idn + i_dy];
        
				//cut the cube cell into five tetras.
                m_Tetras[5*idc].m_node_1 = a;
                m_Tetras[5*idc].m_node_2 = d;
                m_Tetras[5*idc].m_node_3 = e;
                m_Tetras[5*idc].m_node_4 = b;
                
                m_Tetras[5*idc + 1].m_node_1 = f;
                m_Tetras[5*idc + 1].m_node_2 = g;
                m_Tetras[5*idc + 1].m_node_3 = b;
                m_Tetras[5*idc + 1].m_node_4 = e;
                
                m_Tetras[5*idc + 2].m_node_1 = c;
                m_Tetras[5*idc + 2].m_node_2 = g;
                m_Tetras[5*idc + 2].m_node_3 = d;
                m_Tetras[5*idc + 2].m_node_4 = b;
                
                m_Tetras[5*idc + 3].m_node_1 = h;
                m_Tetras[5*idc + 3].m_node_2 = g;
                m_Tetras[5*idc + 3].m_node_3 = e;
                m_Tetras[5*idc + 3].m_node_4 = d;
                
                m_Tetras[5*idc + 4].m_node_1 = e;
                m_Tetras[5*idc + 4].m_node_2 = g;
                m_Tetras[5*idc + 4].m_node_3 = b;
                m_Tetras[5*idc + 4].m_node_4 = d;
        
            }
        }
    }
}

std::vector<Node*> CubicTetraMesh::GetTopNodes(){
	//return the nodes on the top surface of the cubic meshnet
	std::vector<Node*> top_nodes;
	top_nodes.clear();
	int idx, idy, idz, idc;
	idy = m_Num[1];
	for(idx = 0; idx< (m_Num[0]+1); idx++){
        for(idz = 0; idz < (m_Num[2]+1); idz++){
			idc = idx*(m_Num[1]+1)*(m_Num[2]+1) + idy*(m_Num[2]+1) + idz;
			top_nodes.push_back(&m_Nodes[idc]);

		}
	}
	
	return top_nodes;
}


void CubicTetraMesh::GetTopSurface(std::vector<Node*>& nodes, std::vector<int>& faces){
	//return the nodes on the top surface of the cubic meshnet
	nodes = GetTopNodes();

	faces.clear();
	int a,b,c,d, idx, idz;
	for(idx = 0; idx< (m_Num[0]); idx++){
        for(idz = 0; idz < (m_Num[2]); idz++){
			a = idx*(m_Num[2]+1) + idz;
			b = (idx+1)*(m_Num[2]+1) + idz;
			c = idx*(m_Num[2]+1) + idz + 1;
			d = (idx+1)*(m_Num[2]+1) + idz + 1;

			faces.push_back(a);
			faces.push_back(b);
			faces.push_back(c);
			faces.push_back(b);
			faces.push_back(c);
			faces.push_back(d);
		}
	}
}



std::vector<Node*> CubicTetraMesh::GetBottomNodes(){
	//return the nodes on the bottom surface of the cubic meshnet
	//return the nodes on the top surface of the cubic meshnet
	std::vector<Node*> bottom_nodes;
	bottom_nodes.clear();
	int idx, idy, idz, idc;
	idy = 0;
	for(idx = 0; idx< (m_Num[0]+1); idx++){
        for(idz = 0; idz < (m_Num[2]+1); idz++){

			idc = idx*(m_Num[1]+1)*(m_Num[2]+1) + idy*(m_Num[2]+1) + idz;
			bottom_nodes.push_back(&m_Nodes[idc]);

		}
	}
	
	return bottom_nodes;
}


void CubicTetraMesh::GetBottomSurface(std::vector<Node*>& nodes, std::vector<int>& faces){
	//return the nodes on the top surface of the cubic meshnet
	nodes = GetBottomNodes();

	faces.clear();
	int a,b,c,d, idx, idz;
	for(idx = 0; idx< (m_Num[0]); idx++){
        for(idz = 0; idz < (m_Num[2]); idz++){
			a = idx*(m_Num[2]+1) + idz;
			b = (idx+1)*(m_Num[2]+1) + idz;
			c = idx*(m_Num[2]+1) + idz + 1;
			d = (idx+1)*(m_Num[2]+1) + idz + 1;

			faces.push_back(a);
			faces.push_back(b);
			faces.push_back(c);
			faces.push_back(b);
			faces.push_back(c);
			faces.push_back(d);
		}
	}
}


std::vector<Node*> CubicTetraMesh::GetFrontNodes(){
	//return the nodes on the bottom surface of the cubic meshnet
	//return the nodes on the top surface of the cubic meshnet
	std::vector<Node*> front_nodes;
	front_nodes.clear();
	int idx, idy, idz, idc;
	idz = m_Num[2];
	for(idx = 0; idx< (m_Num[0]+1); idx++){
        for(idy = 0; idy < (m_Num[1]+1); idy++){
            
			idc = idx*(m_Num[1]+1)*(m_Num[2]+1) + idy*(m_Num[2]+1) + idz;
			front_nodes.push_back(&m_Nodes[idc]);
            
		}
	}
	
	return front_nodes;
}


void CubicTetraMesh::GetFrontSurface(std::vector<Node*>& nodes, std::vector<int>& faces){
	//return the nodes on the top surface of the cubic meshnet
	nodes = GetFrontNodes();

	faces.clear();
	int a,b,c,d, idx, idy;
	for(idx = 0; idx< (m_Num[0]); idx++){
        for(idy = 0; idy < (m_Num[1]); idy++){
			a = idx*(m_Num[1]+1) + idy;
			b = (idx+1)*(m_Num[1]+1) + idy;
			c = idx*(m_Num[1]+1) + idy + 1;
			d = (idx+1)*(m_Num[1]+1) + idy + 1;

			faces.push_back(a);
			faces.push_back(b);
			faces.push_back(c);
			faces.push_back(b);
			faces.push_back(c);
			faces.push_back(d);
		}
	}
}



std::vector<Node*> CubicTetraMesh::GetBackNodes(){
	//return the nodes on the bottom surface of the cubic meshnet
	//return the nodes on the top surface of the cubic meshnet
	std::vector<Node*> back_nodes;
	back_nodes.clear();
	int idx, idy, idz, idc;
	idz = 0;
	for(idx = 0; idx< (m_Num[0]+1); idx++){
        for(idy = 0; idy < (m_Num[1]+1); idy++){
            
			idc = idx*(m_Num[1]+1)*(m_Num[2]+1) + idy*(m_Num[2]+1) + idz;
			back_nodes.push_back(&m_Nodes[idc]);
        
		}
	}
	
	return back_nodes;
}

void CubicTetraMesh::GetBackSurface(std::vector<Node*>& nodes, std::vector<int>& faces){
	//return the nodes on the top surface of the cubic meshnet
	nodes = GetBackNodes();

	faces.clear();
	int a,b,c,d, idx, idy;
	for(idx = 0; idx< (m_Num[0]); idx++){
        for(idy = 0; idy < (m_Num[1]); idy++){
			a = idx*(m_Num[1]+1) + idy;
			b = (idx+1)*(m_Num[1]+1) + idy;
			c = idx*(m_Num[1]+1) + idy + 1;
			d = (idx+1)*(m_Num[1]+1) + idy + 1;

			faces.push_back(a);
			faces.push_back(b);
			faces.push_back(c);
			faces.push_back(b);
			faces.push_back(c);
			faces.push_back(d);
		}
	}
}



std::vector<Node*> CubicTetraMesh::GetLeftNodes(){
	//return the nodes on the bottom surface of the cubic meshnet
	//return the nodes on the top surface of the cubic meshnet
	std::vector<Node*> left_nodes;
	left_nodes.clear();
	int idx, idy, idz, idc;
	idx = 0;
	for(idy = 0; idy< (m_Num[1]+1); idy++){
        for(idz = 0; idz < (m_Num[2]+1); idz++){
            
			idc = idx*(m_Num[1]+1)*(m_Num[2]+1) + idy*(m_Num[2]+1) + idz;
			left_nodes.push_back(&m_Nodes[idc]);
            
		}
	}
	
	return left_nodes;
}

void CubicTetraMesh::GetLeftSurface(std::vector<Node*>& nodes, std::vector<int>& faces){
	//return the nodes on the top surface of the cubic meshnet
	nodes = GetLeftNodes();

	faces.clear();
	int a,b,c,d, idz, idy;
	for(idy = 0; idy< (m_Num[1]); idy++){
        for(idz = 0; idz < (m_Num[2]); idz++){
			a = idy*(m_Num[2]+1) + idz;
			b = (idy+1)*(m_Num[2]+1) + idz;
			c = idy*(m_Num[2]+1) + idz + 1;
			d = (idy+1)*(m_Num[2]+1) + idz + 1;

			faces.push_back(a);
			faces.push_back(b);
			faces.push_back(c);
			faces.push_back(b);
			faces.push_back(c);
			faces.push_back(d);
		}
	}
}



std::vector<Node*> CubicTetraMesh::GetRightNodes(){
	//return the nodes on the bottom surface of the cubic meshnet
	//return the nodes on the top surface of the cubic meshnet
	std::vector<Node*> right_nodes;
	right_nodes.clear();
	int idx, idy, idz, idc;
	idx = m_Num[0];
	for(idy = 0; idy< (m_Num[1]+1); idy++){
        for(idz = 0; idz < (m_Num[2]+1); idz++){
            
			idc = idx*(m_Num[1]+1)*(m_Num[2]+1) + idy*(m_Num[2]+1) + idz;
			right_nodes.push_back(&m_Nodes[idc]);
            
		}
	}
	
	return right_nodes;
}

void CubicTetraMesh::GetRightSurface(std::vector<Node*>& nodes, std::vector<int>& faces){
	//return the nodes on the top surface of the cubic meshnet
	nodes = GetRightNodes();

	faces.clear();
	int a,b,c,d, idz, idy;
	for(idy = 0; idy< (m_Num[1]); idy++){
        for(idz = 0; idz < (m_Num[2]); idz++){
			a = idy*(m_Num[2]+1) + idz;
			b = (idy+1)*(m_Num[2]+1) + idz;
			c = idy*(m_Num[2]+1) + idz + 1;
			d = (idy+1)*(m_Num[2]+1) + idz + 1;

			faces.push_back(a);
			faces.push_back(b);
			faces.push_back(c);
			faces.push_back(b);
			faces.push_back(c);
			faces.push_back(d);
		}
	}
}

void CubicTetraMesh::GetSurface(std::vector<Node*>& nodes, std::vector<int>& faces){

	std::vector<Node*> nodes1, nodes2, nodes3, nodes4, nodes5, nodes6;
	std::vector<int> faces1, faces2, faces3, faces4, faces5, faces6;
	GetTopSurface(nodes1, faces1);
	GetBottomSurface(nodes2, faces2);
	GetFrontSurface(nodes3, faces3);
	GetBackSurface(nodes4, faces4);
	GetRightSurface(nodes5, faces5);
	GetLeftSurface(nodes6, faces6);

	int off_set = 0;

	faces = faces1;
	nodes = nodes1;
	off_set += nodes1.size(); 
	
	
	std::transform(faces2.begin(), faces2.end(), faces2.begin(), bind2nd(std::plus<int>(), off_set));
	faces.insert(faces.end(), faces2.begin(), faces2.end());
	nodes.insert(nodes.end(), nodes2.begin(), nodes2.end()); 
	off_set += nodes2.size(); 

	std::transform(faces3.begin(), faces3.end(), faces3.begin(), bind2nd(std::plus<int>(), off_set));
	faces.insert(faces.end(), faces3.begin(), faces3.end());
	nodes.insert(nodes.end(), nodes3.begin(), nodes3.end()); 
	off_set += nodes3.size();

	std::transform(faces4.begin(), faces4.end(), faces4.begin(), bind2nd(std::plus<int>(), off_set));
	faces.insert(faces.end(), faces4.begin(), faces4.end());
	nodes.insert(nodes.end(), nodes4.begin(), nodes4.end()); 
	off_set += nodes4.size();
	
	std::transform(faces5.begin(), faces5.end(), faces5.begin(), bind2nd(std::plus<int>(), off_set));
	faces.insert(faces.end(), faces5.begin(), faces5.end());
	nodes.insert(nodes.end(), nodes5.begin(), nodes5.end()); 
	off_set += nodes5.size();

	std::transform(faces6.begin(), faces6.end(), faces6.begin(), bind2nd(std::plus<int>(), off_set));
	faces.insert(faces.end(), faces6.begin(), faces6.end());
	nodes.insert(nodes.end(), nodes6.begin(), nodes6.end()); 
	
}

Object::Object(){

    m_type = NoType; 
    m_name = "No Name";
    m_world = NULL;
    
}

void Object::SetWorld(World *a_world){
	m_world = a_world; 
}

bool SurfaceMesh::LoadObjFile(char * filename){
		
		m_objdata = new objLoader();
		if(!m_objdata->load(filename)){
			std::cout<<"Loading Error!"<<std::endl;
			return false;
		}

		//generate mesh
		//create a Mesh Object for inside/outside test
	int num_vertices= m_objdata->vertexCount;
    int num_triangles= m_objdata->faceCount;
	//create a Mesh Object for drawing
	m_Num_v = num_vertices;
	m_Num_f = num_triangles;
    double *positions=new double[3*num_vertices];Node* temp_node;
	
	//apply scale and translation to the model
	double scale(2); Eigen::Vector3f translation(0,0,0);

   	for(std::size_t v=0;v<num_vertices;v++){
        positions[3*v] = scale*(translation[0] + m_objdata->vertexList[v]->e[0]);
        positions[3*v+1] = scale*(translation[1] + m_objdata->vertexList[v]->e[1]);
        positions[3*v+2] = scale*(translation[2] + m_objdata->vertexList[v]->e[2]);
		temp_node = new Node;
		temp_node->m_Position = Eigen::Vector3f(positions[3*v],positions[3*v+1],positions[3*v+2]);
		temp_node->m_Normal = Eigen::Vector3f(0,0,0);//initalize to 0, generate later
		m_Nodes.push_back(temp_node);
	}
	
    int *triangles=new int[3*num_triangles];
	obj_face *o; Triangle* temp_tria;

    for(std::size_t t=0;t<num_triangles;t++){
		o = m_objdata->faceList[t];
        triangles[3*t] = o->vertex_index[0];
        triangles[3*t+1] = o->vertex_index[1];
        triangles[3*t+2] = o->vertex_index[2];
		temp_tria = new Triangle;
		temp_tria->m_node_1 = m_Nodes[triangles[3*t]];
		temp_tria->m_node_2 = m_Nodes[triangles[3*t + 1]];
		temp_tria->m_node_3 = m_Nodes[triangles[3*t + 2]];
		m_Trias.push_back(temp_tria);
	}

	m_mesh_obj = construct_mesh_object(num_vertices,positions,num_triangles,triangles);
	
	//generate the spatial hash table for triangles
	std::cout<<"Generating Spatial Hash for the Model"<<std::endl;
	Eigen::Vector2f obj_bounding_x(10000,-10000),obj_bounding_y(10000,-10000),obj_bounding_z(10000,-10000);
	double tx, ty, tz;
	for(std::size_t v =0;v<num_vertices; v++){
		
		tx = m_Nodes[v]->m_Position[0];
		ty = m_Nodes[v]->m_Position[1];
		tz = m_Nodes[v]->m_Position[2];

		if(obj_bounding_x[0] > tx)
			obj_bounding_x[0] = tx;
		if(obj_bounding_x[1] < tx)
			obj_bounding_x[1] = tx;

		if(obj_bounding_y[0] > ty)
			obj_bounding_y[0] = ty;
		if(obj_bounding_y[1] < ty)
			obj_bounding_y[1] = ty;
		
		if(obj_bounding_z[0] > tz)
			obj_bounding_z[0] = tz;
		if(obj_bounding_z[1] < tz)
			obj_bounding_z[1] = tz;
	}//getting the bounding volum for the model;

	//creating a buffer zone
	obj_bounding_x += Eigen::Vector2f(-20,20);
	obj_bounding_y += Eigen::Vector2f(-20,20);
	obj_bounding_z += Eigen::Vector2f(-20,20);

	m_space_grid.m_res_x = 30;
	m_space_grid.m_res_y = 30;
	m_space_grid.m_res_z = 30;
	m_space_grid.m_dx = (obj_bounding_x[1] - obj_bounding_x[0])/m_space_grid.m_res_x;
	m_space_grid.m_dy = (obj_bounding_y[1] - obj_bounding_y[0])/m_space_grid.m_res_y;
	m_space_grid.m_dz = (obj_bounding_z[1] - obj_bounding_z[0])/m_space_grid.m_res_z;
	m_space_grid.m_start_x = obj_bounding_x[0];
	m_space_grid.m_start_y = obj_bounding_y[0];
	m_space_grid.m_start_z = obj_bounding_z[0];
	m_space_grid.m_end_x = obj_bounding_x[1];
	m_space_grid.m_end_y = obj_bounding_y[1];
	m_space_grid.m_end_z = obj_bounding_z[1];

	int cell_x_min, cell_x_max, cell_y_min, cell_y_max, cell_z_min, cell_z_max;
	Eigen::Vector2f current_tria_bounding_x,current_tria_bounding_y,current_tria_bounding_z;
	for(std::size_t t =0;t<num_triangles;t++){
		temp_tria = m_Trias[t];
#ifdef WIN32
		current_tria_bounding_x[0] = min(min(temp_tria->m_node_1->m_Position[0],
			temp_tria->m_node_2->m_Position[0]),temp_tria->m_node_3->m_Position[0]);
		current_tria_bounding_x[1] = max(max(temp_tria->m_node_1->m_Position[0],
			temp_tria->m_node_2->m_Position[0]),temp_tria->m_node_3->m_Position[0]);
		current_tria_bounding_y[0] = min(min(temp_tria->m_node_1->m_Position[1],
			temp_tria->m_node_2->m_Position[1]),temp_tria->m_node_3->m_Position[1]);
		current_tria_bounding_y[1] = max(max(temp_tria->m_node_1->m_Position[1],
			temp_tria->m_node_2->m_Position[1]),temp_tria->m_node_3->m_Position[1]);
		current_tria_bounding_z[0] = min(min(temp_tria->m_node_1->m_Position[2],
			temp_tria->m_node_2->m_Position[2]),temp_tria->m_node_3->m_Position[2]);
		current_tria_bounding_z[1] = max(max(temp_tria->m_node_1->m_Position[2],
			temp_tria->m_node_2->m_Position[2]),temp_tria->m_node_3->m_Position[2]);

#else
		current_tria_bounding_x[0] = std::min(std::min(temp_tria->m_node_1->m_Position[0],
			temp_tria->m_node_2->m_Position[0]),temp_tria->m_node_3->m_Position[0]);
		current_tria_bounding_x[1] = std::max(std::max(temp_tria->m_node_1->m_Position[0],
			temp_tria->m_node_2->m_Position[0]),temp_tria->m_node_3->m_Position[0]);
		current_tria_bounding_y[0] = std::min(std::min(temp_tria->m_node_1->m_Position[1],
			temp_tria->m_node_2->m_Position[1]),temp_tria->m_node_3->m_Position[1]);
		current_tria_bounding_y[1] = std::max(std::max(temp_tria->m_node_1->m_Position[1],
			temp_tria->m_node_2->m_Position[1]),temp_tria->m_node_3->m_Position[1]);
		current_tria_bounding_z[0] = std::min(std::min(temp_tria->m_node_1->m_Position[2],
			temp_tria->m_node_2->m_Position[2]),temp_tria->m_node_3->m_Position[2]);
		current_tria_bounding_z[1] = std::max(std::max(temp_tria->m_node_1->m_Position[2],
			temp_tria->m_node_2->m_Position[2]),temp_tria->m_node_3->m_Position[2]);
#endif

		cell_x_min = m_space_grid.QueryGridIDX(current_tria_bounding_x[0]);
		cell_x_max = m_space_grid.QueryGridIDX(current_tria_bounding_x[1]);
		
		cell_y_min = m_space_grid.QueryGridIDY(current_tria_bounding_y[0]);
		cell_y_max = m_space_grid.QueryGridIDY(current_tria_bounding_y[1]);
		
		cell_z_min = m_space_grid.QueryGridIDZ(current_tria_bounding_z[0]);
		cell_z_max = m_space_grid.QueryGridIDZ(current_tria_bounding_z[1]);

		int hash_key;
		for(int ix = cell_x_min; ix < cell_x_max + 1; ix++)
			for(int iy = cell_y_min; iy < cell_y_max + 1; iy++)
				for(int iz = cell_z_min; iz < cell_z_max + 1; iz++){
					hash_key = ix*m_space_grid.m_res_y*m_space_grid.m_res_z + iy*m_space_grid.m_res_z + iz;
					m_spatial_hash[hash_key].push_back(t);
				}
	}

	Eigen::Vector3f v1,v2;
	Eigen::Vector3f current_normal, temp_face_normal;
	
	for(int i = 0; i < m_Num_f; i++)
	{
		v1 = m_Trias[i]->m_node_2->m_Position - m_Trias[i]->m_node_1->m_Position;
		v2 = m_Trias[i]->m_node_3->m_Position - m_Trias[i]->m_node_1->m_Position;
		temp_face_normal = v1.cross(v2);
		temp_face_normal.normalize();
		m_Trias[i]->m_face_normal = temp_face_normal;
		m_Trias[i]->m_node_1->m_Normal += temp_face_normal;
		m_Trias[i]->m_node_2->m_Normal += temp_face_normal;
		m_Trias[i]->m_node_3->m_Normal += temp_face_normal;
	}
	for(int i = 0; i < m_Num_v; i++)
	{//normalize
		m_Nodes[i]->m_Normal.normalize();
	}

		return true;
}

bool SurfaceMesh::PointInsideMesh(Eigen::Vector3f point){
		double* p = new double[3];
		p[0] = point[0];p[1] = point[1];p[2] = point[2];
		return point_inside_mesh(p,m_mesh_obj);
	}

bool SurfaceMesh::ClosestTriangle(const Eigen::Vector3f& xyz, Triangle& triangle){
		
		//first determine hash key for position xyz
		int ix, iy,iz; int flat_id;
		std::vector<int> tria_ids;
		ix = m_space_grid.QueryGridIDX(xyz.x());
		iy = m_space_grid.QueryGridIDY(xyz.y());
		iz = m_space_grid.QueryGridIDZ(xyz.z());
		if(ix == -1||iy == -1||iz == -1){
			return false;
		
		}
	
		flat_id = ix*m_space_grid.m_res_y*m_space_grid.m_res_z + iy*m_space_grid.m_res_z + iz;
		tria_ids = m_spatial_hash[flat_id];
		if(tria_ids.size() == 0)//query neighboring boxes as well.
		{
			tria_ids.empty();
			std::vector<int> temp;
#ifdef WIN32
			for(int i = max(0, ix-1); i < min(ix + 2,m_space_grid.m_res_x); i++)
				for(int j = max(0, iy-1); j < min(iy + 2,m_space_grid.m_res_y); j++)
					for(int k = max(0, iz-1); k < min(iz + 2,m_space_grid.m_res_z); k++){
#else
			for(int i = std::max(0, ix-1); i < std::min(ix + 2,m_space_grid.m_res_x); i++)
				for(int j = std::max(0, iy-1); j < std::min(iy + 2,m_space_grid.m_res_y); j++)
					for(int k = std::max(0, iz-1); k < std::min(iz + 2,m_space_grid.m_res_z); k++){
#endif						
						if(i == ix && j==iy&&k==iz)
							continue;//skipp current cell
						flat_id = i*m_space_grid.m_res_y*m_space_grid.m_res_z + j*m_space_grid.m_res_z + k;
						temp = m_spatial_hash[flat_id];
						tria_ids.insert(tria_ids.end(),temp.begin(), temp.end());
					}
			if(tria_ids.size() == 0){
				return false;
			}
		}
	
		double smallest_dist_surface = 10000, temp_dist_surface;
		for(int i=0; i<tria_ids.size(); i++){
			temp_dist_surface = (xyz - (m_Trias[tria_ids[i]]->m_node_1->m_Position
				+m_Trias[tria_ids[i]]->m_node_2->m_Position + 
				m_Trias[tria_ids[i]]->m_node_3->m_Position)/3.0).norm();
			if(temp_dist_surface < smallest_dist_surface){
				smallest_dist_surface = temp_dist_surface;
				triangle = *m_Trias[tria_ids[i]];
			}
		}
		return true;
	
	}
