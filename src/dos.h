#pragma once

// PCH

// STL
#include <string>
#include <iostream>
#include <functional>
#include <memory>
#include <numeric>
#include <vector>
#include <array>
#include <stdint.h>
#include <cassert>

// DOS
#include "logger.h"

// HMM
#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_CPP_MODE
#define HANDMADE_MATH_NO_INLINE
#include "HandmadeMath.h"

#define check(expr) if (!(expr)) {Logger::Error("Triggered assertion at" __LOCATION_INFO__); assert(false); }

// DAG for hierarchy
// actually, when an entity is removed or added, we update the array representation of the depth first order, otherwise we can reuse the representation
// process sorted hierarchy to output world transforms
// input world transforms into culling
//	test against frustum
// output set of culled objects

// Maybe we can use hierarchy to test against frustum hierarchically -> if so, we need to implement bounds aswell

namespace dos // D ata O riented S cene
{

}

namespace dos { namespace hierarchy {

}}

namespace dos { namespace culling {
	
}}