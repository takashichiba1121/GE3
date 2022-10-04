#pragma once
#include<d3d12.h>
#include<DirectXMath.h>
#include<wrl.h>
using namespace DirectX;
using namespace std;
using namespace Microsoft::WRL;


class Triangle
{
public:
	struct Vertex
	{
		XMFLOAT3 pos; //xyz���W
		XMFLOAT2 uv;  //uv���W
	};
	ComPtr<ID3D12PipelineState> pipelineState=nullptr;
	ComPtr<ID3D12RootSignature> rootSignature=nullptr;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView;
	HRESULT result;
	Vertex vertex1;
	Vertex vertex2;
	Vertex vertex3;
	//�C���f�b�N�X�f�[�^
	unsigned short indices[3] =
	{
		0,1,2,//�O�p�`�P
	};
public:
	//�R���X�g���N�^
	Triangle();
	Triangle(Vertex vertex1, Vertex vertex2, Vertex vertex3);
	//�f�X�g���N�^
	~Triangle();
	//�`�揉��������
	void Initialize(ID3D12Device* device);
	void Draw(ID3D12GraphicsCommandList* commandList);
};
