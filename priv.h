#ifndef __PRIV_H_
#define __PRIV_H_

#include <stdio.h>

#define DEBUG 1

#define MSG_ERR(format, ...) fprintf(stderr, "[WebMP](%s): Error - " format "\n",__PRETTY_FUNCTION__ ,##__VA_ARGS__)

#ifdef DEBUG
#define DBG_LVL_DEF 2
//#define MSG_DBG(dl, format, ...) {if(dl <= DBG_LVL_DEF ) fprintf(stderr, "[WebMP](%s):" format "\n", __func__, ##__VA_ARGS__);}
#define MSG_DBG(dl, format, ...) {if(dl <= DBG_LVL_DEF ) fprintf(stderr, "[WebMP](%s):" format "\n", __PRETTY_FUNCTION__, ##__VA_ARGS__);}
#define MSG_DBG_ONCE(format, ...) do { static uint8_t __once; if (!__once) { fprintf(stderr, "[WebMP] " format "\n", ##__VA_ARGS__); __once = 1; } } while(0)
#else
#define MSG_DBG(dl, format, ...)
#define MSG_DBG_ONCE(format, ...)
#endif

#endif
