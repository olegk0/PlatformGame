/*
 * MSolidAdd.cpp
 *
 *      Author: olegvedi@gmail.com
 */

#include "MSolidAdd.h"

/*
MSolidAdd::MSolidAdd() {
	// TODO Auto-generated constructor stub

}

MSolidAdd::~MSolidAdd() {
	// TODO Auto-generated destructor stub
}
*/

void MSolidAdd::RecalcCoord()
{

	cols_pos.x_left = x_drw + sprite_width_offs;
	cols_pos.y_top = y_drw - (sprite_top_offs - 1);
	map_cols_pos.x_left = X_TO_MAP_COORD_EXT(cols_pos.x_left);
	map_cols_pos.y_top = Y_TO_MAP_COORD_EXT(cols_pos.y_top);
	cols_pos.x_right = x_drw + sprite_width - sprite_width_offs;
	cols_pos.y_bottom = y_drw;
	map_cols_pos.x_right = X_TO_MAP_COORD_EXT(cols_pos.x_right);
	map_cols_pos.y_bottom = Y_TO_MAP_COORD_EXT(cols_pos.y_bottom);
	map_cols_pos.y_under_bottom = Y_TO_MAP_COORD_EXT(cols_pos.y_bottom + 1);


	MSG_DBG(2, "x_drw:%d y_drv:%d   y_under_bottom:%d",x_drw,y_drw, map_cols_pos.y_under_bottom);
	MSG_DBG(2, "x_pos:%d:%d y_pos:%d:%d  x_map_pos:%d:%d  y_map_pos:%d:%d  ud_curr:%d",
			cols_pos.x_left,cols_pos.x_right, cols_pos.y_top, cols_pos.y_bottom,
			map_cols_pos.x_left,map_cols_pos.x_right, map_cols_pos.y_top, map_cols_pos.y_bottom, sprite_set_curr);

}

