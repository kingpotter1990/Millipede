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
  location  <-8, 12,  -45>
  look_at   <-8, 12,  0.0>   

  //location  <-8, 30,  0.0>
  //look_at   <-8, 12,  0.0> 

  //right x*image_width/image_height
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
    pigment{rgbt<0.4,0.4,0.4,0.9>}
    finish {
      phong 1 phong_size 30
      reflection{0.01, 0.7 fresnel}}}

  interior {
    ior 1.4

    fade_color rgb<1,1,0>
    fade_distance 0.1
    fade_power 1001}  

   photons {
    target
    reflection on
    refraction on}
}

 
sky_sphere {
		pigment { gradient y
			color_map {
				[0 rgb <0.5, 0.6, 1> ]
				[1 rgb <0,0,1> ]
			}
		}
		pigment { wrinkles turbulence 0.7
			color_map {
				[0 rgbt <1,1,1,1>]
				[0.5 rgbt <0.98, 0.99, 0.99, .6>]
				[1 rgbt <1, 1, 1, 1>]
			}
			scale <.8, .1, .8>
		}
	}


plane { y, 0
		pigment { checker rgb <0, 0, 0> rgb <1, 1, 1.0> scale 15 }
		finish { reflection 0.5 ambient 0.1 diffuse 0.8}
	}

fog {
		distance 500
		color rgb 0.9
		fog_offset 10
		fog_alt 5
		fog_type 2
	}


light_source {
  <40,40,-40>
  color rgb 0.6       // light's color

    spotlight
    radius 100
    falloff 14
    tightness 10
    point_at <0,0,0>

   photons {
      reflection on
      refraction on
    }
  jitter
}  
