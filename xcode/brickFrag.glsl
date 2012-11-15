#version 120

varying float       LightIntensity;
uniform sampler2D   tex;

void main() {
    
    vec3 color;

    
    color = vec3(0.6,0.9,0.3);
    color *= LightIntensity;
    gl_FragColor = vec4(color,1.0);
    
    
}