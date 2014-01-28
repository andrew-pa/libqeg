struct psinput
{
	float4 pos : SV_POSITION;
	float3 posW : POSITION;
	float2 texc : TEXCOORD0;
	float3 normW : NORMAL;
};


float4 main(psinput i) : SV_TARGET
{
	return float4(0, 1, 0, 1);
}