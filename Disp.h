/*
 * Disp.h
 *
 *      Author: olegvedi@gmail.com
 */

#ifndef DISP_H_
#define DISP_H_

#include <SDL/SDL.h>

#define VID_BUF_CNTs 2
#define BG_CNTs 2

class Disp {
public:
	Disp(const char *bp_names[], unsigned width, unsigned height);
	virtual ~Disp();

	int BlitSurface(SDL_Surface *srf, SDL_Rect *sprite_rect, int x, int y);
	void ShowPrepare();
	void ShowGo();
	SDL_Surface *GetScreenSurface(){return MainSurface;};
	void SetDispOffset(int offs);
	void Free();
private:
	Disp( const Disp&);
	Disp& operator=( Disp& );

	SDL_Surface *MainSurface;
	SDL_Surface *BufSurface[VID_BUF_CNTs];
	SDL_Surface *BackPics[BG_CNTs];

	unsigned bg_x_offs[BG_CNTs];
	unsigned disp_width;
	unsigned disp_height;
	unsigned frame_cnt;
	int disp_x_offs;
	pthread_t main_thread;
    pthread_mutex_t th_mutex;
    pthread_cond_t th_cond;
    bool thread_run;
    unsigned write_buf_num, disp_buf_num;

	friend void *ThreadHandle(void *data);

	void BlitBg(unsigned idx, bool BottomAlign);

};

#endif /* DISP_H_ */
