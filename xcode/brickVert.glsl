#version 120

varying vec4 Color;

uniform vec3 LightPosition;
uniform vec3 SurfaceColor;

const float PI =  3.14159;
const float TWO_PI =  PI * 2.0;

uniform float Radius;
uniform float Blend;

vec3 sphere(vec2 domain) {
    vec3 range;
    
    range.x = Radius * cos(domain.y) * sin(domain.x); 
    range.y = Radius * sin(domain.y) * sin(domain.x);
    range.z = Radius * cos(domain.x);
    
    return range;
}

void main() {
    vec2 po = gl_Vertex.xy * TWO_PI;    // 큐브의 점에 파이를 곱한다?
    vec3 normal = sphere(po);
    normal = gl_Normal;
    vec3 ro = Radius * normal;
    vec3 vertex = ro;
    
    vertex = gl_Vertex.xyz * 2;
    
    //normal = mix(gl_Normal, normal, Blend);
    vertex = mix(gl_Vertex.xyz, vertex, Blend);
    
    normal = normalize(gl_NormalMatrix * normal );
    vec3 ecPosition = vec3( gl_ModelViewMatrix * vec4(vertex, 1.0) );
    
    vec3 ecLight = vec3( gl_ModelViewMatrix * vec4(LightPosition, 1.0) );
    
    vec3 lightVec = normalize(ecLight - ecPosition);
    float diffuse = max(dot(lightVec, normal), 0.0);
    
    if (diffuse < 0.125) {
        diffuse = 0.125;
    }
    
    Color = vec4(SurfaceColor * diffuse, 1.0 );
    gl_Position = gl_ModelViewProjectionMatrix * vec4(vertex, 1.0);
}