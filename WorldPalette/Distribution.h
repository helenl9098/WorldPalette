#pragma once

#include "vec.h"
#include <vector>
#include "helper.h"
#include <maya/MSelectionList.h>
#include <maya/MObjectArray.h>
#include <maya/MFnSet.h>
#include <maya/MFnTransform.h>
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MVector.h>
#include <maya/MString.h>

enum class LAYER { VEGETATION, SETTLEMENT }; // unused right now... unclear if we will be doing layer stuff
enum class CATEGORY { ROCK = 0, TREE = 1, HOUSE = 2 }; // can add more later
enum class DATATYPE { DISTRIBUTION, GRAPH, EXTERNAL }; // this is all the possibilities!

struct SceneObject {
	LAYER layer;
	CATEGORY category;
	DATATYPE datatype; 
	vec3 position; // this is the local position, as if the selected region is located at 0,0,0
	MString name;
};

enum class SelectionType {
	NONE,
	PLANAR,
	RADIAL,
};

class SelectedRegion
{
public: 
	SelectionType selectionType;
	std::vector<SceneObject> objects; // objects in the selected region
	
	// if bounding box region
	float width; // x axis only in one direction, so the true width is twice as long
	float height; // z axis only in one direction, so the true height is twice as long
	vec3 minBounds;
	vec3 maxBounds;

	// if circle region
	float radius; // if circle

	// all regions
	vec3 position; // in world space


public: 
	SelectedRegion(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos);
	float getArea();
	~SelectedRegion();

private: 
	void findSceneObjects(); // this populates the objects vector, will be automatically called by the constructor. 
};


class Distribution
{
public: 
	bool empty; // is this an empty distribution? aka if the user selected nothing
	SelectedRegion selectedRegion;

	// TO DO: Add more about histograms here

public:
	Distribution(); // default constructor that makes an empty distribution
	Distribution(SelectedRegion r);
	Distribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos); // makes a selected region for you
	~Distribution();
};

