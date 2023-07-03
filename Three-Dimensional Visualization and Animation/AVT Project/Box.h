#ifndef BOX_H_
#define BOX_H_

#include <vector>
#include <array>
#include "VSShaderlib.h"
#include "AVTmathLib.h"
#include "geometry.h"

struct Corner {
	float x, y, z;
};

struct Box {
//	  7-------6
//   /        /
//   4-------5
//           
//    3-------2
//   /        /
//   0-------1
	Corner corners[8];
};

struct MinMaxBox {
	Corner min;
	Corner max;
};

#endif
