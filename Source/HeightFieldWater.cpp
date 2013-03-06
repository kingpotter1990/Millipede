//
//  HeightFieldWater.cpp
//
//  Created by Jingyi Fang on 3/1/13.
//  Copyright 2013 UCLA. All rights reserved.
//

#include "World.h"
#include "Sphere.h"
#include "HeightFieldWater.h"

HFWater::HFWater(Eigen::Vector2i res, double dx, double depth){
	m_res_x = res.x();
	m_res_z = res.y();
	m_size_x = m_res_x*dx;
	m_size_z = m_res_z*dx;
	m_dx = dx;
    m_depth = depth;
	m_height_data = new double[m_res_x*m_res_z];
	m_velocity_data = new double[m_res_x*m_res_z];
    m_normal_data = new Eigen::Vector3f[m_res_x*m_res_z];	
	m_NTrianglePoints = 6*(m_res_x-1)*(m_res_z-1);

	InitWave();
	InitDraw();
}

void HFWater::InitWave(){
	
	for(int i = 0; i < m_res_x*m_res_z; i++){
		m_height_data[i] = m_depth;//clear to 0
		m_velocity_data[i] = 0.0;
	}
	
	/*
		double hill_height_max = 115;
		double hill_center_x, hill_center_z, hill_height, hill_narrowness_x, hill_narrowness_z;
		double dev_x, dev_z;
		
		for(int i = 0; i <1 ; i++){	

			hill_center_x = (Util::getRand()-0.5)*m_size_x;
			hill_center_z = (Util::getRand()-0.5)*m_size_z;
			hill_height = Util::getRand()*(hill_height_max - 1) + 1;
			hill_narrowness_x = Util::getRand()*5 + 5; //20~40
			hill_narrowness_z = Util::getRand()*5 + 5; //20~40

			//add the hill to current height map
			for(int ix = 0; ix < m_res_x; ix++)
				for(int iz = 0; iz < m_res_z; iz++){
					dev_x = ix*m_dx - 0.5*m_size_x - hill_center_x;
					dev_z = iz*m_dx - 0.5*m_size_z - hill_center_z;
					m_height_data[ix*m_res_z + iz] += //guassian hill
						hill_height*exp(-dev_x*dev_x/(2*hill_narrowness_x*hill_narrowness_x)-dev_z*dev_z/(2*hill_narrowness_z*hill_narrowness_z));
				}
		}
		*/
	/*
		//normalize
		double temp_largest = m_height_data[0];
		double temp_smallest = m_height_data[0];
		for(int i = 0; i < m_res_x*m_res_z; i++)
		{
			if(m_height_data[i] > temp_largest)
				temp_largest = m_height_data[i];
			if(m_height_data[i] < temp_smallest)
				temp_smallest = m_height_data[i];
		}

		for(int i = 0; i < m_res_x*m_res_z; i++)
		{
			//eventually the range would be from 0~m_hill_height_max
			m_height_data[i] = hill_height_max*(m_height_data[i] - temp_smallest)/temp_largest;
		}

*/	


}

void HFWater::InitDraw(){
	
	// Initialize the data array on CPU
		m_TrianglePoints = new Eigen::Vector4f[m_NTrianglePoints];
		m_TPointNormals = new Eigen::Vector3f[m_NTrianglePoints];
		m_TriangleColors = new Eigen::Vector4f[m_NTrianglePoints];

	
    //Create the Vertex Array and Buffers, bind them
#ifdef __APPLE__
	glGenVertexArraysAPPLE(1, &m_vertexArrayObject);
	glBindVertexArrayAPPLE(m_vertexArrayObject);//use as current vertex array   
#else
	 glGenVertexArrays(1, &m_vertexArrayObject);
#endif
   
    glGenBuffers(1, &m_vertexBufferObject);//generate buffer for current vertex array
    
    //load and compile shaders on GPU, use current shader program
    m_shader = Util::InitShader( "vSmoothPhong.glsl", "fSmoothPhong.glsl" );
    glUseProgram(m_shader);
   
	
}

