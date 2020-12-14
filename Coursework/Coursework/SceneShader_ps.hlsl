
#define MAX_LIGHTS 8
#define MAX_POINTLIGHTS 1
#define SMAP_SIZE 2048.f    /* Must match the shadow map size used in C++ */

// Textures
Texture2D texture0 : register(t0);
Texture2D depthMapTexture[MAX_LIGHTS] : register(t1);
//Texture2D pointLightDepthMap[MAX_POINTLIGHTS * 6] : register(t9); // 6 depth maps per point light, t9 because there's 8 lights from t1

// Samplers
SamplerState sampler0 : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
    float4 ambient;
    float4 diffuse[MAX_LIGHTS];
    float4 direction[MAX_LIGHTS];
    float4 position[MAX_LIGHTS];
    float4 specularity[MAX_LIGHTS];
    float4 attenValues[MAX_LIGHTS]; // w is padding
    
	/*
		position[i].w:
			0 = directional light
            1 = point light
			2 = spotlight

		direction[i].w = spotlight exponents

		specularity[i].xyz = colour
		specularity[i].w = specular power
		must set w to 1 when using it as colour

        attenValues:
		    x = constant
		    y = linear
		    z = quadratic 
        
        AttenValues[0].w = number of lights that aren't point lights
        AttenValues[1].w = number of lights that are point lights
	*/
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPosition : TEXCOORD1;
    float3 vertexCameraVector : TEXCOORD2;
    float4 lightViews[MAX_LIGHTS] : TEXCOORD3;
    //float4 pointLightView[MAX_POINTLIGHTS * 6] : TEXCOORD11; // 3 + MAX_LIGHTS
};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateDirectionalLight(float3 lightDirection, float3 normal, float4 ldiffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = saturate(ldiffuse * intensity);
    return colour;
}

float4 calculatePointLight(float3 lightPosition, float3 worldPosition, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, normalize(lightPosition - worldPosition)));
    float4 colour = saturate(diffuse * intensity);
    return colour;
}

float4 calculateSpotlightLighting(float3 lightPosition, float3 worldPosition, float3 lightDirection, float exponent, float4 diffuse)
{
    float dot_ = dot(-normalize(lightPosition - worldPosition), lightDirection);
    float intensity = pow(abs(dot_), exponent);
    float4 colour = diffuse * intensity;
    return colour;
}

float4 calculateSpecular(float3 lightDirection, float3 normal, float3 viewVector, float3 specularColour, float specularPower)
{
    // Blinn-Phong method
    float4 colour = float4(0, 0, 0, 1.f);
    colour.xyz = specularColour;
   
    float3 halfway = normalize(lightDirection + viewVector);
    float specularIntensity = pow(max(dot(normal, halfway), 0.0), specularPower);
    return saturate(colour * specularIntensity);

}

float calculateAttenuation(float constFactor, float linearFactor, float quadFactor, float3 lightPos, float3 worldPos)
{
    float distance = abs(length(worldPos - lightPos));
    return 1 / (constFactor + linearFactor * distance + quadFactor * pow(distance, 2));

}

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInFront(float3 lightPos, float3 lightDirection, float3 worldPos)
{
    float3 lightWorldVector = normalize(worldPos - lightPos);
    float angle = acos(dot(lightWorldVector, normalize(lightDirection)));
    
    if (angle <= radians(90.f))
        return true; 
    return false;
}

float getShadowFactor(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    const float SMAP_INC = 1.f / SMAP_SIZE;
    
    // Sample the shadow map to get the four nearest texels (it won't line up to just one texel perfectly, of course)
    float depth0 = sMap.Sample(shadowSampler, uv).r;
    float depth1 = sMap.Sample(shadowSampler, uv + float2(SMAP_INC, 0)).r;
    float depth2 = sMap.Sample(shadowSampler, uv + float2(0, SMAP_INC)).r;
    float depth3 = sMap.Sample(shadowSampler, uv + float2(SMAP_INC, SMAP_INC)).r;
    
	// Calculate the depth from the light
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;    // Apply bias
    
    // Get results for which texels are in shadow or not
    // Use float format so you can blend them afterwards
    float result0 = lightDepthValue <= depth0;
    float result1 = lightDepthValue <= depth1;
    float result2 = lightDepthValue <= depth2;
    float result3 = lightDepthValue <= depth3;
    
    // Convert to texel space
    float2 texelPos = SMAP_SIZE * uv;
    
    // Determine interpolation amounts
    // This returns the fractional part of the texel position which tells us which texels are closer
    float2 t = frac(texelPos);
    
    // Lerp to get results
    return lerp(lerp(result0, result1, t.x),    // Lerp top X axis
                lerp(result2, result3, t.x),    // Lerp bottom X axis
                t.y);                           // Lerp between X axes

}

