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

cbuffer vs_reg_0 : register(b0)
{
	float4x4 wvp;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;
	float4 pos = float4(input.pos, 1.0f);
	output.posW = pos;
	pos = mul(pos, wvp);
	//pos.z = (pos.z + 1) / 2;
	output.pos = pos;
	output.texc = input.texc;
	output.normW = input.norm;
	return output;
}
