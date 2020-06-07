////////////////////////////////////////////////////////////////////////////////
// Filename: light.ps
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
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
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 viewDirection : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 LightPixelShader(PixelInputType input) : SV_TARGET
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