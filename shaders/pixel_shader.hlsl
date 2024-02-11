#pragma warning (disable : 3571)

Texture2D texDiffuse : register(t0);
Texture2D normalMap : register(t1);

SamplerState texSampler : register(s0);

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
    //return texDiffuse.Sample(texSampler, input.TexCoord);
	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
    //max(ambientColor * (), 0);
    // Debug shading #2: map and return texture coordinates as a color (blue = 0)
    //return float4(input.Normal * 0.5 + 0.5, 1);
    
    //return float4(input.TexCoord, 0, 1);
    
    //return (texDiffuse.Sample(texSampler, input.TexCoord));
    //float3 test = normalMap.Sample(normalSampler, input.TexCoord).xyz;
    //return float4(test, 1);

    


    float3x3 TBN = float3x3(normalize(input.Tangent), normalize(input.Binormal), normalize(input.Normal));
    TBN = transpose(TBN);
    

    float3 sampledNormal = normalMap.Sample(texSampler, input.TexCoord).rgb;
    sampledNormal = (sampledNormal * 2.0) - 1.0;
    
    float3 newNormal = normalize(mul(TBN, sampledNormal));

    float4 lightVector = normalize(lightPos - input.PosWorld);
    
    //start With normalMap:
    
    float4 diffuse = max(texDiffuse.Sample(texSampler, input.TexCoord) * dot(lightVector, float4(newNormal, 0)), 0);
    
    float4 reflection = float4(reflect(-lightVector.xyz, newNormal), 0);
    
    // end with normal Map
    
    
    //start Without normalMap:
    
    //float4 diffuse = max(texDiffuse.Sample(texSampler, input.TexCoord) * dot(lightVector, float4(input.Normal, 0)), 0);
    
    //float4 reflection = float4(reflect(-lightVector.xyz, input.Normal), 0);
    
    // end without normal map
    
    float4 camVector = normalize(camPos - input.PosWorld);
    float4 specular = specularColor * pow(max(0.0, dot(reflection, camVector)), specularColor.w);
    
    
    float4 phong = (ambientColor * 0.3f) + (diffuse * 0.5f) + (specular * 0.3f);
    
    
    return phong;
	
	
	

	
}


