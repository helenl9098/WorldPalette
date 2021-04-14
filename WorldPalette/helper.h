#pragma once
#ifndef HELPER_H
#define HELPER_H
#include <maya/MGlobal.h>
#include "vec.h"

// used for DEBUG print statements
#define DEBUG false
#define NUM_BUCKETS 10 // number of histogram buckets
#define NUM_ITERS 1000 // number of random iterations
#define ELEMENT_BUFFER 8 // number of elements we can at most delete or add
#define NORM_FACTOR 0.7 // how steep we allow objects to be at (the lower, the steeper)

void printFloat(MString caption, float f);

void printVec2(MString caption, vec2 v);

void printVec3(MString caption, vec3 v);

void printString(MString caption, MString s);
#endif