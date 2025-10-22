//�r���[�ˉe�ϊ��s��錾
float4x4 gMatVP;

//�����s��
float4x4 gMatW[4];

//���_�ɉe����^����{�[���̐�
int gNumMaxInfle;

//�A���r�G���g���C�g�̐F
float4 gAmbientColor;

//�f�B���N�V���i�����C�g�̕���
float3 gLightDir;

//�f�B���N�V���i�����C�g�̐F
float4 gLightColor;

//�}�e���A���̐F
float4 gMaterialDiffuse;

//�}�e���A���̃A���r�G���g
float4 gMaterialAmbient;

//�e�N�X�`���[
texture gTexture;
sampler texSampler = sampler_state
{
	Texture = <gTexture>;

	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = LINEAR;

	AddressU = WRAP;
	AddressV = WRAP;
};

//���_�V�F�[�_�[���͗p
struct VS_INPUT_PB1NT
{
	float3 position : POSITION;
	float  blend    : BLENDWEIGHT;
	float3 normal   : NORMAL;
	float2 texCoord : TEXCOORD0;
};

struct VS_INPUT_PB3NT
{
	float3 position : POSITION;
	float3 blend    : BLENDWEIGHT;
	float3 normal   : NORMAL;
	float2 texCoord : TEXCOORD0;
};

struct VS_INPUT_PB1NCT
{
	float3 position : POSITION;
	float  blend : BLENDWEIGHT;
	float3 normal   : NORMAL;
	float4 color    : COLOR0;
	float2 texCoord : TEXCOORD0;
};

struct VS_INPUT_PB2NCT
{
	float3 position : POSITION;
	float2 blend    : BLENDWEIGHT;
	float3 normal   : NORMAL;
	float4 color    : COLOR0;
	float2 texCoord : TEXCOORD0;
};

