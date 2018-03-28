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

int LoadObj(EObject *EObj, EObject::sprites_sheet_num_t sprite_num, unsigned sprite_width, unsigned min_sprite_loop,
	string path, bool Mirror)
{
    int ret;

//    assert(EObj == NULL);

    ret = EObj->LoadSpriteSheet(sprite_num, path, sprite_width, min_sprite_loop, Mirror);

    return ret;
}

void InitTerra(GMap *GM)
{
    int ret;
    string lpath;

    lpath = res_path;
    lpath += "/terra";
    ret = LoadObj(&GM->ObjListTerra[OBJ_Pnt_Terra_Set_Bottom], EObject::Sprite_Front_Sheet, 100, 0, lpath, false);
    ret = LoadObj(&GM->ObjListTerra[OBJ_Pnt_Terra_Set_Bottom], EObject::Sprite_Front_Sheet_v1, 100, 0, lpath, false);
    ret = LoadObj(&GM->ObjListTerra[OBJ_Pnt_Terra_Set_Bottom], EObject::Sprite_Down_Sheet, 100, 0, lpath, false);

    lpath = res_path;
    lpath += "/terra/top";
    ret = LoadObj(&GM->ObjListTerra[OBJ_Pnt_Terra_Set_Top], EObject::Sprite_Front_Sheet, 100, 0, lpath, false);

    lpath = res_path;
    lpath += "/terra/water";
    ret = LoadObj(&GM->ObjListTerra[OBJ_Pnt_Set_Misc_Water], EObject::Sprite_Front_Sheet, 120, 0, lpath, false);

    lpath = res_path;
    lpath += "/terra/grass";
    ret = LoadObj(&GM->ObjListTerra[OBJ_Pnt_Set_Misc_Grass], EObject::Sprite_Front_Sheet, 100, 0, lpath, false);

    lpath = res_path;
    lpath += "/misc/decor";
    ret = LoadObj(&GM->ObjListTerra[OBJ_Pnt_Set_Misc_Decor_Small], EObject::Sprite_Front_Sheet, 70, 0, lpath, false);

    lpath = res_path;
    lpath += "/misc/decor/mid";
    ret = LoadObj(&GM->ObjListTerra[OBJ_Pnt_Set_Misc_Decor_Mid], EObject::Sprite_Front_Sheet, 100, 0, lpath, false);

    lpath = res_path;
    lpath += "/misc/decor/big";
    ret = LoadObj(&GM->ObjListTerra[OBJ_Pnt_Set_Misc_Decor_Big], EObject::Sprite_Front_Sheet, 220, 0, lpath, false);

    lpath = res_path;
    lpath += "/misc/periodic";
    ret = LoadObj(&GM->ObjListTerra[OBJ_Pnt_Set_Misc_Decor_Periodic], EObject::Sprite_Front_Sheet, 70, 0, lpath, false);

    lpath = res_path;
    lpath += "/misc/useful";
    ret = LoadObj(&GM->ObjListTerra[OBJ_Pnt_Set_Misc_Useful], EObject::Sprite_Front_Sheet, 70, 0, lpath, false);

}

void InitUser(Mob *MU)
{
    int ret;
    string lpath;

    MSG_DBG(2, "path:%s ret:%d", lpath.c_str(), ret);

}

#define FPS (50)
#define FRAME_DELAY_US (1000000/FPS)

