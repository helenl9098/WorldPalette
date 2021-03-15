#include "WorldPalette.h"

WorldPalette::WorldPalette() {
	printString(MString("Created World Palette Object"), MString(""));
}

void WorldPalette::setCurrentDistribution(Distribution d) {
	currentlySelectedRegion = d;
}

void WorldPalette::setCurrentDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos) {
	currentlySelectedRegion = Distribution(st, w, h, min, max, pos);
}
void WorldPalette::saveDistribution(Distribution d, int index) {
	if (index < maxPaletteSize) {
		palette[index] = d;
		currentlySelectedRegion = d;
	}
}

void WorldPalette::saveDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos, int index) {
	if (index < maxPaletteSize) {
		palette[index] = Distribution(st, w, h, min, max, pos);
		currentlySelectedRegion = palette[index];
	}
}

void WorldPalette::deleteDistribution(int index) {
	palette[index].empty = true;
}

WorldPalette::~WorldPalette()
{}