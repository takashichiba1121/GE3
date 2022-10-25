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
#include"Triangle.h"
#include"object3d.h"
#include"input.h"
#include"WinApp.h"

using namespace DirectX;
using namespace std;


//�萔�o�b�t�@�p�f�[�^�\���́i�}�e���A���j
struct ConstBufferDataMaterial {
	XMFLOAT4 color;//�F�iRGBA�j
};

//�E�B���h�E�v���[�W���[
LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wapram, LPARAM lparam) {
	//���b�Z�[�W�ɉ����ăQ�[���ŗL�̏������s��
	switch (msg)
	{
		//�E�C���h�E���j�����ꂽ
	case WM_DESTROY:
		//OS�ɑ΂��āA�A�v���̏I����`����
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wapram, lparam);
}
//windows�A�v���̃G���g���[�|�C���g�imain�֐��j
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
	//const int window_width = 1280;//��
	//const int window_heigit = 720;//�c

	////�E�C���h�E�N���X�̐ݒ�
	//WNDCLASSEX w{};
	//w.cbSize = sizeof(WNDCLASSEX);
	//w.lpfnWndProc = (WNDPROC)WindowProc;//�E�C���h�E�v���\�W���[�̐ݒ�
	//w.lpszClassName = L"DirectXGame";//�E�C���h�E�N���X��
	//w.hInstance = GetModuleHandle(nullptr);//�E�C���h�E�n���h��
	//w.hCursor = LoadCursor(NULL, IDC_ARROW);//�J�[�\���w��

	////�E�C���h�E�N���X��OS�ɓo�^����
	//RegisterClassEx(&w);
	////�E�C���h�E�T�C�Y�oX���W�@Y���W�@�����@�c���p
	//RECT wrc = { 0,0,window_width,window_heigit };
	////�����T�C�Y��␳����
	//AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
	////�E�C���h�E�I�u�W�F�N�g�̐���
	//HWND hwnd = CreateWindow(w.lpszClassName,//�N���X��
	//	L"DirectXGame",//�^�C�g���o�[�̕���
	//	WS_OVERLAPPEDWINDOW,//�W���I�ȃE�C���h�E�X�^�C��
	//	CW_USEDEFAULT,//�\��X���W�iOS�ɔC����j
	//	CW_USEDEFAULT,//�\��Y���W�iOS�ɔC����j
	//	wrc.right - wrc.left,//�E�C���h�E����
	//	wrc.bottom - wrc.top,//�E�C���h�E�c��
	//	nullptr,//�e�E�C���h�E�n���h��
	//	nullptr,//���j���[�n���h��
	//	w.hInstance,//�Ăяo���A�v���P�[�V�����n���h��
	//	nullptr);//�I�v�V����

	////�E�C���h�E��\����Ԃɂ���
	//ShowWindow(hwnd, SW_SHOW);

	WinApp* winApp = nullptr;
	winApp = new WinApp;
	winApp->Initialize();

	MSG msg{};

	//DirectX�����������@��������
#ifdef _DEBUG
   //�f�o�b�O���C���[���I����
	ID3D12Debug1* debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif
	HRESULT result;
	ComPtr<ID3D12Device> device;
	ComPtr<IDXGIFactory7> dxgiFactory;
	ComPtr <IDXGISwapChain4> swapChain;
	ComPtr<ID3D12CommandAllocator> comdAllocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	ComPtr<ID3D12DescriptorHeap> rtvHeap = nullptr;
	//�Ή����x���̔z��
	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};
	//DXGI�t�@�N�g���[�̐���
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	assert(SUCCEEDED(result));

	//�A�_�v�^�[�̗񋓗p
	std::vector<ComPtr<IDXGIAdapter4>>adapters;
	//�����ɓ���̖��O�����A�_�v�^�[�I�u�W�F�N�g������
	ComPtr<IDXGIAdapter4> tmpAdapter = nullptr;

	//�p�t�H�[�}���X���������̂��珇�ɁA���ׂẴA�_�v�^�[��񋓂���
	for (UINT i = 0;
		dxgiFactory->EnumAdapterByGpuPreference(i,
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE,
			IID_PPV_ARGS(&tmpAdapter)) != DXGI_ERROR_NOT_FOUND;
		i++) {
		//���I�z��ɒǉ�����
		adapters.push_back(tmpAdapter);
	}
	//�Ó��ȃA�_�v�^��I�ʂ���
	for (size_t i = 0; i < adapters.size(); i++) {
		DXGI_ADAPTER_DESC3 adapterDesc;
		//�A�_�v�^�[�̏����擾����
		adapters[i]->GetDesc3(&adapterDesc);

		//�\�t�g�E�F�A�f�o�C�X�����
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			//�f�o�C�X���̗p���ă��[�v�𔲂���
			tmpAdapter = adapters[i].Get();
			break;
		}
	}
	D3D_FEATURE_LEVEL featureLevel;

	for (size_t i = 0; i < _countof(levels); i++) {
		//�̗p�����A�_�v�^�[�Ńf�o�C�X�𐶐�
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i],
			IID_PPV_ARGS(&device));
		if (result == S_OK) {
			//�f�o�C�X�𐶐��ł������_�Ń��[�v�𔲂���
			featureLevel = levels[i];
		}
	}

