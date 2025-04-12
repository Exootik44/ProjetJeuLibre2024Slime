Texture2D textureUI : register(t0); // Texture de l'UI
SamplerState samplerUI : register(s0); // Échantillonneur

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 PSMain(PSInput input) : SV_TARGET
{
    return textureUI.Sample(samplerUI, input.uv); // Retourne la couleur de la texture
}