//
//  World.h
//  FEMCloth2D-FEM
//
//  Created by Jingyi Fang on 2/15/12.
//  Copyright 2012 UCLA. All rights reserved.
//

#ifndef FEMCloth2D_FEM_World_h
#define FEMCloth2D_FEM_World_h

#include "Utility.h"
#include "Object.h"
#include "Camera.h"
#include "Light.h"
#include <vector>

#ifndef DRAW_TYPES_
#define DRAW_MESH 1
#define DRAW_PHONG 2
#define DRAW_TEXTURE 3
#endif
class World{
    //for now World hold a bounch of objects and can take one point, return a collision force
public:
    World(double hardness);
    ~World();
    void Add_Object(Object *new_object);
    void Remove_Object(std::string object_name);
	void Clear();
	void Draw(int type, const Camera& camera,const Light& light);
	void Update(double dt);

    std::vector<Object*> List_of_Object;//intially point to a dummy node
	double m_hardness;
	
	EIGEN_MAKE_ALIGNED_OPERATOR_NEW
};

#endif
