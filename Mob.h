/*
 * Mob.h
 *
 *      Author: olegvedi@gmail.com
 */

#ifndef MOB_H_
#define MOB_H_

#include "EObject.h"
#include "GMap.h"

//class GMap;

#define MOB_DEF_SPEED 0;
#define MOB_MAX_SPEED 20;

class Mob {
public:
	Mob(EObject *EObj, GMap *GMap) {
		GMapObj = GMap;
		GObj = EObj;
		speed = MOB_DEF_SPEED;
		def_speed = MOB_DEF_SPEED;
		max_speed = MOB_MAX_SPEED;
		sprite_set_curr = EObject::Sprite_Right_Img;
		sprite_set_past = EObject::Sprite_None_Img;
		sprite_begin_idx = 0;
		sprite_show_mode = 0;
		last_to_right = true;
		sprite_framedelay = 0;

		x_drw = 0;
		y_drw = 0;
		disp_width = GMapObj->disp_width;
		disp_height = GMapObj->disp_height;
		sprite_top_div = 2;
		sprite_width_div = 2;
		GObj->SetActiveSprite(sprite_set_curr,sprite_framedelay);
		UpdateSpriteSize();
		MSG_DBG(2, "+++++++");
	};
	virtual ~Mob() {};

	virtual void Draw(int Corr_x) {
		x_drw -= Corr_x;
		if(sprite_set_past != sprite_set_curr){
			GObj->SetActiveSprite(sprite_set_curr, sprite_framedelay);
			GObj->SetActiveSpriteBgIdx(sprite_begin_idx);
			sprite_set_past = sprite_set_curr;
			UpdateSpriteSize();
		}
		GObj->ShowActiveSprite(x_drw, y_drw, sprite_show_mode);
	};

protected:
	typedef enum {
		no_out=0,
		right_edge_out=1,
		left_edge_out=2,
		top_edge_out=4,
		bottom_edge_out=8,
	} edge_out_et;

	EObject *GObj;
	GMap *GMapObj;

	unsigned disp_width, disp_height;
	unsigned sprite_width, sprite_height;
	unsigned sprite_width_offs, sprite_top_offs;
	unsigned sprite_top_div, sprite_width_div;
	unsigned speed, def_speed, max_speed;
	int sprite_show_mode, sprite_begin_idx, sprite_framedelay;
	bool last_to_right;
	int x_drw, y_drw;
	EObject::sprite_num_t sprite_set_curr, sprite_set_past;

	void SetPos(unsigned x, unsigned y) {
		x_drw = x;
		y_drw =y;
	};

	unsigned MovePos(int dx, int dy) {
		unsigned ret=0;

		if (dx != 0 || dy != 0) {
			ret = move_pos(dx,  dy);
		}
		return ret;
	};

	unsigned move_pos(int dx, int dy) {
		unsigned ret=no_out;

		x_drw += dx;
		y_drw += dy;

		if (x_drw <= -MAP_BLOCK_SIZE / 2){
			x_drw = -(MAP_BLOCK_SIZE / 2);
			ret = left_edge_out;
		}
		else if (x_drw > static_cast<int>(disp_width)) {
			x_drw = disp_width;
			ret |= right_edge_out;
		}
		if (y_drw < static_cast<int>(sprite_height)){
			y_drw = sprite_height;
			ret |= bottom_edge_out;
		}
		else if (y_drw > static_cast<int>(disp_height)){
			y_drw = disp_height;
			ret |= top_edge_out;;
		}

		if(speed > max_speed){
			speed = max_speed;
		}

		return ret;
	};

private:
	int UpdateSpriteSize() {
		int ret = GObj->GetActiveSpritePrmt(&sprite_width, &sprite_height);
		MSG_DBG(2, "sprite_width:%d , sprite_height:%d ",sprite_width, sprite_height);
		sprite_width_offs = sprite_width >> sprite_width_div;
		sprite_top_offs = sprite_height >> sprite_top_div;
		return ret;
	};

};

#endif /* MOB_H_ */
