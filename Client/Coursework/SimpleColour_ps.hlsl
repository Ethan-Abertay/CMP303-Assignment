
struct InputType
{
    float4 position : POSITION;
};

float4 main(InputType input) : SV_TARGET
{
	return float4(1.0f, 0.25f, 0.25f, 1.0f);
}