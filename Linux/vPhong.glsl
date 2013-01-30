attribute vec4 vPosition;
attribute vec3 vNormal;

varying vec4 fColor;

uniform mat4 wMo;
uniform mat4 cMw;
uniform mat4 proj;

uniform vec4 cameraPosition;
uniform vec4 lightPosition;
uniform vec4 Color;

void main()
{
    gl_Position = proj*cMw*wMo*vPosition;

	//flat shadding
	vec3 N = (wMo*vec4(vNormal.x,vNormal.y,vNormal.z,0.0)).xyz;
    vec3 V = ((cameraPosition - wMo*vPosition)).xyz;
    vec3 L = ((lightPosition - wMo*vPosition)).xyz;
    vec3 H;

    N = normalize(N);
    V = normalize(V);
    L = normalize(L);
    
    H = normalize(L + V);

    vec4 ambient = 0.2*Color;

    vec4 diffuse = max(dot(L,N),0.0)*0.5*Color;

    vec4 specular = pow(max(dot(N,H),0.0),20.0)*0.5*vec4(1.0,1.0,1.0,1.0);

    if(dot(L,N) < 0.0){
        specular = vec4(0.0,0.0,0.0,1.0);
    }

    fColor = ambient + diffuse + specular;
    fColor.a = 1.0;

}
