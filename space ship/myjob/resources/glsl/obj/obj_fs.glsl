#version 430

out vec4 Color;

//����
struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
    sampler2D texture_normal1;
    sampler2D texture_height1;
    //sampler2D material_texture;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;//�����
}; 

//ƽ�й�
struct DirLight {
    vec4 dir_switch;

    vec3 direction;//���շ���
    vec3 lightColor;//������ɫ

    vec3 ambient;//������
    vec3 diffuse;//������
    vec3 specular;//�߹�

    int powerlevel;
};

//���Դ
struct PointLight {
    vec4 point_switch;

    vec3 position;
    vec3 lightColor;
    
    //˥����
    float constant;
    float linear;
    float quadratic;
    int powerlevel;
	
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

//�۹�
struct SpotLight {
    vec4 spot_switch;

    vec3 lightColor;
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;
  
    float constant;
    float linear;
    float quadratic;
    int powerlevel;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};

//���ڹ���ǿ��
#define Light_Power_Level 6
struct LightPower
{
    float constant;
    float linear;
    float quadratic;
};

LightPower lightpower[Light_Power_Level]=
{
    {1.0f, 0.22f, 0.20f},
    {1.0f, 0.14f, 0.07f},
    {1.0f, 0.09f, 0.032f},
    {1.0f, 0.07f, 0.017f},
    {1.0f, 0.045f, 0.0075f},
    {1.0f, 0.027f, 0.0028f}
};

#define NR_POINT_LIGHTS 20

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform vec3 viewPos;//���λ��
uniform Material material;
uniform DirLight dirLight;
uniform PointLight pointlight[NR_POINT_LIGHTS];
uniform SpotLight spotLight;
uniform sampler2D shadowMap;

// function prototypes
vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir);

void main()
{
    vec3 normal = normalize(fs_in.Normal);
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);//�ӷ���㵽�۲���

    vec4 lingt_dir = dirLight.dir_switch * CalcDirLight(dirLight,normal,viewDir);

	    
    vec4 light_point;
    for(int i=0;i<NR_POINT_LIGHTS;i++)
        light_point += pointlight[i].point_switch * CalcPointLight(pointlight[i],normal,fs_in.FragPos,viewDir);

    vec4 light_spot = spotLight.spot_switch * CalcSpotLight(spotLight,normal,fs_in.FragPos,viewDir);
    
    //ģ�����˥��
    float sunshine = pow(-viewPos.z / 500.0f , 0.5)+0.1f;

    Color=lingt_dir * sunshine + light_point + light_spot;
}

vec4 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    // ������
    vec3 ambient = material.ambient * light.ambient * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));

    // ������ 
    vec3 lightDir = normalize(-light.direction);//����㵽��Դ������
    float diff = max(dot(normal, lightDir), 0.0);//��ˣ�С��0���ڱ��棬�������з���
    vec3 diffuse = material.diffuse * light.diffuse * (diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoords)));

    // �����
    vec3 halfwayDir = normalize(lightDir + viewDir);//Blinn-Phong����������������֮��ļн�
    //vec3 reflectDir = reflect(-lightDir, normal);  //reflect�����涨��һ�������ǹ�Դ������������
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);//����������۲�������ˣ�ԽС��н�ԽС���߹�Խ��
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    vec3 specular = material.specular * light.specular * (spec * vec3(texture(material.texture_specular1, fs_in.TexCoords)));  

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace,normal,lightDir);   
    vec3 color = texture(material.texture_diffuse1, fs_in.TexCoords).rgb;
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * light.lightColor * color;

    result=(ambient+diffuse+specular)*light.lightColor;

    return vec4(result, 1.0);
}

// calculates the color when using a point light.
vec4 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // ������
    vec3 ambient = material.ambient * light.ambient * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    
    //������
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = material.diffuse * light.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));
    
    // �����
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    float distance = length(light.position - fragPos);//��Դ����������
    vec3 specular = material.specular * light.specular * spec * vec3(texture(material.texture_specular1, fs_in.TexCoords));

    //����˥�� 
    float attenuation = 1.0 / (lightpower[light.powerlevel].constant + lightpower[light.powerlevel].linear * distance 
    + lightpower[light.powerlevel].quadratic * (distance * distance));   

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace,normal,lightDir);                      
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * light.lightColor * attenuation;

    result=(ambient+diffuse+specular)*light.lightColor*attenuation;

    return vec4(result, 1.0);
}

// calculates the color when using a spot light.
vec4 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    // ������
    vec3 ambient = material.ambient * light.ambient * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));

    //������
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = material.diffuse * light.diffuse * diff * vec3(texture(material.texture_diffuse1, fs_in.TexCoords));

    // �����
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    float distance = length(light.position - fragPos);//��Դ����������
    vec3 specular = material.specular * light.specular * spec * vec3(texture(material.texture_specular1, fs_in.TexCoords));

    //����˥�� 
    float attenuation = 1.0 / (lightpower[light.powerlevel].constant + lightpower[light.powerlevel].linear * distance 
    + lightpower[light.powerlevel].quadratic * (distance * distance)); 
    
    // spotlight intensity��Եģ����
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // calculate shadow
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace,normal,lightDir);                      
    vec3 result = (ambient + (1.0 - shadow) * (diffuse + specular)) * light.lightColor * attenuation * intensity;

    result = intensity * light.lightColor * (ambient + diffuse + specular);

    return vec4(result, 1.0);
}

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    //��Ӱƫ��
    //float bias = 0.005;
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    //float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    //PCF��ֵģ����Ӱ��Ե
    float shadow = 0.0f;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    //������׶����Ϊ��Ӱ0.0f,����Զ��������û����Ӱ��1.0f�����ִ�������
    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}