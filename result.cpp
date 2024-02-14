//=============================================================================
// ���U���g��ʏ��� [result.cpp]
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
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH			 	(SCREEN_WIDTH)  	// �w�i�T�C�Y
#define TEXTURE_HEIGHT			 	(SCREEN_HEIGHT)	    // �w�i�T�C�Y
#define TEXTURE_MAX				     	 (7)				// �e�N�X�`���̐�

#define TEXTURE_WIDTH_POINT              (80)
#define TEXTURE_HEIGHT_POINT             (80)

#define SOME_UPPER_VALUE_MENU         (SCREEN_HEIGHT / 2  + 100.0f)
#define SOME_LOWER_VALUE_MENU         (SCREEN_HEIGHT / 2  + 270.0f)


//death anime



//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;		        // ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int flog = ANIME;
static XMFLOAT4 restart;

//anime
static float py_t = 0.0f;	            // ���\���ʒuY
static BOOL ghostIn = FALSE;
static float angle = 0.0f;
static float px_g = 0.0f;	            // �H��̕\���ʒuX
static float py_g = 0.0f;	            // �H��̕\���ʒuY


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
// ����������
//=============================================================================
HRESULT InitResult(void)
{
	restart = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
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


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	//LEVELUP�t���[���̏�����
	g_Ponit.w = TEXTURE_WIDTH_POINT;
	g_Ponit.h = TEXTURE_HEIGHT_POINT;
	g_Ponit.pos = XMFLOAT3(SCREEN_WIDTH / 2 - 400.0f, SCREEN_HEIGHT / 2 + 100.0f, 0.0f);
	g_Ponit.speed = 170;


	// BGM�Đ�


	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
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
// �X�V����
//=============================================================================
void UpdateResult(void)
{
	//**************************ANIME MODE*************************************
	if (flog == ANIME)
	{

		// ���\���ʒuY
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
			 
			float cx = SCREEN_WIDTH / 2 - 100.0f;    // ���S�_ X
			float cy = SCREEN_HEIGHT / 2 + 50.0f;    // ���S�_ Y
			float r = 150.0f;                        // ���a
			angle += 0.03f;                          // �񂷊p�x

			// ���W�v�Z����
			px_g = cx + r * cos(angle);
			py_g = cy + r * sin(angle);

		}

		if (GetKeyboardTrigger(DIK_RETURN))
		{
			//�Q�[���̑���
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
			//�Q�[���̑���
			flog = AGAIN;
			PlaySound(7);
		}

	}


	//************************** AGAIN MODE *************************************
	if (flog == AGAIN)
	{
		// POINT�̏���
	    // �G�b�W����
		float upperBoundary = SOME_UPPER_VALUE_MENU; // ��G�b�W
		float lowerBoundary = SOME_LOWER_VALUE_MENU; // ���G�b�W

		//�|�C���g�̈ړ�
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

		// �G�b�W�O�`�F�b�N
		if (g_Ponit.pos.y > SOME_LOWER_VALUE_MENU)
		{
			g_Ponit.pos.y = SOME_UPPER_VALUE_MENU;
		}
		else if (g_Ponit.pos.y < SOME_UPPER_VALUE_MENU)
		{
			g_Ponit.pos.y = SOME_LOWER_VALUE_MENU;
		}

		//���Ɋm�F����
		if (g_Ponit.pos.y == SOME_UPPER_VALUE_MENU)
		{
			if (GetKeyboardTrigger(DIK_RETURN))
			{
				//�Q�[���̑���
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
				//�Q�[���̏I��
				PostQuitMessage(0);
			}
		}
	}
	



}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawResult(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	if (flog == RESULT)
	{
		// RESULT�w�i��`��
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[5]);

		float px_r = 0.0f;	                // �\���ʒuX
		float py_r = 0.0f;	                // �\���ʒuY
		float pw_r = SCREEN_WIDTH;		    // �\����
		float ph_r = SCREEN_HEIGHT;		    // �\������
		float tw_r = 1.0f;    		        // �e�N�X�`���̕�
		float th_r = 1.0f;		            // �e�N�X�`���̍���
		float tx_r = 0.0f;	        	    // �e�N�X�`���̍���X���W
		float ty_r = 0.0f;			        // �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, px_r, py_r, pw_r, ph_r, tx_r, ty_r, tw_r, th_r,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);



		

		// ���x����`��
		{

			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

			int number = level;
			for (int i = 0; i < 2; i++)
			{
				// ����\�����錅�̐���
				float x = (float)(number % 10);

				// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = 1300.0f - 32 * i;	    // �X�R�A�̕\���ʒuX
				float py = 375.0f;			        // �X�R�A�̕\���ʒuY
				float pw = 32.0f;				    // �X�R�A�̕\����
				float ph = 64.0f;				    // �X�R�A�̕\������

				float tw = 1.0f / 10;	        	// �e�N�X�`���̕�
				float th = 1.0f / 1;	        	// �e�N�X�`���̍���
				float tx = x * tw;		        	// �e�N�X�`���̍���X���W
				float ty = 0.0f;		        	// �e�N�X�`���̍���Y���W
										        
				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				// ���̌���
				number /= 10;
			}

		}

		//���j����`��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

			int number = killed;
			for (int i = 0; i < 5; i++)
			{
				// ����\�����錅�̐���
				float x = (float)(number % 10);

				// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = 1300.0f - 32 * i;	    // �X�R�A�̕\���ʒuX
				float py = 525.0f;			        // �X�R�A�̕\���ʒuY
				float pw = 32.0f;				    // �X�R�A�̕\����
				float ph = 64.0f;				    // �X�R�A�̕\������

				float tw = 1.0f / 10;		// �e�N�X�`���̕�
				float th = 1.0f / 1;		// �e�N�X�`���̍���
				float tx = x * tw;			// �e�N�X�`���̍���X���W
				float ty = 0.0f;			// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				// ���̌���
				number /= 10;
			}
		}

		//�����Ă鎞��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

			int number = culTime;
			for (int i = 0; i < 3; i++)
			{
				// ����\�����錅�̐���
				float x = (float)(number % 10);

				// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = 1300.0f - 32 * i;	    // �X�R�A�̕\���ʒuX
				float py = 685.0f;			        // �X�R�A�̕\���ʒuY
				float pw = 32.0f;				    // �X�R�A�̕\����
				float ph = 64.0f;				    // �X�R�A�̕\������

				float tw = 1.0f / 10;		// �e�N�X�`���̕�
				float th = 1.0f / 1;		// �e�N�X�`���̍���
				float tx = x * tw;			// �e�N�X�`���̍���X���W
				float ty = 0.0f;			// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				// ���̌���
				number /= 10;
			}
		}


		//�X�L�����x��
		{
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[6]);

			int number = skill + 1;
			for (int i = 0; i < 2; i++)
			{
				// ����\�����錅�̐���
				float x = (float)(number % 10);

				// �X�R�A�̈ʒu��e�N�X�`���[���W�𔽉f
				float px = 1300.0f - 32 * i;	    // �X�R�A�̕\���ʒuX
				float py = 845.0f;			        // �X�R�A�̕\���ʒuY
				float pw = 32.0f;				    // �X�R�A�̕\����
				float ph = 64.0f;				    // �X�R�A�̕\������

				float tw = 1.0f / 10;		// �e�N�X�`���̕�
				float th = 1.0f / 1;		// �e�N�X�`���̍���
				float tx = x * tw;			// �e�N�X�`���̍���X���W
				float ty = 0.0f;			// �e�N�X�`���̍���Y���W

				// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
				SetSpriteColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
					XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

				// �|���S���`��
				GetDeviceContext()->Draw(4, 0);

				// ���̌���
				number /= 10;
			}
		}


	}




	if (flog == ANIME)
	{
		// DEATH�w�i��`��
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[2]);

		float px_d = 0.0f;	                // �\���ʒuX
		float py_d = 0.0f;	                // �\���ʒuY
		float pw_d = SCREEN_WIDTH;		    // �\����
		float ph_d = SCREEN_HEIGHT;		    // �\������
		float tw_d = 1.0f;    		        // �e�N�X�`���̕�
		float th_d = 1.0f;		            // �e�N�X�`���̍���
		float tx_d = 0.0f;	        	    // �e�N�X�`���̍���X���W
		float ty_d = 0.0f;			        // �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, px_d, py_d, pw_d, ph_d, tx_d, ty_d, tw_d, th_d,
			restart);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);



		// TOMBSTONE��`��i���j
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[4]);

		float px_t = SCREEN_WIDTH/2  - 120.0f;	    // �\���ʒuX
		float pw_t = 300.0f;		                // �\����
		float ph_t = 300.0f;		                // �\������
		float tw_t = 1.0f;    		                // �e�N�X�`���̕�
		float th_t = 1.0f;		                    // �e�N�X�`���̍���
		float tx_t = 0.0f;	        	            // �e�N�X�`���̍���X���W
		float ty_t = 0.0f;			                // �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, px_t, py_t, pw_t, ph_t, tx_t, ty_t, tw_t, th_t,
			restart);

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);



		if (ghostIn)
		{
			// �v���C���[�̗H���`��
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[3]);


			float pw_g = 150.0f;	     	    // �\����
			float ph_g = 150.0f;		        // �\������
			float tw_g = 1.0f;    		        // �e�N�X�`���̕�
			float th_g = 1.0f;		            // �e�N�X�`���̍���
			float tx_g = 0.0f;	        	    // �e�N�X�`���̍���X���W
			float ty_g = 0.0f;			        // �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteLTColor(g_VertexBuffer, px_g, py_g, pw_g, ph_g, tx_g, ty_g, tw_g, th_g,
				XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
		


	}

	if (flog == AGAIN)
	{
		// �w�i��`��
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[0]);

		float px = 0.0f;	                // �\���ʒuX
		float py = 0.0f;	                // �\���ʒuY

		float pw = SCREEN_WIDTH;		    // �\����
		float ph = SCREEN_HEIGHT;		    // �\������
		float tw = 1.0f;    		        // �e�N�X�`���̕�
		float th = 1.0f;		            // �e�N�X�`���̍���
		float tx = 0.0f;	        	    // �e�N�X�`���̍���X���W
		float ty = 0.0f;			        // �e�N�X�`���̍���Y���W

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer, px, py, pw, ph, tx, ty, tw, th,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);



		//POINT��`��
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[1]);

		// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
		SetSpriteLTColor(g_VertexBuffer,
			g_Ponit.pos.x, g_Ponit.pos.y, g_Ponit.w, g_Ponit.h,
			0.0f, 0.0f, 1.0f, 1.0f,
			XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		// �|���S���`��
		GetDeviceContext()->Draw(4, 0);

	}









	



}

