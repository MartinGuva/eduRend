#pragma warning (disable : 3571)

Texture2D texDiffuse : register(t0);
Texture2D normalMap : register(t1);
TextureCube skyboxTexture : register(t2);

SamplerState texSampler : register(s0);

SamplerState skyboxSampler : register(s1);

cbuffer LightCamBuffer : register(b0)
{
    float4 lightPos;
    float4 camPos;
};
cbuffer MaterialBuffer : register(b1)
{
    float4 ambientColor; // w does not contain anything
    float4 diffuseColor; // w does not contain anything
    float4 specularColor; // w contains shinyness
    int skyBox;
};

struct PSIn
{
	float4 Pos  : SV_Position;
	float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
    float3 Binormal : BINORMAL;
	float2 TexCoord : TEX;
    float4 PosWorld : WPOS;

};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
    float4 diffuseTexture = texDiffuse.Sample(texSampler, input.TexCoord);
    // Set up normal map
    float3 sampledNormal = normalMap.Sample(texSampler, input.TexCoord).rgb;
    sampledNormal = (sampledNormal * 2.0) - 1.0;
    sampledNormal = normalize(sampledNormal);
    
    
    float3 T = normalize(input.Tangent);
    float3 B = normalize(input.Binormal);
    float3 N = normalize(input.Normal);
    float3x3 TBN = transpose(float3x3(T, B, N));

    float3 newNormal;
    
    if (skyBox == 0)
    {
        newNormal = normalize(mul(TBN, sampledNormal));
    }
    else if (skyBox == 1)
    {
        newNormal = N;
    }
    
    // Set up light and cam vectors

    float3 lightVector = normalize(lightPos.xyz - input.PosWorld.xyz);
    float3 camVector = normalize(camPos.xyz - input.PosWorld.xyz);
        

    float3 reflection = normalize(reflect(-lightVector, newNormal));
    
    float3 cubeReflect = normalize(reflect(camVector, newNormal));

    float4 reflectionCube = skyboxTexture.Sample(skyboxSampler, cubeReflect);

    float4 diffuse = diffuseTexture * max(0.0f, dot(lightVector, N));
    float4 specular = (reflectionCube * 0.5) * pow(max(0.0f, dot(reflection, camVector)), 5);
    

    
    
    if (skyBox == 1)
    {
        diffuse = skyboxTexture.Sample(skyboxSampler, camVector);
        specular = 0;
    }
    //specular += (reflectionCube * 0.025);

    return (ambientColor * 0.4) + (diffuse * 0.8) + (specular * 0.3);
    
    
}


