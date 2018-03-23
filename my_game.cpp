//============================================================================
// Name        : my_game.cpp
// Author      : iam
// Version     :
// Copyright   : Your copyright notice
// Description : 
//============================================================================

#include <iostream>
#include <string>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "Disp.h"
#include "EObject.h"
#include "Sprite.h"
#include "GMap.h"
#include "MUser.h"
#include "Mob.h"
#include "MFly.h"
#include "MPredator.h"

#include "priv.h"

using namespace std;

const unsigned disp_width = 1200;
const unsigned disp_height = 700;

#define IMGs_PATH "res/imgs"

static const char res_path[] = IMGs_PATH;

static EObject *ObjList[256];

// objects num ranges

#define BUTRF_CNT 19

int CreateObj(Disp *Dsp, unsigned ObjNum, EObject::sprite_num_t sprite_num, unsigned sprite_width, unsigned min_sprite_loop, string path, bool Mirror)
{
	int ret;

    if(ObjList[ObjNum] == NULL)
            ObjList[ObjNum] = new EObject(Dsp);
	ret = ObjList[ObjNum]->LoadSprite(sprite_num, path, sprite_width, min_sprite_loop, Mirror);

	return ret;
}

void Init(Disp *Dsp)
{
	unsigned i;
	int ret;
	string lpath;

	for(i=0;i<256;i++){
		ObjList[i] = NULL;
	}

	lpath = res_path;
	lpath += "/user/girl";
	ret = CreateObj(Dsp, OBJ_Pnt_Set_User, EObject::Sprite_Right_Img, 90, 1, lpath, true);
	ret = CreateObj(Dsp, OBJ_Pnt_Set_User, EObject::Sprite_Jump_Right_Img, 90, 1, lpath, true);
	MSG_DBG(2, "path:%s ret:%d", lpath.c_str(), ret);

	lpath = res_path;
	lpath += "/terra";
	ret = CreateObj(Dsp, OBJ_Pnt_Terra_Set_Bottom, EObject::Sprite_Front_Img, 100, 0, lpath, false);
	ret = CreateObj(Dsp, OBJ_Pnt_Terra_Set_Bottom, EObject::Sprite_Front_Img_v1, 100, 0, lpath, false);
	ret = CreateObj(Dsp, OBJ_Pnt_Terra_Set_Bottom, EObject::Sprite_Down_Img, 100, 0, lpath, false);

	lpath = res_path;
	lpath += "/terra/top";
	ret = CreateObj(Dsp, OBJ_Pnt_Terra_Set_Top, EObject::Sprite_Front_Img, 100, 0, lpath, false);

	lpath = res_path;
	lpath += "/misc";
	ret = CreateObj(Dsp, OBJ_Pnt_Terra_Set_Misc, EObject::Sprite_Front_Img, 70, 0, lpath, false);

	lpath = res_path;
	lpath += "/misc/big";
	ret = CreateObj(Dsp, OBJ_Pnt_Terra_Set_Misc + 1, EObject::Sprite_Front_Img, 100, 0, lpath, false);

	lpath = res_path;
	lpath += "/terra/grass";
	ret = CreateObj(Dsp, OBJ_Pnt_Terra_Set_Misc_grass, EObject::Sprite_Front_Img, 100, 0, lpath, false);

	lpath = res_path;
	lpath += "/spirit/butterfly";
	ret = CreateObj(Dsp, OBJ_Pnt_Set_Spirit, EObject::Sprite_Left_Img, 70, 0, lpath, true);

	lpath = res_path;
	lpath += "/enemy/wolf";
	ret = CreateObj(Dsp, OBJ_Pnt_Set_Enemy, EObject::Sprite_Right_Img, 135, 1, lpath, true);
	ret = CreateObj(Dsp, OBJ_Pnt_Set_Enemy, EObject::Sprite_Jump_Right_Img, 145, 1, lpath, true);
}

void Free()
{
	unsigned i;

	for(i=0;i<256;i++){
		if(ObjList[i] != NULL){
			delete ObjList[i];
			ObjList[i] = NULL;
		}
	}

}

#define FRAME_DELAY_US (1000000/25)

