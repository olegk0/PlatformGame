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

EObject::EObject(Disp *Dis)
:Dsp(Dis), active_sprite(0)
{
	unsigned i;

	for(i=0;i<MAX_SPRITES;i++){
		Sprites[i].SpriteImg = NULL;
		Sprites[i].curr_img = 0;
	}
	frame_delay = DEF_FRAME_DELAY;
}

EObject::~EObject() {
	Free();
}

void EObject::Free()
{
	unsigned i;

	for(i=0;i<MAX_SPRITES;i++){
		if(Sprites[i].SpriteImg != NULL){
			Sprites[i].SpriteImg->PurgeSprite();
			Sprites[i].SpriteImg = NULL;
			Sprites[i].curr_img = 0;
		}
	}
}

EObject *EObject::GetCopyEObject()
{
	int i;
	EObject *EObj = new/*(std::nothrow)*/ EObject(Dsp);
	if(EObj){
		for(i=0;i<MAX_SPRITES;i++){
			if(Sprites[i].SpriteImg != NULL){
				EObj->AssignSprite(static_cast<sprite_num_t>(i), Sprites[i].SpriteImg);
			}
		}
	}

	return EObj;
}

void EObject::InitSprite(sprite_num_t sprite_num)
{
	if (Sprites[sprite_num].SpriteImg == NULL)
		Sprites[sprite_num].SpriteImg = Sprite::TakeSprite();//create new
	Sprites[sprite_num].curr_img = 0;
	Sprites[sprite_num].delay = DEF_FRAME_DELAY;
//	Sprites[sprite_num].OneShoot = false;
}

int EObject::LoadSprite(sprite_num_t sprite_num, string &img_path, unsigned sprite_width, unsigned min_loop_idx, bool AutoMirror)
{
	if (sprite_num >= MAX_SPRITES)
		return -EINVAL;
	int ret = -ENOENT;
	string fname=img_path;

	switch(sprite_num){
	case Sprite_Front_Img_v1:
		fname += "/front_v1.bmp";
		break;
	case Sprite_Front_Img:
		fname += "/front.bmp";
		break;
	case Sprite_Down_Img:
		fname += "/down.bmp";
		break;
	case Sprite_Right_Img:
		fname += "/right.bmp";
		break;
	case Sprite_Left_Img:
		fname += "/left.bmp";
		break;
	case Sprite_Jump_Right_Img:
		fname += "/jump_right.bmp";
		break;
	case Sprite_Jump_Left_Img:
		fname += "/jump_left.bmp";
		break;
	case Sprite_Atack_Right_Img:
		fname += "/atack_right.bmp";
		break;
	case Sprite_Atack_Left_Img:
		fname += "/atack_left.bmp";
		break;

	default:{}
	}

//	MSG_DBG(3, "file :%s", img_path.c_str());
	if( access( fname.c_str(), F_OK ) != -1 ) {
		InitSprite(sprite_num );
		unsigned t1;
		ret = Sprites[sprite_num].SpriteImg->LoadSpriteSheet(Dsp->GetScreenSurface() ,fname.c_str(), sprite_width, min_loop_idx);
		Sprites[sprite_num].SpriteImg->GetImgPrm(&t1, &Sprites[sprite_num].img_cnt, &Sprites[sprite_num].width, &Sprites[sprite_num].heigth);

		MSG_DBG(3, "LoadSpriteSheet:%s ret:%d", fname.c_str(), ret);
		if (ret <= 0) {
			MSG_ERR("LoadSpriteSheet");
			Sprites[sprite_num].SpriteImg->PurgeSprite();
			Sprites[sprite_num].SpriteImg = NULL;
		} else {
			if (AutoMirror) {
				sprite_num_t mirror_sprite = Sprite_None_Img;
				switch (sprite_num) {
				case Sprite_Right_Img:
					mirror_sprite = Sprite_Left_Img;
					break;
				case Sprite_Left_Img:
					mirror_sprite = Sprite_Right_Img;
					break;
				case Sprite_Jump_Right_Img:
					mirror_sprite = Sprite_Jump_Left_Img;
					break;
				case Sprite_Jump_Left_Img:
					mirror_sprite = Sprite_Jump_Right_Img;
					break;
				case Sprite_Atack_Right_Img:
					mirror_sprite = Sprite_Atack_Left_Img;
					break;
				case Sprite_Atack_Left_Img:
					mirror_sprite = Sprite_Atack_Right_Img;
					break;
				default: {
				}
				}

				if (mirror_sprite != Sprite_None_Img) {
					InitSprite(mirror_sprite);
					ret = Sprites[mirror_sprite].SpriteImg->CopySpriteSheet(Sprites[sprite_num].SpriteImg, true);
					Sprites[mirror_sprite].SpriteImg->GetImgPrm(&t1, &Sprites[mirror_sprite].img_cnt, &Sprites[mirror_sprite].width, &Sprites[mirror_sprite].heigth);
				}
			}
		}
	}else{
		MSG_ERR("Load :%s", fname.c_str());
	}

	return ret;
}

int EObject::AssignSprite(sprite_num_t sprite_num, Sprite *PS)
{
	if(sprite_num >= MAX_SPRITES || PS == NULL) return -EINVAL;

	if(Sprites[sprite_num].SpriteImg != NULL)
		Sprites[sprite_num].SpriteImg->PurgeSprite();//flush old sprite

	Sprites[sprite_num].SpriteImg = Sprite::TakeSprite(PS); // take ref link
	InitSprite(sprite_num);
	unsigned min_loop;
	Sprites[sprite_num].SpriteImg->GetImgPrm(&min_loop, &Sprites[sprite_num].img_cnt,
			&Sprites[sprite_num].width, &Sprites[sprite_num].heigth);

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
int EObject::ShowActiveSprite(unsigned x, unsigned y, int show_mode, bool FromTopCoord)
{
	if(Sprites[active_sprite].SpriteImg == NULL){
		MSG_ERR("Pointer");
		return -EBADF;
	}
//	MSG_DBG(2, "***** sprite:%d  ", Sprites[active_sprite].curr_img);

	int ret=0;
	bool OneShoot=false;
	SDL_Rect *sprite_rect;

	switch(show_mode){
	case SPRITE_ANI_LOOP:
//		Sprites[active_sprite].curr_img = 0;
		break;
	case SPRITE_ANI_NO_LOOP:
		OneShoot = true;
//		Sprites[active_sprite].curr_img = 0;
		break;
	default:// get img by index
	{
		Sprites[active_sprite].curr_img = show_mode;
	}
	}

	SDL_Surface *srf = Sprites[active_sprite].SpriteImg->GetImg(&Sprites[active_sprite].curr_img, &sprite_rect, OneShoot);
	if(frame_delay > 0){
		frame_delay--;
	}else{
		frame_delay = Sprites[active_sprite].delay;
		Sprites[active_sprite].curr_img++;
	}

//	MSG_DBG(3, "%p", srf);

	if(srf == NULL) return -EFAULT;
	if(FromTopCoord)
		ret = Dsp->BlitSurface(srf, sprite_rect, x, y);
	else
		ret = Dsp->BlitSurface(srf, sprite_rect, x, y - Sprites[active_sprite].heigth);

	if(ret == 0)
		ret = Sprites[active_sprite].curr_img;
	return ret;
}

