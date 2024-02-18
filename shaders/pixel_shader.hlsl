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

    
    // Set up normal map
    
    
    float3x3 TBN = float3x3(normalize(input.Tangent), normalize(input.Binormal), normalize(input.Normal));
    
    TBN = transpose(TBN);

    float3 sampledNormal = normalMap.Sample(texSampler, input.TexCoord).xyz;
    sampledNormal = (sampledNormal * 2.0) - 1.0;
    sampledNormal = normalize(sampledNormal);
    
    float3 newNormal = mul(TBN, sampledNormal);
    
    
    // Set up light and cam vectors

    float4 lightVector = normalize(lightPos - input.PosWorld);
    float4 camVector = normalize(camPos - input.PosWorld);
    
    //Get textures
    
    float4 cubeTexture;
    
    cubeTexture = skyboxTexture.Sample(skyboxSampler, camVector.xyz);
    
    float4 diffuseTexture;
    diffuseTexture = texDiffuse.Sample(texSampler, input.TexCoord);


    //float3 cubeReflect = reflect(camVector.xyz, input.Normal);

    //float4 reflectionCube;
    //reflectionCube = skyboxTexture.Sample(skyboxSampler, cubeReflect);
    
    
    
    

    
    //start With normalMap:
    
    //float4 diffuse = max(diffuseTexture * dot(lightVector, float4(newNormal, 0)), 0);
    
    //float4 reflection = float4(reflect(-lightVector.xyz, newNormal), 0);
    

    // end with normal Map
    
    
    //start Without normalMap:
    
    float4 diffuse = max(diffuseTexture * dot(lightVector, float4(input.Normal, 0)), 0);
    
    float4 reflection = float4(reflect(-lightVector.xyz, input.Normal), 0);
    
    // end without normal map
    
    
    float4 specular = specularColor * pow(max(0.0, dot(reflection, camVector)), specularColor.w);
    
    
    //// Only have this here for reflection:
    //specular *= reflectionCube;
    ////
    float4 phong = (ambientColor * 0.4) + (diffuse * 0.8) + (specular * 0.4);
	
    return phong;
}


