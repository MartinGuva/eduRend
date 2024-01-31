
Texture2D texDiffuse : register(t0);

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
	float2 TexCoord : TEX;
    float4 PosWorld : WPOS;
};

//-----------------------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------------------

float4 PS_main(PSIn input) : SV_Target
{
	// Debug shading #1: map and return normal as a color, i.e. from [-1,1]->[0,1] per component
	// The 4:th component is opacity and should be = 1
    //max(ambientColor * (), 0);
	
    float4 lightVector = normalize(lightPos - input.PosWorld);
    float4 diffuse = max(diffuseColor * dot(lightVector, float4(input.Normal, 0)), 0);
    
    float4 camVector = normalize(camPos - input.PosWorld);
    
    float4 reflection = lightVector - 2.0 * dot(lightVector, float4(input.Normal, 0)) * float4(input.Normal, 0);
    
    float4 specular = max(0.0, pow(abs(dot(reflection, camVector)), 2));
    
    return float4((ambientColor * 0.5f) + (diffuse * 0.5f) + (specular * 0.5));
	
	return float4(input.Normal*0.5+0.5, 1);
	
	// Debug shading #2: map and return texture coordinates as a color (blue = 0)
	return float4(input.TexCoord, 0, 1);
}