#include "Sprite.h"


using namespace DirectX;
using namespace std;

void Sprite::Initialize(SpriteCommon* SpriteCommon)
{
	this->spriteCommon = SpriteCommon;

	this->dxCommon = spriteCommon->GetDxCommon();

	HRESULT result;

	Vertex vertices_[4] = {
		{{-0.4f,-0.7f,0.0f},{0.0f,1.0f}},
		{{-0.4f,+0.7f,0.0f},{0.0f,0.0f}},
		{{+0.4f,-0.7f,0.0f},{1.0f,1.0f}},
		{{+0.4f,+0.7f,0.0f},{1.0f,0.0f}},
	};
	for (int i = 0; i < 4; i++)
	{
		vertices[i] = vertices_[i];
	}

	//���_�f�[�^�S�̂̃T�C�Y=���_�f�[�^����̃T�C�Y*���_�f�[�^�̗v�f��
	UINT sizeVB = static_cast<UINT>(sizeof(vertices_[0]) * _countof(vertices_));
	//���_�o�b�t�@�̐ݒ�
	D3D12_HEAP_PROPERTIES heapProp{};  //�q�[�v�ݒ�
	heapProp.Type = D3D12_HEAP_TYPE_UPLOAD; //GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC resDesc{};
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resDesc.Width = sizeVB; //���_�f�[�^�S�̂̃T�C�Y
	resDesc.Height = 1;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//���_�o�b�t�@�̐���
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&heapProp, //�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&resDesc, //���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertBuff));
	assert(SUCCEEDED(result));

	//GPU��̃o�b�t�@�ɑΉ��������z�������i���C����������j���擾
	Vertex* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	assert(SUCCEEDED(result));
	//�O���_�ɑ΂���
	for (int i = 0; i < _countof(vertices_); i++) {
		vertMap[i] = vertices_[i]; //���W�R�s�[
	}
	//�q�������
	vertBuff->Unmap(0, nullptr);

	//GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	//���_����̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(vertices_[0]);

	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES cbHeapProp{};
	cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;//GPU�ւ̓]���p
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC cbResourceDesc{};
	cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	cbResourceDesc.Width = (sizeof(ConstBufferDataMaterial) + 0xff) & ~0xff;//256�o�C�g�A���C�������g
	cbResourceDesc.Height = 1;
	cbResourceDesc.DepthOrArraySize = 1;
	cbResourceDesc.MipLevels = 1;
	cbResourceDesc.SampleDesc.Count = 1;
	cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	//�萔�o�b�t�@�̐���
	result = dxCommon->GetDevice()->CreateCommittedResource(
		&cbHeapProp,//�q�[�v�ݒ�
		D3D12_HEAP_FLAG_NONE,
		&cbResourceDesc,//���\�[�X�ݒ�
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuffMaterial));
	assert(SUCCEEDED(result));
	//�萔�o�b�t�@�̃}�b�s���O
	ConstBufferDataMaterial* constMapMaterial = nullptr;
	result = constBuffMaterial->Map(0, nullptr, (void**)&constMapMaterial);//�}�b�s���O
	//�l���������ނƎ����I�ɓ]�������
	constMapMaterial->color = XMFLOAT4(1, 0, 0, 0.5f);//RGBA�Ŕ������̐�
	assert(SUCCEEDED(result));
}

void Sprite::Update()
{
}

void Sprite::Draw()
{
	//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
	dxCommon->GetCommandList()->SetPipelineState(spriteCommon->GetPipelineState());
	dxCommon->GetCommandList()->SetGraphicsRootSignature(spriteCommon->GetRootSignature());
	//�v���~�e�B�u�`��̐ݒ�R�}���h
	dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vbView);
	//�萔�o�b�t�@�r���[�iCBV�j�̐ݒ�R�}���h
	dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
	//�`��R�}���h
	dxCommon->GetCommandList()->DrawInstanced(_countof(vertices), 1, 0, 0);//���ׂĂ̒��_���g���ĕ`��
}
