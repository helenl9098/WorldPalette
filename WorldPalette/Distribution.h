#pragma once

#include "vec.h"
#include <vector>
#include "helper.h"
#include <map>
#include <maya/MSelectionList.h>
#include <maya/MObjectArray.h>
#include <maya/MFnSet.h>
#include <maya/MFnTransform.h>
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MVector.h>
#include <maya/MString.h>
#include <string>  

enum class LAYER { VEGETATION = 0, SETTLEMENT = 1}; // unused right now... unclear if we will be doing layer stuff
enum class CATEGORY { GRASS = 0, ROCK = 1, SHRUB = 2, TREE = 3}; // can add more later
enum class DATATYPE { DISTRIBUTION = 0, GRAPH = 1, EXTERNAL = 2}; // this is all the possibilities!

DATATYPE getType(CATEGORY c);
LAYER getLayer(CATEGORY c);

struct SceneObject {
	LAYER layer;
	CATEGORY category;
	DATATYPE datatype; 
	vec3 position; // this is the local position, as if the selected region is located at 0,0,0
	MString name;
	SceneObject() : layer(LAYER::VEGETATION), category(CATEGORY::GRASS), datatype(DATATYPE::DISTRIBUTION), position(vec3()), name(MString()) {}
	SceneObject(LAYER l, CATEGORY c, DATATYPE d, vec3 p, MString n) : layer(l), category(c), datatype(d), position(p), name(n) {}
};

enum class SelectionType {
	NONE = 0,
	PLANAR = 1,
	RADIAL = 2,
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
	void addSceneObjectsToVector(); // this populates the objects vector, will be automatically called by the constructor.
};


class Distribution
{
public: 
	bool empty; // is this an empty distribution? aka if the user selected nothing
	SelectedRegion selectedRegion;
	std::map<CATEGORY, std::vector<SceneObject>> sceneObjects;

	// this is what stores all of our histograms
	// the histograms are pushed back in order of category priority into the vector, from highest priority to least priority
	// then, for each catgeory, we have the vector of all its pairwise histograms
	// for each histogram, we have a pair describing the dependent category, and vector for the histogram buckets
	vector<vector<std::pair<CATEGORY, std::vector<float>>>> histograms;

public:
	Distribution(); // default constructor that makes an empty distribution
	Distribution(SelectedRegion r, float radius);
	Distribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos, float radius); // makes a selected region for you
	~Distribution();

	std::map<CATEGORY, std::vector<SceneObject>> sortObjects();
	float getHistogramIncrement(float radius);

private: 
	void calculateHistograms(float radius);
	void radialDistribution(std::map<CATEGORY, std::vector<SceneObject>>& orderedSceneObjects, CATEGORY current, CATEGORY dependent, std::vector<float>& histogram, float radius);
};

