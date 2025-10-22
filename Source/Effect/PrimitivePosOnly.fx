//���[���h�r���[�ˉe�ϊ��s��錾
float4x4 gMatWVP;

//�A���r�G���g���C�g�̐F
float4 gAmbientColor;

//�}�e���A���̐F
float4 gMaterialDiffuse;

//�}�e���A���̃A���r�G���g
float4 gMaterialAmbient;

//���_�V�F�[�_�[���͗p
struct VS_INPUT
{
	float3 position : POSITION;
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
VS_OUTPUT PosOnlyVS(VS_INPUT In)
{
	VS_OUTPUT Out;

	Out.position = mul(float4(In.position, 1.0f), gMatWVP);

	Out.color = gMaterialDiffuse;

	return Out;
}

// �s�N�Z���V�F�[�_
PS_OUTPUT PosOnlyPS(PS_INPUT In)
{
	PS_OUTPUT Out;

	Out.color = In.color;

	return Out;
}

//�e�N�j�b�N
technique PrimitivePosOnlyTec
{
   pass P0
   {
      VertexShader = compile vs_3_0 PosOnlyVS();
      PixelShader = compile ps_3_0 PosOnlyPS();
   }
}