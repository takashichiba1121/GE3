#pragma once
#define DIRECTINPUT_VERSION 0x0800 //DirectInput�̃o�[�W�����w��
#include<dinput.h>
#include<windows.h>
#include<wrl.h>
#include"WinApp.h"

//����
class Input
{
public:

	template <class T> using ComPtr= Microsoft::WRL::ComPtr<T>;

public://�����o�֐�

	//������
	void Initialize(WinApp* winApp);
	//�X�V
	void Update();

	/// <summary>
	/// �L�[�̉������`�F�b�N
	/// </summary>
	///<param name="keynumber">�L�[�ԍ�(DIK_0��)</param>
	/// <returns>������Ă��邩</returns>
	bool PushKey(BYTE keyNumber);
	/// <summary>
	/// �L�[�̃g���K�[���`�F�b�N
	/// </summary>
	///<param name="keynumber">�L�[�ԍ�(DIK_0��)</param>
	/// <returns>�g���K�[��</returns>
	bool TriggerKey(BYTE keyNumber);
private://���C���ϐ�
	//�L�[�{�[�h�f�o�C�X
	ComPtr<IDirectInputDevice8> keyboard = nullptr;

	ComPtr<IDirectInput8> directInput = nullptr;

	BYTE key[256] = {};

	BYTE keyPre[256] = {};

	//WindowAPI
	WinApp* winApp_ = nullptr;
};
