#include "Distribution.h"
#include "WorldPalette.h"

SelectedRegion::SelectedRegion(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos)
	: selectionType(st), position(pos), width(w), height(h), minBounds(min), maxBounds(max), radius(w)
{
	if (st != SelectionType::NONE) {
        addSceneObjectsToVector();
	}
}

float SelectedRegion::getArea() {
	if (selectionType == SelectionType::PLANAR) {
		return width * 2.0f * height * 2.0f;
	} else if (selectionType == SelectionType::RADIAL) {
		return M_PI * radius * radius;
	}
}

DATATYPE getType(CATEGORY c) {
    if (c == CATEGORY::ROCK || c == CATEGORY::TREE || c == CATEGORY::HOUSE) {
        return DATATYPE::DISTRIBUTION;
    }
    else {
        return DATATYPE::GRAPH;
    }
}

LAYER getLayer(CATEGORY c) {
    if (c == CATEGORY::ROCK || c == CATEGORY::TREE) {
        return LAYER::VEGETATION;
    }
    else {
        return LAYER::SETTLEMENT;
    }
}

/*
* Goes through all the scene objects and finds the ones that are currently in the selected region
* Looked at : https://download.autodesk.com/us/maya/2010help/api/obj_export_8cpp-example.html
*/
void SelectedRegion::addSceneObjectsToVector() {
	printString(MString("Adding Scene Objects to Vector.."), MString(""));

	MStatus stat;

	// Get all of the sets in maya and put them into
	// a selection list
	MStringArray result;
	MGlobal::executeCommand("ls -transforms", result);
	MSelectionList* setList = new MSelectionList;
	int length = result.length();
	for (int i = 0; i < length; i++)
	{
		setList->add(result[i]);
	}

	// Extract each set as an MObject and add them to the
	// sets array.
	// We may be excluding groups, matierials, or ptGroups
	// in which case we can ignore those sets. 
	//
	MObject mset;
	MObjectArray* sets = new MObjectArray();
	length = setList->length();
	for (int i = 0; i < length; i++)
	{
		setList->getDependNode(i, mset);

		//MFnSet fnSet(mset, &stat);
		MFnTransform fnSet(mset, &stat);
	}
	delete setList;

	int numSets = sets->length();

    //
    // Do a dag-iteration and for every mesh found, create facet and
        // vertex look-up tables. These tables will keep track of which
        // sets each component belongs to.
        //
        // If exportAll is false then iterate over the activeSelection 
        // list instead of the entire DAG.
        //
        // These arrays have a corrisponding entry in the name
        // stringArray.
        //
    MIntArray vertexCounts;
    MIntArray polygonCounts;

        MItDag dagIterator(MItDag::kBreadthFirst, MFn::kInvalid, &stat);

        if (MS::kSuccess != stat) {
            fprintf(stderr, "Failure in DAG iterator setup.\n");
            return;
        }

        for (; !dagIterator.isDone(); dagIterator.next())
        {
            MDagPath dagPath;
            stat = dagIterator.getPath(dagPath);

            if (stat)
            {
                // skip over intermediate objects
                MFnDagNode dagNode(dagPath, &stat);
                if (dagNode.isIntermediateObject())
                {
                    continue;
                }

                if ((dagPath.hasFn(MFn::kMesh)) &&
                    (dagPath.hasFn(MFn::kTransform)))
                {
                    // We want only the shape, 
                    // not the transform-extended-to-shape.
                    MString name = dagPath.partialPathName();
                    //printString("Mesh + Transform Option: ", name);

                    // 1. ignore the selection region, mesh objs and the terrain
                    bool objectInRegion = false;
                    if (name == MString("selectionRegion")) {
                        continue;
                    }
                    if (name == MString("tree:Tree") || name == MString("big_rock:Rock")) {
                        continue;
                    }
                    if (name == WorldPalette::terrain.name) {
                        continue;
                    }

                    // 2. for all geometry, find its transform/position
                    MFnTransform fnTransform(dagPath);
                    MVector trans = fnTransform.getTranslation(MSpace::kWorld, &stat);

                    // 3. check if position is inside bounding box
                    if (this->selectionType == SelectionType::PLANAR) {
                        if (trans[0] > minBounds[0]  // if greater than min X
                            && trans[0] < maxBounds[0] // if less than max X
                            && trans[2] > minBounds[2] // if greater than min z
                            && trans[2] < maxBounds[2]) { // if less than max z
                            
                            // the object is in the bounding box!
                            objectInRegion = true;
                        }
                    }
                    else if (this->selectionType == SelectionType::RADIAL) {
                        if (Distance(vec3(trans[0], trans[1], trans[2]), this->position) < this->radius) {

                            // the object is in the bounding box!
                            objectInRegion = true;
                        }
                    }

                    // TO DO: Figure out y height

                    // 4. if so, we create a scene object struct
                    if (objectInRegion) {
                        // 5. fill in scene object struct with type & position
                        SceneObject obj{};
                        obj.position = vec3(trans[0], trans[1], trans[2]) - this->position;

                        // TO DO: CHANGE DATA TYPES LATER
                        obj.layer = LAYER::VEGETATION;
                        obj.datatype = DATATYPE::DISTRIBUTION;
                        obj.category = CATEGORY::HOUSE;
                        obj.name = name;

                        // 6. push scene object back in vector
                        this->objects.push_back(obj);

#if DEBUG
                        printString("Adding this to the objects vector: ", name);
                        printVec3(MString("Located at: "), vec3(trans[0], trans[1], trans[2]));
                        printVec3(MString("Relative Position: "), vec3(obj.position[0], obj.position[1], obj.position[2]));
#endif
                    }

                    continue;
                }
            }
        }
}

