#version 120

varying float LightIntensity;
varying vec3 MCPosition;

uniform vec3 LightPos;
uniform float Scale;

void main() {
    
    vec3 ecLight = vec3( gl_ModelViewMatrix * vec4(LightPos, 1.0) );
    
    vec3 ecPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
    MCPosition = vec3(gl_Vertex) * Scale;
    vec3 tnorm = normalize(vec3(gl_NormalMatrix * gl_Normal));
    LightIntensity = dot( normalize(ecLight - ecPosition), tnorm );
    LightIntensity *= 1.5;
    gl_Position = ftransform();
}