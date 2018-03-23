/*
 * GMap.cpp
 *
 *      Author: olegvedi@gmail.com
 */

#include "GMap.h"

#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

GMap::GMap(Disp *dsp, EObject **ObjLst, unsigned dsp_width, unsigned dsp_height) {
	unsigned x, y;

	ObjList = ObjLst;
	Wnd = dsp;
	disp_width = dsp_width;
	disp_height = dsp_height;
	blocks_x = dsp_width / MAP_BLOCK_SIZE + 3;
	blocks_y = dsp_height / MAP_BLOCK_SIZE;
	blocks_y_all = blocks_y + 1;
	next_line = false;
	disp_offs = 0;
	maps_speed = 0;
	uoffs_x = 0;
	gmap = NULL;

	try{
		gmap = new gmap_str_t *[blocks_x];
		for(x=0;x<blocks_x;x++)
			gmap[x] = NULL;
		for(x=0;x<blocks_x;x++)
			gmap[x] = new gmap_str_t[blocks_y_all];
	}catch(...){
		Free();
		throw;
	}

	for(y=0;y<blocks_y_all;y++)
		for(x=0;x<blocks_x;x++){
			ObjClear(&gmap[x][y]);
		}

	x = OBJ_Pnt_Terra_Set_Misc;
	y = 0;
	unsigned cnt, w, h;
	while(x <= OBJ_Pnt_Terra_Set_Misc_Last){
		if(ObjList[x] != NULL)
			ObjList[x]->GetSpritePrmt(EObject::Sprite_Front_Img, &cnt, &w, &h);
		else
			cnt = 0;
		misc_objs[y] = cnt;
		MSG_DBG(2, "**** misc_objs:%d  cnt :%d ****", y,cnt);
		y++;
		x++;
	}

	gmap[0][blocks_y - 1].type = OBJ_Type_Terra_BottomM; //first blocks
	gmap[1][blocks_y - 1].type = OBJ_Type_Terra_BottomM;
	gmap[0][blocks_y].type = OBJ_Type_End;
	gmap[1][blocks_y].type = OBJ_Type_End;

//	gmap[blocks_y - 3][1].type = OBJ_Type_Terra_BottomM;

	last_blk_cnt = 1;
	top_level = 0;
	top_level_cnt = 0;
	top_skip = 0;

	srand (time(NULL));
	for(x=2;x<blocks_x;x++){
		GenLine(x);
	}
}

GMap::~GMap() {
	Free();
}

void GMap::Free()
{
	unsigned i;

	if(gmap){ // для внутренних указателей
		for(i=0;i<blocks_x;++i) {
	    	delete [] gmap[i];
		}
		delete [] gmap;
		gmap = NULL;
	}
}

void GMap::AddMiscObj(unsigned ObjOffs, unsigned x, unsigned y)
{
	int t;

	if(misc_objs[ObjOffs] > 0 && rand() % 10 > 7){
		t = rand() % misc_objs[ObjOffs];
		gmap[x][y].type = OBJ_Type_Misc;
		gmap[x][y].ObjectPnt = OBJ_Pnt_Terra_Set_Misc + ObjOffs;
		gmap[x][y].prm = t;
	}

}

void GMap::GenLine(unsigned x_pos)
{
	unsigned y, yt;
/*
	if(last_blk_cnt){
		last_blk_cnt=0;
		gmap[blocks_y - 2][x_pos].type = OBJ_Type_Terra_Top;
	}else
		last_blk_cnt=1;

	gmap[blocks_y - 1][x_pos].type = OBJ_Type_Terra_Bottom;
	return;
*/
	y = blocks_y - last_blk_cnt;
//	MSG_DBG(2, "****  blocks_y:%d +++++++++:%d  %d ****", blocks_y , x_pos-1, y);
	if(last_blk_cnt > 0 && gmap[x_pos-1][y].type == OBJ_Type_Terra_BottomM && gmap[x_pos-2][y].type != OBJ_Type_Terra_BottomM){
		last_blk_cnt = rand() % 2 + last_blk_cnt;
	}else{

	if(rand() % 20 < 15/* || last_blk_cnt == 0*/)// holes freq
		last_blk_cnt = rand() % (last_blk_cnt + 1) + 1;
	else // hole
		last_blk_cnt = 0;
	}

	if(last_blk_cnt > (blocks_y - 3)){
		last_blk_cnt = blocks_y - 3;
	}

	y = blocks_y - 1;
	if(last_blk_cnt > 0){
		for(yt=0;yt<last_blk_cnt;yt++){
			gmap[x_pos][y].type = OBJ_Type_Terra_BottomM;
			y--;
		}
		// misc objects
		if(rand() % 10 > 4)
			AddMiscObj( 1, x_pos, y);
		else
			AddMiscObj( 0, x_pos, y);

	}
//	MSG_DBG(2, "**** last_blk_cnt:%d ****",last_blk_cnt);

	if(top_level > y || ( y < blocks_y - 2 && gmap[x_pos-1][y+1].type == OBJ_Type_Terra_TopM)){
		if(gmap[x_pos-2][top_level].type != OBJ_Type_Terra_TopM){
			gmap[x_pos-1][top_level].type = OBJ_Type_Empty;
		}
		top_level_cnt = 0;
	}

	if(top_level_cnt > 0){// верхние дорожки
		if(top_skip > 0){ // пропускаем шаг
			top_skip--;
		}else{
//			if(gmap[x_pos - 1][top_level].type != OBJ_Type_Terra_BottomM)
			{
				gmap[x_pos][top_level].type = OBJ_Type_Terra_TopM;
				top_level_cnt--;
				AddMiscObj( 0, x_pos, top_level - 1);
			}
		}
	}else{
		if(/*rand() % 6 < last_blk_cnt &&*/ last_blk_cnt > 1){// top level chance
			top_level = y;//высота
			top_level_cnt = rand() % 5 + 2;// длинна
			top_skip = 1;
		}
	}

	gmap[x_pos][blocks_y].type = OBJ_Type_End;
}
/*
Obj_Type_t GMap::GetObjType(unsigned map_x, unsigned map_y)
{
	if(map_y >= blocks_y || map_x >= blocks_x - 1)
		return	OBJ_Type_End;

	return gmap[map_x + 1][map_y].type;
}
*/
void GMap::ObjCopy(gmap_str_t *dst, gmap_str_t *src)
{
//	dst->type = src->type;
	memcpy(dst, src, sizeof(gmap_str_t));
}

