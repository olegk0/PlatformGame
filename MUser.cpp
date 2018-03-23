/*
 * MUser.cpp
 *
 *      Author: olegvedi@gmail.com
 */

#include "MUser.h"

MUser::MUser(EObject *UserObj, GMap *GMap)
:MSolidAdd(UserObj, GMap)
{
	x_drw = 0;
	y_drw = MAP_BLOCK_SIZE * 2;
	def_speed = USER_DEF_SPEED;
	max_speed = USER_MAX_SPEED;
	sprite_framedelay = 1;
}

MUser::~MUser() {
}

unsigned MUser::Move(int dx, int dy, bool fire)
{
	Play( dx, dy, fire);

	return x_drw;
}
