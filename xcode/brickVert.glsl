#version 120

uniform vec3    LightPosition;

const float     SpecularContribution = 0.2;
const float     AmbientContribution = 0.2;
const float     DiffuseContribution = 1.0 - SpecularContribution - AmbientContribution;

varying float   LightIntensity;
varying vec2    MCposition;


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
    
    float diffuse = max( dot(lightVec, tnorm), 0.0 );       // lightVec 와 tnorm 사이 각도의 코사인 값.
    float spec = 0.0;
    
    if (diffuse > 0.0) {
        spec = max(dot(reflectVec, viewVec), 0.0);          // reflectVec 와 viewVec 사이 각도의 코사인 값.
        spec = pow(spec, 16.0);                             // 하이타이트를 날카롭게 하는 효과가 있다.
    }
                                                            // 디퓨즈랑 스펙큘러 값 다 구했다! (0.0 - 1.0 사이.)

    LightIntensity = DiffuseContribution * diffuse + SpecularContribution * spec + AmbientContribution * 1.0;   // 빛의 세기 다 구했당!
    MCposition = gl_Vertex.xy;  // 벽돌을 그리기 위해 필요한 자료.
    gl_Position = ftransform(); // 마지막 처리~
}

void process() {
    vec4 ecPosition;
    vec3 ecPosition3;
    vec4 normal;
    vec4 amb;
    vec4 diff;
    vec4 spec;
    vec3 color;
    
    bool SeparateSpecular = true;
    int NumEnabledLights = 5;
    
    gl_Position = ftransform();
    
    ecPosition = gl_ModelViewMatrix * gl_Vertex;
    ecPosition3 = (vec3(ecPosition)) / ecPosition.w;
    
    normal = gl_NormalMatrix * gl_Normal;
    normal = normalize(normal);
    normal = normal * gl_NormalScale;
    
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
    
    eye = -normalize(ecPosition3);
    
    // 인텐시티 어큐뮬레이터
    amb = vec4(0.0);
    spec = vec4(0.0);
    diff = vec4(0.0);
    
    for (int i = 0; i < NumEnabledLights; i++) {
        if (gl_LightSource[i].position.w == 0.0) {
            DirectionalLight(i, normal, amb, diff, spec);
        } else if (gl_LightSource[i].spotCutoff == 180.0) {
            PointLight(i, eye, ecPosition3, normal, amb, diff, spec);
        } else {
            SpotLight(i, eye, ecPosition3, normal, amb, diff, spec);
        }
    }
    
    color = gl_FrontLightModelProduct.sceneColor +
            amb * gl_FrontMaterial.ambient +
            diff * gl_FrontMaterial.diffuse;
    
    if (SeparateSpecular) {
        gl_FrontSecondaryColor = vec4(spec * gl_FrontMaterial.specular, 1.0);
    } else {
        color += spec * gl_FrontMaterial.specular;
    }
    
    gl_FrontColor = color;
    
    // 뒷면 라이팅
    
    normal = -normal;
    
    // 인텐시티 어큐뮬레이터
    amb = vec4(0.0);
    spec = vec4(0.0);
    diff = vec4(0.0);
    
    for (int i = 0; i < NumEnabledLights; i++) {
        if (gl_LightSource[i].position.w == 0.0) {
            DirectionalLight(i, normal, amb, diff, spec);
        } else if (gl_LightSource[i].spotCutoff == 180.0) {
            PointLight(i, eye, ecPosition3, normal, amb, diff, spec);
        } else {
            SpotLight(i, eye, ecPosition3, normal, amb, diff, spec);
        }
    }
    
    color = gl_BackLightModelProduct.sceneColor +
    amb * gl_BackMaterial.ambient +
    diff * gl_BackMaterial.diffuse;
    
    if (SeparateSpecular) {
        gl_BackSecondaryColor = vec4(spec * gl_BackMaterial.specular, 1.0);
    } else {
        color += spec * gl_BackMaterial.specular;
    }
    
    gl_BackColor = color;
    
}

