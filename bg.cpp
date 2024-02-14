//=============================================================================
// BG処理 [bg.cpp]
//=============================================================================

#include "bg.h"
#include "score.h"
#include "sound.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX                    (4)				// テクスチャの数

//BG
#define TEXTURE_WIDTH                 (12789)			// 背景サイズ
#define TEXTURE_HEIGHT                (2354)			// 背景サイズ



//BOX
#define TEXTURE_PATTERN_DIVIDE_X       (5)		         // アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y       (4)		         // アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM   (TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT                      (7)		         // アニメーションの切り替わるWait値
#define BOX_MAX                        (5)
#define TEXTURE_WIDTH_BOX              (120)			// BOXサイズ
#define TEXTURE_HEIGHT_BOX             (120)			// BOXサイズ


//L範囲
#define TEXTURE_WIDTH_L                 (280)			// 背景サイズ
#define TEXTURE_HEIGHT_L                (520)			// 背景サイズ

//M範囲
#define TEXTURE_WIDTH_M                 (120)			// 背景サイズ
#define TEXTURE_HEIGHT_M                (130)			// 背景サイズ

//R範囲
#define TEXTURE_WIDTH_R                 (300)			// 背景サイズ
#define TEXTURE_HEIGHT_R                (530)			// 背景サイズ

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		        // 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報


static int culTime;
static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/map.png",
	"data/TEXTURE/box.png",
	"data/TEXTURE/bar_white.png",
	"data/TEXTURE/tip.png"
	
};


static BOOL	g_Load = FALSE;		// 初期化を行ったかのフラグ
static BG	g_BG;
static BG	g_Box[BOX_MAX];

static BOOL tip=FALSE;          //BOXのヒント
static int  tipTime;

//園collision用
static BG	g_L[BOX_MAX];
static BG	g_M[BOX_MAX];
static BG	g_R[BOX_MAX];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBG(void)
{
	culTime = 0;
	tipTime = 0;

	ID3D11Device *pDevice = GetDevice();

	//テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// BGの初期化
	g_BG.w     = TEXTURE_WIDTH;
	g_BG.h     = TEXTURE_HEIGHT;
	g_BG.pos   = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_BG.scrl  = 0.0f;		
	g_BG.scrl2 = 0.0f;	


	//BOXの初期化
	for (int i = 0; i < BOX_MAX; i++)
	{
		g_Box[i].w = TEXTURE_WIDTH_BOX;
		g_Box[i].h = TEXTURE_HEIGHT_BOX;
		g_Box[i].countAnim = 0;
		g_Box[i].pos = XMFLOAT3(650.0f + i * 2535.0f, 700.0f, 0.0f);
		g_Box[i].use = FALSE;
		g_Box[i].start = FALSE;
		g_Box[i].frame = 0;
	}


	//L範囲の初期化
	for (int i = 0; i < BOX_MAX; i++)
	{
		g_L[i].w = TEXTURE_WIDTH_L;
		g_L[i].h = TEXTURE_HEIGHT_L;
		g_L[i].pos = XMFLOAT3(450.0f + i * 2535.0f, 800.0f, 0.0f);

	}

	//M範囲の初期化
	for (int i = 0; i < BOX_MAX; i++)
	{
		g_M[i].w = TEXTURE_WIDTH_M;
		g_M[i].h = TEXTURE_HEIGHT_M;
		g_M[i].pos = XMFLOAT3(650.0f + i * 2535.0f, 600.0f, 0.0f);

	}

	//R範囲の初期化
	for (int i = 0; i < BOX_MAX; i++)
	{
		g_R[i].w = TEXTURE_WIDTH_R;
		g_R[i].h = TEXTURE_HEIGHT_R;
		g_R[i].pos = XMFLOAT3(900.0f + i * 2535.0f, 800.0f, 0.0f);

	}


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBG(void)
{

	if (g_Load == FALSE) return;
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}
	g_Load = FALSE;
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBG(void)
{

	// １フレ前の情報を保存
	g_BG.old_pos = g_BG.pos;	

	//BOXのリフレッシュ
	culTime = GetTime();

	if ((culTime % 60 == 0) && (culTime != 0) )
	{
		for (int i = 0; i < BOX_MAX; i++)
		{
			if (g_Box[i].use == FALSE)
			{
				g_Box[i].use = TRUE;
				PlaySound(3);
				tip = TRUE;
				tipTime = GetTime();
			}
		}
	}

	if ((GetTime() - tipTime  >=  3) && (GetTime() - tipTime != 0))
	{
		tip = FALSE;
	}


	//BOXのアニメーション処理
	for (int i = 0; i < BOX_MAX; i++)
	{
		if (g_Box[i].use == TRUE)
		{
			g_Box[i].countAnim += 1.0f;
			if (g_Box[i].countAnim > ANIM_WAIT)
			{
				g_Box[i].countAnim = 0.0f;
				g_Box[i].frame = (g_Box[i].frame + 1) % (TEXTURE_PATTERN_DIVIDE_X * TEXTURE_PATTERN_DIVIDE_Y);
			}
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBG(void)
{

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(     
		0, 
		1,
		&g_VertexBuffer,
		&stride,
		&offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	// タイトルの背景を描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			0 - g_BG.pos.x, 0 - g_BG.pos.y, g_BG.w, g_BG.h,
			1.0f, 1.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);
	}


	//BOXを描画
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);


		for (int i = 0; i < BOX_MAX; i++)
		{
			if (g_Box[i].use == TRUE)
			{
				//BOXの位置やテクスチャー座標を反映
				float px = g_Box[i].pos.x - g_BG.pos.x;	 // BOXの表示位置X
				float py = g_Box[i].pos.y - g_BG.pos.y;	 // BOXの表示位置Y
				float pw = g_Box[i].w;                   // BOXの表示幅
				float ph = g_Box[i].h;                   // BOXの表示高さ


				float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;
				float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;
				float tx = (float)(g_Box[i].frame % TEXTURE_PATTERN_DIVIDE_X) * tw;
				float ty = (float)(g_Box[i].frame / TEXTURE_PATTERN_DIVIDE_Y) * th;

				SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);

				GetDeviceContext()->Draw(4, 0);

			}
		}
	}


	//TIPを描画
	{
		

		if (tip == TRUE)
		{

			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);

			//TIPの位置やテクスチャー座標を反映
			float px = SCREEN_WIDTH/2;	                     //TIPの表示位置X
			float py = SCREEN_HEIGHT/2 - 300.0f;	                     //TIPの表示位置Y
			float pw = 600.0f;                               //TIPの表示幅
			float ph = 400.0f;                               //TIPの表示高さ


			float tw = 1.0f;
			float th = 1.0f;
			float tx = 1.0f;
			float ty = 1.0f;

			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);

			GetDeviceContext()->Draw(4, 0);
		}



	}




}



// BG構造体の先頭アドレスを取得
BG* GetBG(void)
{
	return &g_BG;
}


BG* GetRangeL(void)
{
	return &g_L[0];
}

BG* GetRangeM(void)
{
	return &g_M[0];
}

BG* GetRangeR(void)
{
	return &g_R[0];
}


BG* GetBox(void)
{
	return &g_Box[0];
}


