#version 410 core  
out vec4 ourColour;  
uniform vec3 lightColor;
uniform float time;  

in vec3 Normal;
in vec3 FragPos;
uniform vec3 viewPos;

struct LightStruct {
    vec3 ambiant;
    vec3 diffuse;
    vec3 specular;
    vec3 position;
};

uniform LightStruct Light_1; 

struct Material {
    vec3 ambiant;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

void main() {  
    vec3 ambient = Light_1.ambiant * material.ambiant;
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(Light_1.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0f);

    vec3 diffuse = Light_1.diffuse * diff * material.diffuse;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f);

    vec3 specular = Light_1.specular * spec * material.specular;
    
    vec3 result = ambient + diffuse + specular;
    ourColour = vec4(result, 1.0); 
}  