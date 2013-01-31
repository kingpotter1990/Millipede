//
//  Object.cpp
//  FVM3D_Rigid_coupling
//
//  Created by 静一 方 on 4/2/12.
//  Copyright 2012 UCLA. All rights reserved.
//

#include "Object.h"
#include "World.h"

Mesh3D::Mesh3D(Eigen::Vector3i a_num, Eigen::Vector3f a_size, Eigen::Vector3f a_position){
    Meshialize(a_num, a_size, a_position);
}

void Mesh3D::Meshialize(Eigen::Vector3i a_num, Eigen::Vector3f a_size, Eigen::Vector3f a_position){
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

std::vector<Node*> Mesh3D::GetTopNodes(){
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


void Mesh3D::GetTopSurface(std::vector<Node*>& nodes, std::vector<int>& faces){
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



std::vector<Node*> Mesh3D::GetBottomNodes(){
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


void Mesh3D::GetBottomSurface(std::vector<Node*>& nodes, std::vector<int>& faces){
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


std::vector<Node*> Mesh3D::GetFrontNodes(){
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


void Mesh3D::GetFrontSurface(std::vector<Node*>& nodes, std::vector<int>& faces){
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



std::vector<Node*> Mesh3D::GetBackNodes(){
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

void Mesh3D::GetBackSurface(std::vector<Node*>& nodes, std::vector<int>& faces){
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



std::vector<Node*> Mesh3D::GetLeftNodes(){
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

void Mesh3D::GetLeftSurface(std::vector<Node*>& nodes, std::vector<int>& faces){
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



std::vector<Node*> Mesh3D::GetRightNodes(){
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

void Mesh3D::GetRightSurface(std::vector<Node*>& nodes, std::vector<int>& faces){
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

void Mesh3D::GetSurface(std::vector<Node*>& nodes, std::vector<int>& faces){

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

