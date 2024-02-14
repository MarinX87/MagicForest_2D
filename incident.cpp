//=============================================================================
// TITLE処理 [incident.cpp]
//=============================================================================
#include "incident.h"
#include "main.h"
#include "player.h"
#include "bg.h"
#include "collision.h"
#include "enemy.h"
#include "score.h"
#include "effect.h"
#include "item.h"
#include "pause.h"
#include <time.h>
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX		                  (7)
#define TEXTURE_WIDTH                    (150)	
#define TEXTURE_HEIGHT      	         (150)


//BOMBアニメーション
#define TEXTURE_PATTERN_DIVIDE_X          (4)		         // アニメパターンのテクスチャ内分割数（X)
#define TEXTURE_PATTERN_DIVIDE_Y          (4)		         // アニメパターンのテクスチャ内分割数（Y)
#define ANIM_PATTERN_NUM   (TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// アニメーションパターン数
#define ANIM_WAIT                         (5)		         // アニメーションの切り替わるWait値
#define TEXTURE_WIDTH_BOMB               (200)			     // BOXサイズ
#define TEXTURE_HEIGHT_BOMB              (200)			     // BOXサイズ


//effect
#define PARTICLE_COUNT                   (100)              

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer* g_VertexBuffer = NULL;		                    // 頂点情報
static ID3D11ShaderResourceView* g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static INCIDENT	g_Ici;

static char* g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/levelup2.png",
	"data/TEXTURE/snow.png",
	"data/TEXTURE/rainbow.png",
	"data/TEXTURE/bomb2.png",
	"data/TEXTURE/bombt.png",
	"data/TEXTURE/ice.jpg",
	"data/TEXTURE/effect000.jpg",
};

