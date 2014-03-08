struct psinput
{
	float4 pos : SV_POSITION;
	float3 posW : POSITION;
	float2 texc : TEXCOORD0;
	float3 normW : NORMAL;
};

Texture2D tex : register(t0);
SamplerState smp : register(s0);

struct psoutput
{
	float4 color : SV_TARGET;
};

psoutput main(psinput i)
{
	psoutput o;
	o.color = float4(tex.Sample(smp, i.texc) + (abs(i.normW)*max(0, dot(i.normW, float3(0, 1, 0)))) + 
	(abs(i.normW)*float3(.15f, .15f, .15f)), 1);
	return o;
}