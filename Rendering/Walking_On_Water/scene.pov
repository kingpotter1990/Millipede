// POV-Ray 3.6 Scene File "p_watr0.pov"
// created by  Friedrich A. Lohmueller, 2004
// homepage:  http://www.f-lohmueller.de
// email:  Friedrich.Lohmueller_at_t-online.de
#include "colors.inc"
#include "textures.inc"
#include "water.inc"

global_settings { 


	max_trace_level 20

}

// camera -----------------------------------------------------------
#declare Cam0 = camera {/*ultra_wide_angle*/ angle 65 
                        location  <16.0 , 20.0 ,24.0>
                        look_at   <16.0 , 0.8 , 0.0>}
camera{Cam0}
// sun ---------------------------------------------------------------
light_source{<1500,2500,-2500> color White

}


// sky ---------------------------------------------------------------------
sky_sphere { pigment { gradient <0,1,0>
                       color_map { [0.00 rgb <0.6,0.7,1.0>]
                                   [0.35 rgb <0.0,0.1,0.8>]
                                   [0.65 rgb <0.0,0.1,0.8>]
                                   [1.00 rgb <0.6,0.7,1.0>] 
                                 } 
                       scale 2         
                     } // end of pigment
           } //end of skysphere -------------------------------------
   
// fog ---------------------------------------------------------------
fog{fog_type   2   distance 65  color rgb<1,0.99,0.9>
    fog_offset 0.1 fog_alt  2.0 turbulence 0.2}
//--------------------------------------------------------------------

plane { y, 0
		pigment { checker rgb <0, 0, 0> rgb <1, 1, 1.0> scale 3 }
		finish { reflection 00 ambient 0.1 diffuse 0.8}



	}

object{water
      texture{pigment{ rgbt <0.2, 0.2, 0.2,1> } 

              finish { ambient 0.05 diffuse 0.55 
                       brilliance 6.0 phong 0.8 phong_size 120
                       reflection {0.4,1.0} }
      }

      interior{
		ior 1.3
		dispersion 1.01
		fade_distance 4
  		fade_power 1001
	      }

	
}


