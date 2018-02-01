#version 330 core
const int maxPointLightCount = 10; 
const int maxDirLightCount = 10;
in vec3 FragPos;
in vec2 TextCoord;
in vec3 Normal;

out vec4 color;

// 材质属性结构体
struct MaterialAttr
{
	sampler2D diffuseMap;	// 使用纹理对象根据片元位置取不同的材质属性
	sampler2D specularMap;
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

uniform MaterialAttr material;
uniform DirLight dirLights[maxDirLightCount];
uniform PointLight pointLights[maxPointLightCount];
uniform vec3 viewPos;
uniform bool blinn;
// Function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
void main()
{   
	// 环境光成分
	//vec3	ambient = light.ambient * vec3(texture(material.diffuseMap, TextCoord));

	// 漫反射光成分 此时需要光线方向为指向光源
	//vec3	lightDir = normalize(light.position - FragPos);
	//vec3	normal = normalize(Normal);
	//float	diffFactor = max(dot(lightDir, normal), 0.0);
	//vec3	diffuse = diffFactor * light.diffuse * vec3(texture(material.diffuseMap, TextCoord));

	// 镜面反射成分 此时需要光线方向为由光源指出
	//float	specularStrength = 0.5f;
	//vec3	reflectDir = normalize(reflect(-lightDir, normal));
	//vec3	viewDir = normalize(viewPos - FragPos);
	//float	specFactor = pow(max(dot(reflectDir, viewDir), 0.0), material.shininess);
	//vec3	specular = specFactor * light.specular * vec3(texture(material.specularMap, TextCoord));

	//vec3	result = ambient + diffuse + specular;
	//color	= vec4(result , 1.0f);
	 // Properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 result=vec3(0.0f,0.0f,0.0f);
    // == ======================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == ======================================
    // Phase 1: Directional lighting
	for(int i = 0;i<dirLights.length();i++){
		if(dirLights[i].enabled==true){
			 result += CalcDirLight(dirLights[i], norm, viewDir);
		}
	}
	// Phase 2: Point lights
	for(int i = 0;i<pointLights.length();i++){
		if(pointLights[i].enabled==true){
			result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
		}
	}
    // Phase 3: Spot light
    // result += CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    
    color = vec4(result, 1.0);
    vec4 frozen = vec4(0.6,0.6,0.8,1.0);
    color = mix(color,frozen,0.8);
    color.a-=0.2;
}

// Calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
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
    vec3 ambient = light.ambient * vec3(texture(material.diffuseMap, TextCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap, TextCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap, TextCoord));
    return (ambient + diffuse + specular);
}

// Calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
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
    vec3 ambient = light.ambient * vec3(texture(material.diffuseMap, TextCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuseMap, TextCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specularMap, TextCoord));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;

	
    return (ambient +diffuse + specular);
}

