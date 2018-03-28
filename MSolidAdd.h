/*
 * MSolidAdd.h
 *
 *      Author: olegvedi@gmail.com
 */

#ifndef MSOLIDADD_H_
#define MSOLIDADD_H_

#include "GMap.h"
#include "Mob.h"

#define DEF_JUMP_LEN 10 // итераций прышка вверх
#define DEF_JUMP_OFFS 20 //смещение на итерацию
#define DEF_JUMP_SPEED 2 // начальная скорость падения

class MSolidAdd: public Mob {
public:
    MSolidAdd() :
	    Mob()
    {
	RecalcCoord();
	jump_mode = 1;
	jump_up_len = DEF_JUMP_LEN;
	jump_speed = DEF_JUMP_SPEED;
    }

    virtual ~MSolidAdd()
    {
    }

    /*virtual*/
    void Draw(int Corr_x, bool time_flg)
    {
	Mob::Draw(Corr_x, time_flg);
	if (Corr_x) {
	    RecalcCoord();
	}
    }

    void SetPos(unsigned x, unsigned y)
    {
	Mob::SetPos(x,y);
	RecalcCoord();
    }

protected:
    gmap_pos_str_t cols_pos;
    gmap_pos_str_t map_cols_pos;
    unsigned jump_mode, jump_up_len, jump_speed;

    bool CheckCollisions(bool falling_check);
    void RecalcCoord();

    unsigned MovePos(int dx, int dy)
    {
	unsigned ret = 0, i;
//		MSG_DBG(2, "");

	if (dx != 0 || dy != 0) {
	    ret = move_pos(dx, dy);
	    RecalcCoord();

	    i = MAP_BLOCK_SIZE;

	    while (CheckCollisions(false)) {
		if (i <= 0) {
		    MSG_ERR("CheckCollisions loop");
		    break;
		}
		i--;
		//    	MSG_DBG(2, "+++++++ Corr +++++++");
	    }
	}

	return ret;
    }

    void Play(int dx, int dy, bool Attack);
};

#endif /* MSOLIDADD_H_ */
