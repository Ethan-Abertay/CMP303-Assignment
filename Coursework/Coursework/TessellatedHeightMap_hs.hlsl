
cbuffer hullDataBuffer : register(b0)
{
    int tessellationFactor;
    float3 camPos;
    matrix worldMatrix;
    int maxTessDistance;      // The number of control patches around you that are tessellated 
    float3 padding;
};

struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

bool isWithinMaxTessRange (float3 centre)
{
     // Get world position of this control point's centre
    const float3 patchPos = mul(float4(centre, 1.f), worldMatrix);
    
     // Get the distance between the player and this patch's centre (not including height)
    const float3 playerPatchVector = camPos - patchPos;
    const float distance = length(playerPatchVector);
    
    // Determine if within max tessellation distance
    return distance <= maxTessDistance;
}

ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;
    
    const float baseTessFactor = 1.f;
    
    
    // Get the patch's centre
    const float3 centre = inputPatch[0].position + ((inputPatch[2].position - inputPatch[0].position) / 2);     // 0 and 2 are opposite ends of the square
    
    // Get the width of a patch
    const float width = length(inputPatch[0].position - inputPatch[1].position); // 0 and 1 are on the same edge (not opposites)
    
    // Determine if this patch is within max tessellation range
    const float isMaxTess = isWithinMaxTessRange(centre);
    
    // Determine the new tessellation factor
    const float tessFactor = baseTessFactor + (((float) tessellationFactor - baseTessFactor) * isMaxTess);
    
    // Determine which edge(s) need to be highly tessellated if this is an edge patch
    const float isBottom = isWithinMaxTessRange(centre - float3(0, 0, width)); // If the bottom edge needs to be highly tessellated
    const float isTop = isWithinMaxTessRange(centre + float3(0, 0, width)); // If the top edge needs to be highly tessellated
    const float isLeft = isWithinMaxTessRange(centre - float3(width, 0, 0));    // If the left edge needs to be highly tessellated
    const float isRight = isWithinMaxTessRange(centre + float3(width, 0, 0)); // If the right edge needs to be highly tessellated
    
    // Apply changes to edge tessellation factors
    const float bottomFactor = tessFactor + (((float) tessellationFactor - baseTessFactor) * !isMaxTess * isBottom);
    const float topFactor = tessFactor + (((float) tessellationFactor - baseTessFactor) * !isMaxTess * isTop);
    const float leftFactor = tessFactor + (((float) tessellationFactor - baseTessFactor) * !isMaxTess * isLeft);
    const float rightFactor = tessFactor + (((float) tessellationFactor - baseTessFactor) * !isMaxTess * isRight);
        
    // Set edges
    output.edges[0] = bottomFactor;
    output.edges[1] = leftFactor;
    output.edges[2] = topFactor;
    output.edges[3] = rightFactor;

    // Set inside
    output.inside[0] = tessFactor;
    output.inside[1] = tessFactor;

    return output;
}


[domain("quad")]
[partitioning("integer")] // Set partitioning 
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

	 // Pass info on
    output.position = patch[pointId].position;
    output.tex = patch[pointId].tex;
    output.normal = patch[pointId].normal;

    return output;
}