#include "Distribution.h"

SelectedRegion::SelectedRegion(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos)
	: selectionType(st), position(pos), width(w), height(h), minBounds(min), maxBounds(max), radius(w)
{
	// TO DO: Make sure the user never has a selection type of NONE
	findSceneObjects();
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

}

SelectedRegion::~SelectedRegion() {}

Distribution::Distribution(SelectedRegion r) 
	: selectedRegion(r)
{}

Distribution::Distribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos)
	: selectedRegion(SelectedRegion(st, w, h, min, max, pos))
{}

Distribution::~Distribution() {}