static BOOL	     g_Load = FALSE;		                                 // 初期化を行ったかのフラグ
static INCIDENT  g_Bomb;
static PARTICLE2 particles[PARTICLE_COUNT];

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitIncident(void)
{

	ID3D11Device* pDevice = GetDevice();

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

	//INCIDENTの初期化
	g_Ici.w = TEXTURE_WIDTH;
	g_Ici.h = TEXTURE_HEIGHT;
	g_Ici.pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	g_Ici.texNo = 0;
	g_Ici.use = FALSE;
	g_Ici.alpha = 1.0f;
	g_Ici.color = XMFLOAT4(1.0f, 1.0f, 1.0f, g_Ici.alpha);
	g_Ici.iceTime = 0;

	//Bomb初期化
	g_Bomb.w           = TEXTURE_WIDTH_BOMB;
	g_Bomb.h           = TEXTURE_HEIGHT_BOMB;
	g_Bomb.countAnim   = 0;
	g_Bomb.pos         = XMFLOAT3(0.0f,0.0f, 0.0f);
	g_Bomb.use         = FALSE;
	g_Bomb.frame       = 0;


	//efect初期化
	InitializeParticles(particles);



	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitIncident(void)
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
void UpdateIncident(void)
{
	BG* box        = GetBox();
	BG* bg         = GetBG();
	PLAYER* player = GetPlayer();
	
	ENEMY(*enemy)[ENEMY_MAX] = (ENEMY(*)[ENEMY_MAX])GetEnemy();

	for (int i = 0; i < BOX_MAX; i++)
	{
		if (box[i].start == TRUE)
		{
			g_Ici.pos = box[i].pos;
			g_Ici.texNo = generateTexNo();
			box[i].start = FALSE;
			g_Ici.use = TRUE;


			//LEVEL_UPモデル
			if (g_Ici.texNo == LEVEL_UP)
			{
				player->lv += 2;
				if (checkXpMAX())
				{
					Switch(LEVELUP);
				}
				
			}


			//凍るモデル
			if (g_Ici.texNo == SNOW)
			{
				for (int j = 0; j < 4; j++)
				{
					for (int k = 0; k < ENEMY_MAX; k++)
					{
						if (enemy[j][k].use == TRUE)
						{
							enemy[j][k].isFrozen = TRUE;
							enemy[j][k].freezeStartTime = GetTime();
						}
					}
				}

				g_Ici.iceTime = GetRelativeTime();
				g_Ici.isIce = TRUE;
			}


			//BOMBトラップ
			if (g_Ici.texNo == BOMB)
			{
				g_Bomb.use = TRUE;
				g_Bomb.pos = box[i].pos;

				//プレイヤーの影響
				player->flashing = TRUE;
				player->blood -= 2;
				player->flashStartTime = GetRelativeTime();
			}


			//RAINBOW
			if (g_Ici.texNo == RAINBOW)
			{
				for (int e = 0; e < PARTICLE_COUNT; e++)
				{
					particles[e].use = TRUE;
					particles[e].pos = box[i].pos;
				}
			}
	
			break;
		}
	}


	//RAINBOW　effect
	for (int o = 0; o < PARTICLE_COUNT; o++)
	{
		if (particles[o].use == TRUE)
		{
			particles[o].pos.x += particles[o].dx;
			particles[o].pos.y += particles[o].dy;
			particles[o].color.w -= particles[o].alphaDecay;
			if (particles[o].color.w < 0)
			{
				particles[o].color.w = 0;
			}

			for (int l = 0; l < 4; l++)
			{
				for (int m = 0; m < ENEMY_MAX; m++)
				{

					// 生きてるエネミーと当たり判定をする
					if (enemy[l][m].use == TRUE)
					{
						//当たり判定関数で
						BOOL ans = CollisionBB(particles[o].pos, particles[o].w, particles[o].h,
							enemy[l][m].pos, enemy[l][m].w, enemy[l][m].h);

						// 当たっている？
						if (ans == TRUE)
						{
							enemy[l][m].use = FALSE;

							// 撃破カウンター
							player->killed += 1;

							// エフェクト発生
							SetEffect(enemy[l][m].pos.x, enemy[l][m].pos.y, 30);

							// アイテム生成する
							SetItem(enemy[l][m].pos.x, enemy[l][m].pos.y);
						}
					}
				}
			}
		}
	}


	// effect
	BOOL allParticlesInactive = TRUE;  

	for (int i = 0; i < PARTICLE_COUNT; i++)
	{
		if (particles[i].color.w <= 0.0f)
		{
			particles[i].use = FALSE;
		}

		if (particles[i].use == TRUE)
		{
			allParticlesInactive = FALSE;  
		}
	}

	if (allParticlesInactive)
	{
		InitializeParticles(particles);
	}
		
		
	//BOMBアニメーション処理
	if (g_Bomb.use == TRUE)
	{
		g_Bomb.countAnim += 1.0f;
		if (g_Bomb.countAnim > ANIM_WAIT)
		{
			g_Bomb.countAnim = 0.0f;
			g_Bomb.frame++;

			if (g_Bomb.frame >= TEXTURE_PATTERN_DIVIDE_X * TEXTURE_PATTERN_DIVIDE_Y)
			{
				g_Bomb.frame = 0;
				g_Bomb.use = FALSE;
			}
		}
	}


	//凍る画面時間
	if (GetRelativeTime() - g_Ici.iceTime >= 650 )
	{
		g_Ici.iceTime = 0.0f;
		g_Ici.isIce = FALSE;
	}


	//事件のテクスチャ処理
	if (g_Ici.use)
	{
		g_Ici.alpha -= 0.01f;
		g_Ici.color = XMFLOAT4(1.0f, 1.0f, 1.0f, g_Ici.alpha);
		if (g_Ici.alpha <= 0) 
		{
			g_Ici.use = FALSE;
			player->boxTouch = FALSE;
			g_Ici.alpha = 1.0f;
		}
		
	}

	
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawIncident(void)
{

	BG* bg = GetBG();


	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);


	//描画
	if (g_Ici.use == TRUE)
	{
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Ici.texNo]);
		float px = g_Ici.pos.x - bg->pos.x - 60.0f;	  // 表示位置X
		float py = g_Ici.pos.y - bg->pos.y - 80.0f;	  // 表示位置Y
		float pw = g_Ici.w;		                      // 表示幅
		float ph = g_Ici.h;		                      // 表示高さ
									              
		float tw = 1.0f;	                          // テクスチャの幅
		float th = 1.0f;	                          // テクスチャの高さ
		float tx = 0.0f;	                          // テクスチャの左上X座標
		float ty = 0.0f;	                          // テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			px, py, pw, ph,
			tx, ty, tw, th,
			g_Ici.color);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}


	//BOMBを描画

	if (g_Bomb.use == TRUE)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);
		{
			//BOXの位置やテクスチャー座標を反映
			float px = g_Bomb.pos.x - bg->pos.x;	 // BOMBの表示位置X
			float py = g_Bomb.pos.y - bg->pos.y;	 // BOMBの表示位置Y
			float pw = g_Bomb.w;                     // BOMBの表示幅
			float ph = g_Bomb.h;                     // BOMBの表示高さ


			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;
			float tx = (float)(g_Bomb.frame % TEXTURE_PATTERN_DIVIDE_X) * tw;
			float ty = (float)(g_Bomb.frame / TEXTURE_PATTERN_DIVIDE_Y) * th;

			SetSpriteColorRotation(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), 0.0f);

			GetDeviceContext()->Draw(4, 0);

		}
		
	}



	//ICEを描画

	if (g_Ici.isIce)
	{
		//ICE
		float g_Ice_x = 0.0f;
		float g_Ice_y = 0.0f;
		float g_Ice_w = 3872.0f;
		float g_Ice_h = 2592.0f;
		XMFLOAT4 g_Ice_color = XMFLOAT4(0.5f, 1.0f, 1.0f, 0.5f);

		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);
		SetSpriteColor(g_VertexBuffer, g_Ice_x, g_Ice_y, g_Ice_w, g_Ice_h, 0.0f, 0.0f, 1.0f, 1.0f, g_Ice_color);
		GetDeviceContext()->Draw(4, 0);

	}



	//RAINBOWを描画
	for (int i = 0; i < PARTICLE_COUNT; i++)
	{
		if (particles[i].use == TRUE)
		{
			SetBlendState(BLEND_MODE_ADD);
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

			SetSpriteColor(g_VertexBuffer, particles[i].pos.x-bg->pos.x, particles[i].pos.y-bg->pos.y, particles[i].w, particles[i].h, 0.0f, 0.0f, 1.0f, 1.0f, particles[i].color);
			GetDeviceContext()->Draw(4, 0);

			SetBlendState(BLEND_MODE_ALPHABLEND);
		}
	}

}



//TexNo生成する
int generateTexNo(void)
{
	int randNum = rand() % 100; 

	if (randNum < 39) 
	{
		return LEVEL_UP;
	}
	else if (randNum < 76)
	{
		return SNOW;
	}
	else if (randNum < 96) 
	{
		return RAINBOW;
	}
	else 
	{
		return BOMB;
	}
}


//effect 初期化
void InitializeParticles(PARTICLE2 particles[PARTICLE_COUNT])
{
	for (int i = 0; i < PARTICLE_COUNT; i++)
	{
		float speed = (float)(rand() % 100) / 50.0f + 4.0f;
		float angle = (float)(rand() % 360);
		particles[i].dx = speed * cos(angle);
		particles[i].dy = speed * sin(angle);
		particles[i].pos.x = 0.0f;
		particles[i].pos.y = 0.0f;
		particles[i].w = 80.0f;
		particles[i].h = 80.0f;
		particles[i].color = XMFLOAT4(
			0.5f + (float)(rand() % 51) / 100.0f,
			0.5f + (float)(rand() % 51) / 100.0f,
			0.5f + (float)(rand() % 51) / 100.0f,
			1.0f
		);
		particles[i].alphaDecay = 0.003f;
		particles[i].use = FALSE;
	}
}