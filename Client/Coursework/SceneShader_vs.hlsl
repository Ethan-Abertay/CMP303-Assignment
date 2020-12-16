
#define MAX_LIGHTS 8
#define MAX_POINTLIGHTS 1

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer LightMatrixBuffer : register(b1)
{
    matrix lightViewMatrix[MAX_LIGHTS];
    matrix lightProjectionMatrix[MAX_LIGHTS];
    matrix pointLightViewMatrix[MAX_POINTLIGHTS * 6];
    matrix pointLightProjectionMatrix[MAX_POINTLIGHTS];
    float4 cameraPosition;
};

struct InputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 vertexCameraVector : TEXCOORD2;
    float4 lightViews[MAX_LIGHTS] : TEXCOORD3;
    //float4 pointLightView[MAX_POINTLIGHTS * 6] : TEXCOORD11; // 3 + MAX_LIGHTS
};

OutputType main(InputType input)
{
    OutputType output;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

	// Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
    output.normal = normalize(output.normal);

    // Get the world position
    output.worldPosition = mul(input.position, worldMatrix).xyz;
    
    // Output view vector
    output.vertexCameraVector = normalize(cameraPosition.xyz - output.worldPosition.xyz);
	
    // Get the light views
    const int noOfLights = MAX_LIGHTS;
    [unroll]
    for (int i = 0; i < noOfLights; i++)
    {
        float4 val = mul(input.position, worldMatrix);
        val = mul(val, lightViewMatrix[i]);
        val = mul(val, lightProjectionMatrix[i]);
        output.lightViews[i] = val;
        
        //output.lightViews[i] = mul(input.position, worldMatrix);
        //output.lightViews[i] = mul(output.lightViews[i], lightViewMatrix[i]);
        //output.lightViews[i] = mul(output.lightViews[i], lightProjectionMatrix[i]);
    }
    
    //const int noOfPointLights = MAX_POINTLIGHTS;
    //int counter = 0;
    //for (int j = 0; j < noOfPointLights; j++)   // For each point light
    //{
    //    for (int k = 0; k < 6; k++) // For each shadow map for each point light (6 of them)
    //    {
    //        counter = (j * 6) + k;
    //        float4 val = mul(input.position, worldMatrix);
    //        val = mul(val, pointLightViewMatrix[counter]);
    //        val = mul(val, pointLightProjectionMatrix[j]);
    //        output.pointLightView[counter] = val;
            
    //        //output.pointLightView[counter] = mul(input.position, worldMatrix);
    //        //output.pointLightView[counter] = mul(output.pointLightView[counter], pointLightViewMatrix[counter]);
    //        //output.pointLightView[counter] = mul(output.pointLightView[counter], pointLightProjectionMatrix[j]);
    //    }
    //}
    
     return output;
}