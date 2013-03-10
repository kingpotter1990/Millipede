#include "metals.inc"
#include "glass.inc"
#include "woods.inc"
#include "textures.inc"
#include "colors.inc"
#include "@@@"

global_settings {
  //radiosity{  }
  max_trace_level 20
  photons {    spacing 0.008}
}

// camera -----------------------------------------------------------
#declare Cam0 = camera {/*ultra_wide_angle*/ angle 65 
                        location  <0 , 14 ,26>
                        look_at   <0 , 5 , 0.0>
  // focal_point < 0, 0, 0>    // pink sphere in focus
  //   aperture 0.1
  //    blur_samples 2     // more samples, higher quality image

}
camera{Cam0}

// sun ---------------------------------------------------------------
light_source{
  <1500,2500,-2500> 
  color White 
  photons {
      reflection on
      refraction on
    }
}


// sky ---------------------------------------------------------------------
sky_sphere { pigment { gradient <0,1,0>
                       color_map { [0.00 rgb <0.6,0.7,1.0>]
                                   [0.35 rgb <0.0,0.1,0.8>]
                                   [0.65 rgb <0.0,0.1,0.8>]
                                   [1.00 rgb <0.6,0.7,1.0>] 
                                 } 

                     } // end of pigment
           } //end of skysphere -------------------------------------


// ground  ---------------------------------------------------
#declare pool_bottom_xmin=-200;
#declare pool_bottom_xmax=200;
#declare pool_bottom_ymin=0;
#declare pool_bottom_ymax=0;
#declare pool_bottom_zmin=-50;
#declare pool_bottom_zmax=50;
box {  <pool_bottom_xmin,pool_bottom_ymin,pool_bottom_zmin>,<pool_bottom_xmax,pool_bottom_ymax,pool_bottom_zmax>
pigment { checker rgb <0, 0, 0> rgb <1, 1, 1.0> scale 4 }
 		finish { reflection 0.5 ambient 0.1 diffuse 0.8}
}

object {
  MillipedeAntenneaPart

  texture {
        pigment { color rgbf <1, 0, 0, 0.77>      }
      finish  { ambient 0.1 diffuse 0.30 brilliance 0.6767 phong 0.68
         phong_size 80 specular 0.05 roughness 0.135 reflection 0.05}
  }
  interior{ior 1.69 }
}

object {
   MillipedeRigidPart

  texture { Chrome_Metal }
  finish { ambient 0.0 brilliance 1.2 phong 0.4 }
}

object {
   MillipedeSoftPart

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


// ////////////////////////////////
// /////// bottle /////////////////
// ////////////////////////////////
// #declare glas_green_tx=
// texture
// {
//   pigment
//   {
//     color rgbt<0.1,0.6,0.5,0.9>
//   }
//   finish
//   {
//     specular 0.2
//     phong_size 200
//     phong 0.3
//     reflection 0.3
//   }
// }
// ////////////////////////////////
// #declare bottle_outside=
// merge
// {
//   difference
//   {
//     cylinder{<0,1.3,0>,<0,2,0>,0.4}
//     torus{1.2,1.0 scale <1,1,1> translate y*1.9 }
//   }
//   difference
//   {
//     cylinder{<0,0,0>,<0,1,0>,0.5}
//     sphere{<0,0,0>,0.2 scale <1,0.9,1>}
//   }
//   torus{0.2,0.05 scale <1,2,1> translate y*2.0 sturm}
//   torus{0.35,0.15 scale <1,1,1> sturm}
//   sphere{<0,1,0>,0.5 scale <1,1,1>}
//   translate y*0.15
// }

// #declare bottle_inside=
// merge
// {
//   difference
//   {
//     cylinder{<0,1.3,0>,<0,2,0>,0.4}
//     torus{1.2,1.0 scale <1,1,1> translate y*1.9 }
//   }
//   difference
//   {
//     cylinder{<0,0,0>,<0,1,0>,0.5}
//     sphere{<0,0,0>,0.0 scale <1,0.5,1>}
//   }
//   cylinder{<0,1,0>,<0,2.5,0>,0.2}
//   torus{0.4,0.1 scale <1,0.5,1> sturm}
//   sphere{<0,1,0>,0.5 scale <1,1,1>}
//   translate y*0.05
// }

// #declare bottle=
// difference
// { 
//   object {bottle_outside}
//   object {bottle_inside scale<0.75,0.9,0.75>translate y*0.25}
//   interior{caustics 0.2 ior 1.45}
//   texture{glas_green_tx}
//   scale<1,1.5,1>
// }
// ////////////////////////////////
// ////////////////////////////////
// object{bottle scale 2}


// #declare DIE = difference {
//   superellipsoid {  <0.1, 0.1> }
// texture {
//       // pigment { color rgbf <0.334, 0.375, 1.0, 0.77>      }
//         pigment { color rgbf <1, 0, 0, 0.77>      }
//       finish  { ambient 0.1 diffuse 0.30 brilliance 0.6767 phong 0.68
//          phong_size 80 specular 0.05 roughness 0.135 reflection 0.05}
// }
// interior{ior 1.69 }


//   scale 0.5
// }

// object{DIE
// scale 2
// translate<3,3,-3>
// }

