#include"Sprite.hlsli"
Texture2D<float4> tex:register(t0);//0番スロットに設定されたテクスチャ
SamplerState smp:register(s0); //0番スロットに設定されたサンプラー

////マテリアル
//cbuffer ConstBufferDateMaterial:register(b0)
//{
//	float4 color;//色（RGBA）
//}

float4 main(VSOutput input) :SV_TARGET
{
	return float4(tex.Sample(smp,input.uv)*color);
}