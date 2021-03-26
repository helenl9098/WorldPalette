#include "WorldPalette.h"

std::vector<CATEGORY> WorldPalette::priorityOrder = { CATEGORY::HOUSE, CATEGORY::TREE, CATEGORY::ROCK }; // default order

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

void WorldPalette::updatePriorityOrder(std::vector<int>& newOrder) {
	for (int i = 0; i < newOrder.size(); ++i) {
		WorldPalette::priorityOrder[i] = static_cast<CATEGORY>(newOrder[i]);
	}
}

WorldPalette::~WorldPalette()
{}