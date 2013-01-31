#version 3.6;
#include "metals.inc"
#include "textures.inc"
#include "@@@"

global_settings {
  radiosity{  }
  max_trace_level 20
}

// perspective (default) camera
camera {
  location  <2, 1,  -70>
  look_at   <2, 1,  0.0>   
  angle     33
}           

light_source {
  0*x                  // light's position (translated below)
  color rgb 0.95       // light's color
  translate <40, 40, -40>
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


