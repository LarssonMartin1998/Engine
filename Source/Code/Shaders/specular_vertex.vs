cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer CameraBuffer
{
	float3 cameraPosition;
	float pad01;	
};

struct VertexInput
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 viewDirection : TEXCOORD1;
};

////////////////////////////////////////////////////////////////////

PixelInput SpecularVertexShader(VertexInput input)
{
	PixelInput output;

	// Change the position vector to be 4 units for proper matrix calculations.
	input.position.w = 1.0;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Calculate the normal vector against the world matrix only.
	output.normal = mul(input.normal, worldMatrix);
	output.normal = normalize(output.normal);

	output.viewDirection = cameraPosition.xyz - output.position.xyz;
	output.viewDirection = normalize(output.viewDirection);

	output.tex = input.tex;

	return output;
}