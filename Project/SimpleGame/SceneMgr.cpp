#include "stdafx.h"
#include "SceneMgr.h"
#include "GameObject.h"
#include "Renderer.h"
#include "Sound.h"


void CSceneMgr::Initialize(void)
{
	if(m_Renderer == NULL)
		m_Renderer = new Renderer(WINSX, WINSY);
	if (!m_Renderer->IsInitialized())
	{
		std::cout << "Renderer could not be initialized.. \n";
	}

	//빌딩 각 팀에 3개씩 설치
	int BuildingNum = 0;
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 2; ++j)
		{
			Position Pos = Position(-float(WINSX / 2 * 0.8) + float(WINSX / 2 * 0.8) * i, -float(WINSY/2 * 0.8) + float(WINSY * 0.8)  * j);
			CGameObject *Obj = new CGameObject(&Pos, OBJECT_BUILDING, 1-j);
			m_ObjArr[BuildingNum][OBJECT_BUILDING] = Obj;

			BuildingNum++;
		}
	}

	//현재 다른 리소스는 추가되지 않았음
	m_ObjTex[Team_Red][OBJECT_BUILDING] = m_Renderer->CreatePngTexture("../Resource/Building_Red.png");
	m_ObjTex[Team_Blue][OBJECT_BUILDING] = m_Renderer->CreatePngTexture("../Resource/Building_Blue.png");
	//플레이어
	m_ObjTex[Team_Red][OBJECT_CHARACTER] = m_Renderer->CreatePngTexture("../Resource/char_spritered.png");
	m_ObjTex[Team_Blue][OBJECT_CHARACTER] = m_Renderer->CreatePngTexture("../Resource/char_spriteblue.png");

	m_BackGround = m_Renderer->CreatePngTexture("../Resource/background.png");
	m_ParticleTex = m_Renderer->CreatePngTexture("../Resource/particle_shine.png");

	Cooltime = 1.f;

	m_Sound = new Sound;

	m_BGMusic = m_Sound->CreateSound("./Dependencies/SoundSamples/MF-W-90.XM");

	m_Sound->PlaySound(m_BGMusic, true, 0.2f);
}

void CSceneMgr::Release(void)
{
	delete m_Renderer;

	for (int i = 0; i < MAXCOUNT; ++i)
	{
		for (int j = 0; j < OBJECT_end; ++j)
		{
			if (m_ObjArr[i][j] != NULL)
			{
				delete m_ObjArr[i][j];
				m_ObjArr[i][j] = NULL;
			}
		}
	}
}

void CSceneMgr::Update(void)
{
	CGameObject* temp = NULL;
	for (int i = 0; i < MAXCOUNT; ++i)
	{
		for (int j = 0; j < OBJECT_end; j++)
		{
			if (m_ObjArr[i][j] != NULL)
			{
				if (m_ObjArr[i][j]->Update() == 1)
				{
					delete m_ObjArr[i][j];
					m_ObjArr[i][j] = NULL;
				}
				else if (m_ObjArr[i][j]->Update() == 2)
				{//총알 생성
					if (j == OBJECT_BUILDING)
					{
						CGameObject* temp = new CGameObject(m_ObjArr[i][j]->GetPos(), OBJECT_BULLET, m_ObjArr[i][j]->getMyTeam());
						PushBullet(temp);
					}
					if (j == OBJECT_CHARACTER)
					{
						CGameObject* temp = new CGameObject(m_ObjArr[i][j]->GetPos(), OBJECT_ARROW, m_ObjArr[i][j]->getMyTeam());
						PushArrow(temp);
					}
				}
			}
		}
	}
	

	CollisionCheck_Optimi();
}

