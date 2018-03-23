/*
 * Disp.h
 *
 *      Author: olegvedi@gmail.com
 */

#ifndef DISP_H_
#define DISP_H_

#include <SDL/SDL.h>

#define BG_CNTs 2

class Disp {
public:
	Disp(const char *bp_names[], unsigned width, unsigned height);
	virtual ~Disp();

	int BlitSurface(SDL_Surface *srf, SDL_Rect *sprite_rect, unsigned x, unsigned y);
	void ShowPrepare();
	void ShowGo();
	SDL_Surface *GetScreenSurface(){return MainSurface;};
	void SetDispOffset(int offs);
	void Free();
private:
	SDL_Surface *MainSurface;
//	SDL_Surface *BufSurface;
	SDL_Surface *BackPics[BG_CNTs];

	unsigned bg_x_offs[BG_CNTs];
	unsigned disp_width;
	unsigned disp_height;
	unsigned frame_cnt;
	int disp_x_offs;

	void BlitBg(unsigned idx, bool BottomAlign);

};

#endif /* DISP_H_ */
