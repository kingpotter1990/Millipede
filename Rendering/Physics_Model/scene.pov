//--------------------------------------------------------------------------
// Millipede scene
//--------------------------------------------------------------------------

#version 3.7;
global_settings{
  assumed_gamma 1.0
  photons {    spacing 0.01 }
  max_trace_level 20}
#default{ finish{ ambient 0.1 diffuse 0.9 conserve_energy}}

//--------------------------------------------------------------------------
#include "colors.inc"
#include "textures.inc"
#include "glass.inc"
#include "metals.inc"
#include "golds.inc"
#include "stones.inc"
#include "woods.inc"
#include "shapes.inc"
#include "shapes2.inc"
#include "functions.inc"
#include "math.inc"
#include "transforms.inc"
//--------------------------------------------------------------------------

#declare Camera_Position = < 8.00, 0,-15.00> ;
#declare Camera_Look_At  = < 0.00, 0.00,  0.00> ;
#declare Camera_Angle    =  65 ;

camera{ location Camera_Position
  right    x*image_width/image_height
  angle    Camera_Angle
  look_at  Camera_Look_At
}

//--------------------------------------------------------------------------
#declare RasterScale = 1.2;
#declare RasterHalfLine  = 0.04;
#declare RasterHalfLineZ = 0.04;

#macro Raster1(RScale, HLine)
       pigment{ gradient x scale RScale
                color_map{[0.000   color rgbt<1,1,1,0>*0.9]
                          [0+HLine color rgbt<1,1,1,0>*0.9]
                          [0+HLine color rgbt<1,1,1,1>*0.9 ]
                          [1-HLine color rgbt<1,1,1,1>*0.9]
                          [1-HLine color rgbt<1,1,1,0>*0.9]
                          [1.000   color rgbt<1,1,1,0>*0.9]} }
#end
#macro Raster2(RScale, HLine)
         pigment{ gradient x scale RScale
                color_map{[0.000   color rgbt<1,1,1,0>*0.1]
                          [0+HLine color rgbt<1,1,1,0>*0.5]
                          [0+HLine color rgbt<1,1,1,1>*0.5]
                          [1-HLine color rgbt<1,1,1,1>*0.5]
                          [1-HLine color rgbt<1,1,1,0>*0.5]
                          [1.000   color rgbt<1,1,1,0>*0.1]} }

#end

plane { <0,1,0>, -2
  texture { pigment{color White*1.1}
    finish {ambient 0.1 diffuse 0.9}}
  texture { Raster2(RasterScale,RasterHalfLine ) rotate<0,0,0> }
  texture { Raster2(RasterScale,RasterHalfLineZ) rotate<0,90,0>}
  
  finish{
    reflection 0.3
  }
}


plane{<0,1,0>,1 hollow  // 
      
        texture{ pigment {color rgb<0.1,0.35,0.8>*0.8}
                          finish {ambient 1  diffuse 0}
               } // end texture 1

        texture{ pigment{ bozo turbulence 0.75
                          octaves 6  omega 0.7 lambda 2 
                          color_map {
                          [0.0  color rgb <0.95, 0.95, 0.95> ]
                          [0.05  color rgb <1, 1, 1>*1.25 ]
                          [0.15 color rgb <0.85, 0.85, 0.85> ]
                          [0.55 color rgbt <1, 1, 1, 1>*1 ]
                          [1.0 color rgbt <1, 1, 1, 1>*1 ]
                          } // end color_map 
                         translate< 3, 0,-1>
                         scale <0.3, 0.4, 0.2>*50
                        } // end pigment
                 finish {ambient 1 diffuse 0}
               } // end texture 2

      scale 10000}  

// fog at the horizon     
fog{distance 300000 color White}

//--------------------------------------------------------------------------

#declare DEFORMABLE_TEXTURE =
texture {
    pigment{rgbt<1,1,1,0.9>}
    finish {
      phong 1 phong_size 30
      reflection{0.01, 0.7 fresnel}}}

#declare DEFORMABLE_INTERIOR = 
  interior {
    ior 1.4
    fade_color rgb<1,0,1>
    fade_distance 0.1
    fade_power 1001}  

object {
  box{<-1.5,-0.5,-0.5>,<-0.5,0.5,0.5>}
  translate<0,0.7,-6>
  texture {
  		pigment { color red 0/255 green 255/255 blue 127/255}
  		finish { phong 1 phong_size 250 specular 0.9 roughness 0.0005 metallic ambient .2 diffuse 0.9 reflection 0.005}
	}
}

object {
  box{<-0.5,-0.5,-0.5>,<0.5,0.5,0.5>}
  translate<0,0.7,-6>
  texture{DEFORMABLE_TEXTURE}
  interior{DEFORMABLE_INTERIOR}
  photons {
    target
    reflection on
    refraction on}
}

object {
  box{<0.5,-0.5,-0.5>,<1.5,0.5,0.5>}
  translate<0,0.7,-6>
  texture {
  		pigment { color red 0/255 green 255/255 blue 127/255}
  		finish { phong 1 phong_size 250 specular 0.9 roughness 0.0005 metallic ambient .2 diffuse 0.9 reflection 0.005}
	}
}
object {
  box{<1.5,-0.5,-0.5>,<2.5,0.5,0.5>}
  translate<0,0.7,-6>
  texture{DEFORMABLE_TEXTURE}
  interior{DEFORMABLE_INTERIOR}
  photons {
    target
    reflection on
    refraction on}
}

object {
  box{<2.5,-0.5,-0.5>,<3.5,0.5,0.5>}
  translate<0,0.7,-6>
  texture {
  		pigment { color red 0/255 green 255/255 blue 127/255}
  		finish { phong 1 phong_size 250 specular 0.9 roughness 0.0005 metallic ambient .2 diffuse 0.9 reflection 0.005}
	}
}

object {
  box{<3.5,-0.5,-0.5>,<4.5,0.5,0.5>}
  translate<0,0.7,-6>
  texture{DEFORMABLE_TEXTURE}
  interior{DEFORMABLE_INTERIOR}
  photons {
    target
    reflection on
    refraction on}
}

object {
  box{<4.5,-0.5,-0.5>,<5.5,0.5,0.5>}
  translate<0,0.7,-6>
  texture {
  		pigment { color red 0/255 green 255/255 blue 127/255}
  		finish { phong 1 phong_size 250 specular 0.9 roughness 0.0005 metallic ambient .2 diffuse 0.9 reflection 0.005}
	}
}


//--------------------------------------------------------------------------

// light_source{<2800,2500,-700> color White*0.3 parallel point_at 0 shadowless}

light_source{<20,50,-10> color White*0.8
  area_light <10, 0, 0>, <0, 0, 10>, 10, 10
  adaptive 1
  photons {
    reflection on
    refraction on}
  jitter}