void HFWater::UpdateNormal(){
	//generate normal
	Eigen::Vector3f v1,v2;
	Eigen::Vector3f current_normal, temp_face_normal;
	
	for(int ix = 0; ix < m_res_x; ix++)
		for(int iz = 0; iz < m_res_z; iz++){
			current_normal *= 0.0;
			//upper left face normal
			
			if(ix > 0 && iz < m_res_z -1)
			{
				v1.x() = 0;
				v1.z() = m_dx;
				v1.y() = m_height_data[ix*(m_res_z) + iz + 1] - m_height_data[ix*(m_res_z) + iz];
				v1.normalize();

				v2.x() = -m_dx;
				v2.z() = 0;
				v2.y() = m_height_data[(ix-1)*(m_res_z) + iz] - m_height_data[ix*(m_res_z) + iz];
				v2.normalize();

				temp_face_normal = v1.cross(v2);
				current_normal += temp_face_normal;
			}
			//upper right face normal
			if(ix < m_res_x - 1 && iz < m_res_z -1)
			{
				v1.x() = 0;
				v1.z() = m_dx;
				v1.y() = m_height_data[ix*(m_res_z) + iz + 1] - m_height_data[ix*(m_res_z) + iz];
				v1.normalize();

				v2.x() = m_dx;
				v2.z() = 0;
				v2.y() = m_height_data[(ix+1)*(m_res_z) + iz] - m_height_data[ix*(m_res_z) + iz];
				v2.normalize();

				temp_face_normal = v2.cross(v1);
				current_normal += temp_face_normal;
			}
			//lower left face normal
			if(ix > 0 && iz > 0)
			{
				v1.x() = 0;
				v1.z() = -m_dx;
				v1.y() = m_height_data[ix*(m_res_z) + iz - 1] - m_height_data[ix*(m_res_z) + iz];
				v1.normalize();

				v2.x() = -m_dx;
				v2.z() = 0;
				v2.y() = m_height_data[(ix-1)*(m_res_z) + iz] - m_height_data[ix*(m_res_z) + iz];
				v2.normalize();

				temp_face_normal = v2.cross(v1);
				current_normal += temp_face_normal;
			}
			//lower right face normal
			if(ix < m_res_x - 1  && iz > 0)
			{
				v1.x() = 0;
				v1.z() = -m_dx;
				v1.y() = m_height_data[ix*(m_res_z) + iz - 1] - m_height_data[ix*(m_res_z) + iz];
				v1.normalize();

				v2.x() = m_dx;
				v2.z() = 0;
				v2.y() = m_height_data[(ix+1)*(m_res_z) + iz] - m_height_data[ix*(m_res_z) + iz];
				v2.normalize();

				temp_face_normal = v1.cross(v2);
				current_normal += temp_face_normal;
			}
			
			m_normal_data[ix*(m_res_z) + iz] = -current_normal.normalized();//average
		}

}

void HFWater::Draw(int type, const Camera& camera, const Light& light){
	UpdateDraw();
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
	GLuint position;
    GLuint color;
    GLuint normal;
    
    //send the updated data to buffer
    glBufferData(GL_ARRAY_BUFFER, (sizeof(m_TrianglePoints[0]) + sizeof(m_TriangleColors[0]) + sizeof(m_TPointNormals[0]))*m_NTrianglePoints, NULL, GL_STATIC_DRAW);//send data to current buffer
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_TrianglePoints[0])*m_NTrianglePoints, m_TrianglePoints);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(m_TrianglePoints[0])*m_NTrianglePoints, sizeof(m_TriangleColors[0])*m_NTrianglePoints, m_TriangleColors);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(m_TriangleColors[0])*m_NTrianglePoints+sizeof(m_TrianglePoints[0])*m_NTrianglePoints,sizeof(m_TPointNormals[0])*m_NTrianglePoints, m_TPointNormals);
    
    position = glGetAttribLocation( m_shader, "vPosition" );
    glEnableVertexAttribArray( position );
    glVertexAttribPointer(position, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0)); 
    
    color = glGetAttribLocation(m_shader, "vColor");
    glEnableVertexAttribArray(color);
    glVertexAttribPointer(color, 4, GL_FLOAT, GL_FALSE,0, BUFFER_OFFSET(sizeof(m_TrianglePoints[0])*m_NTrianglePoints));
    
    normal = glGetAttribLocation(m_shader, "vNormal");
    glEnableVertexAttribArray(normal);
    glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE,0, BUFFER_OFFSET((sizeof(m_TriangleColors[0])+sizeof(m_TrianglePoints[0]))*m_NTrianglePoints));
    
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


}

