#pragma pack_matrix( row_major )
struct __shader_input_t
{
float3 pos : POSITION;
float3 norm : NORMAL;
float2 tex : TEXCOORD;
};
struct __shader_output_t
{
float4 pos : SV_POSITION;
float3 posW : POSITION;
float3 normW : NORMAL;
float2 texc : TEXCOORD;
};
cbuffer ___reg_0___ : register(b0){
float4x4 wvp;
float4x4 inw;
float4 t;
};
__shader_output_t main(__shader_input_t input){
__shader_output_t output;
float4 v =  float4(input.pos + input.norm * float3(sin(t.x * .2 + input.pos.z * 3.0), cos(t.x * .6 + input.pos.y * 3.0), sin(t.x * .3 + input.pos.x * 3.0)), 1);
float4 p = mul(v, wvp);
output.pos = p;
output.posW = input.pos;
output.normW = mul(float4(input.norm, 0), inw).xyz;
output.texc = input.tex;
return output;
}