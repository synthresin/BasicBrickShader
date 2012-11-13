#version 120

uniform vec3 BaseColor;
uniform float MixRatio;

uniform samplerCube EnvMap;

varying vec3 ReflectDir;
varying float LightIntensity;

void main() {
    
    vec3 envColor = vec3(textureCube(EnvMap, ReflectDir));
    
    vec3 base = LightIntensity * BaseColor;
    envColor = mix (envColor, base, MixRatio);
    
    gl_FragColor = vec4(envColor, 1.0);
    
}