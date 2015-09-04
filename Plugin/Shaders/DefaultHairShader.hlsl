#include "GFSDK_HairWorks_ShaderCommon.h" 

GFSDK_HAIR_DECLARE_SHADER_RESOURCES(t0, t1, t2);

Texture2D	g_rootHairColorTexture	: register(t3);
Texture2D	g_tipHairColorTexture	: register(t4);

cbuffer cbPerFrame : register(b0)
{
    GFSDK_Hair_ConstantBuffer	g_hairConstantBuffer;
}


SamplerState texSampler: register(s0);


[earlydepthstencil] float4 ps_main(GFSDK_Hair_PixelShaderInput input) : SV_Target
{
    GFSDK_Hair_ShaderAttributes attr = GFSDK_Hair_GetShaderAttributes(input, g_hairConstantBuffer);
    GFSDK_Hair_Material mat = g_hairConstantBuffer.defaultMaterial;

    float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);

    if (GFSDK_Hair_VisualizeColor(g_hairConstantBuffer, mat, attr, color.rgb))
        return color;

    float3 hairColor = GFSDK_Hair_SampleHairColorTex(g_hairConstantBuffer, mat, texSampler, g_rootHairColorTexture, g_tipHairColorTexture, attr.texcoords);

#define NUM_LIGHTS 4
    // todo: 
    const float3 lightDir[NUM_LIGHTS] = 
    {
        float3(1, -2, -5),
        float3(1,-1,1),
        float3(-1,-1,1),
        float3(-1,-1,1)
    };

    const float3 lightColor[NUM_LIGHTS] = 
    {
        float3(1,1,1),
        float3(0.2,0.2,0.2),
        float3(0.1,0.1,0.1),
        float3(0.1,0.1,0.1)
    };

    [unroll]
    for (int i = 0; i < NUM_LIGHTS; i++)
    {
        float3 Lcolor = lightColor[i];
        float3 Ldir = normalize(lightDir[i]);
        color.rgb += GFSDK_Hair_ComputeHairShading(Lcolor, Ldir, attr, mat, hairColor.rgb);
    }

    return color;
}

