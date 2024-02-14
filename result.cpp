//=============================================================================
// リザルト画面処理 [result.cpp]
//=============================================================================
#include "result.h"
#include "input.h"
#include "score.h"
#include "fade.h"
#include "pause.h"
#include "sound.h"
#include "player.h"
#include "bullet.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH			 	(SCREEN_WIDTH)  	// 背景サイズ
#define TEXTURE_HEIGHT			 	(SCREEN_HEIGHT)	    // 背景サイズ
#define TEXTURE_MAX				     	 (7)				// テクスチャの数

#define TEXTURE_WIDTH_POINT              (80)
#define TEXTURE_HEIGHT_POINT             (80)

#define SOME_UPPER_VALUE_MENU         (SCREEN_HEIGHT / 2  + 100.0f)
#define SOME_LOWER_VALUE_MENU         (SCREEN_HEIGHT / 2  + 270.0f)


//death anime



//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		        // 頂点情報
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int flog = ANIME;
static XMFLOAT4 restart;

//anime
static float py_t = 0.0f;	            // 墓碑表示位置Y
static BOOL ghostIn = FALSE;
static float angle = 0.0f;
static float px_g = 0.0f;	            // 幽霊の表示位置X
static float py_g = 0.0f;	            // 幽霊の表示位置Y


//result
static int level = 0;
static int killed = 0;
static int skill = 0;
static int culTime = 0;



static char *g_TexturName[TEXTURE_MAX] = {
	"data/TEXTURE/gameover.png", 
	"data/TEXTURE/point.png",
	"data/TEXTURE/death1.png",
	"data/TEXTURE/death2.png",
	"data/TEXTURE/death3.png",
	"data/TEXTURE/result000.png",
	"data/TEXTURE/number.png"

};



