/*
 * Disp.cpp
 *
 *      Author: olegvedi@gmail.com
 */
#include <string>
#include "Disp.h"

#include "priv.h"

Disp::Disp(const char *bp_names[], unsigned width, unsigned height)
: MainSurface(NULL), /*BufSurface(NULL),*/ disp_width(width), disp_height(height)
{
	unsigned i;
	for(i=0;i<sizeof(BackPics) / sizeof(BackPics[0]);i++){
		BackPics[i] = NULL;
	}

	frame_cnt = 0;
	disp_x_offs = 0;

	for(i=0;i<BG_CNTs;i++){
		bg_x_offs[i] = 0;
	}

    if(SDL_Init(SDL_INIT_VIDEO))
    {
    	MSG_ERR("SDL_Init");
    }else{
	    MainSurface = SDL_SetVideoMode(width, height, 0, SDL_SWSURFACE);
	    if(!MainSurface){
	    	MSG_ERR("SDL_SetVideoMode");
	    }else{
	    	int ck = SDL_MapRGB(MainSurface->format, 255, 0, 255);
	    	SDL_Surface *stemp = SDL_LoadBMP(bp_names[0]);
	    	if(!stemp){
	    		MSG_ERR("SDL_LoadBMP 0");
	    	}else{
			    BackPics[0] = SDL_DisplayFormat(stemp);
				//SDL_SetColorKey(BackPic, SDL_SRCCOLORKEY | SDL_RLEACCEL, ck);
				SDL_FreeSurface(stemp);
	    	}
	    	stemp = SDL_LoadBMP(bp_names[1]);
	    	if(!stemp){
	    		MSG_ERR("SDL_LoadBMP 1");
	    	}else{
	    		BackPics[1] = SDL_DisplayFormat(stemp);
	    		SDL_SetColorKey(BackPics[1], SDL_SRCCOLORKEY | SDL_RLEACCEL, ck);
	    		SDL_FreeSurface(stemp);
	    		return; // OK
	    	}
	//	BufSurface = SDL_CreateRGBSurface( 0 , width, height, 24, 0, 0, 0, 0);
	    }
    }
    Free();
    throw;
}

Disp::~Disp()
{
	Free();
}

void Disp::Free()
{
	int i;

	if (MainSurface != NULL)
		SDL_FreeSurface(MainSurface);

	i = sizeof(BackPics) / sizeof(BackPics[0]);
	while(i >= 0){
		i--;
		if (BackPics[i] != NULL)
			SDL_FreeSurface(BackPics[i]);
	}
/*
	if (BufSurface != NULL)
			SDL_FreeSurface(BufSurface);
			*/
	SDL_Quit();
}

void Disp::SetDispOffset(int offs)
{
	unsigned i;

	if(disp_x_offs != offs){
		for(i=0;i<BG_CNTs;i++){
			bg_x_offs[i] += i+1;
			if(	bg_x_offs[i] >= disp_width)
				bg_x_offs[i] = 0;
		}

		disp_x_offs = offs;
	}
}

void Disp::BlitBg(unsigned idx, bool BottomAlign)
{
	SDL_Rect DstRect, SrcRect;

	DstRect.h = BackPics[idx]->h;
	DstRect.w = BackPics[idx]->w - bg_x_offs[idx];
	DstRect.x = 0;
	if(BottomAlign)
		DstRect.y = disp_height - BackPics[idx]->h;
	else
		DstRect.y = 0;

	SrcRect.h = BackPics[idx]->h;
	SrcRect.w = BackPics[idx]->w - bg_x_offs[idx];
	SrcRect.x = bg_x_offs[idx];
	SrcRect.y = 0;

	SDL_BlitSurface(BackPics[idx], &SrcRect, MainSurface, &DstRect);

	if(bg_x_offs[idx] > 0){
//	DstRect.h = BackPics[idx]->h;
		DstRect.x = DstRect.w;
		DstRect.w = bg_x_offs[idx];
//	DstRect.x = BackPics[idx]->w - bg_x_offs;
//	DstRect.y = disp_height - BackPics[idx]->h;

//	SrcRect.h = BackPics[idx]->h;
		SrcRect.w = bg_x_offs[idx];
		SrcRect.x = 0;
//	SrcRect.y = 0;
		SDL_BlitSurface(BackPics[idx], &SrcRect, MainSurface, &DstRect);
	}
}

void Disp::ShowPrepare() //TODO build background in thread
{

//	SDL_FillRect(BufSurface, NULL, 0x000000);
//	SDL_LockSurface(BufSurface);
	BlitBg(0, false);
	BlitBg(1, true);
}

void Disp::ShowGo()
{
//	SDL_UnlockSurface(BufSurface);

//	SDL_BlitSurface(BackPic, NULL, MainSurface, NULL);
//	SDL_BlitSurface(BufSurface, NULL, MainSurface, NULL);
	SDL_Flip(MainSurface);
	frame_cnt++;

}

int Disp::BlitSurface(SDL_Surface *srf, SDL_Rect *sprite_rect, unsigned x, unsigned y)
{
	int ret =0;
	SDL_Rect DstRect;
//TODO check	disp_width, disp_height

//	MSG_DBG(3, "%d %d %d %d", sprite_rect->h, sprite_rect->w, sprite_rect->x, sprite_rect->y );

	DstRect.x = x + disp_x_offs;
	DstRect.y = y;
	DstRect.h = sprite_rect->h;
	DstRect.w = sprite_rect->w;
	SDL_BlitSurface(srf, sprite_rect, MainSurface, &DstRect);
//	SDL_BlitSurface(srf, NULL, MainSurface, NULL);
/*
	unsigned char *dst = (unsigned char*)BufSurface->pixels;
	unsigned i, dw = disp_width * 4;
	img_width  = img_width * 4;
	dst += x * 4 + y * dw;
	for(i=0;i<img_heigth;i++){
		memcpy(dst, srf, img_width);
		dst += dw;
		srf += img_width;
	}
*/
	return ret;
}

