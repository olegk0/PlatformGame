/*
 * Sprite.h
 *
 *      Author: olegvedi@gmail.com
 */

#ifndef SPRITE_H_
#define SPRITE_H_

#include <SDL/SDL.h>

#define MAX_SPRITE_IMAGES 20
#define SPRITE_DEF_SIZE 100
#define SPRITE_NARROW_SIZE 30

class Sprite {
public:
	static Sprite *TakeSprite(Sprite *RefSprite=NULL);
	bool PurgeSprite() {refcnt--; if(refcnt <= 0){ delete this; return true;} return false;};
	int LoadSpriteSheet(SDL_Surface *screen, const char *img_file, unsigned sprite_width, unsigned min_loop_idx);
	int CopySpriteSheet(Sprite *src_sprite, bool hor_mirror);
	void Free();
	SDL_Surface *GetImg(unsigned *ImgNum, SDL_Rect **ImgRec, bool OneShoot);
	void GetImgPrm(unsigned *min_idx ,unsigned *cnt, unsigned *width, unsigned *height) {
		*min_idx = min_loop_idx;
		*cnt = sprites_cnt;
		*width=img_width;
		*height=img_height;
	};
	SDL_Surface *GetSurface(){return sprite_data;};
private:
	Sprite();
	virtual ~Sprite();
	unsigned sprites_cnt;
	SDL_Surface *sprite_data;
	SDL_Rect sprite_offs[MAX_SPRITE_IMAGES];
	int refcnt;
	unsigned img_width;
	unsigned img_height;
	unsigned min_loop_idx;

//	void AddBlk(unsigned img_size);

};

#endif /* SPRITE_H_ */
