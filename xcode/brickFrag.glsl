#version 120

varying float LightIntensity;
varying vec3 MCPosition;

uniform sampler3D   Noise;
uniform vec3        SkyColor;
uniform vec3        CloudColor;


void main() {
    float NoiseScale = 1;
    vec4 noisevec = texture3D(Noise, NoiseScale * MCPosition);
    
    float intensity = min(1.0, noisevec[3] * 18.0);
    
    vec3 color = vec3(intensity * LightIntensity);
    gl_FragColor = vec4(color, 1.0);
}