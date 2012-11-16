#version 120

varying float LightIntensity;
varying vec3 MCPosition;

uniform sampler3D   Noise;
uniform vec3        CloudColor;
uniform vec3        SkyColor;
uniform vec3        Offset;

void main() {
    vec4 noisevec = texture3D(Noise, MCPosition + Offset);
    
    float intensity = (noisevec[0] + noisevec[1] + noisevec[2] + noisevec[3] + 0.03125) * 1.5;
    
    vec3 color = mix(SkyColor, CloudColor, intensity);// * LightIntensity;
    gl_FragColor = vec4(color, 1.0);
}