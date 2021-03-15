#include "helper.h"

void printFloat(MString caption, float f) {
	MGlobal::displayInfo(caption + f);
}

void printVec3(MString caption, vec3 v) {
	MGlobal::displayInfo(caption + v[0] + MString(", ") + v[1] + MString(", ") + v[2]);
}

void printString(MString caption, MString s) {
	MGlobal::displayInfo(caption + s);
}