void CSceneMgr::Render(void)
{
	Cooltime += m_fElapsedTime;
	m_Renderer->DrawParticleClimate( 0, 0, 0, 1, 1, 1, 1, 1, -0.1, -0.1, m_ParticleTex, Cooltime, 0.01);

	m_Renderer->DrawTexturedRect(0.f,0.f, 0.f, 1000, 1.0, 1.0, 1.0, 1.0, m_BackGround, 9 / 10.f);
	m_Renderer->DrawText(0, 0, GLUT_BITMAP_HELVETICA_18, 0, 0, 1, "RED");
	m_Renderer->DrawText(0, -20, GLUT_BITMAP_HELVETICA_18, 1, 0, 0, "BLUE");

	for (int i = 0; i < MAXCOUNT; ++i)
	{
		for (int j = 0; j < OBJECT_end; j++)
		{
			if (m_ObjArr[i][j] != NULL)
			{
				float flevel = 0.f;
				if (m_ObjArr[i][j]->getObjType() == OBJECT_ARROW || m_ObjArr[i][j]->getObjType() == OBJECT_BULLET)
				{
					if (m_ObjArr[i][j]->getObjType() == OBJECT_BULLET)
						m_Renderer->DrawParticle(m_ObjArr[i][j]->GetPos()->fX, m_ObjArr[i][j]->GetPos()->fY, 0.f, 10, 1, 1, 1, 1,
							-m_ObjArr[i][j]->GetDir()->fX, -m_ObjArr[i][j]->GetDir()->fY, m_ParticleTex, m_ObjArr[i][j]->GetParticleTime(), (LEVEL_BANDA+ 0.01) / 10.f);

					m_Renderer->DrawSolidRect(m_ObjArr[i][j]->GetPos()->fX, m_ObjArr[i][j]->GetPos()->fY, 0.f,
						*m_ObjArr[i][j]->GetSize(),
						m_ObjArr[i][j]->GetColor()->fR, m_ObjArr[i][j]->GetColor()->fG, m_ObjArr[i][j]->GetColor()->fB,
						m_ObjArr[i][j]->GetColor()->fAlpha, LEVEL_BANDA/10.f);
				}
				else
				{
					if(m_ObjArr[i][j]->getObjType() == OBJECT_BUILDING)
						flevel = LEVEL_BUILD / 10.f;
					else if(m_ObjArr[i][j]->getObjType() == OBJECT_CHARACTER)
						flevel = LEVEL_CHAR / 10.f;
					if (m_ObjArr[i][j]->getMyTeam() == Team_Red)
					{
						if (m_ObjArr[i][j]->getObjType() == OBJECT_BUILDING)
						{
							/*m_Renderer->DrawTexturedRect(m_ObjArr[i][j]->GetPos()->fX, m_ObjArr[i][j]->GetPos()->fY, 0.f,
								*m_ObjArr[i][j]->GetSize(),
								m_ObjArr[i][j]->GetColor()->fR, m_ObjArr[i][j]->GetColor()->fG, m_ObjArr[i][j]->GetColor()->fB,
								m_ObjArr[i][j]->GetColor()->fAlpha,
								m_ObjTex[Team_Red][j], flevel);*/
						}
						else
							m_Renderer->DrawTexturedRectSeq(m_ObjArr[i][j]->GetPos()->fX, m_ObjArr[i][j]->GetPos()->fY, 0.f,
								*m_ObjArr[i][j]->GetSize(), m_ObjArr[i][j]->GetColor()->fR, m_ObjArr[i][j]->GetColor()->fG, m_ObjArr[i][j]->GetColor()->fB,
								m_ObjArr[i][j]->GetColor()->fAlpha, m_ObjTex[Team_Red][j], m_ObjArr[i][j]->GetSpriteX(), 0, 4, 1, flevel);

							m_Renderer->DrawSolidRectGauge(m_ObjArr[i][j]->GetPos()->fX, m_ObjArr[i][j]->GetPos()->fY + *m_ObjArr[i][j]->GetSize() / 2.f + 4, 0.f,
							*m_ObjArr[i][j]->GetSize(), 3.f,
							1.f, 0.f, 0.f,
							m_ObjArr[i][j]->GetColor()->fAlpha, m_ObjArr[i][j]->GetLifeTime(), flevel);
					}
					else if (m_ObjArr[i][j]->getMyTeam() == Team_Blue)
					{
						if (m_ObjArr[i][j]->getObjType() == OBJECT_BUILDING)
							m_Renderer->DrawTexturedRect(m_ObjArr[i][j]->GetPos()->fX, m_ObjArr[i][j]->GetPos()->fY, 0.f,
							*m_ObjArr[i][j]->GetSize(),
							m_ObjArr[i][j]->GetColor()->fR, m_ObjArr[i][j]->GetColor()->fG, m_ObjArr[i][j]->GetColor()->fB,
							m_ObjArr[i][j]->GetColor()->fAlpha,
							m_ObjTex[Team_Blue][j], flevel);
						else
							m_Renderer->DrawTexturedRectSeq(m_ObjArr[i][j]->GetPos()->fX, m_ObjArr[i][j]->GetPos()->fY, 0.f,
							*m_ObjArr[i][j]->GetSize(), m_ObjArr[i][j]->GetColor()->fR, m_ObjArr[i][j]->GetColor()->fG, m_ObjArr[i][j]->GetColor()->fB,
							m_ObjArr[i][j]->GetColor()->fAlpha, m_ObjTex[Team_Blue][j], m_ObjArr[i][j]->GetSpriteX(), 0, 4, 1, flevel);

						m_Renderer->DrawSolidRectGauge(m_ObjArr[i][j]->GetPos()->fX, m_ObjArr[i][j]->GetPos()->fY + *m_ObjArr[i][j]->GetSize() / 2.f + 4, 0.f,
							*m_ObjArr[i][j]->GetSize(), 3.f,
							0.f, 0.f, 1.f,
							m_ObjArr[i][j]->GetColor()->fAlpha, m_ObjArr[i][j]->GetLifeTime(), flevel);
					}
				}
				
			}
		}
	}
}

