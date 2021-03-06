
#include "runtime/api/impl/SeedEnginePublic.h"
#include "runtime/api/impl/CollisionManager.h"
#include <math.h>
#include "common/egal-d.h"

const INT BUILD_COLLISION_SEARCH_RANGE = 30;
#define GRIDSCALE 4.0f
CollisionManager::CollisionManager(VOID)
{
}

CollisionManager::~CollisionManager(VOID)
{
}

bool CollisionManager::get_hight_at_pos(float fx, float fz, float& fy)
{
	POINT_2D pos;

	float fu = 0;
	float fv = 0;

	float fX21 = 0;
	float fX31 = 0;
	float fY21 = 0;
	float fY31 = 0;

	float fX1 = 0;
	float fY1 = 0;

	vector3 v1;
	vector3 v2;
	vector3 v3;

	float fHeightValue = 0;
	float fFindValue = -1000.0f;

	get_collision_grid(pos.iX, pos.iY, fx, fz);

	bool bFind = false;

	if (m_triInfoMap.count(pos))
	{
		TRI_INFO_VECTOR& triInfo = m_triInfoMap[pos];
		INT iSize = (INT)triInfo.size();
		for (INT i = 0; i < iSize; i++)
		{
			v1.x = triInfo[i].v1.x;
			v1.y = triInfo[i].v1.y;
			v1.z = triInfo[i].v1.z;

			v2.x = triInfo[i].v2.x;
			v2.y = triInfo[i].v2.y;
			v2.z = triInfo[i].v2.z;

			v3.x = triInfo[i].v3.x;
			v3.y = triInfo[i].v3.y;
			v3.z = triInfo[i].v3.z;

			fX1 = fx - v1.x;
			fY1 = fz - v1.z;

			fX21 = v2.x - v1.x;
			fY21 = v2.z - v1.z;

			fX31 = v3.x - v1.x;
			fY31 = v3.z - v1.z;

			if (fabs(fX21 * fY31 - fY21 * fX31) < 0.0001f)
			{
				continue;
			}

			fu = (fX1 * fY31 - fY1 * fX31) / (fX21 * fY31 - fY21 * fX31);
			fv = (fY1 * fX21 - fX1 * fY21) / (fX21 * fY31 - fY21 * fX31);

			if ((fv > 0) && (fv < 1) && (fu > 0) && (fu < 1) && (1 - fv - fu > 0))
			{
				bFind = true;
				fHeightValue = v1.y + fu * (v2.y - v1.y) + fv * (v3.y - v1.y);
				if (fHeightValue > fFindValue)
				{
					fFindValue = fHeightValue;
				}
			}
		}
	}

	fy = fFindValue;
	return bFind;
}
void CollisionManager::get_collision_grid(int& nx, int& ny, float fx, float fz)
{
	nx = floor(fx / GRIDSCALE);//(int)(fx / GRIDSCALE);
	ny = floor(fz / GRIDSCALE);//(int)(fz/ GRIDSCALE);
	return;
}

void CollisionManager::clear_data()
{
	m_triInfoMap.clear();
}

unsigned CollisionManager::load_collision_file(const char* szFileName)
{
	clear_data();
	if (!szFileName || szFileName[0] == '\0') return 1;

	egal::FS::IFile* pData = egal::g_file_system->open(egal::g_file_system->getDiskDevice(), szFileName, egal::FS::Mode::READ);
	if (pData)
	{
		TRI_INFO	triInfo;
		POINT_2D	pos;
		uint32_t	dwVersion = 0;
		INT         iPosCount = 0;
		INT         iTriCount = 0;

		TRI_INFO_VECTOR triInfoVector;

		// 读取版本号
		pData->read(&dwVersion, sizeof(dwVersion));

		// 读取注册过的三角形的位置的个数
		pData->read(&iPosCount, sizeof(iPosCount));
		for (INT i = 0; i < iPosCount; i++)
		{
			// 清空三角形信息数组。
			triInfoVector.clear();

			// 读取位置
			pData->read(&pos, sizeof(pos));

			// 读取注册这个位置的三角形的个数
			pData->read(&iTriCount, sizeof(iTriCount));
			for (INT j = 0; j < iTriCount; j++)
			{
				pData->read(&triInfo, sizeof(triInfo));
				triInfoVector.push_back(triInfo);
			}

			if (triInfoVector.size())
			{
				m_triInfoMap[pos] = triInfoVector;
			}
		}
		pData->close();
	}
	return 0;
}

