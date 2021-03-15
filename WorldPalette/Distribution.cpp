#include "Distribution.h"

SelectedRegion::SelectedRegion(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos)
	: selectionType(st), position(pos), width(w), height(h), minBounds(min), maxBounds(max), radius(w)
{
	if (st != SelectionType::NONE) {
		findSceneObjects();
	}
}

float SelectedRegion::getArea() {
	if (selectionType == SelectionType::PLANAR) {
		return width * 2.0f * height * 2.0f;
	} else if (selectionType == SelectionType::RADIAL) {
		return M_PI * radius * radius;
	}
}

/*
* Goes through all the scene objects and finds the ones that are currently in the selected region
*/
void SelectedRegion::findSceneObjects() {
	printString(MString("Finding Scene Objects.."), MString(""));
}

SelectedRegion::~SelectedRegion() {}

// default constructor that makes an empty distribution
Distribution::Distribution() 
	: selectedRegion(SelectedRegion(SelectionType::NONE, 0, 0, vec3(0, 0, 0), vec3(0, 0 , 0), vec3(0, 0, 0))), empty(true)
{}

Distribution::Distribution(SelectedRegion r) 
	: selectedRegion(r), empty(false)
{
	if (r.selectionType == SelectionType::NONE) {
		empty = true;
	}
}

Distribution::Distribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos)
	: selectedRegion(SelectedRegion(st, w, h, min, max, pos)), empty(false)
{
	if (st == SelectionType::NONE) {
		empty = true;
	}
}

Distribution::~Distribution() {}