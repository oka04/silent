//���[���h���W�ϊ��s��
float4x4 gMatW;

//���[���h�r���[�ˉe�ϊ��s��錾
float4x4 gMatWVP;

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

//���_�V�F�[�_�[���͗p
struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
};

//���_�V�F�[�_�[�o�͗p
struct VS_OUTPUT {
	float4 position : POSITION;
	float4 color    : COLOR0;
};

//�s�N�Z���V�F�[�_�[���͗p
struct PS_INPUT {
	float4 position : POSITION;
	float4 color    : COLOR0;
};

//�s�N�Z���V�F�[�_�[�o�͗p
struct PS_OUTPUT {
	float4 color : COLOR0;
};

// ���_�V�F�[�_
VS_OUTPUT NormalVS(VS_INPUT In)
{
	VS_OUTPUT Out;

	//���_�ɕϊ��s�����Z����
	Out.position = mul(float4(In.position, 1.0f), gMatWVP);

	//float4�֕ϊ�
	float4 normal = mul(float4(In.normal, 0.0f), gMatW);

	//�f�B���N�V���i�����C�g�̕������t�ɂ���i���̌v�Z�̂��߁j
	float4 revLightDir = -float4(gLightDir, 0.0f);

	//�@���ƃf�B���N�V���i�����C�g�̕����̓��ςɃf�B���N�V���i�����C�g�̐F����Z���A���r�G���g���C�g�̐F�Ɖ��Z
	//saturate�@���@���ʂ�0.0����1.0�܂ł͈̔͂Ɏ��߂�
	Out.color = saturate(gAmbientColor * gMaterialAmbient + gLightColor * gMaterialDiffuse * max(0, dot(revLightDir, normal)));

	return Out;
}

// �s�N�Z���V�F�[�_
PS_OUTPUT NormalPS(PS_INPUT In)
{
	PS_OUTPUT Out;

	Out.color = In.color;

	return Out;
}

//�e�N�j�b�N
technique PrimitiveNormalTec
{
   pass P0
   {
      VertexShader = compile vs_3_0 NormalVS();
      PixelShader = compile ps_3_0 NormalPS();
   }
}