float2 getProjectiveCoords(float4 lightViewPosition)
{
    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float4 calculateLighting(float3 normal, float3 worldPos, float3 vertexCameraVector, int i)
{
    if (position[i].w == 0)			// If this is a directional light
    {
        return calculateDirectionalLight(-direction[i].xyz, normal, diffuse[i]) + calculateSpecular(-direction[i].xyz, normal, vertexCameraVector, specularity[i].xyz, specularity[i].w);
    }
    else if (position[i].w == 1)	// If this is a point light
    {
        return (calculatePointLight(position[i].xyz, worldPos, normal, diffuse[i])
        + calculateSpecular(-direction[i].xyz, normal, vertexCameraVector, specularity[i].xyz, specularity[i].w))
        * calculateAttenuation(attenValues[i].x, attenValues[i].y, attenValues[i].z, position[i].xyz, worldPos);
    }
    else if (position[i].w == 2)    // If this is a spotlight
    {
        return (calculateSpotlightLighting(position[i].xyz, worldPos, direction[i].xyz, direction[i].w, diffuse[i])
        + calculateSpecular(-direction[i].xyz, normal, vertexCameraVector, specularity[i].xyz, specularity[i].w))
        * calculateAttenuation(attenValues[i].x, attenValues[i].y, attenValues[i].z, position[i].xyz, worldPos);
    }
    
    return float4(0, 0, 0, 1);
}

float4 main(InputType input) : SV_TARGET
{
    float4 textureColour = texture0.Sample(sampler0, input.tex);
    float4 finalLightColour = ambient; // Set ambient
    float shadowMapBias = 0.005f; // Default 0.005f

    // For each light
    float2 texCoords; // Get projected coordinates
    float shadowFactor; // 0 if in shadow, 1 if not, and all values inbetween
    bool bHasDepthData; // If coordinate is within light's view
    const int noOfLights = MAX_LIGHTS;
    for (int i = 0; i < noOfLights; i++)    // Process all but point lights
    {
        texCoords = getProjectiveCoords(input.lightViews[i]); // Get projected coordinates
        shadowFactor = getShadowFactor(depthMapTexture[i], texCoords, input.lightViews[i], shadowMapBias); // 0 if in shadow, 1 if not, and all values inbetween
        bHasDepthData = hasDepthData(texCoords); // If coordinate is within light's view
        
        //// If this is a point light, don't output data yet
        //shadowFactor *= position[i].w != 1; // Makes shadow factor 0 if this is a point light
        
        //// Check if behind surface (lights behind planes don't normally make the plane in shadow, this corrects that)
        //float angle = acos(dot(input.normal, direction[i].xyz) / (length(input.normal) * length(direction[i].xyz)));
        //float planeCheck = angle < degrees(40.f);    // 1 if in front of plane, 0 if not
        //shadowFactor *= planeCheck;
        
        if (isInFront(position[i].xyz, direction[i].xyz, input.worldPosition))  // Don't light behind (doesn't cull frustum behind)
        {
            if (bHasDepthData)    // If coordinate is within light's view
            {
                finalLightColour += calculateLighting(input.normal, input.worldPosition, input.vertexCameraVector, i) * shadowFactor;
            }
        }
    }
    //int pointLightCounter = 0;
    //for (int j = attenValues[1].w; j < noOfLights; j++)    // Only process point lights
    //{
    //    // This is a point light
    //    shadowFactor = 0; // Initialise
            
    //    [unroll]
    //    for (int k = 0; k < 6; k++) // For each shadow map (6 of them)
    //    {
    //        texCoords = getProjectiveCoords(input.pointLightView[(pointLightCounter * 6) + k]); // Get projected coordinates
    //        bHasDepthData = hasDepthData(texCoords); // If coordinate is within this shadow map
    //        shadowFactor += (float) bHasDepthData * getShadowFactor(pointLightDepthMap[(pointLightCounter * 6) + k], texCoords, input.pointLightView[(pointLightCounter * 6) + k], shadowMapBias); // The bool ensures that only the depth map that contians the coordiantes can make any difference
    //    }

    //    finalLightColour += calculateLighting(input.normal, input.worldPosition, input.vertexCameraVector, i) * shadowFactor;
        
    //    pointLightCounter++;
    //}
    finalLightColour = saturate(finalLightColour); // Saturate
	
    return saturate(finalLightColour * textureColour);
}
