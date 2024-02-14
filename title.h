//=============================================================================
// タイトル画面処理 [title.h]
//=============================================================================
#pragma once
#include "main.h"
#include "renderer.h"
#include "debugproc.h"
#include "sprite.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************
struct TITLE
{
	XMFLOAT3	pos;		// ポリゴンの座標
	float		w, h;		// 幅と高さ
	int         speed;      //ポイント移動スビート

};

enum
{
	M_TITLE,
	M_GUIDE
};

struct SUN
{
	float       x, y;          // 太陽の位置
	float       size;          // サイズ
	float       breathSpeed;   // 呼吸スビート
	float       maxScale;      // 拡大
	float       minScale;      // 縮小
	BOOL        isBreathingIn; // 拡大と縮小判定
	XMFLOAT4    color;
	float       minAlpha;      // 最小透明度
	float       maxAlpha;      // 最大透明度
};



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitTitle(void);
void UninitTitle(void);
void UpdateTitle(void);
void DrawTitle(void);


