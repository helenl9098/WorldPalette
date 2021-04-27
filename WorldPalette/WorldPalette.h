#pragma once
#ifndef WORLDPALETTE_H
#define WORLDPALETTE_H
#include "Distribution.h"
#include "Terrain.h"
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

private: 
	vec3 moveOldPosition; // this is used for the move undo button. Essentially it stores the position of the region before move.
	std::vector<std::pair<CATEGORY, vec3>> pasteOldGeometry; // this is used for the paste undo button. stores all the geometry that used to be there.
	bool pasteUndo; // this is used for the paste undo button. This tells us if we can undo our last paste.
	std::vector<MString> pasteAddedObjects; // this is used fo the paste undo button. stores all the geometry added in teh previous paste.
	std::vector<std::pair<CATEGORY, vec3>> brushOldGeometry; // this is used for the brush undo button. stores all the geometry that used to be there.
	bool brushUndo; // this is used for the brush undo button. This tells us if we can undo our last brush.
	std::vector<MString> brushAddedObjects; // this is used fo the brush undo button. stores all the geometry added in teh previous brush operation.
	std::vector<std::pair<CATEGORY, vec3>> eraseOldGeometry; // this is used for the erase undo button. stores all the geometry that used to be there.
	bool eraseUndo;
	std::vector<std::pair<CATEGORY, vec3>> clearOldGeometry; // this is used for the clear undo button. stores all the geometry that used to be there.
	bool clearUndo;


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

	int findIndexGivenCategory(CATEGORY c);

	float calculateRatio(Distribution& tmpDist);
	std::vector<SceneObject> metropolisHastingSampling(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos, float influenceRadius, std::vector<SceneObject>& influenceObject); // generates a vector of scene objects in the given area using the Metropolis-Hastings Sampling Algorithm
	
	// TO DO: add future editing operations here
	void pasteDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos, int index);
	void pasteDistributionUndo(); // Button should call this function
	void moveDistribution(float dx, float dz);
	void moveDistributionSave();
	void moveDistributionUndo(); // Button should call this function
	void brushDistribution(float brushWidth);
	void brushDistributionUndo(); // Button should call this function
	void eraseDistribution(float brushWidth);
	void eraseDistributionUndo(); // Button should call this function
	void clearDistribution();
	void clearDistributionUndo(); // Button should call this function

	void updatePriorityOrder(std::vector<int> &newOrder); // assumes newOrder.size == priorityOrder.size

	// Static variables
	static std::vector<CATEGORY> priorityOrder; // priority order - items in the beginning are prioritized
	static Terrain terrain; // World terrain
	static std::vector<MString> objNames; // names of scene objects to duplicate
	static std::vector<vec3> brushStrokes; // list of brush stroke center positions during brush drag

	~WorldPalette();
};

#endif