void DirectionalLight(in int i, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
    // 모두다 eye coordinate 가 되야겠지...
    float   nDotVP;   // 노멀과 빛 방향의 내적
    float   nDotHV;   // 노멀과 라이트 하프 벡터
    float   pf;       // 파워 팩터
    
    nDotVP = max( 0.0, dot( normal, normalize( vec3(gl_LightSource[i].position) ) ) );
    nDotHV = max( 0.0, dot( normal, vec3(gl_LightSource[i].halfVector) ) );
    
    if (nDotVP == 0.0) {
        pf == 0.0;
    } else {
        pf = pow( nDotHV, gl_FrontMaterial.shininess );
    }
    
    ambient += gl_LightSource[i].ambient;
    diffuse += gl_LightSource[i].diffuse * nDotVP;
    specular += gl_LightSource[i].specular * pf;
}

void PointLight(in int i, in vec3 eye, in vec3 ecPosition3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular) // eye 는 viewing direction
{
    float   nDotVP;         // 노멀과 빛 방향의 내적
    float   nDotHV;         // 노멀과 빛의 반사벡터
    float   pf;             // 파워팩터
    float   attenuation;    // 
    float   d;              // 표면에서 빛까지의 거리
    vec3    VP;             // 빛 방향
    vec3    halfVector;     // 빛의 반사 벡터
    
    VP = vec3(gl_LightSource[i].position) - ecPosition3;// 서피스에서 라이트 소스 위치까지
    
    d = length(VP);
    
    VP = normalize(VP);
    
    attenuation = 1.0 / (gl_LightSource[i].constantAttenuation + gl_LightSource[i].linearAttenuation * d + gl_LightSource[i].quadraticAttenuation * d * d);
    
    halfVector = normalize(VP + eye);
    
    nDotVP = max( 0.0, dot( normal, VP ) );
    nDotHV = max( 0.0, dot( normal, halfVector ) );
    
    if (nDotVP == 0.0) {
        pf == 0.0;
    } else {
        pf = pow( nDotHV, gl_FrontMaterial.shininess );
    }
    
    ambient += gl_LightSource[i].ambient * attenuation;
    diffuse += gl_LightSource[i].diffuse * nDotVP * attenuation;
    specular += gl_LightSource[i].specular * pf * attenuation;
}

void SpotLight(in int i, in vec3 eye, in vec3 ecPosition3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
    float   nDotVP;         // 노멀과 빛 방향의 내적
    float   nDotHV;         // 노멀과 빛의 반사벡터
    float   pf;             // 파워팩터
    float   spotDot;
    float   spotAttenuation;
    float   attenuation;    //
    float   d;              // 표면에서 빛까지의 거리
    vec3    VP;             // 표면에서 빛 방향
    vec3    halfVector;     // 빛의 반사 벡터
    
    VP = vec3(gl_LightSource[i].position) - ecPosition3;
    
    d = length(VP);
    
    VP = normalize(VP);
    
    attenuation = 1.0 / (gl_LightSource[i].constantAttenuation + gl_LightSource[i].linearAttenuation * d + gl_LightSource[i].quadraticAttenuation * d * d);
    
    // 스팟라이트 관련부분
    spotDot = dot(-VP, normalize(gl_LightSource[i].spotDirection) );
    
    if (spotDot < gl_LightSource[i].spotCosCutoff) {
        spotAttenuation = 0.0;
    } else {
        spotAttenuation = pow(spotDot, gl_LightSource[i].spotExponent);
    }
    
    attenuation *= spotAttenuation;
    
    // 스팟 라이트 관련 부분 끝.
    
    halfVector = normalize(VP + eye);
    
    nDotVP = max( 0.0, dot( normal, VP ) );
    nDotHV = max( 0.0, dot( normal, halfVector ) );
    
    if (nDotVP == 0.0) {
        pf == 0.0;
    } else {
        pf = pow( nDotHV, gl_FrontMaterial.shininess );
    }
    
    ambient += gl_LightSource[i].ambient * attenuation;
    diffuse += gl_LightSource[i].diffuse * nDotVP * attenuation;
    specular += gl_LightSource[i].specular * pf * attenuation;
}
