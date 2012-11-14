#version 120

varying vec3 DiffuseColor;
varying vec3 SpecularColor;

void main() {
    
    vec3 color;

    
    color = vec3(0.6,0.9,0.3);
    color *= LightIntensity;
    gl_FragColor = vec4(color,1.0);
    
    
}