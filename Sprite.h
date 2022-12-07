#pragma once
#include"SpriteCommon.h"

//�萔�o�b�t�@�p�f�[�^�\���́i�}�e���A���j
struct ConstBufferDataMaterial {
	DirectX::XMFLOAT4 color;//�F�iRGBA�j
};
class Sprite
{
public:
	//������
	void Initialize(SpriteCommon* SpriteCommon);

	void Update();

	void Draw();
private:
	DirectX::XMFLOAT3 vertices[3] = {};

	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff = nullptr;	//���_�o�b�t�@

	SpriteCommon* spriteCommon;

	DirectXCommon* dxCommon;

	//���_�o�b�t�@�r���[�̍쐬
	D3D12_VERTEX_BUFFER_VIEW vbView{};

	Microsoft::WRL::ComPtr<ID3D12Resource> constBuffMaterial = nullptr;

};

