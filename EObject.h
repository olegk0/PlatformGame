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

#define SPRITE_ANI_LOOP (-1)
#define SPRITE_ANI_NO_LOOP (-2)
#define DEF_FRAME_DELAY 10

class EObject {
public:
    typedef enum {
	Sprite_Front_Sheet = 0,
	Sprite_Front_Sheet_v1,
	Sprite_Down_Sheet,
	Sprite_Right_Sheet,
	Sprite_Left_Sheet,
	Sprite_Right_Sheet_v1,
	Sprite_Left_Sheet_v1,
	Sprite_Jump_Right_Sheet,
	Sprite_Jump_Left_Sheet,
	Sprite_Atack_Right_Sheet,
	Sprite_Atack_Left_Sheet,
	Sprite_None_Sheet,
    } sprites_sheet_num_t;

#define MAX_SPRITE_SHEETS (Sprite_None_Sheet + 2)

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
    EObject(const EObject *);
    EObject(const EObject&);
    EObject();
    virtual ~EObject();
    static void Init(Disp *Disp)
    {
	Dsp = Disp;
    }

    void Free();

    int CopyEObject(const EObject *Src);
    int LoadSpriteSheet(sprites_sheet_num_t sheet_num, string &img_path, unsigned sprite_width, unsigned min_loop_idx,
	    bool AutoMirror = false);
    int AssignSpriteSheet(sprites_sheet_num_t sheet, Sprite *PS);
    Sprite *GetSpriteSheet(sprites_sheet_num_t sheet_num)
    {
	if (sheet_num >= MAX_SPRITE_SHEETS)
	    return NULL;

	return Sprites[sheet_num].SpriteImg;
    }
    int ShowSpriteSheet(sprites_sheet_num_t sheet_num, unsigned x, unsigned y, int show_mode, bool FromTopCoord=false,
	    bool DecDelay=true);
    int ShowActiveSpriteSheet(unsigned x, unsigned y, int show_mode, bool FromTopCoord=false, bool DecDelay=true);
//	int ShowActiveSpriteAlignBottom(unsigned x, unsigned blk_y, int show_mode);
    int SetActiveSpriteSheet(sprites_sheet_num_t sheet_num, unsigned frame_del = DEF_FRAME_DELAY)
    {
	if (sheet_num >= MAX_SPRITE_SHEETS)
	    return -EINVAL;
	if (Sprites[sheet_num].SpriteImg == NULL)
	    return -EBADF;
	active_sprite_sheet = sheet_num;
	if (frame_del == 0)
	    Sprites[sheet_num].delay = DEF_FRAME_DELAY;
	else
	    Sprites[sheet_num].delay = frame_del;
	Sprites[sheet_num].curr_img = 0;
	return 0;
    }
    void SetActiveSpriteBeginIndex(unsigned sprite_idx)
    {
	Sprites[active_sprite_sheet].curr_img = sprite_idx;
    }

    int GetActiveSpritePrmt(unsigned *width, unsigned *height)
    {
	if (Sprites[active_sprite_sheet].SpriteImg == NULL)
	    return -EBADF;

	*height = Sprites[active_sprite_sheet].heigth;
	*width = Sprites[active_sprite_sheet].width;
	return 0;
    }
    unsigned &GetActiveSpriteSheetCnt()
    {
	return Sprites[active_sprite_sheet].img_cnt;
    }

    int GetSpriteSheetPrmt(sprites_sheet_num_t sheet_num, unsigned *img_cnt, unsigned *width, unsigned *height)
    {
	if (sheet_num >= MAX_SPRITE_SHEETS || Sprites[sheet_num].SpriteImg == NULL)
	    return -EINVAL;

	*img_cnt = Sprites[sheet_num].img_cnt;
	*height = Sprites[sheet_num].heigth;
	*width = Sprites[sheet_num].width;
	return 0;
    }

    void DecDelayActiveSpriteSheet()
    {
	if (frame_delay > 0) {
	    frame_delay--;
	}
    }
private:
    EObject& operator=(EObject&);
    sprite_img_t Sprites[MAX_SPRITE_SHEETS];
    static Disp *Dsp;
    sprites_sheet_num_t active_sprite_sheet;
    unsigned frame_delay;

    void main_init();
    void InitSpriteSheet(sprites_sheet_num_t sprite_num);
    void PurgeSpriteSheet(sprites_sheet_num_t sprite_num);
};

#endif /* EOBJECT_H_ */
