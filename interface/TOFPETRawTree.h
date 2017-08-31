#ifndef __TOFPET_RAW_TREE__
#define __TOFPET_RAW_TREE__

#include "DynamicTTree/interface/DynamicTTreeBase.h"

typedef long long int int64;

using namespace std;

//****************************************************************************************
//----------Tree reader class-------------------------------------------------------------
#undef DYNAMIC_TREE_NAME
#undef DATA_TABLE
#undef DATA_VECT_TABLE

#define DYNAMIC_TREE_NAME TOFPETRawTree

#define DATA_TABLE                              \
    DATA(unsigned short int,  channelID)       \
    DATA(int64,               time)            \
    DATA(float,               tot)             \
    DATA(float,               energy)          \
    DATA(float,               tqT)          \
    DATA(float,               tqE)          \
    DATA(float,               x)          \
    DATA(float,               y)          \
    DATA(float,               z)          \
    DATA(int,               xi)          \
    DATA(int,               yi)          \

#include "DynamicTTree/interface/DynamicTTreeInterface.h"

#endif 

