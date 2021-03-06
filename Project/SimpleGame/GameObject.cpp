#include "stdafx.h"
#include "GameObject.h"

float CGameObject::m_fElapsedTime = 0.f;

CGameObject::CGameObject()
{
}


CGameObject::~CGameObject()
{
}

CGameObject::CGameObject(Position* pos, float* size, Color* rgba)
	: m_Pos(*pos), m_fSize(*size), m_Color(*rgba), m_fSpeed(200.f), m_fLifeTime(100.f)
{
	m_ObjType = OBJECT_end;
	initialize();
}

CGameObject::CGameObject(Position * pos, int ObjType, int MyTeam /*= Team_Red*/)
	: m_Pos(*pos), m_ObjType(ObjType), m_MyTeam(MyTeam)
{
	initialize();
}

void CGameObject::initialize(void)
{
	float rad = float(rand() % 360);

	//0~1사이의 단위 벡터
	m_Dir.fX = cos(rad * 3.141592f / 180.f);
	m_Dir.fY = sin(rad * 3.141592f / 180.f);

	m_fBulletCoolTime = 0.f;
	m_fArrowCoolTime = 0.f;
	m_iSpriteMaxX = 0.f;
	m_iSpriteX = 0.f;
	m_fSpriteCoolTime = 0.f;

	switch (m_ObjType)
	{
	case OBJECT_BUILDING:
	{
		if (m_MyTeam == Team_Red)
		{
			m_Color.fR = 1, m_Color.fG = 1, m_Color.fB = 1;
		}
		else if (m_MyTeam == Team_Blue)
		{
			m_Color.fR = 1, m_Color.fG = 1, m_Color.fB = 1;
		}
		m_fSize = 100.f;
		m_fSpeed = 0.f;
		m_fLifeTime = 500.f;
		break;
	}
	case OBJECT_CHARACTER:
	{
		m_Color.fR = 1, m_Color.fG = 1, m_Color.fB = 1;
		m_fSize = 50;
		m_fSpeed = 100.f;
		m_fLifeTime = 100.f;
		m_iSpriteMaxX = 4;
		m_fSpriteCoolTime = 0.7;
		break;
	}
	case OBJECT_BULLET:
	{
		if (m_MyTeam == Team_Red)
		{
			m_Color.fR = 1, m_Color.fG = 0, m_Color.fB = 0;
		}
		else if (m_MyTeam == Team_Blue)
		{
			m_Color.fR = 0, m_Color.fG = 0, m_Color.fB = 1;
		}
		m_fSize = 4;
		m_fSpeed = 50.f;
		m_fLifeTime = 20.f;
		break;
	}
	case OBJECT_ARROW:
	{
		if (m_MyTeam == Team_Red)
		{
			m_Color.fR = 0.5, m_Color.fG = 0.2, m_Color.fB = 0.7;
		}
		else if (m_MyTeam == Team_Blue)
		{
			m_Color.fR = 1, m_Color.fG = 1, m_Color.fB = 0;
		}
		m_fSize = 4;
		m_fSpeed = 50.f;
		m_fLifeTime = 10.f;
		break;
	}
	case OBJECT_end:
		m_Color.fR = 0.5, m_Color.fG = 0.5, m_Color.fB = 0.5;
		m_fSize = 1;
		m_fSpeed = 1000000;
		m_fLifeTime = 10.f;
		break;
	default:
		break;
	}
	m_Color.fAlpha = 1.f;

	m_fParticleTime = 0.f;
}