int main(void)
{
	bool GameWork=true;
    struct timeval t_curr, t_past;
    unsigned elapsedTime;
	int ret, i;
	const char *bgs_fname[] = {IMGs_PATH "/bgt.bmp", IMGs_PATH "/bgd.bmp"};
	Disp Dsp(bgs_fname, disp_width, disp_height);

	Init(&Dsp);
	GMap GM(&Dsp, ObjList, disp_width, disp_height);

	MUser MU(ObjList[OBJ_Pnt_Set_User], &GM);
//	MUser MU(ObjList[OBJ_Pnt_Set_Enemy], &GM);

	MPredator Predator(ObjList[OBJ_Pnt_Set_Enemy], &GM);

	Mob *ButterFlyM[BUTRF_CNT];

	for(i=0;i<BUTRF_CNT;i++){
		ObjList[OBJ_Pnt_Set_Spirit + i + 1] = ObjList[OBJ_Pnt_Set_Spirit]->GetCopyEObject();
		ButterFlyM[i] = new	MFly(ObjList[OBJ_Pnt_Set_Spirit + i + 1], &GM);
	}


//	ret = ObjList[OBJ_Pnt_Terra_Set_Bottom]->SetActiveSprite(EObject::Sprite_Front_Img);
//	MSG_DBG(2, "SetActiveSprite ret:%d",ret);

	srand (time(NULL));
//	gettimeofday(&t_past, NULL);

	int fdelay=0;

	SDL_Event event;
	unsigned char *keys;
	int dir_x, dir_y;
	bool fire;
	unsigned map_move_corr, disp_center_map = disp_width / 2;
	while(GameWork){
		gettimeofday(&t_past, NULL);

		while (SDL_PollEvent(&event)) {
			dir_x = 0;
			dir_y = 0;
			fire = false;
			if (event.type == SDL_QUIT) {
				GameWork = false;
			}
			if (event.type == SDL_KEYDOWN) {
				if(event.key.keysym.sym == SDLK_ESCAPE){
					GameWork = false;
				}
			}

			keys=SDL_GetKeyState(NULL);

			if(keys[SDLK_LEFT]){
				dir_x = -1;
			}
			if(keys[SDLK_RIGHT]){
				dir_x = 1;
			}
			if(keys[SDLK_SPACE]){// fire
				fire = true;
			}
			if(keys[SDLK_UP]){
				dir_y = 1;
			}
			if(keys[SDLK_DOWN]){
				dir_y = -1;
			}
		}

		map_move_corr = MU.Move( dir_x, dir_y, fire);
		if(map_move_corr > disp_center_map)
			map_move_corr = GM.ScrollMap(map_move_corr - disp_center_map);
		else
			map_move_corr = GM.ScrollMap(0);
//******************************************************

		gettimeofday(&t_curr, NULL);
        elapsedTime = (t_curr.tv_sec - t_past.tv_sec) * 1000000; // to us
        elapsedTime += (t_curr.tv_usec - t_past.tv_usec);
        MSG_DBG(2, "------ elapsedTime0:%u ------",elapsedTime);

		Dsp.ShowPrepare();

		gettimeofday(&t_curr, NULL);
        elapsedTime = (t_curr.tv_sec - t_past.tv_sec) * 1000000; // to us
        elapsedTime += (t_curr.tv_usec - t_past.tv_usec);
        MSG_DBG(2, "------ elapsedTime1:%u ------",elapsedTime);

		GM.DrawMap();

		gettimeofday(&t_curr, NULL);
        elapsedTime = (t_curr.tv_sec - t_past.tv_sec) * 1000000; // to us
        elapsedTime += (t_curr.tv_usec - t_past.tv_usec);
        MSG_DBG(2, "------ elapsedTime2:%u ------",elapsedTime);

		MU.Draw(map_move_corr);

		for(i=0;i<BUTRF_CNT;i++){
			ButterFlyM[i]->Draw(map_move_corr);
		}

		Predator.Draw(map_move_corr);

		gettimeofday(&t_curr, NULL);
        elapsedTime = (t_curr.tv_sec - t_past.tv_sec) * 1000000; // to us
        elapsedTime += (t_curr.tv_usec - t_past.tv_usec);
        MSG_DBG(2, "------ elapsedTime3:%u ------",elapsedTime);

		Dsp.ShowGo();

		gettimeofday(&t_curr, NULL);
        elapsedTime = (t_curr.tv_sec - t_past.tv_sec) * 1000000; // to us
        elapsedTime += (t_curr.tv_usec - t_past.tv_usec);
//		t_past.tv_sec = t_curr.tv_sec;
//		t_past.tv_usec = t_curr.tv_usec;
        MSG_DBG(2, "------ elapsedTime:%u ------",elapsedTime);
        if(elapsedTime < FRAME_DELAY_US){
            usleep( FRAME_DELAY_US - elapsedTime);
        }

//		sleep(1);
	}

	for(i=0;i<BUTRF_CNT;i++){
		delete ButterFlyM[i];
	}

	Free();
	return EXIT_SUCCESS;
}
