attribute vec4 vPosition;
attribute vec3 vNormal;
attribute vec4 vColor;

varying vec3 fN; //normal at current position
varying vec3 fV; //vector from point to viewer
varying vec3 fL; //vector from point to light
varying vec4 fColor;

uniform mat4 cMw;
uniform mat4 proj;
uniform vec4 cameraPosition;
uniform vec4 lightPosition;

uniform int renderType;

void main()
{
    gl_Position = proj*cMw*vPosition;
    
    if(renderType == 1)// grid
        fColor = vColor;
    else if(renderType == 2)// Phong Smooth Shading
    {
        fN = vNormal;
        fV = ((cameraPosition - vPosition)).xyz;
        fL = ((lightPosition - vPosition)).xyz;
    }
}
