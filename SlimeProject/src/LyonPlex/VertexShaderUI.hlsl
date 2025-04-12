cbuffer TransformBuffer : register(b0)
{
    matrix World;
    matrix OrthoProjection; // Matrice pour transformer en espace écran
};

struct VSInput
{
    float2 position : POSITION;
    float2 uv : TEXCOORD;
};

struct VSOutput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VSOutput VSMain(VSInput input)
{
    VSOutput output;
    

    float4 worldPos = mul(float4(input.position, 0.0f, 1.0f), World);

    // Transformation des coordonnées en espace écran
    //output.position = mul(float4(input.position, 0.0f, 1.0f), OrthoProjection);
    output.position = mul(worldPos, OrthoProjection);
    output.uv = input.uv;
    
    return output;
}