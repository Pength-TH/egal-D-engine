// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
// * illegal driver
//2日
// * File: CollisionManager.h
// *
// * Author SeedEnginer
// * 
// * Desc:
// * ||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
#ifndef __WALK_PLANE_MGR_H__
#define __WALK_PLANE_MGR_H__
#include "runtime/api/IType.h"
#include "runtime/api/impl/SeedEnginePublic.h"

typedef struct _POINT_2D
{
    int iX;
    int iY;
} POINT_2D, *PPOINT_2D;

class Point_2D_less
{
public:
    bool operator()(const POINT_2D& point1, const POINT_2D& point2) const
    {
        if(point1.iX < point2.iX)
        {
            return true;
        }
        else if(point1.iX == point2.iX)
        {
            if(point1.iY < point2.iY)
            {
                return true;
            }
        }
        return false;
    }
};

typedef struct _TRI_INFO
{
    vector3  v1;
	vector3  v2;
	vector3  v3;

} TRI_INFO, *PTRI_INFO;

// 三角形列表信息。
typedef std::vector<TRI_INFO> TRI_INFO_VECTOR;

// 地图中注册的三角形列表与位置映射表。
typedef std::map<POINT_2D, TRI_INFO_VECTOR, Point_2D_less> TRI_INFO_MAP;

class CollisionManager
{
public:
    CollisionManager(VOID);
    virtual ~CollisionManager(VOID);
public:
    TRI_INFO_MAP m_triInfoMap;

	bool intersect_triangle(
		vector3 Orig,	// 射线的原点。
		vector3 Dir,	// 射线的方向。
		vector3 v1,
		vector3 v2,
		vector3 v3,
		float* t,
		float* u,
		float* v
		);

    unsigned load_collision_file(const char* szFileName);

	bool get_hight_at_pos(float fx, float fz, float& fy);
    void get_collision_grid(int& nx, int& ny, float fx, float fz);

	bool build_collision(
		float fOrigx, float fOrigy, float fOrigz,	// 射线的原点。
		float fDirx,  float fDiry,  float fDirz,	// 射线的方向。
		int	  ix, int iz,							// 选中地图的位置
		float& fPosx, float& fPosy, float& fPosz	// 返回建筑物的行走面位置。
		);

    bool CollisionManager::build_collision(
        float fOrigx, float fOrigy, float fOrigz,	// 射线的原点。
        float fDirx,  float fDiry,  float fDirz,	// 射线的方向。
        float fEndx, float fEndy, float fEndz, //
        float& fPosx, float& fPosy, float& fPosz	// 返回建筑物的行走面位置。
    );

    void clear_data();
};

#endif