void CSceneMgr::PushObj(CGameObject * NewObj)
{
	int iObjNum = 0;
	for (int i = 0; i < MAXCOUNT; ++i)
	{
		if (m_ObjArr[i][OBJECT_CHARACTER] != NULL)
		{
			iObjNum++;
		}
	}

	if (iObjNum < MAXCOUNT)
	{
		for (int i = 0; i < MAXCOUNT; ++i)
		{
			if (m_ObjArr[i][OBJECT_CHARACTER] == NULL)
			{
				m_ObjArr[i][OBJECT_CHARACTER] = NewObj;
				break;
			}
		}
	}
	else
	{
		for (int i = 0; i < MAXCOUNT; ++i)
		{
			if (m_ObjArr[i][OBJECT_CHARACTER] != NULL)
			{
				delete m_ObjArr[i][OBJECT_CHARACTER];
				m_ObjArr[i][OBJECT_CHARACTER] = NULL;
				m_ObjArr[i][OBJECT_CHARACTER] = NewObj;
				break;
			}
		}
	}
}

void CSceneMgr::CollisionCheck_Optimi(void)
{
	//빌딩과 캐릭터
	for (int i = 0; i < MAXCOUNT; ++i)
	{
		if (m_ObjArr[i][OBJECT_BUILDING] == NULL)
			break;
		for (int j = 0; j < MAXCOUNT; ++j)
		{
			if (m_ObjArr[j][OBJECT_CHARACTER] == NULL)
				break;

			if (m_ObjArr[i][OBJECT_BUILDING]->CollisionCheck(m_ObjArr[j][OBJECT_CHARACTER]) &&
				m_ObjArr[j][OBJECT_CHARACTER]->CollisionCheck(m_ObjArr[i][OBJECT_BUILDING]) &&
				m_ObjArr[j][OBJECT_CHARACTER]->getMyTeam() != m_ObjArr[i][OBJECT_BUILDING]->getMyTeam())
			{
				float BulidingLife = m_ObjArr[i][OBJECT_BUILDING]->GetLifeTime() - m_ObjArr[j][OBJECT_CHARACTER]->GetLifeTime();
				m_ObjArr[i][OBJECT_BUILDING]->SetLifeTime(BulidingLife);
				m_ObjArr[j][OBJECT_CHARACTER]->SetLifeTime(0.f);
				break;
			}
		}
	}
	//총알과 캐릭터
	for (int i = 0; i < MAXCOUNT; ++i)
	{
		if (m_ObjArr[i][OBJECT_BULLET] == NULL)
			break;
		for (int j = 0; j < MAXCOUNT; ++j)
		{
			if (m_ObjArr[j][OBJECT_CHARACTER] == NULL)
				break;

			if (m_ObjArr[i][OBJECT_BULLET]->CollisionCheck(m_ObjArr[j][OBJECT_CHARACTER]) &&
				m_ObjArr[j][OBJECT_CHARACTER]->CollisionCheck(m_ObjArr[i][OBJECT_BULLET]) &&
				m_ObjArr[j][OBJECT_CHARACTER]->getMyTeam() != m_ObjArr[i][OBJECT_BULLET]->getMyTeam())
			{
				//캐릭터 생명력 - 불릿 생명력
				float CharaLife = m_ObjArr[j][OBJECT_CHARACTER]->GetLifeTime() - m_ObjArr[i][OBJECT_BULLET]->GetLifeTime();
				m_ObjArr[j][OBJECT_CHARACTER]->SetLifeTime(CharaLife);
				m_ObjArr[i][OBJECT_BULLET]->SetLifeTime(0.f);
				break;
			}
		}
	}
	//빌딩과 화살
	for (int i = 0; i < MAXCOUNT; ++i)
	{
		if (m_ObjArr[i][OBJECT_BUILDING] == NULL)
			break;
		for (int j = 0; j < MAXCOUNT; ++j)
		{
			if (m_ObjArr[j][OBJECT_ARROW] == NULL)
				break;

			if (m_ObjArr[i][OBJECT_BUILDING]->CollisionCheck(m_ObjArr[j][OBJECT_ARROW]) &&
				m_ObjArr[j][OBJECT_ARROW]->CollisionCheck(m_ObjArr[i][OBJECT_BUILDING]) &&
				m_ObjArr[j][OBJECT_ARROW]->getMyTeam() != m_ObjArr[i][OBJECT_BUILDING]->getMyTeam())
			{
				float BulidingLife = m_ObjArr[i][OBJECT_BUILDING]->GetLifeTime() - m_ObjArr[j][OBJECT_ARROW]->GetLifeTime();
				m_ObjArr[i][OBJECT_BUILDING]->SetLifeTime(BulidingLife);
				m_ObjArr[j][OBJECT_ARROW]->SetLifeTime(0.f);
				break;
			}
		}
	}
	//빌딩과 총알
	for (int i = 0; i < MAXCOUNT; ++i)
	{
		if (m_ObjArr[i][OBJECT_BUILDING] == NULL)
			break;
		for (int j = 0; j < MAXCOUNT; ++j)
		{
			if (m_ObjArr[j][OBJECT_BULLET] == NULL)
				break;

			if (m_ObjArr[i][OBJECT_BUILDING]->CollisionCheck(m_ObjArr[j][OBJECT_BULLET]) &&
				m_ObjArr[j][OBJECT_BULLET]->CollisionCheck(m_ObjArr[i][OBJECT_BUILDING]) &&
				m_ObjArr[j][OBJECT_BULLET]->getMyTeam() != m_ObjArr[i][OBJECT_BUILDING]->getMyTeam())
			{
				float BulidingLife = m_ObjArr[i][OBJECT_BUILDING]->GetLifeTime() - m_ObjArr[j][OBJECT_BULLET]->GetLifeTime();
				m_ObjArr[i][OBJECT_BUILDING]->SetLifeTime(BulidingLife);
				m_ObjArr[j][OBJECT_BULLET]->SetLifeTime(0.f);
				break;
			}
		}
	}
	//플레이어와 화살
	for (int i = 0; i < MAXCOUNT; ++i)
	{
		if (m_ObjArr[i][OBJECT_CHARACTER] == NULL)
			break;
		for (int j = 0; j < MAXCOUNT; ++j)
		{
			if (m_ObjArr[j][OBJECT_ARROW] == NULL)
				break;

			if (m_ObjArr[i][OBJECT_CHARACTER]->CollisionCheck(m_ObjArr[j][OBJECT_ARROW]) &&	//캐릭터에 화살이 충돌했니?
				m_ObjArr[j][OBJECT_ARROW]->CollisionCheck(m_ObjArr[i][OBJECT_CHARACTER]) &&	//화살에 캐릭터가 충돌했니?
				m_ObjArr[j][OBJECT_ARROW]->getMyTeam() != m_ObjArr[i][OBJECT_CHARACTER]->getMyTeam())	//화살의 주인이 서로 다르니?
			{
				float CharaLife = m_ObjArr[i][OBJECT_CHARACTER]->GetLifeTime() - m_ObjArr[j][OBJECT_ARROW]->GetLifeTime();
				m_ObjArr[i][OBJECT_CHARACTER]->SetLifeTime(CharaLife);
				m_ObjArr[j][OBJECT_ARROW]->SetLifeTime(0.f);
				break;
			}
		}
	}
}

