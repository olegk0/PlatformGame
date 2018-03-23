/*
 * Sprite.cpp
 *
 *      Author: olegvedi@gmail.com
 */

#include "Sprite.h"

#include <cstdio>
#include <string.h>
#include <string>
#include "priv.h"

using namespace std;

Sprite::Sprite()
:sprites_cnt(0), sprite_data(NULL), refcnt(1), img_width(0), img_height(0)
{

}

Sprite::~Sprite() {
	Free();
}

void Sprite::Free() {
	if(sprite_data != NULL){
		SDL_FreeSurface(sprite_data);
		sprite_data = NULL;
	}
	sprites_cnt = 0;
}

Sprite *Sprite::TakeSprite(Sprite *RefSprite)
{
	if(RefSprite != NULL){
		RefSprite->refcnt++;
	}else{
		RefSprite = new(std::nothrow) Sprite();
	}

	return RefSprite;
}

int Sprite::LoadSpriteSheet(SDL_Surface *screen, const char *img_file, unsigned sprite_width, unsigned min_idx) {
	int ret = -101;
	unsigned tt;
	SDL_Surface *stemp=NULL;
	int ck;

	Free();

	stemp = SDL_LoadBMP(img_file);
	if (stemp != NULL) {
		sprite_data = SDL_DisplayFormat(stemp);
		ck = SDL_MapRGB(screen->format, 255, 0, 255);
		SDL_SetColorKey(sprite_data, SDL_SRCCOLORKEY | SDL_RLEACCEL, ck);

		sprites_cnt = stemp->w / sprite_width;
		MSG_DBG(2, ":%s img_w:%d img_h:%d sprites_cnt:%d min_idx:%d",img_file,stemp->w, stemp->h,sprites_cnt,min_idx);

		img_height = stemp->h;
		img_width = sprite_width;

		SDL_FreeSurface(stemp);

		if(sprites_cnt > MAX_SPRITE_IMAGES)
			sprites_cnt = MAX_SPRITE_IMAGES;

		min_loop_idx = min_idx;
		if(min_loop_idx >= sprites_cnt)
			min_loop_idx = sprites_cnt -1;

		unsigned offs = 0;
		for(tt=0;tt<sprites_cnt;tt++){
			sprite_offs[tt].h = img_height;
			sprite_offs[tt].w = img_width;
			sprite_offs[tt].x = offs;
			sprite_offs[tt].y = 0;
			offs += img_width;
		}
		ret = sprites_cnt;
	}else{
		MSG_ERR("load image from: %s",img_file);
	}

	return ret;
}

int Sprite::CopySpriteSheet(Sprite *src_sprite, bool hor_mirror)
{
	int ret = -101;
	unsigned tt;
	SDL_Surface *stemp;

	Free();

	stemp = src_sprite->GetSurface();
	if (stemp != NULL) {
		sprite_data = SDL_DisplayFormat(stemp);

		src_sprite->GetImgPrm(&min_loop_idx ,&sprites_cnt, &img_width, &img_height);

		if(sprites_cnt > MAX_SPRITE_IMAGES)
			sprites_cnt = MAX_SPRITE_IMAGES;

		SDL_LockSurface(sprite_data);
		unsigned offs = 0;
		for(tt=0;tt<sprites_cnt;tt++){
			sprite_offs[tt].h = img_height;
			sprite_offs[tt].w = img_width;
			sprite_offs[tt].x = offs;
			sprite_offs[tt].y = 0;

			if (hor_mirror) {
				unsigned x, y;
				unsigned char *pbdst = (unsigned char *)sprite_data->pixels	+ sprite_data->format->BytesPerPixel * offs;
				unsigned char *pbsrc = pbdst + sprite_data->format->BytesPerPixel * (img_width - 1);
				for (y = 0; y < img_height; y++) {
					unsigned char *pdst = pbdst;
					unsigned char *psrc = pbsrc;
					for (x = 0;x<img_width / 2; x++) {
						for (int c = 0; c < sprite_data->format->BytesPerPixel;	c++) {
							unsigned char t = pdst[c];
							pdst[c] = psrc[c];
							psrc[c] = t;
						}
						pdst += sprite_data->format->BytesPerPixel;
						psrc -= sprite_data->format->BytesPerPixel;
					}
					pbdst += sprite_data->pitch;
					pbsrc += sprite_data->pitch;
				}
			}

			offs += img_width;
		}
		SDL_UnlockSurface(sprite_data);
		ret = sprites_cnt;
	}else{
		MSG_ERR("GetSurface");
	}

	return ret;

}

SDL_Surface *Sprite::GetImg(unsigned *ImgNum, SDL_Rect **ImgRec, bool OneShoot)
{
	if(sprites_cnt > 0){
		if(/**ImgNum >= MAX_SPRITE_IMAGES ||*/ *ImgNum >= sprites_cnt){
			if(OneShoot)
				*ImgNum = sprites_cnt - 1;
			else
				*ImgNum = min_loop_idx;
		}
	}else
		*ImgNum = 0;

//	MSG_DBG(2, "min_loop_idx:%d   %d ", min_loop_idx, *ImgNum);
	*ImgRec = &sprite_offs[*ImgNum];
	return sprite_data;
}
