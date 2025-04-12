cbuffer Transform : register(b0) // Buffer de constantes contenant la matrice de transformation
{
    matrix World; // Matrice monde
    matrix WorldViewProj; // Matrice combinee
    
    // Parametres de highlight
    float highlightActive; 
    float highlightIntensity;
    float tilingFactor;
    float pad[2];
};

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION; // SV_POSITION est utilise pour les shaders modernes
    float3 worldPos : TEXCOORD1; // nouvelle donnee
    float3 normal : NORMAL;
    float4 color : COLOR;
    float2 tex : TEXCOORD;
    float highlightActive : TEXCOORD2;
    float highlightIntensity : TEXCOORD3;
    float tilingFactor : TEXCOORD4; 
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output;
    // output.pos = float4(input.pos, 1.0f);
    output.pos = mul(float4(input.pos, 1.0f), WorldViewProj); // Appliquer la transformation
    output.worldPos = mul(float4(input.pos, 1.0f), World).xyz;
    output.normal = input.normal;
    output.color = input.color;
    output.tex = input.tex;
    output.highlightActive = highlightActive;
    output.highlightIntensity = highlightIntensity;
    output.tilingFactor = tilingFactor;
    return output;
}
