#pragma pack_matrix( row_major )

struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 norm : NORMAL;
	float2 texc : TEXCOORD;
};

struct VertexShaderOutput
{
	float4 pos : SV_POSITION;
	float3 posW : POSITION;
	float2 texc : TEXCOORD0;
	float3 normW : NORMAL;

};

cbuffer __raeawegaweagw_ : register(b0)
{
	float4x4 wvp;
	float4x4 inw;
	float4 t;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	float4 pos = float4(input.pos + input.norm*float3(sin(t.x*.2f + input.pos.z * 3),
		cos(t.y*.6f + input.pos.y * 3), sin(t.x*.3f + input.pos.x * 3)), 1.0f);
	output.posW = pos;
	pos = mul(pos, wvp);
	//pos.z = (pos.z + 1) / 2;
	output.pos = pos;
	output.texc = input.texc;
	output.normW = mul(input.norm, inw);
	return output;
}
