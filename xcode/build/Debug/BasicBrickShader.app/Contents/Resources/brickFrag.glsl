#version 120

varying float LightIntensity;
varying vec3 MCPosition;

uniform sampler3D   Noise;
uniform vec3        SkyColor;
uniform vec3        CloudColor;


void main() {
    vec4 noisevec = texture3D(Noise, MCPosition);
    
    float intensity = abs(noisevec[0] - 0.25) + abs(noisevec[1] - 0.125) + abs(noisevec[2] - 0.0625) + abs(noisevec[3]-0.03125);
    
    intensity = clamp(intensity * 6.0, 0.0, 1.0);
    
    vec3 color = mix(SkyColor, CloudColor, intensity) * LightIntensity;
    gl_FragColor = vec4(color, 1.0);
}