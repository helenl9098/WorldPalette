#pragma once
#ifndef WORLDPALETTE_H
#define WORLDPALETTE_H
#include "Distribution.h"
#include <maya/MGlobal.h>
#include "helper.h"
/*
*
* This class will hold all of our saved distributions. It will also know our currently selected distribution. 
* We initialize one instance of this class in the beginning of our plugin, and it will be altered by all future operations.
*
*/
class WorldPalette
{
public:
	int geomId = 0; // counter for pasted geometry id
	int maxPaletteSize = 10; // if you change this, change the array static initialization below
	Distribution palette[10]; // saved distributions in the palette. MAX 10 FOR NOW.
	Distribution currentlySelectedRegion; // the most recent selected region. This is for copy & pasting and moving, which don't require saving a distribution in a palette. 

public: 
	WorldPalette();
	void findSceneObjects(std::vector<SceneObject>& objects, 
						  SelectionType& st, 
						  float& w, float& h, 
						  vec3& min, vec3& max, 
						  vec3& pos); // this simply finds the scene objects within selection region
	void setCurrentDistribution(Distribution d);
	void setCurrentDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos); // makes a distribution for you
	void saveDistribution(Distribution d, int index); // saves d in index of palette
	void saveDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos, int index); // makes a distribution for you
	void deleteDistribution(int index); // marks the distribution at this index as empty
	
	// TO DO: Discuss what to do if user tries to save a distribution in a slot that already has a distribution
	// TO DO: Discuss way of assigning distributions unique values, for debugging purposes

	// TO DO: add future editing operations here
	void pasteDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos, int index);

	void updatePriorityOrder(std::vector<int> &newOrder); // assumes newOrder.size == priorityOrder.size

	static std::vector<CATEGORY> priorityOrder; // priority order - items in the beginning are prioritized

	~WorldPalette();
};

#endif