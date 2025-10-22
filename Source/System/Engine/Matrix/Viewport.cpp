//*****************************************************************************
//
// �r���[�|�[�g�N���X
//
// Viewport.cpp
//
// K_Yamaoka
//
// 2013/03/04
//
// 2014/11/28 �G���[���b�Z�[�W��ǉ�
//
// 2019/11/28 GetViewportMatrix�֐���ǉ�
//
//*****************************************************************************

#define _USING_V110_SDK71_ 1

#include "Viewport.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
Viewport::Viewport()
{

}

//=============================================================================
// �r���[�|�[�g�̒ǉ�
// ���@���Fconst D3DVIEWPORT9* �r���[�|�[�g�\���̂ւ̃|�C���^
//=============================================================================
void Viewport::Add(const D3DVIEWPORT9* pViewport)
{
	//�ϒ��z��Ƀv�b�V��
	m_container.push_back(*pViewport);
}

//=============================================================================
// �r���[�|�[�g�̒ǉ�
// ���@���Fconst DWORD ����̂w���W
// �@�@�@�@const DWORD ����̂x���W
// �@�@�@�@const DWORD ��
// �@�@�@�@const DWORD ����
// �@�@�@�@const float �R�c���Q�c�ϊ����̂y�̍ŏ��l
// �@�@�@�@const float �R�c���Q�c�ϊ����̂y�̍ő�l
//=============================================================================
void Viewport::Add(const DWORD x, const DWORD y, const DWORD width, const DWORD height, const float minZ, const float maxZ)
{
	D3DVIEWPORT9 work;

	work.X = x;
	work.Y = y;
	work.Width = width;
	work.Height = height;
	work.MinZ = minZ;
	work.MaxZ = maxZ;

	Add(&work);
}

//=============================================================================
// �r���[�|�[�g�̍폜
// ���@���Fconst int �C���f�b�N�X
//=============================================================================
void Viewport::Delete(const int index)
{
	std::vector<D3DVIEWPORT9>::iterator it = m_container.begin();

	std::advance(it, index);

	m_container.erase(it);
}

//=============================================================================
// ���݂̃r���[�|�[�g���̎擾
// �߂�l�Fint �r���[�|�[�g�̐�
//=============================================================================
int Viewport::GetViewportCount() const
{
	return m_container.size();
}

//=============================================================================
// �w�肵���r���[�|�[�g�\���̂̎擾
// �߂�l�FD3DVIEWPORT9 �r���[�|�[�g�\����
// ���@���Fconst int �C���f�b�N�X
//=============================================================================
D3DVIEWPORT9 Viewport::GetData(const int index) const
{
	return m_container[index];
}

//=============================================================================
// �f�o�C�X�փZ�b�g
// ���@���FEngine* �R�c�G���W���N���X�ւ̃|�C���^
// �@�@�@�@const int �C���f�b�N�X
//=============================================================================
void Viewport::SetDevice(Engine* pEngine, const int index)
{
	LPDIRECT3DDEVICE9 pDevice = pEngine->GetDevice();

	//�r���[�|�[�g�\���̂��f�o�C�X�ɃZ�b�g
	HRESULT hResult = pDevice->SetViewport(&m_container[index]);

	if (FAILED(hResult)) {
		throw DxSystemException(DxSystemException::OM_VIEWPORT_SETDEVICE_ERROR);
	}
}

//=============================================================================
// �r���[�|�[�g�ϊ��s��̎擾
// �߂�l�FD3DXMATRIX �r���[�|�[�g�ϊ��s��
//=============================================================================
D3DXMATRIX Viewport::GetViewportMatrix(const int index) const
{
	D3DXMATRIX mat;

	D3DXMatrixIdentity(&mat);

	mat.m[0][0] = m_container[index].Width / 2.0f;
	mat.m[1][1] = -(m_container[index].Height / 2.0f);
	mat.m[2][2] = m_container[index].MaxZ - m_container[index].MinZ;
	mat.m[3][0] = m_container[index].X + m_container[index].Width / 2.0f;
	mat.m[3][1] = m_container[index].Height / 2.0f + m_container[index].Y;
	mat.m[3][2] = m_container[index].MinZ;

	return mat;
}

