void HFWater::UpdateAll(double dt){

	double up, down, left, right, center, force;
	double c_2 = 100;
	int index; double m_dx_2 = m_dx*m_dx;
	for(int ix= 0; ix< m_res_x; ix++)
		for(int iz = 0; iz< m_res_z; iz++){
			index = ix*m_res_z + iz;
			center = m_height_data[index];
			if(ix == 0)
				up = m_height_data[index];
			else
				up = m_height_data[index - m_res_z];

			if(ix == m_res_x - 1)
				down = m_height_data[index];
			else
				down = m_height_data[index + m_res_z];

			if(iz == 0)
				left = m_height_data[index];
			else
				left = m_height_data[index - 1];

			if(iz == m_res_z - 1)
				right = m_height_data[index];
			else
				right = m_height_data[index + 1];

			force = c_2*(up + left +right +down - 4*center)/(m_dx_2);
			//add damping force
			force -= 0.1*m_velocity_data[index];

			m_velocity_data[index] += force*dt;

		}//udpate velocity
	//update height map
	for(int ix= 0; ix< m_res_x; ix++)
		for(int iz = 0; iz< m_res_z; iz++){
			index = ix*m_res_z + iz;
			m_height_data[index] += m_velocity_data[index]*dt;
		}
	
	//Handle Collision with spheres
	double tdx, tdz, tr;
	int sx,ex, sz, ez;
	Sphere* current_sphere;Eigen::Vector3f temp_point; Eigen::Vector3f trans_point;double dif;
	
	for(int i = 0; i < m_spheres.size(); i++){
			current_sphere = m_spheres[i];
			if(current_sphere->m_Center[1] > current_sphere->m_Size[1] + 0.2)
				continue;//this is a hack
			assert(current_sphere->m_Size[0] == current_sphere->m_Size[1] && current_sphere->m_Size[2] == current_sphere->m_Size[1]);
			//only check the box of water that is in sphere's y direction shadow
			sx = max((current_sphere->m_Center[0] - current_sphere->m_Size[0] +  m_size_x*0.5)/m_dx - 2, 0);
			sz = max((current_sphere->m_Center[2] - current_sphere->m_Size[2] +  m_size_z*0.5)/m_dx - 2, 0);

			ex = min((current_sphere->m_Center[0] + current_sphere->m_Size[0] + m_size_x*0.5)/m_dx + 2, m_res_x);
			ez = min((current_sphere->m_Center[2] + current_sphere->m_Size[2] + m_size_z*0.5)/m_dx + 2, m_res_z);

			for(int ix= sx; ix< ex; ix++)
				for(int iz = sz; iz< ez; iz++){
					//intersection of sphere and water column
					tdx = current_sphere->m_Center[0] - (ix*m_dx - 0.5*m_size_x); 
					tdz = current_sphere->m_Center[2] - (iz*m_dx - 0.5*m_size_z);
					tr = current_sphere->m_Size[0]*current_sphere->m_Size[0] - tdx*tdx - tdz*tdz;
					if(tr < 0)
						continue;
					dif = sqrt(tr) - (current_sphere->m_Center[1] - m_height_data[ix*m_res_z + iz]);
					if(dif > 0)
					{
						m_height_data[ix*m_res_z+iz] -= dif;
						m_velocity_data[ix*m_res_z+iz] = 0;
						//distribute the dif term
						assert(ix > 1 &&ix < m_res_x - 2&&iz > 1&&iz < m_res_z-2);
						
						m_height_data[ix*m_res_z+iz -1] += dif/10;
						m_velocity_data[ix*m_res_z+iz -1] = 0;
						m_height_data[ix*m_res_z+iz +1] += dif/10;
						m_velocity_data[ix*m_res_z+iz +1] = 0;
						m_height_data[(ix+1)*m_res_z + iz] += dif/10;
						m_velocity_data[(ix+1)*m_res_z+iz] = 0;
						m_height_data[(ix-1)*m_res_z + iz] += dif/10;
						m_velocity_data[(ix-1)*m_res_z+iz -1] = 0;
						
						}
						
				}
	}
}