SelectedRegion::~SelectedRegion() {}

// default constructor that makes an empty distribution
Distribution::Distribution() 
	: Distribution(SelectedRegion(SelectionType::NONE, 0, 0, vec3(0, 0, 0), vec3(0, 0 , 0), vec3(0, 0, 0)), 0)
{}

Distribution::Distribution(SelectedRegion r, float radius)
    : selectedRegion(r), empty(false)
{
    if (r.selectionType == SelectionType::NONE) {
        empty = true;
    }

    // calculate histograms
    if (!empty) {
        calculateHistograms(radius);
    }
}

Distribution::Distribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos, float radius)
	: Distribution(SelectedRegion(st, w, h, min, max, pos), radius)
{}

Distribution::~Distribution() {}

std::map<CATEGORY, std::vector<SceneObject>> Distribution::sortObjects() {
    // map the vector of scene objects to the category (doesn't exist in particular order)
    std::map<CATEGORY, std::vector<SceneObject>> orderedSceneObjects;
    // iterate through all the scene objects
    for (int i = 0; i < this->selectedRegion.objects.size(); i++) {
        SceneObject currentObject = this->selectedRegion.objects[i];
        if (orderedSceneObjects.find(currentObject.category) == orderedSceneObjects.end()) {
            // not found
            std::vector<SceneObject> tmpObjs;
            tmpObjs.push_back(currentObject);
            orderedSceneObjects.insert(std::pair<CATEGORY, std::vector<SceneObject>>(currentObject.category, tmpObjs));
        }
        else {
            // found
            orderedSceneObjects.at(currentObject.category).push_back(currentObject);
        }
    }

#if DEBUG
    for (auto const& x : orderedSceneObjects)
    {
        printFloat(MString("Category: "), static_cast<std::underlying_type<CATEGORY>::type>(x.first));
        printFloat(MString("Items: "), x.second.size());
    }
#endif
    this->sceneObjects = orderedSceneObjects;
    return orderedSceneObjects;
}

float Distribution::getHistogramIncrement(float radius) {
    // calculate the distance of each bucket
    float increment = 0.f;
    if (this->selectedRegion.selectionType == SelectionType::PLANAR) {
        float distance = Distance(this->selectedRegion.maxBounds, this->selectedRegion.position);
        increment = (distance * 2.0) / NUM_BUCKETS;
    }
    else if (this->selectedRegion.selectionType == SelectionType::RADIAL) {
        //increment = (this->selectedRegion.radius * 2.0) / NUM_BUCKETS;
        increment = (radius * 1.5) / NUM_BUCKETS;
    }
    return increment;
}

