#pragma once
#ifndef HELPER_H
#define HELPER_H
#include <maya/MGlobal.h>
#include "vec.h"

// used for DEBUG print statements
#define DEBUG false

void printFloat(MString caption, float f);

void printVec3(MString caption, vec3 v);

void printString(MString caption, MString s);
#endif