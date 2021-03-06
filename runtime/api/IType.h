#ifndef SEED_ITYPE_INCLUDE
#define SEED_ITYPE_INCLUDE
#pragma once

#define SEED_STATIC 1

#ifdef SEED_STATIC
#  define SeedExport
#else
#      ifdef SEED_EXPORT
#         define SeedExport __declspec (dllexport)
#      else
#         define SeedExport __declspec (dllimport)
#      endif
#endif

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

#include <vector>

#ifndef VOID
#define VOID void
#endif
#define FALSE 0
#define TRUE  1

typedef char				   *LPSTR, *PSTR;
typedef int						BOOL;
typedef unsigned short			uint16_t;
typedef signed   int			int32_t;
typedef unsigned int			uint32_t;
typedef unsigned long long int	uint64_t;
typedef unsigned long			DWORD;
typedef float					Real;

struct vector2
{
public:
	vector2() : x(0.0f), y(0.0f) { }
	vector2(Real _x, Real _y) : x(_x), y(_y) { }
	Real x; Real y;
};

struct vector3
{
public:
	vector3() : x(0.0f), y(0.0f), z(0.0f) { }
	vector3(Real _x, Real _y, Real _z) : x(_x), y(_y), z(_z) { }

	Real x; Real y; Real z;
};

struct ray 
{
public:
	vector3 mOrigin;
	vector3 mDirection;
};

enum enum_dummy_location
{
	EDL_NONE = 0,
	EDL_LEFT,
	EDL_RIGHT,
	EDL_BACK,
	EDL_TOP,
	EDL_END,
};

enum enum_remove_type
{
	ERT_NOW = 0,
	ERT_LIFETIME,
	ERT_SHOUT_DOWN,
};

enum enum_game_object_type
{
	EGOT_DEFAULT,
	EGOT_DUMMY,
	EGOT_STATIC_MESH,
	EGOT_DYNAMIC_MESH,
	EGOT_CHARACTOR,
	EGOT_EFFECT,
	EGOT_SCENE,
	EGOT_CAMERA,
	EGOT_TRACKER,
	EGOT_END,
};

enum axis_type
{
	EXT_DEFAULT,
	EXT_GAME = EXT_DEFAULT,
	EXT_GAME_FLAT,
	EXT_ENGINE,
	EXT_SCREEN,
	EXT_END,
};

enum enum_ray_query_level
{
	ERQL_BEGIN = 0,
	ERQL_UNKNOWN = ERQL_BEGIN,

	ERQL_ITEMBOX = 1,
	ERQL_ENEMY_PLAYER = 2,
	ERQL_ENEMY_NPC = 3,
	ERQL_PLATFORM = 4,
	ERQL_ALLY_NPC = 5,
	ERQL_ALLY_PLAYER = 6,
	ERQL_PET = 7,
	ERQL_CORPSE = 8,
	ERQL_PLAYER_SELF = 9,

	ERQL_END = 10,
};

#endif
