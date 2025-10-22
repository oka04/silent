//���[���h�r���[�ˉe�ϊ��s��錾
float4x4 gMatWVP;

//�A���r�G���g���C�g�̐F
float4 gAmbientColor;

//�f�B���N�V���i�����C�g�̕���
float3 gLightDir;

//�f�B���N�V���i�����C�g�̐F
float4 gLightColor;

//�r���{�[�h�p��]�s��
float4x4 gMatRotate;

//�e�N�X�`���[
texture gTexture;
sampler texSampler = sampler_state
{
	Texture = <gTexture>;
};

//���_�V�F�[�_�[���͗p
struct VS_INPUT
{
	float3 position : POSITION;
	float3 normal   : NORMAL;
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
VS_OUTPUT BillboardVS(VS_INPUT In)
{
	VS_OUTPUT Out;

	//���_�ɕϊ��s�����Z����
	Out.position = mul(float4(In.position, 1.0f), gMatWVP);

	//�@������]������i�J�����Ɍ�����j
	float4 normal = mul(float4(In.normal, 0.0f), gMatRotate);

	//�f�B���N�V���i�����C�g�̕������t�ɂ���i���̌v�Z�̂��߁j
	float4 revLightDir = -float4(gLightDir, 0.0f);

	//�@���ƃf�B���N�V���i�����C�g�̕����̓��ςɃf�B���N�V���i�����C�g�̐F����Z���A���r�G���g���C�g�̐F�Ɖ��Z
	//saturate�@���@���ʂ�0.0����1.0�܂ł͈̔͂Ɏ��߂�
	Out.color = saturate(gAmbientColor + gLightColor * max(0, dot(revLightDir, normal)));

	//UV���W�̐ݒ�
	Out.texCoord = In.texCoord;

	return Out;
}

// �s�N�Z���V�F�[�_
PS_OUTPUT BillboardPS(PS_INPUT In)
{
	PS_OUTPUT Out;

	//�e�N�X�`���[�̐F���擾
	float4 texColor = tex2D(texSampler, In.texCoord);

	//���C�e�B���O�ς݂̐F�ƃe�N�X�`���[�̐F����Z����
	Out.color = texColor * In.color;

	//�A���t�@�͕ʓr�ݒ�i��̌v�Z�Œl���ς���Ă��邩������Ȃ��j
	Out.color.a = texColor.a;

	return Out;
}

//�e�N�j�b�N
technique BillboardTec
{
   pass P0
   {
      VertexShader = compile vs_3_0 BillboardVS();
      PixelShader = compile ps_3_0 BillboardPS();

	  //�A���t�@�e�X�g
	  AlphaTestEnable = True;

	  //臒l�����傫�����̂�`�悷��
	  AlphaFunc = GREATER;

	  //臒l0.5
	  AlphaRef = 0x7f; //127
   }
}