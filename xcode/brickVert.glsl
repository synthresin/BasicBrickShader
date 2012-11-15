#version 120

varying float LightIntensity;
varying vec3 MCPosition;
uniform vec3 LightPos;
uniform float Scale;

void main() {
    
    vec3 ecLight = vec3( gl_ModelViewMatrix * vec4(Lightpos, 1.0) );
    
    vec3 ecPosition = gl_ModelViewMatrix * gl_Vertex;
    MCPosition = vec3(gl_Vertex) * Scale;
    vec3 tnorm = normalize(vec3(gl_NormalMatrix * gl_Normal));
    LightIntensity = max(dot( normalize(ecLight - ecPosition), tnorm ), 0.0);
    LightIntensity *= 1.5;
    gl_Position = ftansform();
}