#pragma once
#ifndef HELPER_H
#define HELPER_H
#include <maya/MGlobal.h>
#include "vec.h"

// used for DEBUG print statements
#define DEBUG false
#define NUM_BUCKETS 7
#define NUM_ITERS 600
#define ELEMENT_BUFFER 3

void printFloat(MString caption, float f);

void printVec3(MString caption, vec3 v);

void printString(MString caption, MString s);
#endif