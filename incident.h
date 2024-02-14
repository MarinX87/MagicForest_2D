//=============================================================================
// �������� [incident.h]
//=============================================================================
#pragma once
#include "main.h"
#include "renderer.h"


//*****************************************************************************
// �}�N����`
//*****************************************************************************

//effect
#define PARTICLE_COUNT                   (200)      

struct INCIDENT
{
	XMFLOAT3	pos;		    // �|���S���̍��W
	float		w, h;		    // ���ƍ���
	int         texNo;		    // �g�p���Ă���e�N�X�`���ԍ�
	BOOL        use;
	XMFLOAT4    color;          // �f�t�H���g�ʒu�l
	float       alpha;         
	float		countAnim;
	int         frame;
	float       iceTime;
	BOOL        isIce;
};


struct PARTICLE2 {
	XMFLOAT3	pos;            // �|���S���̍��W
	float		w, h;		    // ���ƍ���
	float       dx, dy;         // �X�r�[�g
	XMFLOAT4    color;          // �F
	float       alphaDecay;     // �����x����
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
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitIncident(void);
void    UninitIncident(void);
void    UpdateIncident(void);
void    DrawIncident(void);
int     generateTexNo(void);
void InitializeParticles(PARTICLE2 particles[PARTICLE_COUNT]);