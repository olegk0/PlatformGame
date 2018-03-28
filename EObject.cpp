/*
 * EObject.cpp
 *
 *      Author: olegvedi@gmail.com
 */

#include <cstdio>
#include <unistd.h>
#include <string>

#include "EObject.h"
#include "GMap.h"

#include "priv.h"

using namespace std;

Disp *EObject::Dsp = NULL;

void EObject::main_init()
{
    unsigned i;

    if (Dsp == NULL) {
	MSG_ERR("Disp is not initialized!");
	throw 1;
    }

    for (i = 0; i < MAX_SPRITE_SHEETS; i++) {
	Sprites[i].SpriteImg = NULL;
	Sprites[i].curr_img = 0;
    }
    active_sprite_sheet = Sprite_Front_Sheet;
    frame_delay = DEF_FRAME_DELAY;
}

EObject::EObject()
{
    main_init();
}

EObject::EObject(const EObject * Src_EObj)
{
    main_init();
    CopyEObject(Src_EObj);
}

EObject::EObject(const EObject& Src_EObj)
{
    main_init();
    CopyEObject(&Src_EObj);
}

EObject::~EObject()
{
    Free();
}

void EObject::Free()
{
    unsigned i;

    for (i = 0; i < MAX_SPRITE_SHEETS; i++) {
	PurgeSpriteSheet(static_cast<sprites_sheet_num_t>(i));
    }
}

void EObject::PurgeSpriteSheet(sprites_sheet_num_t sprite_num)
{
    if (Sprites[sprite_num].SpriteImg != NULL) {
	Sprites[sprite_num].SpriteImg->PurgeSprite();
	Sprites[sprite_num].SpriteImg = NULL;
	Sprites[sprite_num].curr_img = 0;
    }
}

int EObject::CopyEObject(const EObject *Src_EObj)
{
    int ret = -EINVAL, i;

    if (Src_EObj != NULL) {
	for (i = 0; i < MAX_SPRITE_SHEETS; i++) {
	    if (Src_EObj->Sprites[i].SpriteImg != NULL) {
		AssignSpriteSheet(static_cast<sprites_sheet_num_t>(i), Src_EObj->Sprites[i].SpriteImg);
	    } else {
		PurgeSpriteSheet(static_cast<sprites_sheet_num_t>(i));
	    }
	}
	active_sprite_sheet = Src_EObj->active_sprite_sheet;
	frame_delay = Src_EObj->frame_delay;
	ret = 0;
    }

    return ret;
}

void EObject::InitSpriteSheet(sprites_sheet_num_t sheet_num)
{
    if (Sprites[sheet_num].SpriteImg == NULL)
	Sprites[sheet_num].SpriteImg = Sprite::TakeSprite(); //create new
    Sprites[sheet_num].curr_img = 0;
    Sprites[sheet_num].delay = DEF_FRAME_DELAY;
//	Sprites[sheet_num].OneShoot = false;
}

int EObject::LoadSpriteSheet(sprites_sheet_num_t sheet_num, string &img_path, unsigned sprite_width,
	unsigned min_loop_idx, bool AutoMirror)
{
    if (sheet_num >= MAX_SPRITE_SHEETS)
	return -EINVAL;
    int ret = -ENOENT;
    string fname = img_path;

    switch (sheet_num) {
    case Sprite_Front_Sheet_v1:
	fname += "/front_v1.bmp";
	break;
    case Sprite_Front_Sheet:
	fname += "/front.bmp";
	break;
    case Sprite_Down_Sheet:
	fname += "/down.bmp";
	break;
    case Sprite_Right_Sheet:
	fname += "/right.bmp";
	break;
    case Sprite_Left_Sheet:
	fname += "/left.bmp";
	break;
    case Sprite_Jump_Right_Sheet:
	fname += "/jump_right.bmp";
	break;
    case Sprite_Jump_Left_Sheet:
	fname += "/jump_left.bmp";
	break;
    case Sprite_Atack_Right_Sheet:
	fname += "/atack_right.bmp";
	break;
    case Sprite_Atack_Left_Sheet:
	fname += "/atack_left.bmp";
	break;

    default: {
    }
    }

//	MSG_DBG(3, "file :%s", img_path.c_str());
    if (access(fname.c_str(), F_OK) != -1) {
	InitSpriteSheet(sheet_num);
	unsigned t1;
	ret = Sprites[sheet_num].SpriteImg->LoadSpriteSheet(Dsp->GetScreenSurface(), fname.c_str(), sprite_width,
		min_loop_idx);
	Sprites[sheet_num].SpriteImg->GetImgPrm(&t1, &Sprites[sheet_num].img_cnt, &Sprites[sheet_num].width,
		&Sprites[sheet_num].heigth);

	MSG_DBG(3, "LoadSpriteSheet:%s ret:%d", fname.c_str(), ret);
	if (ret <= 0) {
	    MSG_ERR("LoadSpriteSheet");
	    PurgeSpriteSheet(sheet_num);
	} else {
	    if (AutoMirror) {
		sprites_sheet_num_t mirror_sprite = Sprite_None_Sheet;
		switch (sheet_num) {
		case Sprite_Right_Sheet:
		    mirror_sprite = Sprite_Left_Sheet;
		    break;
		case Sprite_Left_Sheet:
		    mirror_sprite = Sprite_Right_Sheet;
		    break;
		case Sprite_Jump_Right_Sheet:
		    mirror_sprite = Sprite_Jump_Left_Sheet;
		    break;
		case Sprite_Jump_Left_Sheet:
		    mirror_sprite = Sprite_Jump_Right_Sheet;
		    break;
		case Sprite_Atack_Right_Sheet:
		    mirror_sprite = Sprite_Atack_Left_Sheet;
		    break;
		case Sprite_Atack_Left_Sheet:
		    mirror_sprite = Sprite_Atack_Right_Sheet;
		    break;
		default: {
		}
		}

		if (mirror_sprite != Sprite_None_Sheet) {
		    InitSpriteSheet(mirror_sprite);
		    ret = Sprites[mirror_sprite].SpriteImg->CopySpriteSheet(Sprites[sheet_num].SpriteImg, true);
		    Sprites[mirror_sprite].SpriteImg->GetImgPrm(&t1, &Sprites[mirror_sprite].img_cnt,
			    &Sprites[mirror_sprite].width, &Sprites[mirror_sprite].heigth);
		}
	    }
	}
    } else {
	MSG_ERR("Load :%s", fname.c_str());
    }

    if (active_sprite_sheet > sheet_num)
	active_sprite_sheet = sheet_num;
    return ret;
}

