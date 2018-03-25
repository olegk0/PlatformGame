/*
 * Disp.cpp
 *
 *      Author: olegvedi@gmail.com
 */
#include <string>
#include <unistd.h>
#include <cerrno>

#include "Disp.h"

#include "priv.h"

void *ThreadHandle(void *data);

Disp::Disp(const char *bp_names[], unsigned width, unsigned height)
: MainSurface(NULL), /*BufSurface(NULL),*/ disp_width(width), disp_height(height)
{
	unsigned i;

	BufSurface[0] = NULL;
	BufSurface[1] = NULL;
	thread_run = false;
	for(i=0;i<sizeof(BackPics) / sizeof(BackPics[0]);i++){
		BackPics[i] = NULL;
	}

	frame_cnt = 0;
	disp_x_offs = 0;

	for(i=0;i<BG_CNTs;i++){
		bg_x_offs[i] = 0;
	}

    pthread_mutex_init(&th_mutex, NULL);
    pthread_cond_init(&th_cond, NULL);
    write_buf_num = 0;
    disp_buf_num = write_buf_num;

    if(SDL_Init(SDL_INIT_VIDEO))
    {
    	MSG_ERR("SDL_Init");
    	throw;
    }

    try{
    	MainSurface = SDL_SetVideoMode(width, height, 0, SDL_SWSURFACE);
    	if(!MainSurface){
    		MSG_ERR("SDL_SetVideoMode");
    		throw;
    	}

	    SDL_Surface *stemp  = SDL_CreateRGBSurface( 0 , width, height, 24, 0, 0, 0, 0);
		if (!stemp) {
			MSG_ERR("SDL_CreateRGBSurface 0");
			throw;
		}
		BufSurface[0] = SDL_DisplayFormat(stemp);
		SDL_FreeSurface(stemp);
		if (!BufSurface[0]) {
			MSG_ERR("BufSurface 0");
			throw;
		}

	    stemp  = SDL_CreateRGBSurface( 0 , width, height, 24, 0, 0, 0, 0);
		if (!stemp) {
			MSG_ERR("SDL_CreateRGBSurface 1");
			throw;
		}
		BufSurface[1] = SDL_DisplayFormat(stemp);
		SDL_FreeSurface(stemp);
		if (!BufSurface[1]) {
			MSG_ERR("BufSurface 1");
			throw;
		}

		int ck = SDL_MapRGB(MainSurface->format, 255, 0, 255);
		MSG_DBG(2, "Load bg pic :%s", bp_names[0]);
		stemp = SDL_LoadBMP(bp_names[0]);
		if (!stemp) {
			MSG_ERR("SDL_LoadBMP 0");
			throw;
		}
		BackPics[0] = SDL_DisplayFormat(stemp);
		//SDL_SetColorKey(BackPic, SDL_SRCCOLORKEY | SDL_RLEACCEL, ck);
		SDL_FreeSurface(stemp);

		MSG_DBG(2, "Load bg pic :%s", bp_names[1]);
		stemp = SDL_LoadBMP(bp_names[1]);
		if (!stemp) {
			MSG_ERR("SDL_LoadBMP 1");
			throw;
		}
		BackPics[1] = SDL_DisplayFormat(stemp);
		SDL_FreeSurface(stemp);
		SDL_SetColorKey(BackPics[1], SDL_SRCCOLORKEY | SDL_RLEACCEL, ck);

		thread_run = true;
		int ret = pthread_create(&main_thread, NULL, &ThreadHandle,	this);
		if (ret) {
			MSG_ERR("pthread_create");
			throw;
		}

	//	BufSurface = SDL_CreateRGBSurface( 0 , width, height, 24, 0, 0, 0, 0);
    }catch(...){
        Free();
        throw;
    }

    return;
}

Disp::~Disp()
{
	Free();
}

void Disp::Free()
{
	int i;

	MSG_DBG(2, "");
	thread_run = false;
	ShowPrepare();
	pthread_join(main_thread, NULL);
	MSG_DBG(2, "+++");

	if (MainSurface != NULL)
		SDL_FreeSurface(MainSurface);

	i = sizeof(BackPics) / sizeof(BackPics[0]);
	while(i >= 0){
		i--;
		if (BackPics[i] != NULL)
			SDL_FreeSurface(BackPics[i]);
	}

	if (BufSurface[0] != NULL)
		SDL_FreeSurface(BufSurface[0]);

	if (BufSurface[1] != NULL)
		SDL_FreeSurface(BufSurface[1]);

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

	SDL_BlitSurface(BackPics[idx], &SrcRect, BufSurface[write_buf_num], &DstRect);

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
		SDL_BlitSurface(BackPics[idx], &SrcRect, BufSurface[write_buf_num], &DstRect);
	}
}

