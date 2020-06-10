////////////////////////////////////////////////////////////////////////////////
// Filename: light.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer CameraBuffer
{
    float3 cameraPosition;
    float padding;
}

Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
    float4 ambientColor;
    float4 diffuseColor;
    float3 lightDirection;
    float specularPower;
    float4 specularColor;
};


//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType VS(VertexInputType input)
{
    PixelInputType output;
    

	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
	// Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
	// Calculate the normal vector against the world matrix only.
    output.normal = mul(input.normal, (float3x3) worldMatrix);
	
    // Normalize the normal vector.
    output.normal = normalize(output.normal);

    float4 worldPosition = mul(input.position, worldMatrix);
    output.viewDirection = cameraPosition.xyz - worldPosition.xyz;

    output.viewDirection = normalize(output.viewDirection);

    return output;
}

float4 PS(PixelInputType input) : SV_TARGET
{
   	// �� �ؽ�ó ��ǥ ��ġ���� ���÷��� ����Ͽ� �ؽ�ó���� �ȼ� ������ ���ø��մϴ�.
    float4 textureColor = shaderTexture.Sample(SampleType, input.tex);

	// ��� �ȼ��� �⺻ ��� ������ �ֺ� ���� ������ �����մϴ�.
    float4 color = ambientColor;

	// specular color�� �ʱ�ȭ�Ѵ�.
    float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// ����� ���� �� ������ ������ŵ�ϴ�.
    float3 lightDir = -lightDirection;

	// �� �ȼ��� ���� ���� ����մϴ�.
    float lightIntensity = saturate(dot(input.normal, lightDir));

    if (lightIntensity > 0.0f)
    {
        // Ȯ�� ���� �� ������ �翡 ���� ���� Ȯ�� ���� �����մϴ�.
        color += (diffuseColor * lightIntensity);
    
    	// ���� ���� ������ ä ��ϴ�.
        color = saturate(color);

		// ���� ����, ���� ���� �� ���� ���⿡ ���� �ݻ� ���͸� ����մϴ�.
        float3 reflection = normalize(2 * lightIntensity * input.normal - lightDir);

		// �ݻ� ����, �ü� ���� �� �ݻ� ����� �������� �ݻ� ������ ���� �����մϴ�.
        specular = pow(saturate(dot(reflection, input.viewDirection)), specularPower);
    }

	// �ؽ�ó �ȼ��� ���� Ȯ�� ���� ���Ͽ� ���� �ȼ� ���� ����� ����ϴ�.
    color = color * textureColor;

	// ��� ������ �������� �ݻ� ������Ʈ�� �߰��մϴ�.
    color = saturate(color + specular);

    return color;
}


technique11 Light1
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}