// Returns a histogram for this type of distribution
void Distribution::radialDistribution(std::map<CATEGORY, std::vector<SceneObject>>& orderedSceneObjects, CATEGORY current, CATEGORY dependent, std::vector<float>& histogram, float radius) {
    int numObjects = 0;

    // calculate the distance of each bucket
    float increment = this->getHistogramIncrement(radius);

    // initialize all the buckets to be 0 
    for (int i = 0; i < NUM_BUCKETS; i++) {
        histogram.push_back(0);
    }

#if DEBUG
    printFloat(MString("*** We've expanded the histogram to be of size: "), NUM_BUCKETS);
    printFloat(MString("*** The increment is of size: "), increment);
#endif

    // if there are no elements with this category, we don't add anything.
    if (orderedSceneObjects.find(current) == orderedSceneObjects.end() || 
        orderedSceneObjects.find(dependent) == orderedSceneObjects.end()) {
#if DEBUG
        printString(MString("*** Nothing to compare to"), MString(""));
#endif
        return;
    }

    // for each object of the current category, calculate how objects of the dependent category is positioned around it
    for (SceneObject& currentObject : orderedSceneObjects.at(current)) {
        for (SceneObject& dependentObject : orderedSceneObjects.at(dependent)) {
            if (dependentObject.name != currentObject.name) {
                float distanceToCurrent= Distance(dependentObject.position, currentObject.position);
                int index = floor(distanceToCurrent / increment);

                if (index < 0 || index >= NUM_BUCKETS) {
#if DEBUG
                    printFloat(MString("*** Distance To Center"), distanceToCurrent);
                    printFloat(MString("*** WRONG Index"), index);
#endif
                }
                else {
                    histogram[index]++;
                    numObjects++;
                }
            }
        }
    }

    // normalize histogram
    float totalArea = this->selectedRegion.getArea();
    for (int i = 0; i < NUM_BUCKETS; i++) {
        // calculate the annular shell area
        float innerRadius = i * increment;
        float outerRadius = (i + 1) * increment;
        float innerArea = M_PI * innerRadius * innerRadius;
        float outerArea = M_PI * outerRadius * outerRadius;

        float shellArea = outerArea - innerArea;
        histogram[i] *= totalArea / (shellArea * numObjects);
    }
}

void Distribution::calculateHistograms(float radius) {
#if DEBUG
    printString(MString("****************************************************************"), MString(""));
    printString(MString("Calculating histograms"), MString(""));
#endif

    static std::vector<CATEGORY> priorityList = WorldPalette::priorityOrder;

    // **************************
    //1. organize the scene objects by category
    // **************************
    // map the vector of scene objects to the category (doesn't exist in particular order)
    this->sortObjects();

    // *****************************
    // 2. in priority order, we look at the categories, and make pairs
    // *****************************
    for (int x = 0; x < priorityList.size(); x++) {
        CATEGORY currentCategory = priorityList[x];
        std::vector<std::pair<CATEGORY, std::vector<float>>> currentCategoryHistograms;
        for (int y = 0; y < x + 1; y++) {
            CATEGORY dependentCategory = priorityList[y];
            // TO DO: Check the dependent category's type. For now, we assume it's always distribution
            DATATYPE dependentType = DATATYPE::DISTRIBUTION;
            
            // *****************************
            // 3. for each pair, depending on the dependent category's type, we generate histograms in a certain way
            // *****************************
            if (dependentType == DATATYPE::DISTRIBUTION) {
#if DEBUG
                printString(MString("****************************************************************"), MString(""));
                printFloat(MString("Current Category We are Looking At: "), static_cast<std::underlying_type<CATEGORY>::type>(currentCategory));
                printFloat(MString("Dependent Category We are Looking At: "), static_cast<std::underlying_type<CATEGORY>::type>(dependentCategory));
#endif
                std::vector<float> histogram;
                radialDistribution(this->sceneObjects, currentCategory, dependentCategory, histogram, radius);
                std::pair<CATEGORY, std::vector<float>> pairwiseHistogram(dependentCategory, histogram);
                currentCategoryHistograms.push_back(pairwiseHistogram);

#if DEBUG
                printString(MString("This is their pair wise histogram (RADIAL DIST):"), MString(""));
                printFloat(MString("Histogram Size"), histogram.size());
                for (int i = 0; i < histogram.size(); i++) {
                    printFloat(MString("Bucket #"), i);
                    printFloat(MString("*** Number of Elements:"), histogram[i]);
                }
#endif

            } // TO DO: Add else if for different data types
        } 

        // *****************************
        // 4. Add pairwise histogram to overall chart
        // now that we are all done calculating the pairwise histograms for this category, we add it to the histograms member variable
        // this should push it back in priority order because we start the loop from 0 (most important)
        // *****************************
        this->histograms.push_back(currentCategoryHistograms);
    }

}