bool CollisionManager::build_collision(
	float fOrigx, float fOrigy, float fOrigz,	// 射线的原点。
	float fDirx, float fDiry, float fDirz,	// 射线的方向。
	float fEndx, float fEndy, float fEndz, //
	float& fPosx, float& fPosy, float& fPosz	// 返回建筑物的行走面位置。
)
{
	bool bResult = false;
	float fzperx = 0.0f;
	float fxperz = 0.0f;
	if (!egal::RealEqual(fDirx, 0))
	{
		fzperx = fDirz / fDirx;
	}

	if (!egal::RealEqual(fDirz, 0))
	{
		fxperz = fDirx / fDirz;
	}

	float fMaxDistance = 1000;
	POINT_2D pos;
	INT iSize = 0;
	vector3 Orig;	// 射线的原点。
	vector3 Dir;	// 射线的方向。
	vector3 v1;
	vector3 v2;
	vector3 v3;

	float fd = 0;
	float fv = 0;
	float fu = 0;

	Orig.x = fOrigx;
	Orig.y = fOrigy;
	Orig.z = fOrigz;

	Dir.x = fDirx;
	Dir.y = fDiry;
	Dir.z = fDirz;

	int nDistance = 0;
	float fRealPosx, fRealPosz;
	float fStep = 0.0f;
	float fInit = 0.0f;
	int nX, nY;
	if (fOrigx < fEndx)
	{
		fInit = -GRIDSCALE;
		fStep = GRIDSCALE;
		nDistance = (int)((fEndx - fOrigx) / GRIDSCALE) + 1;
	}
	else
	{
		fInit = GRIDSCALE;
		fStep = -GRIDSCALE;
		nDistance = (int)((fOrigx - fEndx) / GRIDSCALE) + 1;
	}

	for (int i = 0; i <= nDistance; i++)
	{
		fInit += fStep;
		fRealPosx = fOrigx + fInit;
		fRealPosz = fOrigz + fInit * fzperx;
		get_collision_grid(nX, nY, fRealPosx, fRealPosz);

		pos.iX = nX;
		pos.iY = nY;
		if (0 == m_triInfoMap.count(pos))
		{
			continue;
		}

		TRI_INFO_VECTOR& triInfo = m_triInfoMap[pos];
		iSize = (INT)triInfo.size();
		for (INT iRri = 0; iRri < iSize; iRri++)
		{
			v1.x = triInfo[iRri].v1.x;
			v1.y = triInfo[iRri].v1.y;
			v1.z = triInfo[iRri].v1.z;

			v2.x = triInfo[iRri].v2.x;
			v2.y = triInfo[iRri].v2.y;
			v2.z = triInfo[iRri].v2.z;

			v3.x = triInfo[iRri].v3.x;
			v3.y = triInfo[iRri].v3.y;
			v3.z = triInfo[iRri].v3.z;

			if (intersect_triangle(Orig, Dir, v1, v2, v3, &fd, &fu, &fv))
			{
				if (fMaxDistance > fd)
				{
					fMaxDistance = fd;
					fPosx = (1 - fv - fu) * v1.x + fu * v2.x + fv * v3.x;
					fPosy = (1 - fv - fu) * v1.y + fu * v2.y + fv * v3.y;
					fPosz = (1 - fv - fu) * v1.z + fu * v2.z + fv * v3.z;
				}

				bResult = true;
			}
		}
	}


	if (fOrigz < fEndz)
	{
		fInit = -GRIDSCALE;
		fStep = GRIDSCALE;
		nDistance = (int)((fEndz - fOrigz) / GRIDSCALE) + 1;
	}
	else
	{
		fInit = GRIDSCALE;
		fStep = -GRIDSCALE;
		nDistance = (int)((fOrigz - fEndz) / GRIDSCALE) + 1;
	}

	for (int i = 0; i <= nDistance; i++)
	{
		fInit += fStep;
		fRealPosz = fOrigz + fInit;
		fRealPosx = fOrigx + fInit * fxperz;
		get_collision_grid(nX, nY, fRealPosx, fRealPosz);
		pos.iX = nX;
		pos.iY = nY;
		if (0 == m_triInfoMap.count(pos))
		{
			continue;
		}

		TRI_INFO_VECTOR& triInfo = m_triInfoMap[pos];
		iSize = (INT)triInfo.size();
		for (INT iRri = 0; iRri < iSize; iRri++)
		{
			v1.x = triInfo[iRri].v1.x;
			v1.y = triInfo[iRri].v1.y;
			v1.z = triInfo[iRri].v1.z;

			v2.x = triInfo[iRri].v2.x;
			v2.y = triInfo[iRri].v2.y;
			v2.z = triInfo[iRri].v2.z;

			v3.x = triInfo[iRri].v3.x;
			v3.y = triInfo[iRri].v3.y;
			v3.z = triInfo[iRri].v3.z;

			if (intersect_triangle(Orig, Dir, v1, v2, v3, &fd, &fu, &fv))
			{
				if (fMaxDistance > fd)
				{
					fMaxDistance = fd;
					fPosx = (1 - fv - fu) * v1.x + fu * v2.x + fv * v3.x;
					fPosy = (1 - fv - fu) * v1.y + fu * v2.y + fv * v3.y;
					fPosz = (1 - fv - fu) * v1.z + fu * v2.z + fv * v3.z;
				}
				bResult = true;
			}
		}
	}
	return bResult;
}

