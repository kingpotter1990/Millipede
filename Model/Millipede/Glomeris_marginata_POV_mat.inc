//==================================================
//POV-Ray Materials list
//==================================================
//This file has been created by PoseRay v3.12.2.456
//3D model to POV-Ray/Moray Converter.
//Author: FlyerX
//Email: flyerx_2000@yahoo.com
//Web: http://mysite.verizon.net/sfg0000/
//==================================================
//Files needed to run the POV-Ray scene:
//Glomeris_marginata_POV_main.ini (initialization file - open this to render)
//Glomeris_marginata_POV_scene.pov (scene setup of cameras, lights and geometry)
//Glomeris_marginata_POV_geom.inc (geometry mesh)
//Glomeris_marginata_POV_mat.inc (materials)
//diff_1.tga
//h_alpha.tga
// 
 
#include "colors.inc"
 
//==================================================
//Map definitions
#declare p_map1=pigment {image_map{tga "diff_1.tga" interpolate 2 transmit all 0 filter all 0} }
#declare p_map2=pigment {image_map{tga "h_alpha.tga" interpolate 2 }}
 
//==================================================
//Material definitions
#declare Archispirostreptus_gigas_body_= 
material{
texture{ pigment{uv_mapping p_map1} 
        finish{specular 0.9019608 roughness 0.03641292 ambient rgb <0.60,0.60,0.60> diffuse 0.6   reflection{0 } conserve_energy}}
}
//==================================================
#declare Archispirostreptus_gigas_legs_2__opaque=
texture{ pigment {color rgb <0.00,0.00,0.00> transmit  0} 
        finish{specular 0.9019608 roughness 0.04120575 ambient rgb <0.00,0.00,0.00> diffuse 0.6   reflection{0 } conserve_energy}}
#declare Archispirostreptus_gigas_legs_2_= 
material{
texture{  pigment_pattern{uv_mapping p_map2} 
texture_map{[0 pigment{Clear}]
            [1 Archispirostreptus_gigas_legs_2__opaque]
           }}
}
//==================================================
