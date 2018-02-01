#version 330 core

const float consider_range = 0.08f;
const int maxPointLightCount = 10;
const int maxDirLightCount = 10;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TextCoord;
    vec4 FragPosLightSpace;
} fs_in;

// 材质属性结构体
struct MaterialAttr
{
    sampler2D diffuseMap;   // 使用纹理对象根据片元位置取不同的材质属性
    float shininess; //镜面高光系数
};

// 光源属性结构体
struct DirLight {
    bool enabled;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    bool enabled;
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform sampler2D texture_noise;
uniform sampler2D shadowMap;

uniform MaterialAttr material;
uniform DirLight dirLights[maxDirLightCount];
uniform PointLight pointLights[maxPointLightCount];
uniform vec3 viewPos;
uniform bool blinn;
uniform vec3 lightPos;

uniform float disolve_threshold;

out vec4 color;


// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 shadowLightDir);
vec4 GetColor();

void main()
{
    float value = fs_in.TextCoord.y;
    if(value < disolve_threshold - consider_range){
        discard;
    }

    if(value > disolve_threshold){
        color = GetColor();
        return;
    }

    // float noise_value = texture(texture_noise, vec2(fs_in.TextCoord.x, 0.5f)).r;
    float noise_value = texture(texture_noise, fs_in.TextCoord).r;
    value = 1 - (disolve_threshold - value) / consider_range;
    noise_value = noise_value * 1.0f;
    value = value + noise_value;

    if(value < 1.1f){
        discard;
    }

    if(value < 1.3f){
        color = vec4(1.0f, 1.0f, 1.0f, 0.3f);
        return;
    }

    if(value < 1.2f){
        color = vec4(1.0f, 1.0f, 1.0f, 0.8f);
        return;
    }

    color = GetColor();
}


vec4 GetColor(){
    // Properties
    vec3 norm = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    vec3 shadowLightDir = normalize(lightPos - fs_in.FragPos);
    vec3 result = vec3(0.0f,0.0f,0.0f);

    float shadow = ShadowCalculation(fs_in.FragPosLightSpace, norm, shadowLightDir);

    // Phase 1: Directional lighting
    for(int i = 0;i<dirLights.length();i++){
        if(dirLights[i].enabled==true){
             result += CalcDirLight(dirLights[i], norm, viewDir, shadow);
        }
    }
    // Phase 2: Point lights
    for(int i = 0;i<pointLights.length();i++){
        if(pointLights[i].enabled==true){
            result += CalcPointLight(pointLights[i], norm, fs_in.FragPos, viewDir, shadow);
        }
    }

    return vec4(result, 1.0);
}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(-light.direction);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal,halfwayDir),0.0),material.shininess);
    }
    else{
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    // Combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuseMap, fs_in.TextCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap, fs_in.TextCoord));
    vec3 specular = light.specular * spec;

    return (ambient*(2.0-shadow)*0.5 + (1.0 - shadow) * (diffuse + specular));
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float shadow)
{
    vec3 lightDir = normalize(light.position - fragPos);

    // Diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // Specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    if(blinn){
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal,halfwayDir),0.0),material.shininess);
    }
    else{
        spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    }

    // Attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // Combine results
    vec3 ambient = light.ambient * vec3(texture(material.diffuseMap, fs_in.TextCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap, fs_in.TextCoord));
    vec3 specular = light.specular * spec;

    return (ambient*(2.0-shadow)*0.5 + (1.0 - shadow) * (diffuse + specular)) * attenuation;
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 shadowLightDir)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;

    if(projCoords.z >= 1.0 || projCoords.z <= 0.0)
    // if(projCoords.z >= 1.0 )
        return 1.0;

    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // 取得当前片元在光源视角下的深度
    float currentDepth = projCoords.z;

    // 检查当前片元是否在阴影中
    float bias = max(0.00015 * (1.0 - dot(normal, shadowLightDir)), 0.00015);
    float shadow = (currentDepth - bias) > closestDepth  ? 1.0 : 0.0;

    return shadow;
}