void *ThreadHandle(void *data)
{
	Disp *Dsp = static_cast<Disp *>(data);

	while(Dsp->thread_run){
		pthread_mutex_lock(&Dsp->th_mutex);
		while (Dsp->thread_run && Dsp->disp_buf_num == Dsp->write_buf_num) {
			MSG_DBG(1, "WaitFrame");
			pthread_cond_wait(&Dsp->th_cond, &Dsp->th_mutex);
		}
		pthread_mutex_unlock(&Dsp->th_mutex);
//		MSG_DBG(1, "****************************Display show:%d",Dsp->disp_buf_num);
		SDL_BlitSurface(Dsp->BufSurface[Dsp->disp_buf_num], NULL, Dsp->MainSurface, NULL);
		//SDL_Flip(Dsp->MainSurface);
		SDL_UpdateRect(Dsp->MainSurface, 0, 0, 0, 0);
		pthread_mutex_lock(&Dsp->th_mutex);
		Dsp->disp_buf_num = Dsp->write_buf_num;
		pthread_cond_signal(&Dsp->th_cond);
		pthread_mutex_unlock(&Dsp->th_mutex);

	}

	return NULL;
}

void Disp::ShowPrepare()
{

//	MSG_DBG(1, "+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
//	SDL_FillRect(BufSurface, NULL, 0x000000);
//	SDL_LockSurface(BufSurface);
	pthread_mutex_lock(&th_mutex);
	while (disp_buf_num != write_buf_num) {
		MSG_DBG(1, "WaitFrame");
		struct timespec ts;
		clock_gettime(CLOCK_REALTIME, &ts);
		ts.tv_sec += 2;
		int p_ret = pthread_cond_timedwait(&th_cond, &th_mutex, &ts);
		if(p_ret == ETIMEDOUT){
			MSG_ERR("Wait timeout");
			break;
		}
	}
	pthread_mutex_unlock(&th_mutex);

	pthread_mutex_lock(&th_mutex);
	if(write_buf_num){
		write_buf_num = 0;
		disp_buf_num = 1;
	}else{
		write_buf_num = 1;
		disp_buf_num = 0;
	}
	pthread_cond_signal(&th_cond);
	pthread_mutex_unlock(&th_mutex);

//	MSG_DBG(1, "******************************Paint Bg to :%d", write_buf_num);

	BlitBg(0, false);
	BlitBg(1, true);
}

void Disp::ShowGo()
{
//	SDL_UnlockSurface(BufSurface);

//	SDL_Flip(MainSurface);
	frame_cnt++;

}

int Disp::BlitSurface(SDL_Surface *srf, SDL_Rect *sprite_rect, int x, int y)
{
	int ret = -EINVAL;

//	MSG_DBG(2, "x:%d  offs:%d  w:%d", x, disp_x_offs, sprite_rect->w);
	x += disp_x_offs;
	if(x + sprite_rect->w >= 0 && x < static_cast<int>(disp_width))	{
		SDL_Rect DstRect;
/* oversize check - not need for SDL surface
		SDL_Rect SrcRect;

		if(x < 0){
			DstRect.x = 0;
			SrcRect.x = sprite_rect->x - x;
			SrcRect.w = sprite_rect->w - (- x);
		}else{
			DstRect.x = x;
			SrcRect.x = sprite_rect->x;
			int os = (x + sprite_rect->w) - disp_width;
			if( os > 0){
				SrcRect.w = sprite_rect->w - os;
			}else{
				SrcRect.w = sprite_rect->w;
			}
		}

		SrcRect.y = sprite_rect->y;
		SrcRect.h = sprite_rect->h;

		DstRect.y = y;
		DstRect.h = SrcRect.h;
		DstRect.w = SrcRect.w;

		ret = SDL_BlitSurface(srf, &SrcRect, MainSurface, &DstRect);
		*/

        DstRect.x = x;
        DstRect.y = y;
        DstRect.h = sprite_rect->h;
        DstRect.w = sprite_rect->w;

//        MSG_DBG(1, "************************Paint Obj to :%d", write_buf_num);
		ret = SDL_BlitSurface(srf, sprite_rect, BufSurface[write_buf_num], &DstRect);
	}

	return ret;
}

