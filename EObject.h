/*
 * EObject.h
 *
 *      Author: olegvedi@gmail.com
 */

#ifndef EOBJECT_H_
#define EOBJECT_H_

#include "Sprite.h"
#include "Disp.h"
#include <SDL/SDL.h>
#include <string>
#include <cerrno>

using namespace std;

#define SPRITE_ANI_LOOP -1
#define SPRITE_ANI_NO_LOOP -2
#define DEF_FRAME_DELAY 5

class EObject {
public:
	typedef enum{
		Sprite_Front_Img=0,
		Sprite_Front_Img_v1,
		Sprite_Down_Img,
		Sprite_Right_Img,
		Sprite_Left_Img,
		Sprite_Right_Img_v1,
		Sprite_Left_Img_v1,
		Sprite_Jump_Right_Img,
		Sprite_Jump_Left_Img,
		Sprite_Atack_Right_Img,
		Sprite_Atack_Left_Img,
		Sprite_None_Img,
	} sprite_num_t;

	#define MAX_SPRITES (Sprite_None_Img + 2)

private:
    typedef struct {
//    	bool OneShoot;
    	unsigned delay;
    	unsigned curr_img;
    	unsigned img_cnt;
    	unsigned width;
    	unsigned heigth;
    	Sprite *SpriteImg;
    } sprite_img_t;

public:
	EObject(Disp *Window);
	virtual ~EObject();
	EObject *GetCopyEObject();
	void Free();
	int LoadSprite(sprite_num_t sprite_num, string &img_path, unsigned sprite_width, unsigned min_loop_idx, bool AutoMirror=false);
	int AssignSprite(sprite_num_t sprite_num, Sprite *PS);
	Sprite *GetSprite(sprite_num_t sprite_num)
	{
		if(sprite_num >= MAX_SPRITES) return NULL;

		return Sprites[sprite_num].SpriteImg;
	}
	int ShowActiveSprite(unsigned x, unsigned y, int show_mode, bool FromTopCoord=false);
//	int ShowActiveSpriteAlignBottom(unsigned x, unsigned blk_y, int show_mode);
	int SetActiveSprite(sprite_num_t sprite_num, unsigned frame_del=DEF_FRAME_DELAY)
	{
		if(sprite_num >= MAX_SPRITES) return -EINVAL;
		if(Sprites[sprite_num].SpriteImg == NULL) return -EBADF;
		active_sprite = sprite_num;
		if(frame_del == 0)
			Sprites[sprite_num].delay = DEF_FRAME_DELAY;
		else
			Sprites[sprite_num].delay = frame_del;
		Sprites[sprite_num].curr_img = 0;
		return 0;
	}
	void SetActiveSpriteBgIdx(unsigned sprite_idx) {Sprites[active_sprite].curr_img = sprite_idx;};
	int GetActiveSpritePrmt(unsigned *width, unsigned *height)
	{
		if(Sprites[active_sprite].SpriteImg == NULL) return -EBADF;

		*height = Sprites[active_sprite].heigth;
		*width = Sprites[active_sprite].width;
		return 0;
	}
	int GetSpritePrmt(sprite_num_t sprite_num, unsigned *img_cnt, unsigned *width, unsigned *height)
	{
		if(sprite_num >= MAX_SPRITES || Sprites[sprite_num].SpriteImg == NULL) return -EINVAL;

		*img_cnt = Sprites[sprite_num].img_cnt;
		*height = Sprites[sprite_num].heigth;
		*width = Sprites[sprite_num].width;
		return 0;
	}
private:
	sprite_img_t Sprites[MAX_SPRITES];
	Disp *Dsp;
	unsigned active_sprite;
	unsigned frame_delay;

	void InitSprite(sprite_num_t sprite_num);
};

#endif /* EOBJECT_H_ */
