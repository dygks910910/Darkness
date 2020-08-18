////////////////////////////////////////////////////////////////////////////////
// Filename: water_ps.hlsl
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
SamplerState SampleType;
Texture2D reflectionTexture;
Texture2D refractionTexture;
Texture2D normalTexture;

cbuffer WaterBuffer
{
	float waterTranslation;
	float reflectRefractScale;
	float2 padding;
};


//////////////
// TYPEDEFS //
//////////////
struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
	float4 reflectionPosition : TEXCOORD1;
    float4 refractionPosition : TEXCOORD2;
};


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 WaterPixelShader(PixelInputType input) : SV_TARGET
{
	float2 reflectTexCoord;
	float2 refractTexCoord;
	float4 normalMap;
	float3 normal;
	float4 reflectionColor;
	float4 refractionColor;
	float4 color;

	
	// �����̴� ���� �ùķ���Ʈ�ϱ� ���� �� �������� ���ø� �� ��ġ�� �̵��մϴ�.
	input.tex.y += waterTranslation;
	
	// ���� �� �ݻ� �ؽ�ó ��ǥ�� ����մϴ�.
    reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
    reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;
	
	// ���� �� ���� �ؽ�ó ��ǥ�� ����մϴ�.
    refractTexCoord.x = input.refractionPosition.x / input.refractionPosition.w / 2.0f + 0.5f;
    refractTexCoord.y = -input.refractionPosition.y / input.refractionPosition.w / 2.0f + 0.5f;

	// ��� �� �ؽ�ó�κ��� ������ ���ø��մϴ�.
	normalMap = normalTexture.Sample(SampleType, input.tex);

	// ������ ������ (0,1)���� (-1, + 1)�� Ȯ���մϴ�.
	normal = (normalMap.xyz * 2.0f) - 1.0f;

	// �ؽ�ó ��ǥ ���ø� ��ġ�� ��� �� ������ ���ġ�Ͽ� �ĵ� �� ȿ���� �ùķ���Ʈ�մϴ�.
	reflectTexCoord = reflectTexCoord + (normal.xy * reflectRefractScale);
	refractTexCoord = refractTexCoord + (normal.xy * reflectRefractScale);

	// ������Ʈ �� �ؽ�ó ��ǥ�� ����Ͽ� �ؽ�ó���� �ؽ�ó �ȼ��� ���ø��մϴ�.
    reflectionColor = reflectionTexture.Sample(SampleType, reflectTexCoord);
    refractionColor = refractionTexture.Sample(SampleType, refractTexCoord);

	// ���� ������ �ݻ� �� ���� ����� �����մϴ�.
	color = lerp(reflectionColor, refractionColor, 0.6f);
	
	return color;
}
