#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<wrl.h>

using namespace DirectX;

//�萔�o�b�t�@�p�f�[�^�\���́i3D�ϊ��s��j
struct ConstBufferDataTransform
{
	XMMATRIX mat; //3D�ϊ��s��
};

class Object3D
{
public:
	void Initialize(ID3D12Device* device);

	void Update(XMMATRIX matView, XMMATRIX matProjection);

	void Draw(ID3D12GraphicsCommandList* commandList, D3D12_VERTEX_BUFFER_VIEW vbView, D3D12_INDEX_BUFFER_VIEW ibView, UINT numIndices);

	//�萔�o�b�t�@�i�s��p�j
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffTransform;
	//�萔�o�b�t�@�}�b�v�i�s��p�j
	ConstBufferDataTransform* constMapTransform;
	//�A�t�B���ϊ����
	XMFLOAT3 scale = { 1,1,1 };
	XMFLOAT3 rotation = { 0,0,0 };
	XMFLOAT3 position = { 0,0,0 };
	//���[���h�ϊ��s��
	XMMATRIX matWorld;
	//�e�I�u�W�F�N�g�ւ̃|�C���^
	Object3D* parent = nullptr;

};