bool MSolidAdd::CheckCollisions(bool falling_check) {
	bool ret = false;
	int dx=0, dy=0;

	Obj_Type_t BlkLeftTop = GMapObj->gmap[map_cols_pos.x_left][map_cols_pos.y_top].type;
	Obj_Type_t BlkRightTop = GMapObj->gmap[map_cols_pos.x_right][map_cols_pos.y_top].type;
	Obj_Type_t BlkLeftBtm = GMapObj->gmap[map_cols_pos.x_left][map_cols_pos.y_bottom].type;
	Obj_Type_t BlkRightBtm = GMapObj->gmap[map_cols_pos.x_right][map_cols_pos.y_bottom].type;
	Obj_Type_t BlkLeftUBtm = GMapObj->gmap[map_cols_pos.x_left][map_cols_pos.y_under_bottom].type; // next pixel
	Obj_Type_t BlkRightUBtm = GMapObj->gmap[map_cols_pos.x_right][map_cols_pos.y_under_bottom].type; //

	MSG_DBG(4, "map_cols_pos.x_left:%d map_cols_pos.y_bottom:%d map_cols_pos.y_under_bottom:%d",
			map_cols_pos.x_left,map_cols_pos.y_bottom, map_cols_pos.y_under_bottom);
	MSG_DBG(4, "BlkLeftTop:%d  BlkRightTop:%d  BlkLeftBtm:%d  BlkRightBtm:%d  BlkLeftUBtm:%d  BlkRightUBtm:%d",
			BlkLeftTop, BlkRightTop, BlkLeftBtm, BlkRightBtm,BlkLeftUBtm,BlkRightUBtm);

	if (falling_check) { // проверка падение
		MSG_DBG(4, "****01****");
		//ниже спрайта не свободно и позиция на границе ячейки
		if ((BLK_NOT_FREE(BlkLeftUBtm) || BLK_NOT_FREE(BlkRightUBtm)) &&
				cols_pos.y_bottom == Y_TO_DISP_COORD_EXT(map_cols_pos.y_under_bottom) - 1){ // приземлились
			if(jump_mode > 0){
				MSG_DBG(4, "****01-1****  jump_mode = 0 ");
				jump_mode = 0;
//				Move(0, false, false);
			}

		}else{// будем падать дальше
			ret = true;
			jump_mode = 1;
			MSG_DBG(4, "****02****");
			if (BLK_IS_TRANSP(BlkLeftBtm) && BLK_IS_TRANSP(BlkRightBtm)) { //низ спрайта свободен
				MSG_DBG(4, "****03****");
				dy = 1;
				GObj->SetActiveSpriteBgIdx(0);//TODO
			}
		}
	}
	if (BLK_NOT_TRANSP(BlkLeftBtm) || BLK_NOT_TRANSP(BlkRightBtm)) { //низ спрайта увяз
		ret = true;
		MSG_DBG(4, "****1****");
		if (BLK_NOT_TRANSP(BlkLeftBtm) && BLK_NOT_TRANSP(BlkRightBtm)) { //увязли оба низа
			MSG_DBG(4, "****2****");
			dy = -1;
			/*			if(BLK_NOT_TRANSP(BlkLeftTop)){//еще и левым верхом в непрозрачное
			 dx = 1;
			 }else if(BLK_NOT_TRANSP(BlkRightTop)){//или правым верхом в непрозрачное
			 dx = -1;
			 }*/
		} else { // увяз какойто один низ
			MSG_DBG(4, "****3****");
			if (BLK_NOT_TRANSP(BlkLeftBtm)) { // увяз левый нижний
				MSG_DBG(4, "****4****");
				dx = 1;
				/*
				if (BLK_IS_TRANSP(BlkLeftTop)) { // и левый верхний не плотный
					dy = -1;
				}
				*/
			} else { // BLK_NOT_FREE(BlkRightBtm // или правый нижний
				MSG_DBG(4, "****5****");
				dx = -1;
				/*
				if (BLK_IS_TRANSP(BlkRightTop)) { // и правый верхний не плотный
					MSG_DBG(4, "****6****");
					dy = -1;
				} //else if(BLK_NOT_TRANSP(BlkRightTop)) dx = -1;
				*/
			}
		}

	}else{
		if (BLK_NOT_TRANSP(BlkLeftTop) || BLK_NOT_TRANSP(BlkRightTop)) { //уперлись верхом
			MSG_DBG(4, "****7****");
			/*
			ret = true;
			dy = 2;
			SetPos(x_drw, y_drw + dy);
			*/
			dy = Y_TO_DISP_COORD_EXT(map_cols_pos.y_bottom) - y_drw;
		}
	}

	MovePos(dx, dy);

	MSG_DBG(2, "");

	return ret;
}

void MSolidAdd::Play(int dx, int dy, bool Attack)
{
	int offs=0;

	if(dy){
		if(dy > 0){
			if(jump_mode == 0){
				jump_speed = DEF_JUMP_SPEED;
				jump_mode = jump_up_len;
				sprite_show_mode = SPRITE_ANI_NO_LOOP;
				sprite_begin_idx = 1;
			}
		}else{
			MovePos(0, speed);
		}
	}

	if (dx) {
		if (dx < 0) {
			offs = -speed;
			last_to_right = false;
		} else {
			offs = speed;
			last_to_right = true;
		}
		MovePos( offs, 0);
		speed++;

	}else{
		if(speed != def_speed){
			speed = def_speed;
		}
	}

	if(jump_mode > 0 ){
		if(last_to_right)
			sprite_set_curr = EObject::Sprite_Jump_Right_Img;
		else
			sprite_set_curr = EObject::Sprite_Jump_Left_Img;
	}else{
		if (!dx)
			sprite_show_mode = 0;
		else
			sprite_show_mode = SPRITE_ANI_LOOP;
		if(last_to_right)
			sprite_set_curr = EObject::Sprite_Right_Img;
		else
			sprite_set_curr = EObject::Sprite_Left_Img;
	}

	if(jump_mode > 1){
		MovePos( 0, -DEF_JUMP_OFFS);
		MSG_DBG(2, "+++++++ Jump :%d  x_drw:%d, y_drw:%d", jump_mode, x_drw, y_drw);
		jump_mode--;
	}else{
		int jc = jump_up_len + jump_speed;
	    while(jc > 0 && CheckCollisions(true)) {
	    	jc--;
		//	MSG_DBG(2, "+++++++ Falling Corr x_drw:%d  y_drw:%d +++++++",x_drw, y_drw);
		}
	    jump_speed++;
	}

}
