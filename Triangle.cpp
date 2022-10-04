#include"Triangle.h"
#define DIRECTINPUT_VERSION 0x0800 //DirectInput�̃o�[�W�����w��
#include<dinput.h>
#include<d3dcompiler.h>
#include<Windows.h>
#include <tchar.h>
#include<iostream>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<vector>
#include<string>
#include<DirectXMath.h>
#include<DirectXTex.h>
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")

using namespace DirectX;
using namespace std;
Triangle::Triangle()
{
	vertex1 = { {0.0f,0.0f,0.0f},{1.0f,1.0f} };
	vertex2 = { {0.0f,0.0f,0.0f},{1.0f,1.0f} };
	vertex3 = { {0.0f,0.0f,0.0f},{1.0f,1.0f} };
}
Triangle::Triangle(Vertex vertex1, Vertex vertex2, Vertex vertex3)
{
	this->vertex1=vertex1;
	this->vertex2=vertex2;
	this->vertex3=vertex3;
}
Triangle::~Triangle()
{

}
 void Triangle::Initialize(ID3D12Device* device)
{
	 Vertex vertices[] = {
		{vertex1},//����
		{vertex2 },//����
		{vertex3 },//�E��
	 };
	 //���_�f�[�^�S�̂̃T�C�Y=���_�f�[�^����̃T�C�Y*���_�f�[�^�̗v�f��
	 UINT sizeVB = static_cast<UINT>(sizeof(vertices[0]) * _countof(vertices));
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
	 ID3D12Resource* vertBuff = nullptr;
	 result = device->CreateCommittedResource(
		 &heapProp, //�q�[�v�ݒ�
		 D3D12_HEAP_FLAG_NONE,
		 &resDesc, //���\�[�X�ݒ�
		 D3D12_RESOURCE_STATE_GENERIC_READ,
		 nullptr,
		 IID_PPV_ARGS(&vertBuff));
	 assert(SUCCEEDED(result));
	 //�C���f�b�N�X�f�[�^�S�̂̃T�C�Y
	 UINT sizeIB = static_cast<UINT>(sizeof(uint16_t) * _countof(indices));

	 //���\�[�X�ݒ�
	 resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	 resDesc.Width = sizeIB;//�C���f�b�N�X��񂪓��镪�̃T�C�Y
	 resDesc.Height = 1;
	 resDesc.DepthOrArraySize = 1;
	 resDesc.MipLevels = 1;
	 resDesc.SampleDesc.Count = 1;
	 resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	 //�C���f�b�N�X�o�b�t�@�̐���
	 ID3D12Resource* indexBuff = nullptr;
	 result = device->CreateCommittedResource(
		 &heapProp,//�q�[�v�ݒ�
		 D3D12_HEAP_FLAG_NONE,
		 &resDesc,
		 D3D12_RESOURCE_STATE_GENERIC_READ,
		 nullptr,
		 IID_PPV_ARGS(&indexBuff));
	 //�C���f�b�N�X�o�b�t�@���}�b�s���O
	 uint16_t* indexMap = nullptr;
	 result = indexBuff->Map(0, nullptr, (void**)&indexMap);
	 //�S�C���f�b�N�X�ɑ΂���
	 for (int i = 0; i < _countof(indices); i++)
	 {
		 indexMap[i] = indices[i];//�C���f�b�N�X���R�s�[
	 }
	 //�}�b�s���O����
	 indexBuff->Unmap(0, nullptr);
	 //GPU��̃o�b�t�@�ɑΉ��������z�������i���C����������j���擾
	 Vertex* vertMap = nullptr;
	 result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	 assert(SUCCEEDED(result));
	 //�O���_�ɑ΂���
	 for (int i = 0; i < _countof(vertices); i++) {
		 vertMap[i] = vertices[i]; //���W�R�s�[
	 }
	 //�q�������
	 vertBuff->Unmap(0, nullptr);

	 //���_�o�b�t�@�r���[�̍쐬
	 vbView;
	 //GPU���z�A�h���X
	 vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	 //���_�o�b�t�@�̃T�C�Y
	 vbView.SizeInBytes = sizeVB;
	 //���_����̃f�[�^�T�C�Y
	 vbView.StrideInBytes = sizeof(vertices[0]);

	 //�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	 ibView;
	 ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	 ibView.Format = DXGI_FORMAT_R16_UINT;
	 ibView.SizeInBytes = sizeIB;

	 ID3DBlob* vsBlob = nullptr;//���_�V�F�[�_�[�I�u�W�F�N�g
	 ID3DBlob* psBlob = nullptr;//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	 ID3DBlob* errorBlob = nullptr;//�G���[�I�u�W�F�N�g

	 //���_�V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
	 result = D3DCompileFromFile(
		 L"BasicVS.hlsl", //�V�F�[�_�[�t�@�C����
		 nullptr,
		 D3D_COMPILE_STANDARD_FILE_INCLUDE,//�C���N���[�h�\�ɂ���
		 "main", "vs_5_0",//�G���g���[�|�C���g���A�V�F�[�_�[���f���w��
		 D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, //�f�o�b�O�p�ݒ�
		 0,
		 &vsBlob, &errorBlob);

	 //�G���[�Ȃ�
	 if (FAILED(result)) {
		 //errorBlob����G���[���e��string�^�ɃR�s�[
		 std::string error;
		 error.resize(errorBlob->GetBufferSize());

		 std::copy_n((char*)errorBlob->GetBufferPointer(),
			 errorBlob->GetBufferSize(),
			 error.begin());
		 error += "\n";
		 //�G���[���e���o�̓E�C���h�E�ɕ\��
		 OutputDebugStringA(error.c_str());
		 assert(0);
	 }
	 //�s�N�Z���V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
	 result = D3DCompileFromFile(
		 L"BasicPS.hlsl",
		 nullptr,
		 D3D_COMPILE_STANDARD_FILE_INCLUDE,
		 "main", "ps_5_0",
		 D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		 0,
		 &psBlob, &errorBlob);
	 //�G���[�Ȃ�
	 if (FAILED(result)) {
		 //errorBlob������e��string�^�ɃR�s�[
		 std::string error;
		 error.resize(errorBlob->GetBufferSize());

		 std::copy_n((char*)errorBlob->GetBufferPointer(),
			 errorBlob->GetBufferSize(),
			 error.begin());
		 error += "\n";
		 //�G���[���e���o�̓E�C���h�E�ɕ\��
		 OutputDebugStringA(error.c_str());
		 assert(0);
	 }
	 //���_���C�A�E�g
	 D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
		 {
			 "POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			 D3D12_APPEND_ALIGNED_ELEMENT,
			 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
			 },//��s�ŏ������ق������₷��
			 {
			 "TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			 D3D12_APPEND_ALIGNED_ELEMENT,
			 D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
			 },//��s�ŏ������ق������₷��
	 };

	 //�ݒ�
	 D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineDesc{};
	 //�V�F�[�_�[�̐ݒ�
	 pipelineDesc.VS.pShaderBytecode = vsBlob->GetBufferPointer();
	 pipelineDesc.VS.BytecodeLength = vsBlob->GetBufferSize();
	 pipelineDesc.PS.pShaderBytecode = psBlob->GetBufferPointer();
	 pipelineDesc.PS.BytecodeLength = psBlob->GetBufferSize();

	 //�T���v���}�X�N�̐ݒ�
	 pipelineDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	 //���X�^���C�U�̐ݒ�
	 pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;//�J�����O���Ȃ�
	 pipelineDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;//�|���S�����h��Ԃ�
	 pipelineDesc.RasterizerState.DepthClipEnable = true;//�[�x�O���b�s���O��L����
	 //�u�����h�X�e�[�g
	 //pipelineDesc.BlendState.RenderTarget[0].RenderTargetWriteMask
	 //	= D3D12_COLOR_WRITE_ENABLE_ALL;//RBGA���ׂẴ`�����l����`��
	 //�����_�[�^�[�Q�b�g�̃u�����h�ݒ�
	 D3D12_RENDER_TARGET_BLEND_DESC& blenddesc = pipelineDesc.BlendState.RenderTarget[0];
	 blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;//RGBA�S�Ẵ`�����l����`��
	 blenddesc.BlendEnable = false;//�u�����h��L���ɂ���
	 blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;//���Z
	 blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��
	 blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;//�f�X�g�̒l��0%�g��
	 //���Z����
	 blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//���Z
	 blenddesc.SrcBlend = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��
	 blenddesc.DestBlend = D3D12_BLEND_ONE;//�f�X�g�̒l��100%�g��
	 ////���Z����
	 //blenddesc.BlendOp = D3D12_BLEND_OP_REV_SUBTRACT;//�f�X�g����\�[�X�����Z
	 //blenddesc.SrcBlend = D3D12_BLEND_ONE;//�\�[�X�̒l��100%�g��
	 //blenddesc.DestBlend = D3D12_BLEND_ONE;//�f�X�g�̒l��100%�g��
	 ////�F���]
	 //blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//���Z
	 //blenddesc.SrcBlend = D3D12_BLEND_INV_DEST_COLOR;//1.0f-�f�X�g�J���[�̒l
	 //blenddesc.DestBlend = D3D12_BLEND_ZERO;//�g��Ȃ�
	 ////����������
	 //blenddesc.BlendOp = D3D12_BLEND_OP_ADD;//���Z
	 //blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;//�\�[�X�̃A���t�@�l
	 //blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;//1.0f-�\�[�X�̃A���t�@�l
	 //���_���C�A�E�g�̐ݒ�
	 pipelineDesc.InputLayout.pInputElementDescs = inputLayout;
	 pipelineDesc.InputLayout.NumElements = _countof(inputLayout);
	 //�}�`�̌`��ݒ�
	 pipelineDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	 //���̑��̐ݒ�
	 pipelineDesc.NumRenderTargets = 1;//�`��Ώۂ͈��
	 pipelineDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;//0�`255�w���RGBA
	 pipelineDesc.SampleDesc.Count = 1; //1�s�N�Z���ɂ����T���v�����O

	 //�f�X�N���v�^�����W�̐ݒ�
	 D3D12_DESCRIPTOR_RANGE descriptorRange{};
	 descriptorRange.NumDescriptors = 1;//��x�̕`��Ɏg���e�N�X�`�����ꖇ�Ȃ̂łP
	 descriptorRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	 descriptorRange.BaseShaderRegister = 0;//�e�N�X�`�����W�X�^0��
	 descriptorRange.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	 //���[�g�p�����[�^�[�̐ݒ�
	 D3D12_ROOT_PARAMETER rootParams[2] = {};
	 rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@
	 rootParams[0].Descriptor.ShaderRegister = 0;//�萔�o�b�t�@�ԍ�
	 rootParams[0].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	 rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_�[���猩����
	 //�e�N�X�`�����W�X�^0��
	 rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//���
	 rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;//�f�X�N���v�^�����W
	 rootParams[1].DescriptorTable.NumDescriptorRanges = 1;//�f�X�N���v�^�����W��
	 rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_�[���猩����
	 //�e�N�X�`���T���v���[�ݒ�
	 D3D12_STATIC_SAMPLER_DESC samplerDesc{};
	 samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���J��Ԃ��i�^�C�����O�j
	 samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//�c�J��Ԃ��i�^�C�����O�j
	 samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//���s�J��Ԃ��i�^�C�����O�j
	 samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;//�{�[�_�[�̎��͍�
	 samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//�S�ă��j�A�⊮
	 samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;//�~�b�v�}�b�v�ő�l
	 samplerDesc.MinLOD = 0.0f;//�~�b�v�}�b�v�ŏ��l
	 samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	 samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//�s�N�Z���V�F�[�_�[����̂ݎg�p�\
	 //���[�g�V�O�l�`��
	 rootSignature;
	 //���[�g�V�O�l�`���̐ݒ�
	 D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	 rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	 rootSignatureDesc.pParameters = rootParams;//���[�g�p�����[�^�[�̐擪�A�h���X
	 rootSignatureDesc.NumParameters = _countof(rootParams);//���[�g�p�����[�^��
	 rootSignatureDesc.pStaticSamplers = &samplerDesc;
	 rootSignatureDesc.NumStaticSamplers = 1;
	 //���[�g�V�O�l�`���̃V���A���C�Y
	 ID3DBlob* rootSigBlob = nullptr;
	 result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		 &rootSigBlob, &errorBlob);
	 assert(SUCCEEDED(result));
	 result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		 IID_PPV_ARGS(&rootSignature));
	 assert(SUCCEEDED(result));
	 rootSigBlob->Release();
	 //�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	 pipelineDesc.pRootSignature = rootSignature.Get();

	 //�p�C�v�����X�X�e�[�g�̐���
	 result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	 assert(SUCCEEDED(result));
}
void Triangle::Draw(ID3D12GraphicsCommandList* commandList)
{
	//���_�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetVertexBuffers(0, 1, &vbView);
	//�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
	commandList->IASetIndexBuffer(&ibView);
	//�`��R�}���h
	commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);//���ׂĂ̒��_���g���ĕ`��
}
