#version 3.7;

#include "colors.inc"
#include "glass.inc"
#include "textures.inc"
#include "metals.inc"
#include "glass.inc"
#include "woods.inc"
#include "textures.inc"
#include "colors.inc"
#include "t.inc"

#default{ finish{ ambient 0.2 diffuse 0.6 }}

global_settings {   max_trace_level 20   assumed_gamma 1.9 }

sky_sphere{
pigment{ gradient <0,1,0>
          color_map{
          [0.1 color rgb<0.2,0.2,0.53>]
          [0.8 color rgb<0.1,0.25,0.85>]
          [1.0 color rgb<0.1,0.25,0.85>]}}}

plane {
  <0,1,0>, 0
  pigment { color rgb <0.93,0.93,1> }
  finish {
    diffuse 0.65
    ambient <0.2,0.2,0.22>
    roughness 0.5
    reflection 0.3}}

fog{ fog_type 1
      distance  500
      color  rgb <0.7,0.7,0.75>
      fog_offset 0
      fog_alt    5
      turbulence 0}

camera {
  location <25,25,-50>
  look_at <25,10,0>
  angle 40
}

mesh {
  MillipedeSoftPart
  texture {
    pigment { color rgbt <1, 1, 0, 0.4> }
    finish {
      ambient 0.2
      brilliance 1
      crand 0
      diffuse 0.625
      metallic 0.01
      phong 1
      phong_size 0.4
      reflection 0.01
      roughness 0.5
      specular 0.1  }}
  interior_texture {
    pigment { color rgbt <1, 1, 0,0.2> }
    finish { 
      ambient 0.25
      brilliance 1
      crand 0
      diffuse 0.625
      metallic 0.01
      phong 0.5
      phong_size 1
      reflection 0.1
      roughness 0.2
      specular 0.1}}
  interior { ior 1.4}
}

object {
  merge{     MillipedeAntenneaPart   }
  texture {
    pigment { color rgb <1, 130/255, 171/255> }
    finish {
      ambient 0.2
      brilliance 1
      crand 0
      diffuse 0.625
      metallic 0.01
      phong 1
      phong_size 0.4
      reflection 0.01
      roughness 0.5
      specular 0.1  }}
  interior_texture {
    pigment { color rgb <1, 130/255, 171/255> }
    finish { 
      ambient 0.25
      brilliance 1
      crand 0
      diffuse 0.625
      metallic 0.01
      phong 0.5
      phong_size 1
      reflection 0.1
      roughness 0.2
      specular 0.1}}
  interior {     ior 1.4   }
}

object {
  MillipedeRigidPart
  texture{Chrome_Metal}
  finish {
    ambient 0.2
    brilliance 1
    crand 0
    diffuse 0.625
    metallic 0.01
    phong 1
    phong_size 0.4
    reflection 0.01
    roughness 0.5
    specular 0.1  
  }
}

light_source{<160,100,-50> color White*0.4*0.4  // 160 100 -50 normal
  area_light <10, 0, 0>, <0, 0, 10>, 10, 10
  adaptive 1
  photons {
    reflection on
    refraction on
  }
  jitter}
light_source{<2800,2500,-700> color White*0.5 parallel point_at 0 shadowless}
light_source{ <0,40,0>   color rgb <0.4,0.4,0.4>*0.9
              spotlight
              point_at<0,0,0>
              radius 0  // hotspot
              tightness 0
              falloff 0
              translate< 0, 0, 0>
              shadowless}
light_source{ <-40,30,-100>   color rgb <0.2,0.2,0.2>*0.9
              spotlight
              point_at<0,0,0>
              radius 0  // hotspot
              tightness 1
              falloff 1
              translate< 0.1, 0.1, 0.1>
              shadowless}



