
//  Cube.cpp
//  FEMCloth2D
//
//  Created by 静一 方 on 11/22/11.
//  Copyright 2011 UCLA. All rights reserved.
//

#include "Cube.h"
#include "World.h"
#include "Drawer.h"

Cube::Cube(){
    m_type = TypeCube;
    Init(Eigen::Vector3f(0.0,0.0,0.0),Eigen::Vector3f(1.0,1.0,1.0),Eigen::Vector3f(1.0,0.0,0.0));
}

Cube::Cube(Eigen::Vector3f center, Eigen::Vector3f scale, Eigen::Vector3f color){
	 m_type = TypeCube;
    Init(center, scale, color);
}

void Cube::Init(Eigen::Vector3f center, Eigen::Vector3f scale, Eigen::Vector3f color){

    m_Center = center;
    m_Size = scale;
	m_Color = color;
    
	m_Trans.setIdentity();
	m_Trans.translate(m_Center);
    m_Trans.scale(m_Size);
    
    m_TransBack = m_Trans.inverse();

	InitDraw();
}

void Cube::GenFace(int colorid, int a, int b, int c, int d )
{//generate two triangles for each face and assign colors to the vertices
   
	//face normal
	Eigen::Vector3f temp1 = m_Points[a].head(3) - m_Points[b].head(3);
	Eigen::Vector3f temp2 = m_Points[c].head(3) - m_Points[b].head(3);
	Eigen::Vector3f temp_norm = temp2.cross(temp1);
	//first triangle
    m_Vertices[m_Index] = m_Points[a];  m_Normals[m_Index] = temp_norm; m_Index++;
    m_Vertices[m_Index] = m_Points[b];  m_Normals[m_Index] = temp_norm; m_Index++; 
	m_Vertices[m_Index] = m_Points[c];  m_Normals[m_Index] = temp_norm; m_Index++;
	//second triangle
	m_Vertices[m_Index] = m_Points[a]; m_Normals[m_Index] = temp_norm; m_Index++; 
    m_Vertices[m_Index] = m_Points[c]; m_Normals[m_Index] = temp_norm; m_Index++;
    m_Vertices[m_Index] = m_Points[d]; m_Normals[m_Index] = temp_norm; m_Index++;
	
}

void Cube::InitDraw(){

    // Initialize the data array on CPU

    m_Points[0] = Eigen::Vector4f( -0.5, -0.5,  0.5, 1.0 );
    m_Points[1] = Eigen::Vector4f( -0.5,  0.5,  0.5, 1.0 );
    m_Points[2] = Eigen::Vector4f(  0.5,  0.5,  0.5, 1.0 );
    m_Points[3] = Eigen::Vector4f(  0.5, -0.5,  0.5, 1.0 );
    m_Points[4] = Eigen::Vector4f( -0.5, -0.5, -0.5, 1.0 );
    m_Points[5] = Eigen::Vector4f( -0.5,  0.5, -0.5, 1.0 );
    m_Points[6] = Eigen::Vector4f(  0.5,  0.5, -0.5, 1.0 );
    m_Points[7] = Eigen::Vector4f(  0.5, -0.5, -0.5, 1.0 );
    
    m_Vertices = new Eigen::Vector4f[36];
	m_Normals = new Eigen::Vector3f[36];
    m_Index = 0;
    
    //generate the 6 faces with distinct colors
    GenFace( 5, 1, 0, 3, 2 );
    GenFace( 1, 2, 3, 7, 6 );
    GenFace( 2, 3, 0, 4, 7 );
    GenFace( 3, 6, 5, 1, 2 );
    GenFace( 4, 4, 5, 6, 7 );
    GenFace( 0, 5, 4, 0, 1 );

    //Create the Vertex Array and Buffers, bind them
#ifdef __APPLE__
    glGenVertexArraysAPPLE(1, &m_vertexArrayObject);
    glBindVertexArrayAPPLE(m_vertexArrayObject);//use as current vertex array
#else
	glGenVertexArrays(1, &m_vertexArrayObject);
    glBindVertexArray(m_vertexArrayObject);//use as current vertex array
#endif
    glGenBuffers(1, &m_vertexBufferObject);//generate buffer for current vertex array
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);//use as current buffer

     //Send data from CPU to GPU
    glBufferData(GL_ARRAY_BUFFER, (sizeof(m_Vertices[0]) + sizeof(m_Normals[0]))*36, NULL, GL_STATIC_DRAW);//send data to current buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_Vertices[0])*36, m_Vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(m_Vertices[0])*36, sizeof(m_Normals[0])*36, m_Normals);
    
    
    //load and compile shaders on GPU, use current shader program
    m_shader = Util::InitShader( "vPhong.glsl", "fPhong.glsl" );
    glUseProgram(m_shader);
    
    
    // Link the Shader with the buffer data
    // initialize the vertex position attribute from the vertex shader
    GLuint position = glGetAttribLocation( m_shader, "vPosition" );
    glEnableVertexAttribArray( position );
    glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); 

	GLuint normal = glGetAttribLocation(m_shader, "vNormal");
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE,0, BUFFER_OFFSET(sizeof(m_Vertices[0])*36));
    
}