void CSceneMgr::SetElapsedTime(float fElapsedTime)
{
	for (int i = 0; i < MAXCOUNT; ++i)
	{
		for (int j = 0; j < OBJECT_end; ++j)
		{
			if (m_ObjArr[i][j] != NULL)
			{
				m_ObjArr[i][j]->SetElapsedTime(fElapsedTime);
				break;
			}
		}
	}

	m_fElapsedTime = fElapsedTime;
}

void CSceneMgr::PushBullet(CGameObject * NewObj)
{
	for (int i = 0; i < MAXCOUNT; ++i)
	{
		if (m_ObjArr[i][OBJECT_BULLET] == NULL)
		{
			m_ObjArr[i][OBJECT_BULLET] = NewObj;
			break;
		}
	}
}

void CSceneMgr::PushArrow(CGameObject * NewObj)
{
	for (int i = 0; i < MAXCOUNT; ++i)
	{
		if (m_ObjArr[i][OBJECT_ARROW] == NULL)
		{
			m_ObjArr[i][OBJECT_ARROW] = NewObj;
			break;
		}
	}
}

CSceneMgr::CSceneMgr()
	: m_Renderer(NULL), m_fElapsedTime(0.f), m_BGMusic(NULL)
{
	Initialize();
}


CSceneMgr::~CSceneMgr()
{
	Release();
}