struct VS_INPUT_PB3NCT
{
	float3 position : POSITION;
	float3 blend    : BLENDWEIGHT;
	float3 normal   : NORMAL;
	float4 color    : COLOR0;
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
VS_OUTPUT TextureVS_PB1NT(VS_INPUT_PB1NT In)
{
	VS_OUTPUT Out;

	float4x4 comb = (float4x4)0;

	//�����s��Ƀu�����h�l���������[���h���W�ϊ��s����쐬
	int i;
	for (i = 0; i < gNumMaxInfle - 1; i++) {
		comb += gMatW[i] * In.blend;
	}
	comb += gMatW[i] * (1.0f - In.blend);

	//���_�𒲐�
	Out.position = mul(float4(In.position, 1.0f), comb);
	Out.position = mul(Out.position, gMatVP);

	//�@���𒲐�
	float4 normal = mul(float4(In.normal, 0.0f), comb);

	//�f�B���N�V���i�����C�g�̕������t�ɂ���i���̌v�Z�̂��߁j
	float4 revLightDir = -float4(gLightDir, 0.0f);

	//�@���ƃf�B���N�V���i�����C�g�̕����̓��ςɃf�B���N�V���i�����C�g�̐F����Z���A���r�G���g���C�g�̐F�Ɖ��Z
	//saturate�@���@���ʂ�0.0����1.0�܂ł͈̔͂Ɏ��߂�
	Out.color = saturate(gAmbientColor * gMaterialAmbient + gLightColor * gMaterialDiffuse * max(0, dot(revLightDir, normal)));

	//UV���W�̐ݒ�
	Out.texCoord = In.texCoord;

	return Out;
}

// ���_�V�F�[�_
VS_OUTPUT TextureVS_PB3NT(VS_INPUT_PB3NT In)
{
	VS_OUTPUT Out;

	float blend[3] = (float[3])In.blend;

	float4x4 comb = (float4x4)0;

	//�����s��Ƀu�����h�l���������[���h���W�ϊ��s����쐬
	int i;
	for (i = 0; i < gNumMaxInfle - 1; i++) {
		comb += gMatW[i] * blend[i];
	}
	comb += gMatW[i] * (1.0f - blend[0] - blend[1] - blend[2]);

	//���_�𒲐�
	Out.position = mul(float4(In.position, 1.0f), comb);
	Out.position = mul(Out.position, gMatVP);

	//�@���𒲐�
	float4 normal = mul(float4(In.normal, 0.0f), comb);

	//�f�B���N�V���i�����C�g�̕������t�ɂ���i���̌v�Z�̂��߁j
	float4 revLightDir = -float4(gLightDir, 0.0f);

	//�@���ƃf�B���N�V���i�����C�g�̕����̓��ςɃf�B���N�V���i�����C�g�̐F����Z���A���r�G���g���C�g�̐F�Ɖ��Z
	//saturate�@���@���ʂ�0.0����1.0�܂ł͈̔͂Ɏ��߂�
	Out.color = saturate(gAmbientColor * gMaterialAmbient + gLightColor * gMaterialDiffuse * max(0, dot(revLightDir, normal)));

	//UV���W�̐ݒ�
	Out.texCoord = In.texCoord;

	return Out;
}

VS_OUTPUT TextureVS_PB1NCT(VS_INPUT_PB1NCT In)
{
	VS_OUTPUT Out;

	float4x4 comb = (float4x4)0;

	//�����s��Ƀu�����h�l���������[���h���W�ϊ��s����쐬
	int i;
	for (i = 0; i < gNumMaxInfle - 1; i++) {
		comb += gMatW[i] * In.blend;
	}
	comb += gMatW[i] * (1.0f - In.blend);
	
	//���_�𒲐�
	Out.position = mul(float4(In.position, 1.0f), comb);
	Out.position = mul(Out.position, gMatVP);

	//�@���𒲐�
	float4 normal = mul(float4(In.normal, 0.0f), comb);

	//�f�B���N�V���i�����C�g�̕������t�ɂ���i���̌v�Z�̂��߁j
	float4 revLightDir = -float4(gLightDir, 0.0f);

	//�@���ƃf�B���N�V���i�����C�g�̕����̓��ςɃf�B���N�V���i�����C�g�̐F����Z���A���r�G���g���C�g�̐F�Ɖ��Z
	//saturate�@���@���ʂ�0.0����1.0�܂ł͈̔͂Ɏ��߂�
	Out.color = saturate(gAmbientColor * gMaterialAmbient + gLightColor * gMaterialDiffuse * max(0, dot(revLightDir, normal)) * In.color);

	//UV���W�̐ݒ�
	Out.texCoord = In.texCoord;

	return Out;
}

VS_OUTPUT TextureVS_PB2NCT(VS_INPUT_PB2NCT In)
{
	VS_OUTPUT Out;

	float blend[2] = (float[2])In.blend;

	float4x4 comb = (float4x4)0;

	//�����s��Ƀu�����h�l���������[���h���W�ϊ��s����쐬
	int i;
	for (i = 0; i < gNumMaxInfle - 1; i++) {
		comb += gMatW[i] * In.blend[i];
	}
	comb += gMatW[i] * (1.0f - In.blend[0] - In.blend[1]);

	//���_�𒲐�
	Out.position = mul(float4(In.position, 1.0f), comb);
	Out.position = mul(Out.position, gMatVP);

	//�@���𒲐�
	float4 normal = mul(float4(In.normal, 0.0f), comb);

	//�f�B���N�V���i�����C�g�̕������t�ɂ���i���̌v�Z�̂��߁j
	float4 revLightDir = -float4(gLightDir, 0.0f);

	//�@���ƃf�B���N�V���i�����C�g�̕����̓��ςɃf�B���N�V���i�����C�g�̐F����Z���A���r�G���g���C�g�̐F�Ɖ��Z
	//saturate�@���@���ʂ�0.0����1.0�܂ł͈̔͂Ɏ��߂�
	Out.color = saturate(gAmbientColor * gMaterialAmbient + gLightColor * gMaterialDiffuse * max(0, dot(revLightDir, normal)) * In.color);

	//UV���W�̐ݒ�
	Out.texCoord = In.texCoord;

	return Out;
}

VS_OUTPUT TextureVS_PB3NCT(VS_INPUT_PB3NCT In)
{
	VS_OUTPUT Out;

	float blend[3] = (float[3])In.blend;

	float4x4 comb = (float4x4)0;

	//�����s��Ƀu�����h�l���������[���h���W�ϊ��s����쐬
	int i;
	for (i = 0; i < gNumMaxInfle - 1; i++) {
		comb += gMatW[i] * In.blend[i];
	}
	comb += gMatW[i] * (1.0f - In.blend[0] - In.blend[1] - In.blend[2]);

	//���_�𒲐�
	Out.position = mul(float4(In.position, 1.0f), comb);
	Out.position = mul(Out.position, gMatVP);

	//�@���𒲐�
	float4 normal = mul(float4(In.normal, 0.0f), comb);

	//�f�B���N�V���i�����C�g�̕������t�ɂ���i���̌v�Z�̂��߁j
	float4 revLightDir = -float4(gLightDir, 0.0f);

	//�@���ƃf�B���N�V���i�����C�g�̕����̓��ςɃf�B���N�V���i�����C�g�̐F����Z���A���r�G���g���C�g�̐F�Ɖ��Z
	//saturate�@���@���ʂ�0.0����1.0�܂ł͈̔͂Ɏ��߂�
	Out.color = saturate(gAmbientColor * gMaterialAmbient + gLightColor * gMaterialDiffuse * max(0, dot(revLightDir, normal)) * In.color);

	//UV���W�̐ݒ�
	Out.texCoord = In.texCoord;

	return Out;
}

// �s�N�Z���V�F�[�_
PS_OUTPUT TexturePS(PS_INPUT In)
{
	PS_OUTPUT Out;

	//�e�N�X�`���[�̐F���擾
	float4 texColor = tex2D(texSampler, In.texCoord);

	//���C�e�B���O�ς݂̐F�ƃe�N�X�`���[�̐F����Z����
	Out.color = texColor * In.color;

	return Out;
}

// �s�N�Z���V�F�[�_�i�e�N�X�`���[�Ȃ��j
PS_OUTPUT NonTexturePS(PS_INPUT In)
{
	PS_OUTPUT Out;

	Out.color = In.color;

	return Out;
}

//�e�N�j�b�N
technique XFileSkinTec
{
	pass P0
	{
		VertexShader = compile vs_3_0 TextureVS_PB1NT();
		PixelShader = compile ps_3_0 TexturePS();

		//�A���t�@�e�X�g
		AlphaTestEnable = True;
		AlphaFunc = GREATEREQUAL;
		AlphaRef = 0xC8;

		//�A���t�@�u�����h�ݒ�
		AlphaBlendEnable = True;
		SrcBlend = SRCALPHA;
		DestBlend = INVSRCALPHA;
	}
	pass P1
	{
		VertexShader = compile vs_3_0 TextureVS_PB1NT();
		PixelShader = compile ps_3_0 NonTexturePS();
	}
	pass P2
	{
		VertexShader = compile vs_3_0 TextureVS_PB1NCT();
		PixelShader = compile ps_3_0 TexturePS();

		//�A���t�@�e�X�g
		AlphaTestEnable = True;
		AlphaFunc = GREATEREQUAL;
		AlphaRef = 0xC8;

		//�A���t�@�u�����h�ݒ�
		AlphaBlendEnable = True;
		SrcBlend = SRCALPHA;
		DestBlend = INVSRCALPHA;
	}
	pass P3
	{
		VertexShader = compile vs_3_0 TextureVS_PB1NCT();
		PixelShader = compile ps_3_0 NonTexturePS();
	}
	pass P4
	{
		VertexShader = compile vs_3_0 TextureVS_PB2NCT();
		PixelShader = compile ps_3_0 TexturePS();

		//�A���t�@�e�X�g
		AlphaTestEnable = True;
		AlphaFunc = GREATEREQUAL;
		AlphaRef = 0xC8;

		//�A���t�@�u�����h�ݒ�
		AlphaBlendEnable = True;
		SrcBlend = SRCALPHA;
		DestBlend = INVSRCALPHA;
	}
	pass P5
	{
		VertexShader = compile vs_3_0 TextureVS_PB2NCT();
		PixelShader = compile ps_3_0 NonTexturePS();
	}
	pass P6
	{
		VertexShader = compile vs_3_0 TextureVS_PB3NCT();
		PixelShader = compile ps_3_0 TexturePS();

		//�A���t�@�e�X�g
		AlphaTestEnable = True;
		AlphaFunc = GREATEREQUAL;
		AlphaRef = 0xC8;

		//�A���t�@�u�����h�ݒ�
		AlphaBlendEnable = True;
		SrcBlend = SRCALPHA;
		DestBlend = INVSRCALPHA;
	}
	pass P7
	{
		VertexShader = compile vs_3_0 TextureVS_PB3NCT();
		PixelShader = compile ps_3_0 NonTexturePS();
	}
	pass P8
	{
		VertexShader = compile vs_3_0 TextureVS_PB3NT();
		PixelShader = compile ps_3_0 TexturePS();

		//�A���t�@�e�X�g
		AlphaTestEnable = True;
		AlphaFunc = GREATEREQUAL;
		AlphaRef = 0xC8;

		//�A���t�@�u�����h�ݒ�
		AlphaBlendEnable = True;
		SrcBlend = SRCALPHA;
		DestBlend = INVSRCALPHA;
	}
	pass P9
	{
		VertexShader = compile vs_3_0 TextureVS_PB3NT();
		PixelShader = compile ps_3_0 NonTexturePS();
	}	
}