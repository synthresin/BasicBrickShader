#version 120

uniform vec3 LightPosition;
uniform vec3 LightColor;
uniform vec3 Specular;
uniform vec3 Ambient;
uniform vec3 Kd;

varying vec3 DiffuseColor;
varying vec3 SpecularColor;

void main()
{
    vec3 ecLight = vec3( gl_ModelViewMatrix * vec4(LightPosition, 1.0) );
    vec3 ecPosition = vec3(gl_ModelViewMatrix * gl_Vertex);
    vec3 tnorm = normalize(gl_NormalMatrix * gl_Normal);
    vec3 lightVec = normalize(ecLight - ecPosition);
    vec3 viewVec  = normalize(-ecPosition);
    vec3 hvec = normalize(viewVec + lightVec);
    
    float spec = clamp(dot(hvec, tnorm), 0.0, 1.0);
    spec = pow(spec, 16.0);
    
    DiffuseColor = LightColor * vec3( Kd * dot(lightVec, tnorm)) ;
    DiffuseColor = clamp(Ambient + DiffuseColor, 0.0, 1.0);
    SpecularColor = clamp((LightColor * Specular * spec), 0.0, 1.0);
    
    gl_TexCoord[0] = gl_MultiTexCoord0;
    gl_Position = ftransform();
}
