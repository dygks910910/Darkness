////////////////////////////////////////////////////////////////////////////////
// Filename: glass_ps.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
SamplerState SampleType;
Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D refractionTexture : register(t2);

cbuffer GlassBuffer
{
    float refractionScale;
    float3 padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float4 refractionPosition : TEXCOORD1;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 GlassPixelShader(PixelInputType input) : SV_TARGET
{
	float2 refractTexCoord;
	float4 normalMap;
	float3 normal;
	float4 refractionColor;
	float4 textureColor;
	float4 color;

	
	// ���� �� ���� �ؽ�ó ��ǥ�� ����մϴ�.
	refractTexCoord.x = input.refractionPosition.x / input.refractionPosition.w / 2.0f + 0.5f;
	refractTexCoord.y = -input.refractionPosition.y / input.refractionPosition.w / 2.0f + 0.5f;

	// ��� �� �ؽ�ó�κ��� ������ ���ø��մϴ�.
	normalMap = normalTexture.Sample(SampleType, input.tex);

	// ������ ������ (0,1)���� (-1, + 1)�� Ȯ���մϴ�.
	normal = (normalMap.xyz * 2.0f) - 1.0f;

	// ������ ���� ������ �ְ��� �ùķ���Ʈ�ϱ� ���� �ؽ�ó ��ǥ ���ø� ��ġ�� ���� �� ������ �ٽ� ��ġ�մϴ�.
	refractTexCoord = refractTexCoord + (normal.xy * refractionScale);

	// ���� �� �ؽ�ó ��ǥ�� ����Ͽ� ���� �ؽ�ó���� �ؽ�ó �ȼ��� ���ø��մϴ�.
	refractionColor = refractionTexture.Sample(SampleType, refractTexCoord);

	// ������ �ؽ�ó���� �ؽ�ó �ȼ��� ���ø��մϴ�.
	textureColor = colorTexture.Sample(SampleType, input.tex);

	// ���� ������ ���� ����� ���� ���� �յ��ϰ� �����մϴ�.
    color = lerp(refractionColor, textureColor, 0.5f);

    return color;
}
