varying vec4 fColor;
varying vec3 fN;
varying vec3 fL;
varying vec3 fV;

uniform float shininess;
uniform vec4 kAmbient, kDiffuse, kSpecular;

uniform int renderType;

void main()
{
    if(renderType == 1)
        gl_FragColor = fColor;

    else if(renderType == 2){

    vec3 N;
    vec3 V;
    vec3 L;
    vec3 H;

    N = normalize(fN);
    V = normalize(fV);
    L = normalize(fL);
    
    H = normalize(L + V);

    vec4 ambient = kAmbient;

    vec4 diffuse = max(dot(L,N),0.0)*kDiffuse;

    vec4 specular = pow(max(dot(N,H),0.0),shininess)*kSpecular;

    if(dot(L,N) < 0.0){
        specular = vec4(0.0,0.0,0.0,1.0);
    }

    gl_FragColor = ambient + diffuse + specular;
    gl_FragColor.a = 1.0;
    
    }
}