bool CollisionManager::build_collision(
	float fOrigx, float fOrigy, float fOrigz,	// 射线的原点。
	float fDirx, float fDiry, float fDirz,	// 射线的方向。
	int	  ix, int iz,							// 选中地图的位置
	float& fPosx, float& fPosy, float& fPosz	// 返回建筑物的行走面位置。
)
{
	bool bReturn = FALSE;

	if (m_triInfoMap.size() == 0)
		return bReturn;
	float fDistance = 1000;

	INT iL = ix - BUILD_COLLISION_SEARCH_RANGE;
	INT iR = ix + BUILD_COLLISION_SEARCH_RANGE;
	INT iT = iz - BUILD_COLLISION_SEARCH_RANGE;
	INT iB = iz + BUILD_COLLISION_SEARCH_RANGE;

	POINT_2D pos;
	INT iSize = 0;
	vector3 Orig;	// 射线的原点。
	vector3 Dir;	// 射线的方向。
	vector3 v1;
	vector3 v2;
	vector3 v3;

	float fd = 0;
	float fv = 0;
	float fu = 0;

	Orig.x = fOrigx;
	Orig.y = fOrigy;
	Orig.z = fOrigz;

	Dir.x = fDirx;
	Dir.y = fDiry;
	Dir.z = fDirz;

	for (INT i = iL; i < iR; i++)
		for (INT j = iT; j < iB; j++)
		{
			pos.iX = i;
			pos.iY = j;
			if (0 == m_triInfoMap.count(pos))
			{
				continue;
			}

			TRI_INFO_VECTOR& triInfo = m_triInfoMap[pos];
			iSize = (INT)triInfo.size();
			for (INT iRri = 0; iRri < iSize; iRri++)
			{
				v1.x = triInfo[iRri].v1.x;
				v1.y = triInfo[iRri].v1.y;
				v1.z = triInfo[iRri].v1.z;

				v2.x = triInfo[iRri].v2.x;
				v2.y = triInfo[iRri].v2.y;
				v2.z = triInfo[iRri].v2.z;

				v3.x = triInfo[iRri].v3.x;
				v3.y = triInfo[iRri].v3.y;
				v3.z = triInfo[iRri].v3.z;

				if (intersect_triangle(Orig, Dir, v1, v2, v3, &fd, &fu, &fv))
				{
					if (fDistance > fd)
					{
						fDistance = fd;
						fPosx = (1 - fv - fu) * v1.x + fu * v2.x + fv * v3.x;
						fPosy = (1 - fv - fu) * v1.y + fu * v2.y + fv * v3.y;
						fPosz = (1 - fv - fu) * v1.z + fu * v2.z + fv * v3.z;
					}

					bReturn = true;
				}
			}
		}
	return bReturn;
}


bool CollisionManager::intersect_triangle(
	vector3 Orig,	// 射线的原点。
	vector3  Dir,	// 射线的方向。
	vector3  v1,
	vector3  v2,
	vector3  v3,
	float* t,
	float* u,
	float* v
)
{
	vector3 edge1;
	edge1.x = v2.x - v1.x;
	edge1.y = v2.y - v1.y;
	edge1.z = v2.z - v1.z;

	vector3 edge2;
	edge2.x = v3.x - v1.x;
	edge2.y = v3.y - v1.y;
	edge2.z = v3.z - v1.z;

	vector3 pvec;
	pvec.x = Dir.y * edge2.z - Dir.z * edge2.y;
	pvec.y = Dir.z * edge2.x - Dir.x * edge2.z;
	pvec.z = Dir.x * edge2.y - Dir.y * edge2.x;

	float  det = edge1.x * pvec.x + edge1.y * pvec.y + edge1.z * pvec.z;
	vector3 tvec;
	if (det > 0)
	{
		tvec.x = Orig.x - v1.x;
		tvec.y = Orig.y - v1.y;
		tvec.z = Orig.z - v1.z;
	}
	else
	{
		tvec.x = -(Orig.x - v1.x);
		tvec.y = -(Orig.y - v1.y);
		tvec.z = -(Orig.z - v1.z);
		det = -det;
	}

	if (det < 0.0001f)
		return false;

	*u = tvec.x * pvec.x + tvec.y * pvec.y + tvec.z * pvec.z;

	if (*u < 0.0f || *u > det)
		return false;

	vector3 qvec;
	qvec.x = tvec.y * edge1.z - tvec.z * edge1.y;
	qvec.y = tvec.z * edge1.x - tvec.x * edge1.z;
	qvec.z = tvec.x * edge1.y - tvec.y * edge1.x;

	*v = Dir.x * qvec.x + Dir.y * qvec.y + Dir.z * qvec.z;
	if (*v < 0.0f || *u + *v > det)
		return false;

	*t = edge2.x * qvec.x + edge2.y * qvec.y + edge2.z * qvec.z;
	float fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;
}

