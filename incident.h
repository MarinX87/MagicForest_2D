//=============================================================================
// 事件処理 [incident.h]
//=============================================================================
#pragma once
#include "main.h"
#include "renderer.h"


//*****************************************************************************
// マクロ定義
//*****************************************************************************

//effect
#define PARTICLE_COUNT                   (200)      

struct INCIDENT
{
	XMFLOAT3	pos;		    // ポリゴンの座標
	float		w, h;		    // 幅と高さ
	int         texNo;		    // 使用しているテクスチャ番号
	BOOL        use;
	XMFLOAT4    color;          // デフォルト位置値
	float       alpha;         
	float		countAnim;
	int         frame;
	float       iceTime;
	BOOL        isIce;
};


struct PARTICLE2 {
	XMFLOAT3	pos;            // ポリゴンの座標
	float		w, h;		    // 幅と高さ
	float       dx, dy;         // スビート
	XMFLOAT4    color;          // 色
	float       alphaDecay;     // 透明度減衰
	BOOL        use;
};


static enum
{
	LEVEL_UP,
	SNOW,
	RAINBOW,
	BOMB
};
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitIncident(void);
void    UninitIncident(void);
void    UpdateIncident(void);
void    DrawIncident(void);
int     generateTexNo(void);
void InitializeParticles(PARTICLE2 particles[PARTICLE_COUNT]);