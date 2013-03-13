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
  location <25,25,-50>*0.5
  //look_at <25,10,0>
  look_at <5,0,0>
  angle 40
}


#declare T_Crasse = texture {
	pigment {
		wrinkles
		color_map {
			[0.0 Clear]
			[1.0 DarkBrown*.5 filter 0.5]
		}
	}
}
#declare T_Crasse2 = texture {
	pigment {
		wrinkles
		color_map {
			[0.0 Clear]
			[1.0 DarkBrown*.4 filter 0.6]
		}
	}
}
#declare T_Rouille = texture {
	pigment {
		granite
		color_map {
			[0 DarkTan*.5]
			[0.7 DarkBrown*.5]
			[1 Orange*.5]
		}
	}
	normal{ granite .6 scale .1 }
	finish{ ambient .4 diffuse .6
		specular .3 roughness .5
	}
}



#declare T_Fer_Rouge = texture {
	pigment { color rgb <.8,.1, .05>*.8 }
	normal {
		average
		normal_map {
			[1 wrinkles 1 scale .2]
			[1 dents 2 scale .04]
		}
	}
	finish {
		ambient .4 diffuse .6
		specular 1 roughness .1 metallic 
	}
}
texture { T_Crasse scale 5 }
texture { T_Crasse2 scale 2 }

#declare T_Fer_Marron = texture {
	pigment { color rgb <.8,.6,.4>*.4 }
	normal {
		average
		normal_map {
			[1 wrinkles .3 scale .2]
			[1 dents .5 scale .04]
		}
	}
	finish {
		ambient .4 diffuse .6
		specular .6 roughness .01 metallic
		phong .6 phong_size 20
	}
}
texture { T_Crasse2 scale 3 }


#declare T_Fer_Rouge_Rouille = texture {
	bozo
	scale 4
	texture_map {
		[0.5 T_Fer_Rouge scale 1/4]
		[1.1 T_Rouille ]
	}
	warp { turbulence 1 lambda 2 octaves 3 }
}

difference{
  cylinder  { <0,0,0>,<10,0,0>,3}
  cylinder  { <-1,0,0>,<11,0,0>,2}
  texture {T_Fer_Rouge_Rouille scale 0.5 }
}

// mesh {
//   MillipedeSoftPart
//   texture {
//     pigment { color rgbt <1, 1, 0, 0.4> }
//     finish {
//       ambient 0.2
//       brilliance 1
//       crand 0
//       diffuse 0.625
//       metallic 0.01
//       phong 1
//       phong_size 0.4
//       reflection 0.01
//       roughness 0.5
//       specular 0.1  }}
//   interior_texture {
//     pigment { color rgbt <1, 1, 0,0.2> }
//     finish { 
//       ambient 0.25
//       brilliance 1
//       crand 0
//       diffuse 0.625
//       metallic 0.01
//       phong 0.5
//       phong_size 1
//       reflection 0.1
//       roughness 0.2
//       specular 0.1}}
//   interior { ior 1.4}
// }

// object {
//   merge{     MillipedeAntenneaPart   }
//   texture {
//     pigment { color rgb <1, 130/255, 171/255> }
//     finish {
//       ambient 0.2
//       brilliance 1
//       crand 0
//       diffuse 0.625
//       metallic 0.01
//       phong 1
//       phong_size 0.4
//       reflection 0.01
//       roughness 0.5
//       specular 0.1  }}
//   interior_texture {
//     pigment { color rgb <1, 130/255, 171/255> }
//     finish { 
//       ambient 0.25
//       brilliance 1
//       crand 0
//       diffuse 0.625
//       metallic 0.01
//       phong 0.5
//       phong_size 1
//       reflection 0.1
//       roughness 0.2
//       specular 0.1}}
//   interior {     ior 1.4   }
// }

// object {
//   MillipedeRigidPart
//   texture{Chrome_Metal}
//   finish {
//     ambient 0.2
//     brilliance 1
//     crand 0
//     diffuse 0.625
//     metallic 0.01
//     phong 1
//     phong_size 0.4
//     reflection 0.01
//     roughness 0.5
//     specular 0.1  
//   }
// }

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



