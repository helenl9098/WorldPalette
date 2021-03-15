#pragma once
#include "Distribution.h"

/*
*
* This class will hold all of our saved distributions. It will also know our currently selected distribution. 
* We initialize one instance of this class in the beginning of our plugin, and it will be altered by all future operations.
*
*/
class WorldPalette
{
public: 
	/*
	* IMPORTANT NOTE: All the distirbutions are initialized on the HEAP (for memory purposes). The WorldPalette owns all these distirbutions, so it's in charge of freeing them. 
	* Make sure that all distributions are tracked by WorldPalette to avoid memory leaks.
	*/
	int maxPaletteSize = 10; // if you change this, change the array static initialization below
	Distribution* palette[10]; // saved distributions in the palette. MAX 10 FOR NOW.
	Distribution* currentlySelectedRegion; // the most recent selected region. This is for copy & pasting and moving, which don't require saving a distribution in a palette. 

public: 
	WorldPalette();
	void setCurrentDistribution(Distribution* d);
	void setCurrentDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos); // makes a distribution for you
	void saveDistribution(Distribution* d, int index); // saves d in index of palette
	void saveDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos, int index); // makes a distribution for you
	void deleteDistribution(int index); // deletes the distribution located at that index in the vector
	
	// TO DO: add future editing operations here

	~WorldPalette();
};

