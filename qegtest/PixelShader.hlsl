float4 main() : SV_TARGET
{
	float4x4 megadata[2][3][3][4];
	float4x4 stuf = megadata[0][ 1][ 2][ 3];
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}