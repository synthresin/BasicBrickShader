#version 120

varying vec3 ReflectDir;
varying float LightIntensity;

uniform vec3    LightPos;


void main()
{
    vec3 ecLight = vec3( gl_ModelViewMatrix * vec4(LightPos, 1.0) );
    gl_Position = ftransform();
    vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
    vec4 pos = gl_ModelViewMatrix * gl_Vertex;
    vec3 eyeDir = pos.xyz;
    ReflectDir = reflect(eyeDir, normal);
    LightIntensity = max( dot(normalize(ecLight - eyeDir) ,normal),0.0);
}