#ifdef _DEBUG

	ID3D12InfoQueue* infoQueue;
	if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);//��΂��G���[�̎��Ɏ~�܂�
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);//�G���[���Ɏ~�܂�
		infoQueue->Release();
	}
#endif

	//�}������G���[
	D3D12_MESSAGE_ID denyIds[] = {

		/*
		*Windows11�ł�DXGI�f�o�b�N���C���[��DX12�f�o�b�N���C���[�̑��ݍ�p�o�O�ɂ��G���[���b�Z�[�W
		* https://stackoverflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
		*/
		D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
	};

	//�}������\�����x��
	D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
	D3D12_INFO_QUEUE_FILTER filter{};
	filter.DenyList.NumIDs = _countof(denyIds);
	filter.DenyList.pIDList = denyIds;
	filter.DenyList.NumSeverities = _countof(severities);
	filter.DenyList.pSeverityList = severities;
	//�w�肵���G���[�̕\����}������
	infoQueue->PushStorageFilter(&filter);

	//�R�}���h�A���P�[�^�𐶐�
	result = device->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&comdAllocator));
	assert(SUCCEEDED(result));
	//�R�}���h���X�g�̐���
	result = device->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		comdAllocator.Get(), nullptr,
		IID_PPV_ARGS(&commandList));
	//�R�}���h�L���[�̐ݒ�
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	//�R�}���h�L���[�𐶐�
	result = device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));
	assert(SUCCEEDED(result));
	//�X���b�v�`�F�[���̐ݒ�
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = 1280;
	swapChainDesc.Height = 720;
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;//�F���̏���
	swapChainDesc.SampleDesc.Count = 1;//�}���`�T���v�����Ȃ�
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;//�o�b�N�o�b�t�@�p
	swapChainDesc.BufferCount = 2;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;//�t���b�v��͔j��
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//IDXGISwapChain1��Ptr��p��
	ComPtr<IDXGISwapChain1> swapchain1;

	//�X���b�v�`�F�[���̐���
	result = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue.Get(), winApp->GetHwnd() , &swapChainDesc, nullptr, nullptr,
		&swapchain1);
	assert(SUCCEEDED(result));

	//��������IDXGISwapChain1�̃I�u�W�F�N�g��IDXGISwapChain4�ɕϊ�����
	swapchain1.As(&swapChain);

	//�f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;//�����_�[�^�[�Q�b�g�r���[
	rtvHeapDesc.NumDescriptors = swapChainDesc.BufferCount;//���\�̓��

	//�f�X�N���v�^�q�[�v�̐���
	device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap));

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC depthResourceDesc{};
	depthResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthResourceDesc.Width = WinApp::window_width;//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResourceDesc.Height = WinApp::window_heigit;//�����_�[�^�[�Q�b�g�ɍ��킹��
	depthResourceDesc.DepthOrArraySize = 1;
	depthResourceDesc.Format = DXGI_FORMAT_D32_FLOAT;//�[�x�t�H�[�}�b�g
	depthResourceDesc.SampleDesc.Count = 1;
	depthResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;//�f�v�X�X�e���V��

	//�[�x�l�p�q�[�v�v���p�e�B
	D3D12_HEAP_PROPERTIES depthHeapProp{};
	depthHeapProp.Type = D3D12_HEAP_TYPE_DEFAULT;
	//�[�w�l�̃N���A�ݒ�
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;//�[�x1.0f(�ő�l)�N���A
	depthClearValue.Format = DXGI_FORMAT_D32_FLOAT;//�[�x�l�t�H�[�}�b�g

	//���\�[�X
	ComPtr<ID3D12Resource> depthBuff = nullptr;
	result = device->CreateCommittedResource(
		&depthHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResourceDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//�[�x�l�������݂Ɏg�p
		&depthClearValue,
		IID_PPV_ARGS(&depthBuff));

	//�[�x�r���[�p�f�X�N���v�^�q�[�v�쐬
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;//�[�x�r���[��1��
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//�f�X�X�e���V���r���[
	ComPtr<ID3D12DescriptorHeap> dsvHeap = nullptr;
	result = device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&dsvHeap));

	//�[�x�r���[�쐬
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//�[�x�l�t�H�[�}�b�g
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	device->CreateDepthStencilView(
		depthBuff.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart());

	//�o�b�N�o�b�t�@
	std::vector<ComPtr<ID3D12Resource>>backBuffers;
	backBuffers.resize(swapChainDesc.BufferCount);

	//�X���b�v�`�F�[���̂��ׂẴo�b�t�@�ɂ��ď�������
	for (size_t i = 0; i < backBuffers.size(); i++) {
		//�X���b�v�`�F�[������o�b�t�@���擾
		swapChain->GetBuffer((UINT)i, IID_PPV_ARGS(&backBuffers[i]));
		//�f�X�N���v�^�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		//�����\���ŃA�h���X�������
		rtvHandle.ptr += i * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		//�����_�[�^�[�Q�b�g�r���[���ݒ�
		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
		//�V�F�[�_�[�̌v�Z���ʂ�SRGB�ɕϊ����ď�������
		rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		//�����_�[�^�[�Q�b�g�r���[�̐���
		device->CreateRenderTargetView(backBuffers[i].Get(), &rtvDesc, rtvHandle);
	}

	//�t�F���X�̐���
	ComPtr<ID3D12Fence> fence = nullptr;
	UINT64 fenceVal = 0;

	result = device->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	//DirectInput�̏�����
	Input* input=nullptr;
	input = new Input;
	input->Initialize(winApp);

	//DirectX����������
	//�`�揉��������
	//���_�f�[�^�\����
	struct Vertex
	{
		XMFLOAT3 pos;    //xyz���W
		XMFLOAT3 normal; //�@���x�N�g��
		XMFLOAT2 uv;     //uv���W
	};
	//���_�f�[�^
	Vertex vertices[] = {
		//x      y	   z	 u    v
		//�O
		{{-5.0f,-5.0f,-5.0f},{}, {0.0f,1.0f} },//����
		{{-5.0f, 5.0f,-5.0f},{},{0.0f,0.0f} },//����
		{{ 5.0f,-5.0f,-5.0f},{},{1.0f,1.0f} },//�E��
		{{ 5.0f, 5.0f,-5.0f},{},{1.0f,0.0f} },//�E��
		//��
		{{-5.0f,-5.0f, 5.0f},{},{0.0f,1.0f} },//����
		{{-5.0f, 5.0f, 5.0f},{},{0.0f,0.0f} },//����
		{{ 5.0f,-5.0f, 5.0f},{},{1.0f,1.0f} },//�E��
		{{ 5.0f, 5.0f, 5.0f},{},{1.0f,0.0f} },//�E��
		//��
		{{-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f} },//����
		{{-5.0f,-5.0f, 5.0f},{},{0.0f,0.0f} },//����
		{{-5.0f, 5.0f,-5.0f},{},{1.0f,1.0f} },//�E��
		{{-5.0f, 5.0f, 5.0f},{},{1.0f,0.0f} },//�E��
		//�E
		{{ 5.0f,-5.0f,-5.0f},{},{0.0f,1.0f} },//����
		{{ 5.0f,-5.0f, 5.0f},{},{0.0f,0.0f} },//����
		{{ 5.0f, 5.0f,-5.0f},{},{1.0f,1.0f} },//�E��
		{{ 5.0f, 5.0f, 5.0f},{},{1.0f,0.0f} },//�E��
		//��
		{{-5.0f,-5.0f,-5.0f},{},{0.0f,1.0f} },//����
		{{-5.0f,-5.0f, 5.0f},{},{0.0f,0.0f} },//����
		{{ 5.0f,-5.0f,-5.0f},{},{1.0f,1.0f} },//�E��
		{{ 5.0f,-5.0f, 5.0f},{},{1.0f,0.0f} },//�E��
		//��
		{{-5.0f, 5.0f,-5.0f},{},{0.0f,1.0f} },//����
		{{-5.0f, 5.0f, 5.0f},{},{0.0f,0.0f} },//����
		{{ 5.0f, 5.0f,-5.0f},{},{1.0f,1.0f} },//�E��
		{{ 5.0f, 5.0f, 5.0f},{},{1.0f,0.0f} },//�E��
	};
	//�C���f�b�N�X�f�[�^
	unsigned short indices[] =
	{
		//�O
		0,1,2,//�O�p�`1
		1,3,2,//�O�p�`2
		//���
		6,5,4,//�O�p�`1
		6,7,5,//�O�p�`2
		//��
		8,9,10,//�O�p�`1
		9,11,10,//�O�p�`2
		//�E
		14,13,12,//�O�p�`1
		14,15,13,//�O�p�`2
		//��
		18,17,16,//�O�p�`1
		18,19,17,//�O�p�`2
		//��
		20,21,22,//�O�p�`1
		21,23,22,//�O�p�`2
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
	ComPtr<ID3D12Resource> vertBuff = nullptr;
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
	ComPtr<ID3D12Resource> indexBuff = nullptr;
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

	//�@���̌v�Z
	for (int i = 0; i < _countof(indices) / 3; i++)
	{//�O�p�`1���Ƃ̌v�Z���Ă���
		//�O�p�`�̃C���f�b�N�X�����o���āA�ꎞ�I�ȕϐ��ɓ����
		unsigned short indices0 = indices[i * 3 + 0];
		unsigned short indices1 = indices[i * 3 + 1];
		unsigned short indices2 = indices[i * 3 + 2];
		//�O�p�`���\�����钸�_���W���x�N�g���ɑ��
		XMVECTOR p0 = XMLoadFloat3(&vertices[indices0].pos);
		XMVECTOR p1 = XMLoadFloat3(&vertices[indices1].pos);
		XMVECTOR p2 = XMLoadFloat3(&vertices[indices2].pos);
		//p0��p1�x�N�g���Ap0��p2�x�N�g�����v�Z(�x�N�g�����Z)
		XMVECTOR v1 = XMVectorSubtract(p1, p0);
		XMVECTOR v2 = XMVectorSubtract(p2, p0);
		//�O�ς͗������琂���ȃx�N�g��
		XMVECTOR normal = XMVector3Cross(v1, v2);
		//���K���i������1�ɂ���j
		normal = XMVector3Normalize(normal);
		//���߂��@���𒸓_�f�[�^�ɑ��
		XMStoreFloat3(&vertices[indices0].normal, normal);
		XMStoreFloat3(&vertices[indices1].normal, normal);
		XMStoreFloat3(&vertices[indices2].normal, normal);
	}

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
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	//GPU���z�A�h���X
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	//���_�o�b�t�@�̃T�C�Y
	vbView.SizeInBytes = sizeVB;
	//���_����̃f�[�^�T�C�Y
	vbView.StrideInBytes = sizeof(vertices[0]);

	//�C���f�b�N�X�o�b�t�@�r���[�̍쐬
	D3D12_INDEX_BUFFER_VIEW ibView{};
	ibView.BufferLocation = indexBuff->GetGPUVirtualAddress();
	ibView.Format = DXGI_FORMAT_R16_UINT;
	ibView.SizeInBytes = sizeIB;

	ID3DBlob* vsBlob = nullptr;//���_�V�F�[�_�[�I�u�W�F�N�g
	ID3DBlob* psBlob = nullptr;//�s�N�Z���V�F�[�_�[�I�u�W�F�N�g
	ID3DBlob* errorBlob = nullptr;//�G���[�I�u�W�F�N�g

	//���_�V�F�[�_�[�̓ǂݍ��݂ƃR���p�C��
	result = D3DCompileFromFile(
		L"Resources/shaders/BasicVS.hlsl", //�V�F�[�_�[�t�@�C����
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
		L"Resources/shaders/BasicPS.hlsl",
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"Graph", "ps_5_0",
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
		},//vyz���W(��s�ŏ������ق������₷��)
		{
			"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},//�@���x�N�g��(��s�ŏ������ق������₷��)
		{
		"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0,
		},//uv(��s�ŏ������ق������₷��)
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
	pipelineDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;//�J�����O���Ȃ�
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
	D3D12_ROOT_PARAMETER rootParams[3] = {};
	rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//�萔�o�b�t�@
	rootParams[0].Descriptor.ShaderRegister = 0;//�萔�o�b�t�@�ԍ�
	rootParams[0].Descriptor.RegisterSpace = 0;//�f�t�H���g�l
	rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_�[���猩����
	//�e�N�X�`�����W�X�^0��
	rootParams[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//���
	rootParams[1].DescriptorTable.pDescriptorRanges = &descriptorRange;//�f�X�N���v�^�����W
	rootParams[1].DescriptorTable.NumDescriptorRanges = 1;//�f�X�N���v�^�����W��
	rootParams[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//�S�ẴV�F�[�_�[���猩����
	//�萔�o�b�t�@1�Ԗ�
	rootParams[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//���
	rootParams[2].Descriptor.ShaderRegister = 1;				//�萔�o�b�t�@�ԍ�
	rootParams[2].Descriptor.RegisterSpace = 0;					//�f�t�H���g�l
	rootParams[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;//���ׂẴV�F�[�_�[���猩����
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
	ComPtr<ID3D12RootSignature> rootSignature;
	//���[�g�V�O�l�`���̐ݒ�
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	rootSignatureDesc.pParameters = rootParams;//���[�g�p�����[�^�[�̐擪�A�h���X
	rootSignatureDesc.NumParameters = _countof(rootParams);//���[�g�p�����[�^��
	rootSignatureDesc.pStaticSamplers = &samplerDesc;
	rootSignatureDesc.NumStaticSamplers = 1;
	//���[�g�V�O�l�`���̃V���A���C�Y
	ComPtr<ID3DBlob> rootSigBlob;
	result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);
	assert(SUCCEEDED(result));
	result = device->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(result));
	//�p�C�v���C���Ƀ��[�g�V�O�l�`�����Z�b�g
	pipelineDesc.pRootSignature = rootSignature.Get();

	//�f�v�X�X�e���V���X�e�[�g�̐ݒ�
	pipelineDesc.DepthStencilState.DepthEnable = true;//�[�x�e�X�g���s��
	pipelineDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;//�������݋���
	pipelineDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;//��������΍��i
	pipelineDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;//�[�x�l�t�H�[�}�b�g

	//�p�C�v�����X�X�e�[�g�̐���
	ComPtr<ID3D12PipelineState> pipelineState = nullptr;
	result = device->CreateGraphicsPipelineState(&pipelineDesc, IID_PPV_ARGS(&pipelineState));
	assert(SUCCEEDED(result));

	ComPtr<ID3D12Resource> constBuffTransform0 = nullptr;
	ConstBufferDataTransform* constMapTransform0 = nullptr;
	////�q�[�v�ݒ�
	//D3D12_HEAP_PROPERTIES cbHeapProp{};
	//cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
	////���\�[�X�ݒ�
	//D3D12_RESOURCE_DESC cbResourceDesc{};
	//cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	//cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;//256�o�C�g�A���C�������g
	//cbResourceDesc.Height = 1;
	//cbResourceDesc.DepthOrArraySize = 1;
	//cbResourceDesc.MipLevels = 1;
	//cbResourceDesc.SampleDesc.Count = 1;
	//cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	//3D�I�u�W�F�N�g�̐�
	const size_t kObjectConst = 2;
	//3D�I�u�W�F�N�g�̔z��
	Object3D object3ds[kObjectConst];

	//�z����̑S�I�u�W�F�N�g�ɑ΂���
	for (int i = 0; i < _countof(object3ds); i++) {
		//������
		object3ds[i].Initialize(device.Get());

		//��������͐e�q�\���̃T���v��
		//�擪�ȊO�Ȃ�
		if (i > 0) {
			////��O�̃I�u�W�F�N�g��e�I�u�W�F�N�g�Ƃ���
			//object3ds[i].parent = &object3ds[i - 1];
			//�e�I�u�W�F�N�g��9���̑傫��
			object3ds[i].scale = { 0.9f,0.9f,0.9f };
			//�e�I�u�W�F�N�g�ɑ΂���Z���܂���30�x��]
			object3ds[i].rotation = { 0.0f,0.0f,XMConvertToRadians(30.0f) };

			//�e�I�u�W�F�N�g�ɑ΂���Z����-8.0���炷
			object3ds[i].position = { 0.0f,0.0f,-8.0f };
		}

		////�萔�o�b�t�@�̐���
		//result = device->CreateCommittedResource(
		//	&cbHeapProp,//�q�[�v�ݒ�
		//	D3D12_HEAP_FLAG_NONE,
		//	&cbResourceDesc,//���\�[�X�ݒ�
		//	D3D12_RESOURCE_STATE_GENERIC_READ,
		//	nullptr,
		//	IID_PPV_ARGS(&constBuffTransform0));
		//assert(SUCCEEDED(result));

		////�萔�o�b�t�@�̃}�b�s���O
		//result = constBuffTransform0->Map(0, nullptr, (void**)&constMapTransform0);//�}�b�s���O
		//assert(SUCCEEDED(result));
	}
	ComPtr<ID3D12Resource> constBuffTransform1 = nullptr;
	ConstBufferDataTransform* constMapTransform1 = nullptr;
	{
		//�q�[�v�ݒ�
		D3D12_HEAP_PROPERTIES cbHeapProp{};
		cbHeapProp.Type = D3D12_HEAP_TYPE_UPLOAD;
		//���\�[�X�ݒ�
		D3D12_RESOURCE_DESC cbResourceDesc{};
		cbResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		cbResourceDesc.Width = (sizeof(ConstBufferDataTransform) + 0xff) & ~0xff;//256�o�C�g�A���C�������g
		cbResourceDesc.Height = 1;
		cbResourceDesc.DepthOrArraySize = 1;
		cbResourceDesc.MipLevels = 1;
		cbResourceDesc.SampleDesc.Count = 1;
		cbResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

		////�萔�o�b�t�@�̐���
		//result = device->CreateCommittedResource(
		//	&cbHeapProp,//�q�[�v�ݒ�
		//	D3D12_HEAP_FLAG_NONE,
		//	&cbResourceDesc,//���\�[�X�ݒ�
		//	D3D12_RESOURCE_STATE_GENERIC_READ,
		//	nullptr,
		//	IID_PPV_ARGS(&constBuffTransform1));

		////�萔�o�b�t�@�̃}�b�s���O
		//result = constBuffTransform1->Map(0, nullptr, (void**)&constMapTransform1);//�}�b�s���O
		//assert(SUCCEEDED(result));
	}
	////���s���e�s��̌v�Z
	//constMapTransform->mat = XMMatrixOrthographicOffCenterLH(
	//	0.0f, window_width,
	//	window_heigit, 0.0f,
	//	0.0f, 1.0f);
	XMMATRIX matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),		   //�㉺������45�x
		(float)WinApp::window_width / WinApp::window_heigit,//�A�X�y�N�g��i��ʉ���/��ʏc���j
		0.1f, 1000.0f					   //�O�[�A���[
	);

	//�r���[�ϊ��s��
	XMMATRIX matView;
	XMFLOAT3 eye(0, 0, -100);//���_�x�N�g��
	XMFLOAT3 target(0, 0, 0);//�����_���W
	XMFLOAT3 up(0, 1, 0);	 //������x�N�g��
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));

	////�s��̌v�Z
	//constMapTransform0->mat = matView * matProjection;

	////�s��̌v�Z
	//constMapTransform1->mat = matView * matProjection;

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
	ComPtr<ID3D12Resource> constBuffMaterial = nullptr;
	//�萔�o�b�t�@�̐���
	result = device->CreateCommittedResource(
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
	constMapMaterial->color = XMFLOAT4(1, 1, 1, 0.5f);//RGBA�Ŕ������̐�
	assert(SUCCEEDED(result));

	//�������s�N�Z����
	const size_t textureWidth = 256;
	//�c�����s�N�Z����
	const size_t textureHeight = 256;
	//�z��̗v�f��
	const size_t imageDataCount = textureWidth * textureHeight;
	//�摜�C���[�W�f�[�^�z��
	XMFLOAT4* imageData = new XMFLOAT4[imageDataCount];//�K����ŉ������

	//�S�s�N�Z���̐F��������
	for (size_t i = 0; i < imageDataCount; i++) {
		imageData[i].x = 1.0f;//R
		imageData[i].y = 0.0f;//G
		imageData[i].z = 0.0f;//B
		imageData[i].w = 1.0f;//A
	}
	TexMetadata metadata{};
	ScratchImage scratchImg{};
	//WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		L"Resources/mario.jpg",//Resources�t�H���_��texture.png
		WIC_FLAGS_NONE,
		&metadata, scratchImg);
	ScratchImage mipChain{};
	//�~�j�}�b�v����
	result = GenerateMipMaps(
		scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
		TEX_FILTER_DEFAULT, 0, mipChain);
	if (SUCCEEDED(result)) {
		scratchImg = std::move(mipChain);
		metadata = scratchImg.GetMetadata();
	}
	//�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	metadata.format = MakeSRGB(metadata.format);
	//�q�[�v�ݒ�
	D3D12_HEAP_PROPERTIES textureHeapProp{};
	textureHeapProp.Type = D3D12_HEAP_TYPE_CUSTOM;
	textureHeapProp.CPUPageProperty =
		D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	textureHeapProp.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc{};
	textureResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc.Format = metadata.format;
	textureResourceDesc.Width = metadata.width;//��
	textureResourceDesc.Height = (UINT)metadata.height;//����
	textureResourceDesc.DepthOrArraySize = (UINT16)metadata.arraySize;
	textureResourceDesc.MipLevels = (UINT16)metadata.mipLevels;
	textureResourceDesc.SampleDesc.Count = 1;
	//�e�N�X�`���o�b�t�@�𐶐�
	ComPtr<ID3D12Resource> texBuff = nullptr;
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff));
	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < metadata.mipLevels; i++)
	{
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^��]��
		result = texBuff->WriteToSubresource(
			(UINT)i,
			nullptr,//�S�̈�փR�s�[
			img->pixels,//���f�[�^�A�h���X
			(UINT)img->rowPitch,//1���C���T�C�Y
			(UINT)img->slicePitch//�ꖇ�T�C�Y
		);
		assert(SUCCEEDED(result));
	}
	//SRV�̍ő��
	const size_t kMaxSRVCount = 2056;

	//�f�X�N���v�^�q�[�v�̐ݒ�
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//�V�F�[�_�[���猩����悤��
	srvHeapDesc.NumDescriptors = kMaxSRVCount;

	//�ݒ������SRV�p�f�X�N���v�^�q�[�v�𐶐�
	ComPtr <ID3D12DescriptorHeap> srvHeap = nullptr;
	result = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap));
	assert(SUCCEEDED(result));
	//SRV�q�[�v�̐擪�n���h�����擾
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle = srvHeap->GetCPUDescriptorHandleForHeapStart();

	//�V�F�[�_���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};//�ݒ�\����
	srvDesc.Format = resDesc.Format;//RGBA float
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc.Texture2D.MipLevels = resDesc.MipLevels;

	//�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	device->CreateShaderResourceView(texBuff.Get(), &srvDesc, srvHandle);

	TexMetadata metadata2{};
	ScratchImage scratchImg2{};
	//WIC�e�N�X�`���̃��[�h
	result = LoadFromWICFile(
		L"Resources/reimu.png",//Resources�t�H���_��texture.png
		WIC_FLAGS_NONE,
		&metadata2, scratchImg2);

	//���\�[�X�ݒ�
	D3D12_RESOURCE_DESC textureResourceDesc2{};
	textureResourceDesc2.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	textureResourceDesc2.Format = metadata2.format;
	textureResourceDesc2.Width = metadata2.width;//��
	textureResourceDesc2.Height = (UINT)metadata2.height;//����
	textureResourceDesc2.DepthOrArraySize = (UINT16)metadata2.arraySize;
	textureResourceDesc2.MipLevels = (UINT16)metadata2.mipLevels;
	textureResourceDesc2.SampleDesc.Count = 1;

	//�e�N�X�`���o�b�t�@�𐶐�
	ComPtr<ID3D12Resource> texBuff2 = nullptr;
	result = device->CreateCommittedResource(
		&textureHeapProp,
		D3D12_HEAP_FLAG_NONE,
		&textureResourceDesc2,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&texBuff2));

	//�S�~�b�v�}�b�v�ɂ���
	for (size_t i = 0; i < metadata2.mipLevels; i++)
	{
		//�~�b�v�}�b�v���x�����w�肵�ăC���[�W���擾
		const Image* img = scratchImg2.GetImage(i, 0, 0);
		//�e�N�X�`���o�b�t�@�Ƀf�[�^��]��
		result = texBuff2->WriteToSubresource(
			(UINT)i,
			nullptr,//�S�̈�փR�s�[
			img->pixels,//���f�[�^�A�h���X
			(UINT)img->rowPitch,//1���C���T�C�Y
			(UINT)img->slicePitch//�ꖇ�T�C�Y
		);
		assert(SUCCEEDED(result));
	}

	//��n���h����i�߂�
	UINT incrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	srvHandle.ptr += incrementSize;

	//�V�F�[�_���\�[�X�r���[�ݒ�
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};//�ݒ�\����
	srvDesc2.Format = textureResourceDesc2.Format;//RGBA float
	srvDesc2.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2D�e�N�X�`��
	srvDesc2.Texture2D.MipLevels = textureResourceDesc2.MipLevels;

	//�n���h���̎w���ʒu�ɃV�F�[�_�[���\�[�X�r���[�쐬
	device->CreateShaderResourceView(texBuff2.Get(), &srvDesc2, srvHandle);

	float angle = 0.0f;//�J�����̉�]�p

	//XMFLOAT3 position = { 0.0f,-30.0f,0.0f };

	//XMFLOAT3 scale = { 1.0f,1.0f,1.0f };

	//XMFLOAT3 rotation = { 0.0f,0.0f,0.0f };

	//XMFLOAT3 position1 = { -20.0f,0,0 };

	//XMFLOAT3 scale1 = { 1.0f,1.0f,1.0f };

	//XMFLOAT3 rotation1 = { 0.0f,45.0f,0.0f };

	////���[���h�ϊ��s��
	//XMMATRIX matWorld = XMMatrixIdentity();

	//XMMATRIX matScale = XMMatrixIdentity();
	//matScale *= XMMatrixScaling(scale.x, scale.y, scale.z);

	//XMMATRIX matRot = XMMatrixIdentity();//��]�s��
	//matRot *= XMMatrixRotationZ(XMConvertToRadians(XM_PI / 180 * rotation.z));//Z������0�x��]
	//matRot *= XMMatrixRotationX(XMConvertToRadians(XM_PI / 180 * rotation.x));//X������15�x��]
	//matRot *= XMMatrixRotationY(XMConvertToRadians(XM_PI / 180 * rotation.y));//Y�����ɓx��]

	//XMMATRIX matTrans = XMMatrixIdentity();//���s�ړ��s��
	//matTrans *= XMMatrixTranslation(position.x, position.y, position.z);//(-50,0,0)���s�ړ�

	////���[���h�ϊ��s��
	//XMMATRIX matWorld1=XMMatrixIdentity();

	//XMMATRIX matScale1=XMMatrixScaling(scale1.x, scale1.y, scale1.z);//�X�P�[�����O�s��

	//XMMATRIX matRot1=XMMatrixRotationY(XM_PI / 180 * rotation1.y);//��]�s��

	//XMMATRIX matTrans1=XMMatrixTranslation(position1.x, position1.y, position1.z);//���s�ړ��s��

	////���[���h�s�������
	//matWorld1 = matScale1 * matRot1 * matTrans1;
	////���[���h�A�r���[�A�ˉe�s����������ăV�F�[�_�[�ɓ]��
	//constMapTransform1->mat = matWorld1 * matView * matProjection;;

	//�Q�[�����[�v
	while (true) {

		//�~�{�^���ŏI�����b�Z�[�W��������Q�[�����[�v�𔲂���
		if (winApp->ProcessMessage()) {
			break;
		}
		//DirectX���t���[�������@��������
		
		//���͂̍X�V
		input->Update();
		
		//�o�b�N�o�b�t�@�̔ԍ����擾�i�Q�Ȃ̂łO�Ԃ��P�ԁj
		UINT bbIndex = swapChain->GetCurrentBackBufferIndex();

		//1.���\�[�X�o���A�ŏ������݉\�ɕύX
		D3D12_RESOURCE_BARRIER barrierDesc{};
		barrierDesc.Transition.pResource = backBuffers[bbIndex].Get();//�o�b�N�o�b�t�@���w��
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;//�\����Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		commandList->ResourceBarrier(1, &barrierDesc);
		//2.�`���̕ύX
		// �����_�[�^�[�Q�b�g�r���[�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += bbIndex * device->GetDescriptorHandleIncrementSize(rtvHeapDesc.Type);
		//�[�x�X�e���V���r���[�p�f�X�N���v�^�q�[�v�̃n���h�����擾
		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap->GetCPUDescriptorHandleForHeapStart();
		commandList->OMSetRenderTargets(1, &rtvHandle, false, &dsvHandle);

		//3.��ʃN���A
		FLOAT clearColor[] = { 0.1f,0.25f,0.5f,0.0f };//���ۂ��F
		commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
		commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


		//�X�V������������
		if (input->PushKey(DIK_Z) || input->PushKey(DIK_X))
		{
			if (input->PushKey(DIK_Z)) { angle += XMConvertToRadians(1.0f); }
			else if (input->PushKey(DIK_X)) { angle -= XMConvertToRadians(1.0f); }

			//angle���W�A������Y���܂��ɉ�]�B���a��-100
			eye.x = -100 * sinf(angle);
			eye.z = -100 * cosf(angle);
			matView = XMMatrixIdentity();
			matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
		}

		//�����ꂩ�̃L�[�������Ă�����
		if (input->TriggerKey(DIK_UP)) { object3ds[0].position.y += 1.0f; }
		else if (input->PushKey(DIK_DOWN)) { object3ds[0].position.y -= 1.0f; }
		if (input->PushKey(DIK_RIGHT)) { object3ds[0].position.x += 1.0f; }
		else if (input->PushKey(DIK_LEFT)) { object3ds[0].position.x -= 1.0f; }

		if (input->PushKey(DIK_Q)) { object3ds[0].rotation.z += 0.1f; }
		else if (input->PushKey(DIK_E)) { object3ds[0].rotation.z -= 0.1f; }


		////���[���h�ϊ��s��
		//matWorld = XMMatrixIdentity();

		//matTrans = XMMatrixIdentity();//���s�ړ��s��
		//matTrans *= XMMatrixTranslation(position.x, position.y, position.z);//(-50,0,0)���s�ړ�

		//matWorld *= matScale;//���[���h���W�ɃX�P�[�����O�𔽉f
		//matWorld *= matRot;//���[���h�s��ɉ�]�𔽉f
		//matWorld *= matTrans;//���[���h�s��ɕ��s�ړ��𔽉f

		////���[���h�ϊ��s��
		//matWorld1 = XMMatrixIdentity();

		//matTrans1 = XMMatrixIdentity();//���s�ړ��s��
		//matTrans1 *= XMMatrixTranslation(position1.x, position1.y, position1.z);//(-50,0,0)���s�ړ�

		//matWorld1 *= matScale1;//���[���h���W�ɃX�P�[�����O�𔽉f
		//matWorld1 *= matRot1;//���[���h�s��ɉ�]�𔽉f
		//matWorld1 *= matTrans1;//���[���h�s��ɕ��s�ړ��𔽉f

		////�s��̌v�Z
		//constMapTransform0->mat = matWorld * matView * matProjection;
		////�s��̌v�Z
		//constMapTransform1->mat = matWorld1 * matView * matProjection;;
		for (size_t i = 0; i < _countof(object3ds); i++)
		{
			object3ds[i].Update(matView, matProjection);
		}
		//�X�V���������܂�

		//�`��R�}���h��������
		//�r���[�|�[�g�ݒ�R�}���h
		D3D12_VIEWPORT viewport{};
		viewport.Width = WinApp::window_width;
		viewport.Height = WinApp::window_heigit;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		//�r���[�|�[�g�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		commandList->RSSetViewports(1, &viewport);

		//�V�U�[�Z�a
		D3D12_RECT scissorRect{};
		scissorRect.left = 0;
		scissorRect.right = scissorRect.left + WinApp::window_width;
		scissorRect.top = 0;
		scissorRect.bottom = scissorRect.top + WinApp::window_heigit;

		//�V�U�\�Z�a�ݒ�R�}���h���A�R�}���h���X�g�ɐς�
		commandList->RSSetScissorRects(1, &scissorRect);

		//�p�C�v���C���X�e�[�g�ƃ��[�g�V�O�l�`���̐ݒ�R�}���h
		commandList->SetPipelineState(pipelineState.Get());
		commandList->SetGraphicsRootSignature(rootSignature.Get());
		//�v���~�e�B�u�`��̐ݒ�R�}���h
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//���_�o�b�t�@�r���[�̐ݒ�R�}���h
		commandList->IASetVertexBuffers(0, 1, &vbView);
		//�萔�o�b�t�@�r���[�iCBV�j�̐ݒ�R�}���h
		commandList->SetGraphicsRootConstantBufferView(0, constBuffMaterial->GetGPUVirtualAddress());
		//SRV�q�[�v�̐ݒ�R�}���h
		commandList->SetDescriptorHeaps(1, srvHeap.GetAddressOf());
		//SRV�q�[�v�̐擪�n���h�����擾�iSRV���w���Ă���͂��j
		D3D12_GPU_DESCRIPTOR_HANDLE srvGpuHandle = srvHeap->GetGPUDescriptorHandleForHeapStart();
		//2���ڂ��w�������悤�ɂ���SRV�̃n���h�������g�p�����[�^�[1�Ԃɐݒ�
		/*srvGpuHandle.ptr += incrementSize;*/
		//SRV�q�[�v�̐擪�ɂ���SRV�����[�g�p�����[�^1�Ԃɐݒ�
		commandList->SetGraphicsRootDescriptorTable(1, srvGpuHandle);
		////�C���f�b�N�X�o�b�t�@�r���[�̐ݒ�R�}���h
		//commandList->IASetIndexBuffer(&ibView);
		////�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
		//commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform0->GetGPUVirtualAddress());
		////�`��R�}���h
		//commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);//���ׂĂ̒��_���g���ĕ`��

		////�萔�o�b�t�@�r���[(CBV)�̐ݒ�R�}���h
		//commandList->SetGraphicsRootConstantBufferView(2, constBuffTransform1->GetGPUVirtualAddress());
		////�`��R�}���h
		//commandList->DrawIndexedInstanced(_countof(indices), 1, 0, 0, 0);//���ׂĂ̒��_���g���ĕ`��

		//�S�I�u�W�F�N�g�ɂ��Ă̏���
		for (int i = 0; i < _countof(object3ds); i++)
		{
			object3ds[i].Draw(commandList.Get(), vbView, ibView, _countof(indices));
		}

		//�`��R�}���h�����܂�

		// 5.���\�[�X�o���A��߂�
		barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;//�`���Ԃ���
		barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		commandList->ResourceBarrier(1, &barrierDesc);

		//���߃N���[�Y
		result = commandList->Close();
		assert(SUCCEEDED(result));
		//�R�}���h���X�g�̎��s
		ID3D12CommandList* commandLists[] = { commandList.Get()};
		commandQueue->ExecuteCommandLists(1, commandLists);

		//��ʂɕ\������o�b�t�@���t���b�v�i���\�̓���ւ��j
		result = swapChain->Present(1, 0);
		assert(SUCCEEDED(result));
		//�R�}���h�̎��s������҂�
		commandQueue->Signal(fence.Get(), ++fenceVal);
		if (fence->GetCompletedValue() != fenceVal) {
			HANDLE event = CreateEvent(nullptr, false, false, nullptr);
			fence->SetEventOnCompletion(fenceVal, event);
			WaitForSingleObject(event, INFINITE);
			CloseHandle(event);
		}
		//�L���[�̃N���A
		result = comdAllocator->Reset();
		assert(SUCCEEDED(result));
		//�ĂуR�}���h���X�g�����߂鏀��
		result = commandList->Reset(comdAllocator.Get(), nullptr);
		assert(SUCCEEDED(result));

		//DirectX���t���[�������@�����܂�

	}
	delete input;

	winApp->Finalize();

	delete winApp;

	return 0;
}