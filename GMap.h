/*
 * GMap.h
 *
 *      Author: olegvedi@gmail.com
 */

#ifndef GMAP_H_
#define GMAP_H_

#include "Disp.h"
#include "EObject.h"

#include "priv.h"

#define MAP_BLOCK_SIZE 100

#define X_MAP_OFFS 1

//#define X_TO_MAP_COORD_EXT(x) (((x) / MAP_BLOCK_SIZE) < 0 ? 0: ((x) / MAP_BLOCK_SIZE) + 1)
#define X_TO_MAP_COORD_EXT(x) (((x) / MAP_BLOCK_SIZE) + 1)
#define Y_TO_MAP_COORD_EXT(x) ((x) / MAP_BLOCK_SIZE)

#define X_TO_DISP_COORD_EXT(x) (((x) - 1) * MAP_BLOCK_SIZE)
#define Y_TO_DISP_COORD_EXT(x) ((x) * MAP_BLOCK_SIZE)

/*
 #define OBJ_USER_NUM 1
 #define OBJ_SPIRIT_NUM 20
 #define OBJ_ENEMY_NUM 50
 #define OBJ_TERRA_NUM 100
 */
#define MISC_DECOR_PER 16
typedef enum { // index for EObj array
    /*
     OBJ_Pnt_Set_User=OBJ_USER_NUM,
     OBJ_Pnt_Set_Spirit=OBJ_SPIRIT_NUM,
     OBJ_Pnt_Set_Enemy=OBJ_ENEMY_NUM,
     OBJ_Pnt_Terra_Set_Bottom=OBJ_TERRA_NUM,*/
    OBJ_Pnt_Terra_Set_Bottom = 0,
    OBJ_Pnt_Terra_Set_Top,
    OBJ_Pnt_Set_Misc_Water,
    OBJ_Pnt_Set_Misc_Grass,
    OBJ_Pnt_Set_Misc_Grass_v1,
    OBJ_Pnt_Set_Misc_Decor_Small,
    OBJ_Pnt_Set_Misc_Decor_Mid,
    OBJ_Pnt_Set_Misc_Decor_Big,
    OBJ_Pnt_Set_Misc_Decor_Periodic,
    OBJ_Pnt_Set_Misc_Useful,
    OBJ_Pnt_Set_Last,
} Obj_Index_t;

typedef enum {
    OBJ_Type_Empty = 0,
    OBJ_Type_User,
    OBJ_Type_Spirit,
    OBJ_Type_Enem,
    OBJ_Type_Misc = 100,
    OBJ_Type_Terra_Water,
    OBJ_Type_Terra_Transparent = 120,
    OBJ_Type_Terra_TopM,
    OBJ_Type_Terra_TopL,
    OBJ_Type_Terra_TopR,
    OBJ_Type_Terra_Hard = 140,
    OBJ_Type_Terra_BottomM,
    OBJ_Type_Terra_BottomL,
    OBJ_Type_Terra_BottomR,
    OBJ_Type_End,
} Obj_Type_t;

#define BLK_IS_FREE(b) (b < OBJ_Type_Terra_Transparent)
#define BLK_NOT_FREE(b) (b >= OBJ_Type_Terra_Transparent)
#define BLK_IS_TRANSP(b) (b < OBJ_Type_Terra_Hard)
#define BLK_NOT_TRANSP(b) (b >= OBJ_Type_Terra_Hard)

typedef struct {
    int y_top;
    int y_bottom;
    int y_under_bottom;
    int x_left;
    int x_right;
} gmap_pos_str_t;

class Mob;
class MSolidAdd;

class GMap {
    friend Mob;
    friend MSolidAdd;
public:
    typedef struct {
	Obj_Type_t type;
	/*unsigned char*/
	Obj_Index_t ObjectPnt;
	int prm;
	bool on_top;
    } gmap_str_t;
public:
    GMap(Disp *dsp, unsigned disp_width, unsigned disp_height);
    virtual ~GMap();

    Obj_Type_t GetObjType(unsigned map_x, unsigned map_y);

    void DrawBgMap();
    void DrawMidMap();
    void DrawTopMap();
    unsigned ScrollMap(int step);
    void Free();
    void InitMap();

    EObject ObjListTerra[OBJ_Pnt_Set_Last + 1];
private:
    GMap(const GMap&);
    GMap& operator=(GMap&);

    Disp *Wnd;
    unsigned disp_width, disp_height;
    unsigned blocks_x;
    unsigned blocks_y;
    unsigned blocks_y_all;
    int uoffs_x;
    unsigned maps_speed;
    gmap_str_t **gmap;
    unsigned last_blk_cnt, top_level, top_level_cnt, top_skip;
    int disp_offs;
    bool next_line;
    int decor_periodic_cnt;

    void ObjCopy(gmap_str_t *dst, gmap_str_t *src);
    void ObjClear(gmap_str_t *obj);
    void GenLine(unsigned x_pos);
    void DrawPlant(int x, int y, int xlpos, int ylpos, Obj_Type_t type, int ObjIdx);
    void AddMiscObj(Obj_Index_t ObjOffs, unsigned x, unsigned y);

};

#endif /* GMAP_H_ */
