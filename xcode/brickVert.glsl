#version 120

varying float   Diffuse;
varying vec3    Specular;
varying vec2    TexCoord;

uniform vec3    LightPosition;


void main()
{
    vec3 ecLight = vec3(gl_ModelViewMatrix * vec4(LightPosition, 1.0) );    // 빛의 위치를 eye coordinate 로.
                                                                            // ecLight 는 아마 GL 변수로 받으면 곧바로 받아질겨.
    
    
    //ecLight = vec3(LightPosition);    // 빛도 변하게 하는 가짜, 잘보이나 확인용.
    
    vec3 ecPosition = vec3(gl_ModelViewMatrix * gl_Vertex); // a vertex's eye coordinate position
    vec3 tnorm = normalize(gl_NormalMatrix * gl_Normal); // 노멀을 아이코디네이트에 맞게 변경후, 노멀라이즈.
    vec3 lightVec = normalize(ecLight - ecPosition); // 눈좌표상의 라이트에서, 버텍스 좌표를 빼기.
    vec3 reflectVec = reflect(-lightVec, tnorm);
    vec3 viewVec = normalize(-ecPosition);
    
    
    float spec = clamp(dot(reflectVec, viewVec), 0.0, 1.0);          // reflectVec 와 viewVec 사이 각도의 코사인 값.
          spec = pow(spec, 8.0);                             // 하이타이트를 날카롭게 하는 효과가 있다.
                                                            // 디퓨즈랑 스펙큘러 값 다 구했다! (0.0 - 1.0 사이.
    Specular = vec3(spec) * vec3(1.0, 0.941, 0.898) * 0.3;

    Diffuse = max( dot(lightVec, tnorm), 0.0 );       // lightVec 와 tnorm 사이 각도의 코사인 값.
    
    TexCoord = gl_MultiTexCoord0.st;
    gl_Position = ftransform(); // 마지막 처리~
}
