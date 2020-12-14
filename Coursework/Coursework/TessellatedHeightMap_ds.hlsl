
#define MAX_LIGHTS 8
#define MAX_POINTLIGHTS 1

Texture2D heightMap : register(t0);
Texture2D normalMap : register(t1);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer DataBuffer : register(b1)
{
    matrix lightViewMatrix[MAX_LIGHTS];
    matrix lightProjectionMatrix[MAX_LIGHTS];
    matrix pointLightViewMatrix[MAX_POINTLIGHTS * 6];
    matrix pointLightProjectionMatrix[MAX_POINTLIGHTS];
    float4 camPos;  // w is max height
};

// Having this second buffer makes the class heirarchy system easier to manage in C++
cbuffer DataBuffer2 : register(b2)
{
    int maxTessRange;
    float3 padding;
    //matrix rotationMatrix;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
    float3 position : POSITION;
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

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    OutputType output;
 
    // Get variable from constant buffer
    const float maxHeight = camPos.w;
    
    // Determine the position of the new vertex.
    const float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    const float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);
    
    // Determine the new texture coordinate of this vertex
    const float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    const float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
    output.tex = lerp(t1, t2, uvwCoord.x);
    
    // Determine the new normal coordinate of this vertex
    //const float3 n1 = lerp(patch[0].normal, patch[1].normal, uvwCoord.y);
    //const float3 n2 = lerp(patch[3].normal, patch[2].normal, uvwCoord.y);
    //output.normal = lerp(n1, n2, uvwCoord.x);

    // Heightmap Displacement
    //// Code to decrease amplitude over distance
    //const float currentRange = length(mul(float4(vertexPosition, 1.f), worldMatrix).xz - camPos.xz); // Get the range of the distance from player to vertex, not including height 
    //float distFactor = 1.f - (currentRange / (float) maxTessRange); // Linearly increases to zero at max tessellation range
    //const float isOver0f = distFactor >= 0; // Detects if the current range is greater than the max range (it requires clamping)
    //distFactor = distFactor * isOver0f; // Apply the clamping if it was needed
    const float distFactor = 1.0f;
    vertexPosition.y += maxHeight * distFactor * heightMap.SampleLevel(sampler0, output.tex, 0).x; // .tif optimises to only use red colour channel
    
    // Normal Map
    float3 normalMapSample = (normalMap.SampleLevel(sampler0, output.tex, 0).xyz); // The sample
    // I can do this because these planes are created facing up, all normals are (0, 1, 0) before normal mapping of course
    // +ve blue (normal map z) goes to +ve Y
    // +ve red (normal map x) goes to +ve x
    //+ve green (normal map y) goes to +ve z
    output.normal = float3(normalMapSample.x, normalMapSample.z, normalMapSample.y); // Put normal coords in right place (when this is a plane facing upwards)
    //output.normal = float3(0, 1, 0);
    output.normal = normalize(mul(output.normal, (float3x3) worldMatrix));  // Rotate to correct position

    // Calculate the position of the new vertex with the world, view, and projection matrices.
    output.position = float4(vertexPosition, 1.0f);
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Get the world position
    output.worldPosition = mul(float4(vertexPosition, 1.f), worldMatrix).xyz;
    
    // Output view vector
    output.vertexCameraVector = normalize(camPos.xyz - output.worldPosition.xyz);
	
    // Get the light views
    const int noOfLights = MAX_LIGHTS;
    [unroll]
    for (int i = 0; i < noOfLights; i++)
    {
        float4 val = mul(float4(vertexPosition, 1.0f), worldMatrix);
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
    //        float4 val = mul(float4(vertexPosition, 1.0f), worldMatrix);`
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

