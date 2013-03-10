#include "colors.inc"
#include "textures.inc"
#include "frame1_POV_geom.inc"
// #include "water.inc"
global_settings { 
//  max_trace_level 20
}

// camera -----------------------------------------------------------
#declare Cam0 = camera {/*ultra_wide_angle*/ angle 65 
                        location  <0 , 15.0 ,-30.0>
                        look_at   <0 , 0 , 0.0>
  // focal_point < 0, 0, 0>    // pink sphere in focus
  //   aperture 0.1
  //    blur_samples 2     // more samples, higher quality image

}
camera{Cam0}
// sun ---------------------------------------------------------------
light_source{<1500,2500,-2500> color White }


// sky ---------------------------------------------------------------------
sky_sphere { pigment { gradient <0,1,0>
                       color_map { [0.00 rgb <0.6,0.7,1.0>]
                                   [0.35 rgb <0.0,0.1,0.8>]
                                   [0.65 rgb <0.0,0.1,0.8>]
                                   [1.00 rgb <0.6,0.7,1.0>] 
                                 } 

                     } // end of pigment
           } //end of skysphere -------------------------------------
   
// fog ---------------------------------------------------------------
// fog{fog_type   2   distance 65  color rgb<1,0.99,0.9>
//     fog_offset 0.1 fog_alt  2.0 turbulence 0.2}
//--------------------------------------------------------------------

// swimming pool  ---------------------------------------------------
#declare pool_bottom_xmin=-20;
#declare pool_bottom_xmax=20;
#declare pool_bottom_ymin=-0.0001;
#declare pool_bottom_ymax=0;
#declare pool_bottom_zmin=-10;
#declare pool_bottom_zmax=10;
#declare pool_water_y=0.5;
box {  <pool_bottom_xmin,pool_bottom_ymin,pool_bottom_zmin>,<pool_bottom_xmax,pool_bottom_ymax,pool_bottom_zmax> // bottom
  pigment { checker rgb <0, 0, 0> rgb <1, 1, 1.0> scale 3 }
		finish { reflection 00 ambient 0.1 diffuse 0.8}	}

#declare pool_wall_texture= texture{
  pigment { rgb <0.9, 0.9, 0.9>}
		finish { phong 1.0 phong_size 70 metallic }	

} // end of texture

box {  <pool_bottom_xmin-0.1,pool_bottom_ymax,pool_bottom_zmin>,<pool_bottom_xmin,pool_water_y,pool_bottom_zmax>
  texture{pool_wall_texture}
}
box {  <pool_bottom_xmax,pool_bottom_ymax,pool_bottom_zmin>,<pool_bottom_xmax+0.1,pool_water_y,pool_bottom_zmax>
    texture{pool_wall_texture}
}
box {  <pool_bottom_xmin,pool_bottom_ymax,pool_bottom_zmin-0.1>,<pool_bottom_xmax,pool_water_y,pool_bottom_zmin>
    texture{pool_wall_texture}
} 
box {  <pool_bottom_xmin,pool_bottom_ymax,pool_bottom_zmax>,<pool_bottom_xmax,pool_water_y,pool_bottom_zmax+0.1>
    texture{pool_wall_texture}
}




//--------------------------------------------------------------------

object{sphere{<0,0.9,0>,0.6}
  texture{pigment{rgb<0,1,0.3>}}}

object{
  plane{y,pool_water_y}


  
  
      texture{pigment{ rgbt <0.2, 0.2, 0.2,1> } 

              finish { ambient 0.05 diffuse 0.55 
                       brilliance 6.0 phong 0.8 phong_size 120
                       reflection {0.4,0.6} }
      }

      interior{
		ior 1.3
		dispersion 1.01
		fade_distance 4
  		fade_power 1001
	      }

	
}

object{
      frame1_
  rotate<0,90,0>
      }