void GMap::ObjClear(gmap_str_t *obj)
{
	memset(obj, 0, sizeof(gmap_str_t));
}

unsigned GMap::ScrollMap(int ux)
{
	unsigned ret=0;
	unsigned x, y;

	if (ux) {
		uoffs_x = ux; // - disp_x;
		maps_speed = (ux >> 3);
		if (maps_speed < 10)
			maps_speed = 10;

		if (disp_offs >= MAP_BLOCK_SIZE) {
			disp_offs = 0;
			next_line = true;
			ret = MAP_BLOCK_SIZE;

			for (y = 0; y < blocks_y_all; y++) {
				for (x = 0; x < blocks_x - 1; x++) {
					ObjCopy(&gmap[x][y], &gmap[x + 1][y]);
				}
				ObjClear(&gmap[blocks_x - 1][y]);
			}
			GenLine(blocks_x - 1);
		}
	}

	if(uoffs_x > 0){
		disp_offs += maps_speed;
		Wnd->SetDispOffset(-disp_offs);
		uoffs_x -= maps_speed;

	}

	return ret;
}

void GMap::DrawPlant(int x, int y, int xlpos, int ylpos, Obj_Type_t type, int ObjIdx)
{
	int idx=0;

	if(gmap[x][y-1].type == type){//вверху такой же блок
		if((gmap[x-1][y-1].type != type && gmap[x+1][y].type == type)||
				(gmap[x+1][y-1].type != type && gmap[x+1][y].type == type)){
			ObjList[ObjIdx]->SetActiveSprite(EObject::Sprite_Down_Img);
			if(gmap[x-1][y-1].type != type)
				idx = 1;
			if(gmap[x+1][y-1].type != type)
				idx = 2;

		}else{
			ObjList[ObjIdx]->SetActiveSprite(EObject::Sprite_Front_Img_v1);
			if(gmap[x-1][y].type != type)
				idx = 1;
			if(gmap[x+1][y].type != type)
				idx = 2;

		}


	}else{
		if(gmap[x-1][y].type != type)
			idx = 1;
		if(gmap[x+1][y].type != type)
			idx = 2;

		ObjList[ObjIdx]->SetActiveSprite(EObject::Sprite_Front_Img);

	}
	ObjList[ObjIdx]->ShowActiveSprite(xlpos, ylpos, idx, true);
}

void GMap::DrawMap()
{
	unsigned x, y;
	int new_grass_loop;
	gmap_str_t *tgm;

	for(y=0;y<blocks_y;y++){
		int ylpos = 100*y;
		for(x=X_MAP_OFFS;x<blocks_x-1;x++){
			int xlpos = 100*(x-1);
			tgm = &gmap[x][y];
			switch(tgm->type){
			case OBJ_Type_Terra_BottomM:
				DrawPlant(x, y, xlpos, ylpos, OBJ_Type_Terra_BottomM, OBJ_Pnt_Terra_Set_Bottom);
				//grass
				if(gmap[x][y-1].type != OBJ_Type_Terra_BottomM)
					new_grass_loop = ObjList[OBJ_Pnt_Terra_Set_Misc_grass]->ShowActiveSprite(xlpos, ylpos, grass_loop);
				break;
			case OBJ_Type_Terra_TopM:
				DrawPlant(x, y, xlpos, ylpos, OBJ_Type_Terra_TopM, OBJ_Pnt_Terra_Set_Top);
				break;
			case OBJ_Type_Misc:
//				MSG_DBG(2, "**** OBJ_Type_Misc:%d  prm:%d ****",tgm->ObjectPnt, tgm->prm);
				ObjList[tgm->ObjectPnt]->ShowActiveSprite(xlpos, ylpos + MAP_BLOCK_SIZE, tgm->prm);
				break;
			case OBJ_Type_Empty:
				//grass
				if(y == blocks_y - 1) // bottom
					ObjList[OBJ_Pnt_Terra_Set_Misc_grass]->ShowActiveSprite(xlpos, ylpos + MAP_BLOCK_SIZE, grass_loop);
				break;
			default:{

			}
			}
		}
	}
	if(new_grass_loop >= 0)
		grass_loop = new_grass_loop;

	MSG_DBG(2, "+++++++++++++++ new_grass_loop:%d", new_grass_loop);
}

