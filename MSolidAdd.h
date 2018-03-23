/*
 * MSolidAdd.h
 *
 *      Author: olegvedi@gmail.com
 */

#ifndef MSOLIDADD_H_
#define MSOLIDADD_H_

#include "GMap.h"
#include "Mob.h"

#define DEF_JUMP_OFFS 20
#define DEF_JUMP_LEN 8
#define DEF_JUMP_SPEED 1

class MSolidAdd: public Mob {
public:
	MSolidAdd(EObject *UserObj, GMap *GMap): Mob(UserObj, GMap) {
		RecalcCoord();
		jump_mode = 1;
		jump_up_len = DEF_JUMP_LEN;
		jump_speed = DEF_JUMP_SPEED;
		MSG_DBG(2, "+++++++");
	};
	virtual ~MSolidAdd() {};

	/*virtual*/ void Draw(int Corr_x) {
		Mob::Draw(Corr_x);
		if(Corr_x){
			RecalcCoord();
		}
	};
protected:
	gmap_pos_str_t cols_pos;
	gmap_pos_str_t map_cols_pos;
	unsigned jump_mode, jump_up_len, jump_speed;

	bool CheckCollisions(bool falling_check);
	void RecalcCoord();

	unsigned MovePos(int dx, int dy) {
		unsigned ret=0, i;
		MSG_DBG(2, "");

		if (dx != 0 || dy != 0) {
			ret = move_pos(dx,  dy);
			RecalcCoord();

		    i = MAP_BLOCK_SIZE;

		    while(CheckCollisions(false)) {
		    	if(i <= 0){
		    		MSG_ERR("CheckCollisions loop");
		    		break;
		    	}
		    	i--;
		//    	MSG_DBG(2, "+++++++ Corr +++++++");
		    }
		}

		return ret;
	};

	void Play(int dx, int dy, bool Attack);
};

#endif /* MSOLIDADD_H_ */
