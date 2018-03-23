/*
 * MUser.h
 *
 *      Author: olegvedi@gmail.com
 */

#ifndef MUSER_H_
#define MUSER_H_


#include "Mob.h"
#include "MSolidAdd.h"

#define USER_DEF_SPEED 2
#define USER_MAX_SPEED 20


class MUser: public MSolidAdd{
public:
	MUser(EObject *UserObj, GMap *GMapObj);
	virtual ~MUser();
	unsigned Move(int dx, int dy, bool fire);

private:

};

#endif /* MUSER_H_ */
