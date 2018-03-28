/*
 * Mob.cpp
 *
 *  Created on: 26 мар. 2018 г.
 *      Author: olegvedi@gmail.com
 */

#include "Mob.h"

GMap *Mob::ModMapObj = NULL;

Mob::Mob()
{
    if (ModMapObj == NULL) {
	MSG_ERR("GMap is not initialized!");
	throw 1;
    }

    speed = MOB_DEF_SPEED;
    def_speed = MOB_DEF_SPEED;
    max_speed = MOB_MAX_SPEED;

    sprite_set_curr = EObject::Sprite_Right_Sheet;
    sprite_set_past = EObject::Sprite_None_Sheet;
    sprite_begin_idx = 0;
    sprite_show_mode = 0;
    last_to_right = true;
    sprite_framedelay = 0;
    time_flag = false;

    x_drw = 0;
    y_drw = 0;
    disp_width = ModMapObj->disp_width;
    disp_height = ModMapObj->disp_height;
    sprite_top_div = 2;
    sprite_width_div = 2;
    sprite_width_offs = 0;
    sprite_top_offs = 0;
    sprite_width = 0;
    sprite_height = 0;

//	GObj.SetActiveSpriteSheet(sprite_set_curr,sprite_framedelay);
//	UpdateSpriteSize();

}

void Mob::Draw(int Corr_x, bool tf)
{
    time_flag = tf;
    x_drw -= Corr_x;
    if (sprite_set_past != sprite_set_curr) {
	GObj.SetActiveSpriteSheet(sprite_set_curr, sprite_framedelay);
	GObj.SetActiveSpriteBeginIndex(sprite_begin_idx);
	sprite_set_past = sprite_set_curr;
	UpdateSpriteSize();
    }
    GObj.ShowActiveSpriteSheet(x_drw, y_drw, sprite_show_mode);
}

unsigned Mob::move_pos(int dx, int dy)
{
    unsigned ret = no_out;

    x_drw += dx;
    y_drw += dy;

    if (x_drw <= -MAP_BLOCK_SIZE / 2) {
	x_drw = -(MAP_BLOCK_SIZE / 2);
	ret = left_edge_out;
    } else if (x_drw > static_cast<int>(disp_width)) {
	x_drw = disp_width;
	ret |= right_edge_out;
    }
    if (y_drw < static_cast<int>(sprite_height)) {
	y_drw = sprite_height;
	ret |= bottom_edge_out;
    } else if (y_drw > static_cast<int>(disp_height)) {
	y_drw = disp_height;
	ret |= top_edge_out;
	;
    }

    if (speed > max_speed) {
	speed = max_speed;
    }

    return ret;
}

int Mob::UpdateSpriteSize()
{
    int ret = GObj.GetActiveSpritePrmt(&sprite_width, &sprite_height);
//		MSG_DBG(2, "sprite_width:%d , sprite_height:%d ",sprite_width, sprite_height);
    sprite_width_offs = sprite_width >> sprite_width_div;
    sprite_top_offs = sprite_height >> sprite_top_div;
    return ret;
}

