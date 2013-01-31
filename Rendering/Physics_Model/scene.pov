#version 3.6;
#include "metals.inc"
#include "glass.inc"
#include "woods.inc"
#include "textures.inc"
#include "@@@"

global_settings {
  radiosity{  }
  max_trace_level 20
  photons {    spacing 0.008}
}

// perspective (default) camera
camera {
  location  <-8, 12,  -15>
  look_at   <-8, 12,  0.0>   
  right x*image_width/image_height
  angle  50
}           

object {
   MillipedeRigidPart
  translate<-15,-5,0>

  texture { Chrome_Metal }
  finish { ambient 0.0 brilliance 1.2 phong 0.4 }
}

object {
   MillipedeSoftPart
  translate<-15,-5,0>

texture {
    pigment{rgbt<1,1,1,0.9>}
    finish {
      phong 1 phong_size 30
      reflection{0.01, 0.7 fresnel}}}

  interior {
    ior 1.4
    fade_color rgb<0,1,1>
    fade_distance 0.1
    fade_power 1001}  

   photons {
    target
    reflection on
    refraction on}
}

 
// Create an infinite sphere around scene
sky_sphere {
  pigment {
    function { abs(y) }
    color_map { 
        [0.0 color rgb 0   ] 
        [1.0 color rgb <0.7, 0.7, 1> ] 
    }
  }     
}

// ground
object{
plane{<0,1,0>,0}
  pigment { color rgb <0.93,0.93,1>*0.6 }
  finish {
    diffuse 0.65
    ambient <0.2,0.2,0.22>
    roughness 0.5
    reflection 0.1
  }

} // end of plane


light_source {
  <40,40,-40>
  color rgb 0.6       // light's color

   photons {
      reflection on
      refraction on
    }
  jitter
}  