static BOOL						g_Load = FALSE;
static PAUSE	                g_Ponit;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
{
	restart = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

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


	//LEVELUPフレームの初期化
	g_Ponit.w = TEXTURE_WIDTH_POINT;
	g_Ponit.h = TEXTURE_HEIGHT_POINT;
	g_Ponit.pos = XMFLOAT3(SCREEN_WIDTH / 2 - 400.0f, SCREEN_HEIGHT / 2 + 100.0f, 0.0f);
	g_Ponit.speed = 170;


	// BGM再生


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
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
void UpdateResult(void)
{
	//**************************ANIME MODE*************************************
	if (flog == ANIME)
	{

		// 墓碑表示位置Y
		if (py_t <= SCREEN_HEIGHT / 2)
		{
			py_t += 10.0f;

			if (py_t == SCREEN_HEIGHT / 2)
			{
				ghostIn = TRUE;
			}

		}

		if (ghostIn)
		{
			 
			float cx = SCREEN_WIDTH / 2 - 100.0f;    // 中心点 X
			float cy = SCREEN_HEIGHT / 2 + 50.0f;    // 中心点 Y
			float r = 150.0f;                        // 半径
			angle += 0.03f;                          // 回す角度

			// 座標計算する
			px_g = cx + r * cos(angle);
			py_g = cy + r * sin(angle);

		}

		if (GetKeyboardTrigger(DIK_RETURN))
		{
			//ゲームの続き
			flog = RESULT;
			py_t = 0.0f;
			ghostIn = FALSE;
			PlaySound(7);
		}
	}



	//************************** RESULT MODE *************************************


	if (flog == RESULT)
	{

		PLAYER* player = GetPlayer();

		level = player->lv;
		killed = player->killed;
		skill = skillCheck();
		culTime = GetTime();
		
		if (GetKeyboardTrigger(DIK_RETURN))
		{
			//ゲームの続き
			flog = AGAIN;
			PlaySound(7);
		}

	}


	//************************** AGAIN MODE *************************************
	if (flog == AGAIN)
	{
		// POINTの処理
	    // エッジ処理
		float upperBoundary = SOME_UPPER_VALUE_MENU; // 上エッジ
		float lowerBoundary = SOME_LOWER_VALUE_MENU; // 下エッジ

		//ポイントの移動
		if (GetKeyboardTrigger(DIK_DOWN))
		{
			g_Ponit.pos.y += g_Ponit.speed;
			PlaySound(6);
		}
		else if (GetKeyboardTrigger(DIK_UP))
		{
			g_Ponit.pos.y -= g_Ponit.speed;
			PlaySound(6);
		}

		// エッジ外チェック
		if (g_Ponit.pos.y > SOME_LOWER_VALUE_MENU)
		{
			g_Ponit.pos.y = SOME_UPPER_VALUE_MENU;
		}
		else if (g_Ponit.pos.y < SOME_UPPER_VALUE_MENU)
		{
			g_Ponit.pos.y = SOME_LOWER_VALUE_MENU;
		}

		//次に確認する
		if (g_Ponit.pos.y == SOME_UPPER_VALUE_MENU)
		{
			if (GetKeyboardTrigger(DIK_RETURN))
			{
				//ゲームの続き
				SetFade(FADE_OUT, MODE_TITLE);
				restart = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
				flog = ANIME;
				PlaySound(7);
			}

			
		}

		else if (g_Ponit.pos.y == SOME_LOWER_VALUE_MENU)
		{
			if (GetKeyboardTrigger(DIK_RETURN))
			{
				//ゲームの終了
				PostQuitMessage(0);
			}
		}
	}
	



}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
{
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

	if (flog == RESULT)
	{
		// RESULT背景を描画
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		float px_r = 0.0f;	                // 表示位置X
		float py_r = 0.0f;	                // 表示位置Y
		float pw_r = SCREEN_WIDTH;		    // 表示幅
		float ph_r = SCREEN_HEIGHT;		    // 表示高さ
		float tw_r = 1.0f;    		        // テクスチャの幅
		float th_r = 1.0f;		            // テクスチャの高さ
		float tx_r = 0.0f;	        	    // テクスチャの左上X座標
		float ty_r = 0.0f;			        // テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, px_r, py_r, pw_r, ph_r, tx_r, ty_r, tw_r, th_r,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);



		

		// レベルを描画
		{

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

			int number = level;
			for (int i = 0; i < 2; i++)
			{
				// 今回表示する桁の数字
				float x = (float)(number % 10);

				// スコアの位置やテクスチャー座標を反映
				float px = 1300.0f - 32 * i;	    // スコアの表示位置X
				float py = 375.0f;			        // スコアの表示位置Y
				float pw = 32.0f;				    // スコアの表示幅
				float ph = 64.0f;				    // スコアの表示高さ

				float tw = 1.0f / 10;	        	// テクスチャの幅
				float th = 1.0f / 1;	        	// テクスチャの高さ
				float tx = x * tw;		        	// テクスチャの左上X座標
				float ty = 0.0f;		        	// テクスチャの左上Y座標
										        
				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				// 次の桁へ
				number /= 10;
			}

		}

		//撃破数を描画
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

			int number = killed;
			for (int i = 0; i < 5; i++)
			{
				// 今回表示する桁の数字
				float x = (float)(number % 10);

				// スコアの位置やテクスチャー座標を反映
				float px = 1300.0f - 32 * i;	    // スコアの表示位置X
				float py = 525.0f;			        // スコアの表示位置Y
				float pw = 32.0f;				    // スコアの表示幅
				float ph = 64.0f;				    // スコアの表示高さ

				float tw = 1.0f / 10;		// テクスチャの幅
				float th = 1.0f / 1;		// テクスチャの高さ
				float tx = x * tw;			// テクスチャの左上X座標
				float ty = 0.0f;			// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				// 次の桁へ
				number /= 10;
			}
		}

		//生きてる時間
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

			int number = culTime;
			for (int i = 0; i < 3; i++)
			{
				// 今回表示する桁の数字
				float x = (float)(number % 10);

				// スコアの位置やテクスチャー座標を反映
				float px = 1300.0f - 32 * i;	    // スコアの表示位置X
				float py = 685.0f;			        // スコアの表示位置Y
				float pw = 32.0f;				    // スコアの表示幅
				float ph = 64.0f;				    // スコアの表示高さ

				float tw = 1.0f / 10;		// テクスチャの幅
				float th = 1.0f / 1;		// テクスチャの高さ
				float tx = x * tw;			// テクスチャの左上X座標
				float ty = 0.0f;			// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				// 次の桁へ
				number /= 10;
			}
		}


		//スキルレベル
		{
			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

			int number = skill + 1;
			for (int i = 0; i < 2; i++)
			{
				// 今回表示する桁の数字
				float x = (float)(number % 10);

				// スコアの位置やテクスチャー座標を反映
				float px = 1300.0f - 32 * i;	    // スコアの表示位置X
				float py = 845.0f;			        // スコアの表示位置Y
				float pw = 32.0f;				    // スコアの表示幅
				float ph = 64.0f;				    // スコアの表示高さ

				float tw = 1.0f / 10;		// テクスチャの幅
				float th = 1.0f / 1;		// テクスチャの高さ
				float tx = x * tw;			// テクスチャの左上X座標
				float ty = 0.0f;			// テクスチャの左上Y座標

				// １枚のポリゴンの頂点とテクスチャ座標を設定
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// ポリゴン描画
				GetDeviceContext()->Draw(4, 0);

				// 次の桁へ
				number /= 10;
			}
		}


	}




	if (flog == ANIME)
	{
		// DEATH背景を描画
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		float px_d = 0.0f;	                // 表示位置X
		float py_d = 0.0f;	                // 表示位置Y
		float pw_d = SCREEN_WIDTH;		    // 表示幅
		float ph_d = SCREEN_HEIGHT;		    // 表示高さ
		float tw_d = 1.0f;    		        // テクスチャの幅
		float th_d = 1.0f;		            // テクスチャの高さ
		float tx_d = 0.0f;	        	    // テクスチャの左上X座標
		float ty_d = 0.0f;			        // テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, px_d, py_d, pw_d, ph_d, tx_d, ty_d, tw_d, th_d,
			restart);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);



		// TOMBSTONEを描画（墓碑）
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		float px_t = SCREEN_WIDTH/2  - 120.0f;	    // 表示位置X
		float pw_t = 300.0f;		                // 表示幅
		float ph_t = 300.0f;		                // 表示高さ
		float tw_t = 1.0f;    		                // テクスチャの幅
		float th_t = 1.0f;		                    // テクスチャの高さ
		float tx_t = 0.0f;	        	            // テクスチャの左上X座標
		float ty_t = 0.0f;			                // テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, px_t, py_t, pw_t, ph_t, tx_t, ty_t, tw_t, th_t,
			restart);

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);



		if (ghostIn)
		{
			// プレイヤーの幽霊を描画
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);


			float pw_g = 150.0f;	     	    // 表示幅
			float ph_g = 150.0f;		        // 表示高さ
			float tw_g = 1.0f;    		        // テクスチャの幅
			float th_g = 1.0f;		            // テクスチャの高さ
			float tx_g = 0.0f;	        	    // テクスチャの左上X座標
			float ty_g = 0.0f;			        // テクスチャの左上Y座標

			// １枚のポリゴンの頂点とテクスチャ座標を設定
			SetSpriteLTColor(g_VertexBuffer, px_g, py_g, pw_g, ph_g, tx_g, ty_g, tw_g, th_g,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// ポリゴン描画
			GetDeviceContext()->Draw(4, 0);
		}
		


	}

	if (flog == AGAIN)
	{
		// 背景を描画
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		float px = 0.0f;	                // 表示位置X
		float py = 0.0f;	                // 表示位置Y

		float pw = SCREEN_WIDTH;		    // 表示幅
		float ph = SCREEN_HEIGHT;		    // 表示高さ
		float tw = 1.0f;    		        // テクスチャの幅
		float th = 1.0f;		            // テクスチャの高さ
		float tx = 0.0f;	        	    // テクスチャの左上X座標
		float ty = 0.0f;			        // テクスチャの左上Y座標

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);



		//POINTを描画
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// １枚のポリゴンの頂点とテクスチャ座標を設定
		SetSpriteLTColor(g_VertexBuffer,
			g_Ponit.pos.x, g_Ponit.pos.y, g_Ponit.w, g_Ponit.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// ポリゴン描画
		GetDeviceContext()->Draw(4, 0);

	}









	



}