void Cube::Draw(int type, const Camera& camera,const Light& light){
	
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
  
	GLuint camera_position = glGetUniformLocation(m_shader, "cameraPosition");
    GLuint light_position = glGetUniformLocation(m_shader, "lightPosition");
	GLuint color = glGetUniformLocation(m_shader, "Color");

    GLuint object2world = glGetUniformLocation(m_shader, "wMo");
    GLuint world2camera = glGetUniformLocation(m_shader, "cMw"); 
	GLuint projection = glGetUniformLocation(m_shader, "proj");

    wMo = m_Trans;

    proj = Util::Perspective( camera.m_fovy, camera.m_aspect, camera.m_znear, camera.m_zfar );
	cMw = camera.m_cMw;//Angel::LookAt(camera.position,camera.lookat, camera.up );
 
    glUniformMatrix4fv( object2world , 1, GL_FALSE, wMo.data() );
    glUniformMatrix4fv( world2camera, 1, GL_FALSE, cMw.data());
    glUniformMatrix4fv( projection, 1, GL_FALSE, proj.data());
	glUniform4fv(camera_position, 1, camera.m_position.data());
    glUniform4fv(light_position, 1, light.m_position.data());
	Eigen::Vector4f l_color(m_Color[0],m_Color[1],m_Color[2],1.0);
  	glUniform4fv(color,1,l_color.data());
	
	switch (type) {
        case DRAW_MESH:
            glDrawArrays(GL_LINES, 0, 36);
            break;
        case DRAW_PHONG:
            glDrawArrays(GL_TRIANGLES, 0, 36);
            break;
    }
    

}

void Cube::Output2File(std::ofstream* filestream){
	

	g_Drawer->SetIdentity();
	g_Drawer->Scale(Eigen::Vector3f(0.99,1,1));

	(*filestream)<<"//BEGIN CUBE"<<std::endl;
	(*filestream)<<"mesh2{"<<std::endl;

	(*filestream)<<"vertex_vectors{"<<std::endl;
	(*filestream)<<"8,"<<std::endl;
	Eigen::Vector4f current_vertex;
	for(int i =0 ; i < 7; i++){
		current_vertex = m_Trans*g_Drawer->m_transformation*m_Points[i];
		(*filestream)<<"<"<<current_vertex[0]<<","<<current_vertex[1]<<","<<current_vertex[2]<<">,"<<std::endl;
	}
        current_vertex = m_Trans*g_Drawer->m_transformation*m_Points[7];
        (*filestream)<<"<"<<current_vertex[0]<<","<<current_vertex[1]<<","<<current_vertex[2]<<">"<<std::endl;
	(*filestream)<<"}"<<std::endl; // end vertex_vectors

	(*filestream)<<"face_indices{"<<std::endl;
	(*filestream)<<"12,"<<std::endl;
        (*filestream)<<"<0,2,1>,<0,3,2>,<4,5,6>,<4,6,7>,<0,1,5>,<0,5,4>,<3,6,2>,<3,7,6>,<1,2,6>,<1,6,5>,<0,7,3>,<0,4,7>"<<std::endl;
	(*filestream)<<"}"<<std::endl; // end face_indices

	(*filestream)<<"}"<<std::endl; // end mesh2
	(*filestream)<<"//END CUBE"<<std::endl;
}

