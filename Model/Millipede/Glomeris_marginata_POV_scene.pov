//==================================================
//POV-Ray Main scene file
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
//==================================================
//Model Statistics:
//Number of triangular faces..... 40308
//Number of vertices............. 24946
//Number of normals.............. 24946
//Number of UV coordinates....... 21296
//Number of lines................ 0
//Number of materials............ 2
//Number of groups/meshes........ 1
//Number of subdivision faces.... 0
//UV boundaries........ from u,v=(0,0)
//                        to u,v=(1,1)
//Bounding Box....... from x,y,z=(-0.853283,-0.043402,-2.455261)
//                      to x,y,z=(0.853283,1.375174,2.629564)
//                 size dx,dy,dz=(1.706566,1.418576,5.084825)
//                  center x,y,z=(0,0.665886,0.0871515)
//                       diagonal 5.547988
//Surface area................... 63.37188
//             Smallest face area 2.606102E-6
//              Largest face area 0.03835754
//Memory allocated for geometry.. 3 MBytes
// 
//==================================================
//IMPORTANT:
//This file was designed to run with the following command line options: 
// +W320 +H240 +FN +AM1 +A -UA
//if you are not using an INI file copy and paste the text above to the command line box before rendering
 
#include "Glomeris_marginata_POV_geom.inc" //Geometry
 
global_settings {
  //This setting is for alpha transparency to work properly.
  //Increase by a small amount if transparent areas appear dark.
   max_trace_level 15
 
}
 
//CAMERA PoseRayCAMERA
camera {
        perspective
        up <0,1,0>
        right -x*image_width/image_height
        location <-0.4598175,0.530094,11.09598>
        look_at <-0.4598175,0.530094,10.09598>
        angle 10.86546 // horizontal FOV angle
        rotate <0,0,189.7664> //roll
        rotate <6.990142,0,0> //pitch
        rotate <0,174.2277,0> //yaw
        translate <0,0.665886,0.0871515>
        }
 
//PoseRay default Light attached to the camera
light_source {
              <-0.4598175,0.530094,11.0959785> //light position
              color rgb <1,1,1>*1.6
              parallel
              point_at <-0.4598175,0.530094,0>
              rotate <0,0,189.7664> //roll
              rotate <6.990142,0,0> //elevation
              rotate <0,174.2277,0> //rotation
             }
 
//Background
background { color rgb<1,1,1>  }
 
//Assembled object that is contained in Glomeris_marginata_POV_geom.inc
object{
      Glomeris_marginata_
      }
//==================================================