void HFWater::UpdateDraw(){
	
		UpdateNormal();

		int box_id;
		Eigen::Vector4f a,b,c,d;
		Eigen::Vector3f na,nb,nc,nd;
		Eigen::Vector4f red(1,0,0,1);
		Eigen::Vector4f green(0,1,0,1); double ratio;
		double va, vb, vc,vd;
		for(int ix = 0; ix < m_res_x - 1 ; ix++)
			for(int iz = 0; iz < m_res_z - 1 ; iz++){
				
			box_id = ix*(m_res_z -1 ) + iz;
		
			a.x() = m_dx*ix - 0.5*m_size_x;a.z() = m_dx*iz - 0.5*m_size_z;a.y() = m_height_data[ix*m_res_z + iz];a.w() = 1.0;
			b.x() = m_dx*(ix+1) - 0.5*m_size_x;b.z() = m_dx*iz - 0.5*m_size_z;b.y() = m_height_data[(ix+1)*m_res_z + iz];b.w() = 1.0;
			c.x() = m_dx*(ix+1) - 0.5*m_size_x;c.z() = m_dx*(iz+1) - 0.5*m_size_z;c.y() = m_height_data[(ix+1)*m_res_z + iz + 1];c.w() = 1.0;
			d.x() = m_dx*ix - 0.5*m_size_x;d.z() = m_dx*(iz+1) - 0.5*m_size_z;d.y() = m_height_data[ix*m_res_z + iz + 1];d.w() = 1.0;
		
			m_TrianglePoints[6*box_id] = a;
			m_TrianglePoints[6*box_id + 1] = b;
			m_TrianglePoints[6*box_id + 2] = c;
			m_TrianglePoints[6*box_id + 3] = c;
			m_TrianglePoints[6*box_id + 4] = d;
			m_TrianglePoints[6*box_id + 5] = a;

			na = m_normal_data[ix*m_res_z + iz];
			nb = m_normal_data[(ix+1)*m_res_z + iz];
			nc = m_normal_data[(ix+1)*m_res_z + iz+1];
			nd = m_normal_data[ix*m_res_z + iz+1];
		
			va = m_velocity_data[ix*m_res_z + iz];
			vb = m_velocity_data[(ix+1)*m_res_z + iz];
			vc = m_velocity_data[(ix+1)*m_res_z + iz+1];
			vd = m_velocity_data[ix*m_res_z + iz+1];

			m_TPointNormals[6*box_id] = na;
			m_TPointNormals[6*box_id + 1] = nb;
			m_TPointNormals[6*box_id + 2] = nc;
			m_TPointNormals[6*box_id + 3] = nc;
			m_TPointNormals[6*box_id + 4] = nd;
			m_TPointNormals[6*box_id + 5] = na;
			
			m_TriangleColors[6*box_id] = fabs(va)*red + green;
			m_TriangleColors[6*box_id + 1] = fabs(vb)*red + green;
			m_TriangleColors[6*box_id + 2] = fabs(vc)*red + green; 
			m_TriangleColors[6*box_id + 3] = fabs(vc)*red + green; 
			m_TriangleColors[6*box_id + 4] = fabs(vd)*red + green; 
			m_TriangleColors[6*box_id + 5] = fabs(va)*red + green; 

			
			}	

}