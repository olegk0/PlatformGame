/*
 * MFly.h
 *
 *      Author: olegvedi@gmail.com
 */

#ifndef MFLY_H_
#define MFLY_H_

#include "EObject.h"
#include "GMap.h"
#include "Mob.h"

class MFly: public Mob {
public:
	MFly(EObject *EObj, GMap *GMap) :Mob(EObj, GMap) {
		SetPos(MAP_BLOCK_SIZE, MAP_BLOCK_SIZE * 2);
		y_offs = 0;
		sprite_show_mode = SPRITE_ANI_LOOP;
		sprite_framedelay = 2;
		flen = 10;
		InitFly(false);
	};

	virtual ~MFly() {};

	void InitFly(bool onRight){
		int x,y;
		if(onRight){
			x = disp_width;
		}else{
			x = rand() % disp_width;
		}
		y = rand() % disp_height / 2;
		SetPos(x,y);
		y_offs = rand() % 10;
		to_right = rand() % 2;
//		sprite_set_past = sprite_set_curr;
	};

	void Draw(int Corr_x){
//		MSG_DBG(2, "+0+");
		Mob::Draw(Corr_x);
		int dx, dy;

		if(to_right){
			sprite_set_curr = EObject::Sprite_Right_Img;
	    	dx = 4;
	    }else{
	    	sprite_set_curr = EObject::Sprite_Left_Img;
	    	dx = -4;
	    }
		dy = (rand() % 5) - y_offs;

		unsigned err = Mob::MovePos( dx, dy);

		if(err & right_edge_out){
			to_right=false;
	    }
	    if(err & left_edge_out){
			InitFly(true);
	    }
	    if(err & top_edge_out){
	    	y_offs++;
	    }
	    if(err & bottom_edge_out){
	    	if(y_offs > 0)
	    		y_offs--;

	    }

	    if(flen-- <=0){
	    	flen = (rand() % 200) + 100;
	    	to_right = !to_right;
	    }

//	    MSG_DBG(2, "*****show*****  x_drw:%d, y_drw:%d  y_offs:%d  err:%X",x_drw, y_drw,y_offs,err);
	};

private:
	bool to_right;
	int y_offs;
	int flen;

};

#endif /* MFLY_H_ */
