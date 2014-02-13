struct psinput
{
	float4 pos : SV_POSITION;
	float3 posW : POSITION;
	float2 texc : TEXCOORD0;
	float3 normW : NORMAL;
};


float4 main(psinput i) : SV_TARGET
{
	return float4((abs(i.normW)*max(0, dot(i.normW, float3(0, 1, 0)))) + 
	(abs(i.normW)*float3(.15f, .15f, .15f)), 1);
}