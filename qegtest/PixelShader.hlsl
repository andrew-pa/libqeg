

float szzzz(in float x, out float w, inout float z)
{
	w = x*z;
	z = sin(x);
	return cos(z);
}

float4 main() : SV_TARGET
{
	return float4(float3(1.f, float2(1.f, 1.f)), 1.f);
}