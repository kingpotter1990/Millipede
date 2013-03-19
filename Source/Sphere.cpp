//
//  Sphere.cpp
//  FEMCloth2D
//
//  Created by 静一 方 on 11/22/11.
//  Copyright 2011 UCLA. All rights reserved.
//

#include "Sphere.h"
#include "World.h"

Sphere::Sphere(){
    m_type = TypeSphere;
	m_count = 0;
    Init(Eigen::Vector3f(0.0,0.0,0.0), Eigen::Vector3f(1.0,1.0,1.0),Eigen::Vector3f(1.0,0.0,0.0));
}

Sphere::Sphere(Eigen::Vector3f center, Eigen::Vector3f scale, Eigen::Vector3f color){
	 m_type = TypeSphere;
	m_count = 0;
    Init(center, scale, color);
}

void Sphere::UpdateMatrix(){
	
	m_Trans.setIdentity();
	m_Trans.translate(m_Center);
    m_Trans.scale(m_Size);
    
    m_TransBack = m_Trans.inverse();

}

void Sphere::MoveUp(double amount){
	m_Center[1] += amount;
	UpdateMatrix();
}

void Sphere::MoveDown(double amount){
	m_Center[1] -= amount;
	UpdateMatrix();
}

void Sphere::Init(Eigen::Vector3f center, Eigen::Vector3f scale, Eigen::Vector3f color){
    m_Center = center;
    m_Size = scale;
	m_Color = color;
   
	m_Trans.setIdentity();
	m_Trans.translate(m_Center);
    m_Trans.scale(m_Size);
    
    m_TransBack = m_Trans.inverse();

	InitDraw();
}

Eigen::Vector4f Sphere::Unit(const Eigen::Vector4f &p)
{
    Eigen::Vector4f c;
    double d=0.0;
    for(int i=0; i<3; i++) d+=p[i]*p[i];
    d=sqrt(d);
    if(d > 0.0) for(int i=0; i<3; i++) c[i] = p[i]/d;
    c[3] = 1.0;
    return c;
}

void Sphere::DividTriangle(const Eigen::Vector4f& a, const Eigen::Vector4f& b, const Eigen::Vector4f& c, int n){
	Eigen::Vector4f v1, v2, v3;
    if(n>0)
    {
        v1 = Unit(a + b);
        v2 = Unit(a + c);
        v3 = Unit(b + c);   
        DividTriangle(a , v2, v1, n-1);
        DividTriangle(c , v3, v2, n-1);
        DividTriangle(b , v1, v3, n-1);
        DividTriangle(v1, v2, v3, n-1);
    }
	else{
		m_Vertices[m_count] = a;
		m_Normals[m_count] = a.head(3);
		m_count++;
		m_Vertices[m_count] = b;
		m_Normals[m_count] = b.head(3);
		m_count++;
		m_Vertices[m_count] = c;
		m_Normals[m_count] = c.head(3);
		m_count++;
	};
}

void Sphere::GenerateSphere(){
	m_count = 0;
	Eigen::Vector4f v[4];
    v[0] = Eigen::Vector4f(0.0, 0.0, 1.0, 1.0);
    v[1] = Eigen::Vector4f(0.0, 0.942809, -0.333333, 1.0);
    v[2] = Eigen::Vector4f(-0.816497, -0.471405, -0.333333, 1.0);
    v[3] = Eigen::Vector4f(0.816497, -0.471405, -0.333333, 1.0);
	
	DividTriangle(v[0], v[1], v[2], 3);
    DividTriangle(v[3], v[2], v[1], 3);
    DividTriangle(v[0], v[3], v[1], 3);
    DividTriangle(v[0], v[3], v[2], 3);

}

void Sphere::InitDraw(){
    // Initialize the data array on CPU
    m_Vertices = new Eigen::Vector4f[16*256*3];
	m_Normals = new Eigen::Vector3f[16*256*3];
    m_Index = 0;
   
	GenerateSphere();
    
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
    glBufferData(GL_ARRAY_BUFFER, (sizeof(m_Vertices[0]) + sizeof(m_Normals[0]))*16*256*3, NULL, GL_STATIC_DRAW);//send data to current buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_Vertices[0])*16*256*3, m_Vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(m_Vertices[0])*16*256*3, sizeof(m_Normals[0])*16*256*3, m_Normals);
    
    
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
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE,0, BUFFER_OFFSET(sizeof(m_Vertices[0])*16*256*3));
    
}

void Sphere::Draw(int type, const Camera& camera, const Light& light){

    //Get new position of the Sphere and update the model view matrix
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
    glUniformMatrix4fv( world2camera, 1, GL_FALSE, cMw.data() );
    glUniformMatrix4fv( projection, 1, GL_FALSE, proj.data() );
	glUniform4fv(camera_position, 1, camera.m_position.data());
    glUniform4fv(light_position, 1, light.m_position.data());

	Eigen::Vector4f l_color(m_Color[0],m_Color[1],m_Color[2],1.0);
  	glUniform4fv(color,1,l_color.data());

	switch (type) {
        case DRAW_MESH:
            glDrawArrays(GL_LINES, 0, 16*256*3);
            break;
        case DRAW_PHONG:
            glDrawArrays(GL_TRIANGLES, 0, 16*256*3);
            break;
    }
}



