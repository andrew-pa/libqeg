#pragma pack_matrix( row_major )
struct __shader_input_t
{
float4 pos : SV_POSITION;
float3 posW : POSITION;
float3 normW : NORMAL;
float2 texc : TEXCOORD;
};
struct __shader_output_t
{
float4 color : SV_TARGET0;
};
Texture2D tex : register(t0);
SamplerState ___smp_tex : register(s0);
__shader_output_t main(__shader_input_t input){
__shader_output_t output;
output.color = float4((tex.Sample(___smp_tex, input.texc) + abs(input.normW) * max(0, dot(input.normW, float3(0, 1, 0)))) + abs(input.normW) * float3(.2, .2, .2), 1);
return output;
}