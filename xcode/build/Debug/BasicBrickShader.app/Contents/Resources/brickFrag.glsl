#version 120

varying float LightIntensity;
varying vec3 MCPosition;

uniform sampler3D   Noise;
uniform vec3        SkyColor;
uniform vec3        CloudColor;


void main() {
    vec4 noisevec = texture3D(Noise, MCPosition);
    
    float intensity = abs(noisevec[0] - 0.25) + abs(noisevec[1] - 0.125) + abs(noisevec[2] - 0.0625) + abs(noisevec[3]-0.03125);
    
    float sineval = sin(MCPosition.y * 6.0 + intensity * 12.0) * 0.5 + 0.5;
    
    vec3 color = mix(SkyColor, CloudColor, sineval) * LightIntensity;
    gl_FragColor = vec4(color, 1.0);
}