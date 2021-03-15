#include "WorldPalette.h"

WorldPalette::WorldPalette() : currentlySelectedRegion(nullptr)
{
	for (int i = 0; i < maxPaletteSize; i++) {
		palette[i] = nullptr;
	}
}

void WorldPalette::setCurrentDistribution(Distribution* d) {
	currentlySelectedRegion = d;
}

void WorldPalette::setCurrentDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos) {
	currentlySelectedRegion = new Distribution(st, w, h, min, max, pos);
}
void WorldPalette::saveDistribution(Distribution* d, int index) {
	if (index < maxPaletteSize) {
		palette[index] = d;
	}
}

void WorldPalette::saveDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos, int index) {
	if (index < maxPaletteSize) {
		palette[index] = new Distribution(st, w, h, min, max, pos);
	}
}

void WorldPalette::deleteDistribution(int index) {
	if (index < maxPaletteSize) {
		if (palette[index] != nullptr) {
			free(palette[index]);
			palette[index] = nullptr;
		}
	}
}

WorldPalette::~WorldPalette()
{
	if (currentlySelectedRegion != nullptr) {
		free(currentlySelectedRegion);
	}
	for (int i = 0; i < maxPaletteSize; i++) {
		if (palette[i] != nullptr) {
			free(palette[i]);
		}
	}
}