int main(void)
{
    bool GameWork = true;
    struct timeval t_curr, t_past;
    unsigned elapsedTime, elapsedTime1;
    int ret, i;
    string lpath;
    EObject *TempObj, *TempObj1;
    const char *bgs_fname[] = { IMGs_PATH "/bgt.bmp", IMGs_PATH "/bgd.bmp" };

    Disp Dsp(bgs_fname, disp_width, disp_height);
    EObject::Init(&Dsp);

    GMap GM(&Dsp, disp_width, disp_height);
    Mob::Init(&GM);

    InitTerra(&GM);
    GM.InitMap();

    MUser MU;
    lpath = res_path;
    lpath += "/user/girl";
    TempObj = MU.GetEObject();
    ret = LoadObj(TempObj, EObject::Sprite_Right_Sheet, 90, 1, lpath, true);
    ret = LoadObj(TempObj, EObject::Sprite_Jump_Right_Sheet, 90, 1, lpath, true);

    MPredator Predator;
    lpath = res_path;
    lpath += "/enemy/wolf";
    TempObj = Predator.GetEObject();
    ret = LoadObj(TempObj, EObject::Sprite_Right_Sheet, 135, 1, lpath, true);
    ret = LoadObj(TempObj, EObject::Sprite_Jump_Right_Sheet, 145, 1, lpath, true);

#define BUTRF_CNT 10

    MFly ButterFlyM[BUTRF_CNT];

    lpath = res_path;
    lpath += "/spirit/butterfly";
    TempObj = ButterFlyM[0].GetEObject();
    ret = LoadObj(TempObj, EObject::Sprite_Left_Sheet, 70, 0, lpath, true);

    for (i = 1; i < BUTRF_CNT; i++) {
	TempObj1 = ButterFlyM[i].GetEObject();
	TempObj1->CopyEObject(TempObj);
    }

    srand(time(NULL));

//    int fdelay = 0;

    SDL_Event event;
    unsigned char *keys;
    int dir_x, dir_y;
    bool fire;
    bool time_flg; // timer 1/10 sec
    unsigned time_cnt = 0;
    unsigned map_move_corr, disp_center_map = disp_width / 2;
    while (GameWork) {
	MSG_DBG(2, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
	gettimeofday(&t_past, NULL);

	time_cnt++;
	if (time_cnt >= (FPS / 10)) {
	    time_cnt = 0;
	    time_flg = true;
	} else
	    time_flg = false;

	while (SDL_PollEvent(&event)) {
	    dir_x = 0;
	    dir_y = 0;
	    fire = false;
	    if (event.type == SDL_QUIT) {
		GameWork = false;
	    }
	    if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
		    GameWork = false;
		}
	    }

	    keys = SDL_GetKeyState(NULL);

	    if (keys[SDLK_LEFT]) {
		dir_x = -1;
	    }
	    if (keys[SDLK_RIGHT]) {
		dir_x = 1;
	    }
	    if (keys[SDLK_SPACE]) { // fire
		fire = true;
	    }
	    if (keys[SDLK_UP]) {
		dir_y = 1;
	    }
	    if (keys[SDLK_DOWN]) {
		dir_y = -1;
	    }
	}

	map_move_corr = MU.Move( dir_x, dir_y, fire);
	if (map_move_corr > disp_center_map)
	    map_move_corr = GM.ScrollMap(map_move_corr - disp_center_map);
	else
	    map_move_corr = GM.ScrollMap(0);
//******************************************************

	gettimeofday(&t_curr, NULL);
	elapsedTime = (t_curr.tv_sec - t_past.tv_sec) * 1000000; // to us
	elapsedTime += (t_curr.tv_usec - t_past.tv_usec);
	MSG_DBG(2, "------ KeysProc dTime0:%u uS------", elapsedTime);

	Dsp.ShowPrepare();

	gettimeofday(&t_curr, NULL);
	elapsedTime1 = (t_curr.tv_sec - t_past.tv_sec) * 1000000; // to us
	elapsedTime1 += (t_curr.tv_usec - t_past.tv_usec);
	MSG_DBG(2, "------ ShowPrepare dTime:%u uS------", elapsedTime1 - elapsedTime);

	GM.DrawBgMap();

	gettimeofday(&t_curr, NULL);
	elapsedTime = (t_curr.tv_sec - t_past.tv_sec) * 1000000; // to us
	elapsedTime += (t_curr.tv_usec - t_past.tv_usec);
	MSG_DBG(2, "------ Draw Bg Map dTime:%u uS------", elapsedTime - elapsedTime1);

	MU.Draw(map_move_corr, time_flg);

	Predator.Draw(map_move_corr, time_flg);

	GM.DrawMidMap();

	gettimeofday(&t_curr, NULL);
	elapsedTime1 = (t_curr.tv_sec - t_past.tv_sec) * 1000000; // to us
	elapsedTime1 += (t_curr.tv_usec - t_past.tv_usec);
	MSG_DBG(2, "------ Draw Mobs dTime:%u uS------", elapsedTime1 - elapsedTime);

	GM.DrawTopMap();

	for (i = 0; i < BUTRF_CNT; i++) {
	    ButterFlyM[i].Draw(map_move_corr, time_flg);
	}

	gettimeofday(&t_curr, NULL);
	elapsedTime = (t_curr.tv_sec - t_past.tv_sec) * 1000000; // to us
	elapsedTime += (t_curr.tv_usec - t_past.tv_usec);
	MSG_DBG(2, "------ Draw Bg Map dTime:%u uS------", elapsedTime - elapsedTime1);

	Dsp.ShowGo();

	gettimeofday(&t_curr, NULL);
	elapsedTime1 = (t_curr.tv_sec - t_past.tv_sec) * 1000000; // to us
	elapsedTime1 += (t_curr.tv_usec - t_past.tv_usec);
//		t_past.tv_sec = t_curr.tv_sec;
//		t_past.tv_usec = t_curr.tv_usec;
	MSG_DBG(2, "------ ShowGo dTime:%u uS------", elapsedTime1 - elapsedTime);
	MSG_DBG(2, "------ Full elapsedTime:%u mS,  Max theoretical FPS:%d------", elapsedTime1 / 1000,
		1000000 / elapsedTime1);
	if (elapsedTime1 < FRAME_DELAY_US) {
	    usleep( FRAME_DELAY_US - elapsedTime1);
	} else {
	    MSG_DBG(2, "+++++++++++++++++++++++++++++++++++++++ SLOW +++++++++++++++++++++++++");
	}

//		sleep(1);
    }

    return EXIT_SUCCESS;
}
