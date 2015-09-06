#include "GFSDK_HairWorks_ShaderCommon.h" 

#define MaxLights               4

#define LightType_Directional   0
#define LightType_Point         1

struct LightData
{
    int4 iparams;   // x: light type
    float4 position;// xyz: direction if directional light, position if point light
                    // w: range
    float4 color;
};


GFSDK_HAIR_DECLARE_SHADER_RESOURCES(t0, t1, t2);

Texture2D	g_rootHairColorTexture	: register(t3);
Texture2D	g_tipHairColorTexture	: register(t4);

cbuffer cbPerFrame : register(b0)
{
    int4                        g_numLights;        // x: num lights
    LightData                   g_lights[MaxLights];
    GFSDK_Hair_ConstantBuffer   g_hairConstantBuffer;
}


SamplerState texSampler: register(s0);


[earlydepthstencil]
float4 ps_main(GFSDK_Hair_PixelShaderInput input) : SV_Target
{
    GFSDK_Hair_ShaderAttributes attr = GFSDK_Hair_GetShaderAttributes(input, g_hairConstantBuffer);
    GFSDK_Hair_Material mat = g_hairConstantBuffer.defaultMaterial;

    float4 r = mat.rootColor;

    if (GFSDK_Hair_VisualizeColor(g_hairConstantBuffer, mat, attr, r.rgb)) {
        return r;
    }

    float3 hairColor = GFSDK_Hair_SampleHairColorTex(g_hairConstantBuffer, mat, texSampler, g_rootHairColorTexture, g_tipHairColorTexture, attr.texcoords);

    for (int i = 0; i < g_numLights.x; i++)
    {
        float3 Lcolor = g_lights[i].color.rgb;
        float3 Ldir = g_lights[i].position.xyz;
        r.rgb += GFSDK_Hair_ComputeHairShading(Lcolor, Ldir, attr, mat, hairColor);
    }
    //r.rgb = saturate(attr.N.xyz)*0.5+0.5;
    return r;
}

