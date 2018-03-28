/*
 * MPredator.h
 *
 *      Author: olegvedi@gmail.com
 */

#ifndef MPREDATOR_H_
#define MPREDATOR_H_

#include "Mob.h"
#include "MSolidAdd.h"

class MPredator: public MSolidAdd{
public:
	MPredator();
	virtual ~MPredator() {};

	void Draw(int Corr_x, bool time_flg);

private:
	int count;
	bool jump;
};

#endif /* MPREDATOR_H_ */