int CGameObject::Update(void)
{
	m_Pos.fX += m_fSpeed * m_fElapsedTime * m_Dir.fX;
	m_Pos.fY += m_fSpeed * m_fElapsedTime * m_Dir.fY;

	if (m_ObjType == OBJECT_CHARACTER)
	{
		m_fSpriteCoolTime -= m_fElapsedTime;
		if (m_fSpriteCoolTime <= 0.f)
		{
			m_fSpriteCoolTime = 0.3;
			m_iSpriteX++;
		}
		m_iSpriteX %= m_iSpriteMaxX;
	}

	if (m_ObjType == OBJECT_BUILDING)
	{
		m_fBulletCoolTime += m_fElapsedTime;
		if (m_fBulletCoolTime >= 3.0f)
		{
			m_fBulletCoolTime = 0.f;
			return 2;
		}
	}
	if (m_ObjType == OBJECT_CHARACTER)
	{
		m_fArrowCoolTime += m_fElapsedTime;
		if (m_fArrowCoolTime >= 1.0f)
		{
			m_fArrowCoolTime = 0.f;
			return 2;
		}
	}

	if (m_ObjType == OBJECT_BULLET)
	{
		m_fParticleTime += m_fElapsedTime * 0.5;
	}

	//만약 >> 이건 임시...입사각과 반사각을 구하자
	if (-float(WINSX/2) > m_Pos.fX || m_Pos.fX > float(WINSX / 2))
	{
		m_Dir.fX *= -1;
	}
	if (-float(WINSY / 2) > m_Pos.fY || m_Pos.fY > float(WINSY / 2))
	{
		m_Dir.fY *= -1;
	}

	m_fLifeTime -= m_fElapsedTime;
	if (m_fLifeTime <= 0.f)
			return 1;

	return 0;
}

void CGameObject::Render(void)
{

}

void CGameObject::Release(void)
{
}

void CGameObject::SetPos(Position* Pos)
{
	m_Pos = *Pos;
}

void CGameObject::SetSize(float * Size)
{
	m_fSize = *Size;
}

void CGameObject::SetColor(Color * RGBA)
{
	m_Color = *RGBA;
}

Position* CGameObject::GetPos(void)
{
	return &m_Pos;
}

float* CGameObject::GetSize(void)
{
	return &m_fSize;
}

Color* CGameObject::GetColor(void)
{
	return &m_Color;
}

bool CGameObject::CollisionCheck(CGameObject* ObjInfo)
{
	//left보다 더 왼쪽에 있는 오른쪽
	float fMyLeft = this->m_Pos.fX - this->m_fSize * 0.5f;
	float fYourRight = ObjInfo->GetPos()->fX + *ObjInfo->GetSize() * 0.5f;
	if (fYourRight < fMyLeft)
		return false;
	
	float fMyRight = this->m_Pos.fX + this->m_fSize * 0.5f;
	float fYourLeft = ObjInfo->GetPos()->fX - *ObjInfo->GetSize() * 0.5f;
	if (fMyRight < fYourLeft)
		return false;

	float fMyTop = this->m_Pos.fY + this->m_fSize * 0.5f;
	float fYourBottom = ObjInfo->GetPos()->fY - *ObjInfo->GetSize() * 0.5f;
	if (fMyTop < fYourBottom)
		return false;

	float fMyBottom = this->m_Pos.fY - this->m_fSize * 0.5f;
	float fYourTop = ObjInfo->GetPos()->fY + *ObjInfo->GetSize() * 0.5f;
	if (fYourTop < fMyBottom)
		return false;

	return true;
}

void CGameObject::SetElapsedTime(float fElapsedTime)
{
	m_fElapsedTime = fElapsedTime;
}

int CGameObject::getObjType(void)
{
	return m_ObjType;
}

int CGameObject::getMyTeam(void)
{
	return m_MyTeam;
}

float CGameObject::GetLifeTime(void)
{
	return m_fLifeTime;
}

void CGameObject::SetLifeTime(float Lifetime)
{
	m_fLifeTime = Lifetime;
}

int CGameObject::GetSpriteX(void)
{
	return m_iSpriteX;
}

Position * CGameObject::GetDir(void)
{
	return &m_Dir;
}

float CGameObject::GetParticleTime(void)
{
	return m_fParticleTime;
}
