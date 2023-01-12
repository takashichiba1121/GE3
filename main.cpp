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
#include"object3d.h"
#include"input.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include"SpriteCommon.h"
#include"Sprite.h"
#include"Texture.h"
#include"GameScene.h"

using namespace DirectX;
using namespace std;


////�萔�o�b�t�@�p�f�[�^�\���́i�}�e���A���j
//struct ConstBufferDataMaterial {
//	XMFLOAT4 color;//�F�iRGBA�j
//};

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

	DirectXCommon* dxCommon = nullptr;
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//DirectX�����������@��������

	Input::Initialize(winApp);

	SpriteCommon* spriteCommon=nullptr;
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);

	Texture::Initialize(dxCommon->GetDevice());

	Sprite::StaticInitialize(spriteCommon);

	Model::SetDevice(dxCommon->GetDevice());

	Object3d::StaticInitialize(dxCommon->GetDevice(), WinApp::window_width, WinApp::window_heigit);

	
	GameScene* gameScene=new GameScene;
	gameScene->Initialize(gameScene);

	//�Q�[�����[�v
	while (true) {

		//�~�{�^���ŏI�����b�Z�[�W��������Q�[�����[�v�𔲂���
		if (winApp->ProcessMessage()) {
			break;
		}
		//DirectX���t���[�������@��������
		
		//���͂̍X�V
		Input::Update();

		gameScene->Update();

		//�`��R�}���h��������
		dxCommon->PreDraw();

		Object3d::PreDraw(dxCommon->GetCommandList());

		gameScene->ObjectDraw();

		Object3d::PostDraw();


		spriteCommon->PreDraw();

		gameScene->SpriteDraw();

		spriteCommon->PostDrow();

		//�`��R�}���h�����܂�

		dxCommon->PostDrow();

		//DirectX���t���[�������@�����܂�

	}

	delete spriteCommon;

	delete gameScene;

	delete dxCommon;

	winApp->Finalize();

	delete winApp;

	return 0;
}