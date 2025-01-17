//
//  Cylinder.cpp
//  FEMCloth2D
//
//  Created by Jingyi Fang on 3/6/2012.
//  Copyright 2012 UCLA. All rights reserved.
//

#include "Cylinder.h"
#include "World.h"

Cylinder::Cylinder(){
    m_type = TypeCylinder;
    Init(Eigen::Vector3f(0.0,0.0,0.0),Eigen::Vector3f(1.0,1.0,1.0),Eigen::Vector3f(1.0,0.0,0.0));
}

Cylinder::Cylinder(Eigen::Vector3f center, Eigen::Vector3f scale, Eigen::Vector3f color){
	 m_type = TypeCylinder;
     Init(center, scale, color);
}


void Cylinder::makeCircle()
{
    for (int i = 0; i < 64; i++)
    {
        float a = i * 2.0f * M_PI / 64;
        m_CirclePoints[i] = 0.5*Eigen::Vector2f(cosf(a), sinf(a));
    }
}


void Cylinder::makeConeWall(double z1, double z2, int dir)
{
	Eigen::Vector3f n;
    for (int i = 0; i < 64; i++)
    {
        Eigen::Vector3f p1(m_CirclePoints[i][0], m_CirclePoints[i][1], z1);
        Eigen::Vector3f p2(0.0f, 0.0f, z2);
        Eigen::Vector3f p3(m_CirclePoints[(i+1)%64][0], m_CirclePoints[(i+1)%64][1], z1);
        if (dir == -1)
        {
            Eigen::Vector3f temp = p1;
            p1 = p3;
            p3 = temp;
        }
        n = (p1-p2).cross(p3-p2);
		
        m_Vertices[m_Index] = Eigen::Vector4f(p1[0],p1[1],p1[2],1); m_Normals[m_Index] = n; m_Index++;
        m_Vertices[m_Index] = Eigen::Vector4f(p2[0],p2[1],p2[2],1); m_Normals[m_Index] = n; m_Index++;
        m_Vertices[m_Index] = Eigen::Vector4f(p3[0],p3[1],p3[2],1); m_Normals[m_Index] = n; m_Index++;
    }

}

void Cylinder::GenerateCylinder()
{
    makeCircle();
    m_Index = 0;
    makeConeWall(0.5f, 0.5f, 1);
    makeConeWall(-0.5f, -0.5f, -1);
    
    for (int i = 0; i < 64; i++)
    {
        int i2 = (i+1)%64;
        Eigen::Vector3f  p1(m_CirclePoints[i2][0], m_CirclePoints[i2][1], -0.5);
        Eigen::Vector3f  p2(m_CirclePoints[i2][0], m_CirclePoints[i2][1], 0.5);
        Eigen::Vector3f  p3(m_CirclePoints[i][0],  m_CirclePoints[i][1],  0.5);
        //point3 n = cross(p3-p2, p1-p2);
        m_Vertices[m_Index] = Eigen::Vector4f(p1[0],p1[1],p1[2],1); m_Normals[m_Index] = Eigen::Vector3f(p1[0], p1[1], 0.0f); m_Index++;
        m_Vertices[m_Index] = Eigen::Vector4f(p2[0],p2[1],p2[2],1); m_Normals[m_Index] = Eigen::Vector3f(p2[0], p2[1], 0.0f); m_Index++;
        m_Vertices[m_Index] = Eigen::Vector4f(p3[0],p3[1],p3[2],1); m_Normals[m_Index] = Eigen::Vector3f(p3[0], p3[1], 0.0f); m_Index++;
        p1 = Eigen::Vector3f(m_CirclePoints[i2][0], m_CirclePoints[i2][1], -0.5);
        p2 = Eigen::Vector3f(m_CirclePoints[i][0],  m_CirclePoints[i][1],  0.5);
        p3 = Eigen::Vector3f(m_CirclePoints[i][0],  m_CirclePoints[i][1],  -0.5);
        //n = cross(p3-p2, p1-p2);
        m_Vertices[m_Index] = Eigen::Vector4f(p1[0],p1[1],p1[2],1); m_Normals[m_Index] = Eigen::Vector3f(p1[0], p1[1], 0.0f); m_Index++;
        m_Vertices[m_Index] = Eigen::Vector4f(p2[0],p2[1],p2[2],1); m_Normals[m_Index] = Eigen::Vector3f(p2[0], p2[1], 0.0f); m_Index++;
        m_Vertices[m_Index] = Eigen::Vector4f(p3[0],p3[1],p3[2],1); m_Normals[m_Index] = Eigen::Vector3f(p3[0], p3[1], 0.0f); m_Index++;
    }
    
}

void Cylinder::Init(Eigen::Vector3f center,Eigen::Vector3f size, Eigen::Vector3f color){
	
	m_Center = center;
    m_Size = size;
	m_Color = color;

	m_Trans.setIdentity();
	m_Trans.translate(m_Center);
    m_Trans.scale(m_Size);
    
    m_TransBack = m_Trans.inverse();


	InitDraw();
}


void Cylinder::InitDraw(){
    // Initialize the data array on CPU
    m_Vertices = new Eigen::Vector4f[64*12];//64 is the number of division
	m_Normals = new Eigen::Vector3f[64*12];//6 is the number of verteces for each division(two triangle)
	m_CirclePoints = new Eigen::Vector2f[64];
	
	m_Index = 0;
	GenerateCylinder();
    
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
    glBufferData(GL_ARRAY_BUFFER, (sizeof(m_Vertices[0]) + sizeof(m_Normals[0]))*64*12, NULL, GL_STATIC_DRAW);//send data to current buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_Vertices[0])*64*12, m_Vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(m_Vertices[0])*64*12, sizeof(m_Normals[0])*64*12, m_Normals);
    
    
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
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE,0, BUFFER_OFFSET(sizeof(m_Vertices[0])*64*12));
    
}

void Cylinder::Draw(int type, const Camera& camera, const Light& light){
    //Get new position of the Cylinder and update the model view matrix
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
	GLuint object_color = glGetUniformLocation(m_shader, "Color");

    GLuint object2world = glGetUniformLocation(m_shader, "wMo");
    GLuint world2camera = glGetUniformLocation(m_shader, "cMw"); 
	GLuint projection = glGetUniformLocation(m_shader, "proj");
   
    wMo = m_Trans;

    proj = Util::Perspective( camera.m_fovy, camera.m_aspect, camera.m_znear, camera.m_zfar );

	//the world to camera matrix is read from camera
	cMw = camera.m_cMw;
    
    glUniformMatrix4fv( object2world , 1, GL_FALSE, wMo.data() );
    glUniformMatrix4fv( world2camera, 1, GL_FALSE, cMw.data() );
    glUniformMatrix4fv( projection, 1, GL_FALSE, proj.data() );
	glUniform4fv(camera_position, 1, camera.m_position.data());
    glUniform4fv(light_position, 1, light.m_position.data());
	Eigen::Vector4f l_color(m_Color[0],m_Color[1],m_Color[2],1.0);
	glUniform4fv(object_color,1,l_color.data());

    switch (type) {
        case DRAW_MESH:
            glDrawArrays(GL_LINES, 0, 64*12);
            break;
        case DRAW_PHONG:
            glDrawArrays(GL_TRIANGLES, 0, 64*12);
            break;
    }
}




