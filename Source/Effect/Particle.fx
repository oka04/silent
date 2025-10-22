//���[���h�r���[�ˉe�ϊ��s��錾
float4x4 gMatWVP;

//�F���
float4 gColor;

//�e�N�X�`���[
texture gTexture;
sampler texSampler = sampler_state
{
	Texture = <gTexture>;

	minFilter = LINEAR;
	magFilter = LINEAR;
	mipFilter = LINEAR;

	AddressU = WRAP;
	AddressV = WRAP;
};

//���_�V�F�[�_�[���͗p
struct VS_INPUT
{
	float3 position : POSITION;
	float2 texCoord : TEXCOORD0;
};

//���_�V�F�[�_�[�o�͗p
struct VS_OUTPUT {
	float4 position : POSITION;
	float4 color    : COLOR0;
	float2 texCoord : TEXCOORD0;
};

//�s�N�Z���V�F�[�_�[���͗p
struct PS_INPUT {
	float4 position : POSITION;
	float4 color    : COLOR0;
	float2 texCoord : TEXCOORD0;
};

//�s�N�Z���V�F�[�_�[�o�͗p
struct PS_OUTPUT {
	float4 color : COLOR0;
};

// ���_�V�F�[�_
VS_OUTPUT ParticleVS(VS_INPUT In)
{
	VS_OUTPUT Out;

	//���_�ɕϊ��s�����Z����
	Out.position = mul(float4(In.position, 1.0f), gMatWVP);

	Out.color = gColor;

	//UV���W�̐ݒ�
	Out.texCoord = In.texCoord;

	return Out;
}

// �s�N�Z���V�F�[�_
PS_OUTPUT ParticlePS(PS_INPUT In)
{
	PS_OUTPUT Out;

	//�e�N�X�`���[�̐F���擾
	float4 texColor = tex2D(texSampler, In.texCoord);

	//�e�N�X�`���[�̐F�Ə�Z����
	Out.color = texColor * In.color;

	//�A���t�@�͕ʓr�ݒ�
	Out.color.a = (texColor.r + texColor.g + texColor.b) * In.color.a;

	return Out;
}

//�e�N�j�b�N
technique ParticleTec
{
   pass P0
   {
      VertexShader = compile vs_3_0 ParticleVS();
      PixelShader = compile ps_3_0 ParticlePS();

	  //�A���t�@�u�����h�ݒ�
	  AlphaBlendEnable = True;
	  SrcBlend = SRCALPHA;
	  DestBlend = ONE;

	  //�J�����OOFF
	  CullMode = NONE;
   }
}