int EObject::AssignSpriteSheet(sprites_sheet_num_t sheet_num, Sprite *PS)
{
    if (sheet_num >= MAX_SPRITE_SHEETS || PS == NULL)
	return -EINVAL;

    PurgeSpriteSheet(sheet_num); //flush old sprite

    Sprites[sheet_num].SpriteImg = Sprite::TakeSprite(PS); // take ref link
    InitSpriteSheet(sheet_num);
    unsigned min_loop;
    Sprites[sheet_num].SpriteImg->GetImgPrm(&min_loop, &Sprites[sheet_num].img_cnt, &Sprites[sheet_num].width,
	    &Sprites[sheet_num].heigth);

    if (active_sprite_sheet > sheet_num)
	active_sprite_sheet = sheet_num;

    return 0;
}
/*
 int EObject::ShowActiveSpriteAlignBottom(unsigned x, unsigned blk_y, int show_mode)
 {
 if(Sprites[active_sprite].SpriteImg == NULL){
 MSG_ERR("Pointer");
 return -EBADF;
 }
 unsigned y = Y_TO_DISP_COORD_EXT(blk_y + 1);
 y -= Sprites[active_sprite].heigth;
 return ShowActiveSprite( x, y, show_mode);
 }
 */

int EObject::ShowSpriteSheet(sprites_sheet_num_t sheet_num, unsigned x, unsigned y, int show_mode, bool FromTopCoord,
	bool DecDelay)
{
    int ret = 0;
    bool OneShoot = false;
    SDL_Rect *sprite_rect;

    switch (show_mode) {
    case SPRITE_ANI_LOOP:
//		Sprites[active_sprite_sheet].curr_img = 0;
	break;
    case SPRITE_ANI_NO_LOOP:
	OneShoot = true;
//		Sprites[active_sprite_sheet].curr_img = 0;
	break;
    default: // get img by index
    {
	Sprites[sheet_num].curr_img = show_mode;
    }
    }

    SDL_Surface *srf = Sprites[sheet_num].SpriteImg->GetImg(&Sprites[sheet_num].curr_img, &sprite_rect, OneShoot);

    if (frame_delay > 0) {
	if (DecDelay)
	    frame_delay--;
    } else {
	frame_delay = Sprites[sheet_num].delay;
	Sprites[sheet_num].curr_img++;
    }

//    MSG_DBG(1, "curr_img %d frame_delay:%d  delay:%d", Sprites[sheet_num].curr_img,frame_delay, Sprites[sheet_num].delay);
//	MSG_DBG(3, "%p", srf);

    if (srf == NULL)
	return -EFAULT;
    if (FromTopCoord)
	ret = Dsp->BlitSurface(srf, sprite_rect, x, y);
    else
	ret = Dsp->BlitSurface(srf, sprite_rect, x, y - Sprites[sheet_num].heigth);

//    if (ret == 0)
//    ret = Sprites[sheet_num].curr_img;
    return ret;
}

int EObject::ShowActiveSpriteSheet(unsigned x, unsigned y, int show_mode, bool FromTopCoord,
	bool DecDelay)
{
    if (Sprites[active_sprite_sheet].SpriteImg == NULL) {
	MSG_ERR("Pointer");
	return -EBADF;
    }
//	MSG_DBG(2, "***** sprite:%d  ", Sprites[active_sprite].curr_img);
    return ShowSpriteSheet(active_sprite_sheet, x, y, show_mode, FromTopCoord, DecDelay);

}

