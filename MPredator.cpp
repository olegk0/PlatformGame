/*
 * MPredator.cpp
 *
 *      Author: olegvedi@gmail.com
 */

#include "MPredator.h"

MPredator::MPredator() :
	MSolidAdd()
{
    SetPos(400, 200);
//		sprite_show_mode = SPRITE_ANI_LOOP;
    count = 0;
    jump = false;
//	    sprite_top_div = 3;
//		sprite_width_div = 3;
    jump_up_len = 10;
    sprite_framedelay = 2;

}
/*
 MPredator::~MPredator() {
 // TODO Auto-generated destructor stub
 }
 */

void MPredator::Draw(int Corr_x, bool time_flg)
{
    MSolidAdd::Draw(Corr_x, time_flg);

    if (last_to_right) {
	MovePos(speed, 0);
	Play(1, jump, false);
    } else {
	Play(-1, jump, false);
    }

    count--;
    if (count < 0) {
	count = rand() % 5 + 10;
	int rn = rand() % 7;
	switch (rn) {
	case 0:
	    last_to_right = true;
	    break;
	case 1:
	    last_to_right = false;
	    break;
	case 2:
	case 3:
	    jump = true;
	    break;
	default:
	    jump = false;
	    break;

	}
	if (jump) {
	    speed = 2;
	} else {
	    speed = 1;
	}
//	    MSG_DBG(2, "+++++++ rn:%d  ", rn);
    }
}
