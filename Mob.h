/*
 * Mob.h
 *
 *      Author: olegvedi@gmail.com
 */

#ifndef MOB_H_
#define MOB_H_

#include "EObject.h"
#include "GMap.h"

#define MOB_DEF_SPEED 5;
#define MOB_MAX_SPEED 100;

class Mob {
public:
    Mob();
    virtual ~Mob()
    {
    }

    virtual void Draw(int Corr_x, bool time_flag);

    static void Init(GMap *GMap)
    {
	ModMapObj = GMap;
    }

    EObject *GetEObject()
    {
	return &GObj;
    }

    virtual void SetPos(unsigned x, unsigned y)
    {
	x_drw = x;
	y_drw = y;
    }

protected:
    typedef enum {
	no_out = 0, right_edge_out = 1, left_edge_out = 2, top_edge_out = 4, bottom_edge_out = 8,
    } edge_out_et;

    static GMap *ModMapObj;

    EObject GObj;
    unsigned disp_width, disp_height;
    unsigned sprite_width, sprite_height;
    unsigned sprite_width_offs, sprite_top_offs;
    unsigned sprite_top_div, sprite_width_div;
    unsigned speed, def_speed, max_speed;
    int sprite_show_mode, sprite_begin_idx, sprite_framedelay;
    bool last_to_right;
    int x_drw, y_drw;
    bool time_flag;
    EObject::sprites_sheet_num_t sprite_set_curr, sprite_set_past;

    unsigned MovePos(int dx, int dy)
    {
	unsigned ret = 0;

	if (dx != 0 || dy != 0) {
	    ret = move_pos(dx, dy);
	}
	return ret;
    }

    unsigned move_pos(int dx, int dy);

private:
    int UpdateSpriteSize();

};

#endif /* MOB_H_ */
