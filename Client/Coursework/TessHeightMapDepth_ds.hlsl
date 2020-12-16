
Texture2D texture0 : register(t0);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

// Having this second buffer makes the class heirarchy system easier to manage in C++
cbuffer DataBuffer2 : register(b1)
{
    int maxTessRange;
    float3 padding;
    float4 camPos; // w is max height
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
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD1;

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
    const float2 tex = lerp(t1, t2, uvwCoord.x);
    
    // Heightmap Displacement
    // Heightmap Displacement
    //// Code to decrease amplitude over distance
    //const float currentRange = length(mul(float4(vertexPosition, 1.f), worldMatrix).xz - camPos.xz); // Get the range of the distance from player to vertex, not including height 
    //float distFactor = 1.f - (currentRange / (float) maxTessRange); // Linearly increases to zero at max tessellation range
    //const float isOver0f = distFactor >= 0; // Detects if the current range is greater than the max range (it requires clamping)
    //distFactor = distFactor * isOver0f; // Apply the clamping if it was needed
    const float distFactor = 1.0f;
    vertexPosition.y += maxHeight * distFactor * texture0.SampleLevel(sampler0, tex, 0).x; // .tif optimises to only use red colour channel
    
    // Calculate the position of the new vertex with the world, view, and projection matrices.
    output.position = float4(vertexPosition, 1.0f);
    output.position = mul(output.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    // Store the position value in a second input value for depth value calculations.
    output.depthPosition = output.position;

    return output;
}

