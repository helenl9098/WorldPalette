#include "WorldPalette.h"

std::vector<CATEGORY> WorldPalette::priorityOrder = { CATEGORY::HOUSE, CATEGORY::TREE, CATEGORY::ROCK }; // default order

WorldPalette::WorldPalette() {
	printString(MString("Created World Palette Object"), MString(""));
}

void WorldPalette::findSceneObjects(std::vector<SceneObject>& objsFound, 
                                    SelectionType& st, 
                                    float& w, float& h, 
                                    vec3& min, vec3& max,
                                    vec3& pos) {
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
                // We want only the shape, not the transform-extended-to-shape.
                MString name = dagPath.partialPathName();

                // 1. ignore the selection region
                bool objectInRegion = false;
                if (name == MString("selectionRegion")) {
                    continue;
                }

                // 2. for all geometry, find its transform/position
                MFnTransform fnTransform(dagPath);
                MVector trans = fnTransform.getTranslation(MSpace::kWorld, &stat);

                // 3. check if position is inside bounding box
                if (st == SelectionType::PLANAR) {
                    if (trans[0] > min[0]  // if greater than min X
                        && trans[0] < max[0] // if less than max X
                        && trans[2] > min[2] // if greater than min z
                        && trans[2] < max[2]) { // if less than max z

                        // the object is in the bounding box!
                        objectInRegion = true;
                    }
                }
                else if (st == SelectionType::RADIAL) {
                    if (Distance(vec3(trans[0], trans[1], trans[2]), pos) < w) {

                        // the object is in the bounding box!
                        objectInRegion = true;
                    }
                }

                // TO DO: Figure out y height

                // 4. if so, we create a scene object struct
                if (objectInRegion) {
                    // 5. fill in scene object struct (we only need the name)
                    SceneObject obj{};
                    obj.name = name;

                    // 6. push scene object back in vector
                    objsFound.push_back(obj);
                }
                continue;
            }
        }
    }
}

void WorldPalette::setCurrentDistribution(Distribution d) {
	currentlySelectedRegion = d;
}

void WorldPalette::setCurrentDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos) {
	currentlySelectedRegion = Distribution(st, w, h, min, max, pos, w);
}
void WorldPalette::saveDistribution(Distribution d, int index) {
	if (index < maxPaletteSize) {
		palette[index] = d;
		currentlySelectedRegion = d;
	}
}

void WorldPalette::saveDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos, int index) {
	if (index < maxPaletteSize) {
		palette[index] = Distribution(st, w, h, min, max, pos, w);
		currentlySelectedRegion = palette[index];
	}
}

void WorldPalette::deleteDistribution(int index) {
	palette[index].empty = true;
}

int WorldPalette::findIndexGivenCategory(CATEGORY c) {
    int index = -1;
    auto it = find(WorldPalette::priorityOrder.begin(), WorldPalette::priorityOrder.end(), c);

    // If element was found
    if (it != WorldPalette::priorityOrder.end())
    {
        // calculating the index of the current category
        index = it - WorldPalette::priorityOrder.begin();
    }
    else {
        // If the element is not
        // present in the vector
        printString(MString("ERROR: "), MString("CANNOT FIND CATEGORY IN PRIORITY ORDER"));
    }
    return index;
}

/*
float findFXGivenCategory(std::map<CATEGORY, std::vector<SceneObject>>& orderedSceneObjects, int currentCategoryIndex, float increment, Distribution d) {
    float currentCatgeoryFX = 1.0;
    CATEGORY currentCategory = WorldPalette::priorityOrder[currentCategoryIndex];
    for (int y = 0; y < currentCategoryIndex + 1; y++) {
        CATEGORY dependentCategory = WorldPalette::priorityOrder[y];

        if (orderedSceneObjects.find(currentCategory) == orderedSceneObjects.end() ||
            orderedSceneObjects.find(dependentCategory) == orderedSceneObjects.end()) {
#if DEBUG
            printString(MString("*** Nothing to compare to"), MString(""));
#endif
        }
        else {
#if DEBUG
            printString(MString("****************************************************************"), MString(""));
            printFloat(MString("Current Category We are Looking At: "), static_cast<std::underlying_type<CATEGORY>::type>(currentCategory));
            printFloat(MString("Dependent Category We are Looking At: "), static_cast<std::underlying_type<CATEGORY>::type>(dependentCategory));
#endif
            // for each object of the current category, calculate how objects of the dependent category is positioned around it
            for (SceneObject& currentObject : orderedSceneObjects.at(currentCategory)) {
                for (SceneObject& dependentObject : orderedSceneObjects.at(dependentCategory)) {
                    if (dependentObject.name != currentObject.name) {
                        float distanceToCurrent = Distance(dependentObject.position, currentObject.position);
                        int index = floor(distanceToCurrent / increment);

                        if (index < 0 || index >= NUM_BUCKETS) {
#if DEBUG
                            printFloat(MString("*** Distance To Center"), distanceToCurrent);
                            printFloat(MString("***Error: WRONG Index"), index);
#endif
                        }
                        else {
                            float histVal = d.histograms[currentCategoryIndex][y].second[index];
                            currentCatgeoryFX *= histVal;

#if DEBUG
                            printString(MString("This is the fx for this pair:"), MString(""));
                            printFloat(MString("FX: "), histVal);
                            printFloat(MString("FX Product: "), currentCatgeoryFX);
#endif
                        }
                    }
                }
            }

        }
    }

#if DEBUG
    printString(MString("============================================================="), MString(""));
    printFloat(MString("Current Category We are Looking At: "), static_cast<std::underlying_type<CATEGORY>::type>(currentCategory));
    printFloat(MString("FX: "), currentCatgeoryFX);
#endif

    return currentCatgeoryFX;
}

std::vector<float> WorldPalette::calculateExampleFX(Distribution d) {
#if DEBUG
    printString(MString("****************************************************************"), MString(""));
    printString(MString("Calculating FX for example histogram"), MString(""));
#endif

    float increment = d.getHistogramIncrement();
    std::vector<float> fxResults; // contains the fx for every category
    for (int x = 0; x < WorldPalette::priorityOrder.size(); x++) {
        fxResults.push_back(findFXGivenCategory(d.sceneObjects, x, increment, d));
    }
    return fxResults;
}

float WorldPalette::calculateFX(Distribution d, CATEGORY current) {
#if DEBUG
    printString(MString("****************************************************************"), MString(""));
    printString(MString("Calculating FX for iteration histogram"), MString(""));
#endif

    float increment = d.getHistogramIncrement();
    CATEGORY currentCategory = current;
    int x = this->findIndexGivenCategory(current);
    return findFXGivenCategory(d.sceneObjects, x, increment, d);
}
*/

vec3 getRandPosInRegion(SelectionType st, float w, float h, vec3 pos) {
    if (st == SelectionType::PLANAR) {
        float randW = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (w * 2.0f))) - w;
        float randH = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (h * 2.0f))) - h;
        vec3 randLocalPos(randW, 0, randH);
        return randLocalPos;
    }
    else if (st == SelectionType::RADIAL) {
        float randR = w * sqrt(static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
        float randTheta = static_cast <float> (rand()) / static_cast <float> (RAND_MAX) * 2.f * M_PI;
        float randW = randR * cos(randTheta);
        float randH = randR * sin(randTheta);
        vec3 randLocalPos(randW, 0, randH);
        return randLocalPos;
    }
    return vec3(0, 0, 0);
}

std::vector<SceneObject> WorldPalette::metropolisHastingSampling(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos) {

    // 1. Create a empty selected region that will represent our area to paste into
    SelectedRegion pasteRegion(SelectionType::NONE, w, h, min, max, pos);
    pasteRegion.selectionType = st;

    // 2. Declare a vector of scene objects that will store the generated meshes
    std::vector<SceneObject> result;

    // 3. Initialize a random distribution depending on the density of the currently selected sample
    float buffer = 1.0; // we don't want to generate meshes right at the edge
    float density = this->currentlySelectedRegion.selectedRegion.objects.size() / this->currentlySelectedRegion.selectedRegion.getArea();
    int numElements = ceil(density * pasteRegion.getArea());
#if DEBUG
    printFloat(MString("Density: "), density);
    printFloat(MString("Original NumElements in Paste Area: "), numElements);
#endif
    if (numElements < 1) {
        printString(MString("Error: "), MString("< 1 num elements in new area"));
        return result;
    }
    for (int i = 0; i < numElements; i++) {
        // make a random position in the region's bound
        vec3 randLocalPos = getRandPosInRegion(st, w - buffer, h - buffer, pos);
        MString name((std::string("NewSphere") + std::to_string(i)).c_str());
        result.push_back(SceneObject(LAYER::VEGETATION, CATEGORY::HOUSE, DATATYPE::DISTRIBUTION, randLocalPos, name));
    }

    // 4. calculate the example histogram f(x) 
    //std::vector<float> exampleFX = calculateExampleFX(this->currentlySelectedRegion);

    // 5. Go through a fixed number of iterations
    // example density
    float exampleDensity = this->currentlySelectedRegion.selectedRegion.objects.size() / this->currentlySelectedRegion.selectedRegion.getArea();
    for (int i = 0; i < NUM_ITERS; i++) {
        
        pasteRegion.objects = result;
        Distribution oldDist(pasteRegion, currentlySelectedRegion.selectedRegion.width);

        // 6. For each iteration, decide whether element birth or death
        float probability = rand() % 100;
        if (probability < 50) {
            // Element Birth
            // --- a) add element at random location
            vec3 randLocation = getRandPosInRegion(st, w - buffer, h - buffer, pos);

            // --- b) assign a random category to it // TO DO : IT'S ALL THE SAME CATEGORY FOR NOW
            MString name((std::string("NewSphere") + std::to_string(result.size())).c_str());
            CATEGORY assignedCategory = CATEGORY::HOUSE;
            result.push_back(SceneObject(LAYER::VEGETATION, assignedCategory, DATATYPE::DISTRIBUTION, randLocation, name));

            // --- c) generate new histogram with this new element
            pasteRegion.objects = result;
            Distribution tmpDist(pasteRegion, currentlySelectedRegion.selectedRegion.width); // this will automatically generate the histograms for us

            // --- d) calculate acceptance rate based on example histogram
            //float FXP = calculateFX(tmpDist, assignedCategory);
            //float FX = exampleFX[findIndexGivenCategory(assignedCategory)];
            //float acceptanceRate = (FXP / FX) * (pasteRegion.getArea() / result.size());

            // TO DO: only comparing HOUSE vs HOUSE right now
            float oldRatio = 0;
            float newRatio = 0;
            for (int x = 0; x < NUM_BUCKETS; x++) {
                if (this->currentlySelectedRegion.histograms[0][0].second[x] == 0) {
                    oldRatio += oldDist.histograms[0][0].second[x];
                    newRatio += tmpDist.histograms[0][0].second[x];
                }
                else {
                    if (oldDist.histograms[0][0].second[x] != 0) {
                        oldRatio += oldDist.histograms[0][0].second[x] / this->currentlySelectedRegion.histograms[0][0].second[x];
                    }
                    if (tmpDist.histograms[0][0].second[x] != 0) {
                        newRatio += tmpDist.histograms[0][0].second[x] / this->currentlySelectedRegion.histograms[0][0].second[x];
                    }
                }
            }
            
#if DEBUG
            printFloat(MString("Current Iteration: "), i);
            //printFloat(MString("(BIRTH) Acceptance Rate: "), acceptanceRate);
            printFloat(MString("(BIRTH) Old Ratio: "), oldRatio);
            printFloat(MString("(BIRTH) New Ratio: "), newRatio);
#endif
            if (abs(oldRatio - 1) < abs(newRatio - 1) || result.size() > numElements + ELEMENT_BUFFER) {
                // we do not accep the new scene object
                //printString(MString("(BIRTH DENIED): "), MString(""));
                result.pop_back();
            }
            else {
                // we accept the new scene object
                float currentDensity = result.size() / pasteRegion.getArea();
                if (exampleDensity / currentDensity < 1) {
                    float probability2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    if (probability2 < exampleDensity / currentDensity) {
                        //printString(MString("(BIRTH ACCEPTED): "), MString(""));
                        continue;
                    }
                    else {
                        // we do not accep the new scene object
                        //printString(MString("(BIRTH DENIED): "), MString(""));
                        result.pop_back();
                    }
                }
                else {
                    //printString(MString("(BIRTH ACCEPTED): "), MString(""));
                    continue;
                }
            }

            /*
            // --- e) accept or deny the birth
            if (acceptanceRate > 1) {
                // we accept the new scene object
                printString(MString("(BIRTH ACCEPTED) >1: "), MString(""));
                continue;
            }
            else {
                float probability2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                if (probability2 < acceptanceRate) {
                    // we accept the new scene object
                    printString(MString("(BIRTH ACCEPTED) <1: "), MString(""));
                    continue;
                }
                else {
                    // we do not accep the new scene object
                    printString(MString("(BIRTH DENIED) <1: "), MString(""));
                    result.pop_back();
                }
            } */
        }
        else {
            // Element Death
            // --- a) select a random index to delete
            int randIndex = rand() % result.size();

            // --- b) delete element (save tmp copy in case death is denied)
            SceneObject tmpObj = result[randIndex];
            result.erase(result.begin() + randIndex);

            // --- c) generate new histogram with this new element removed
            pasteRegion.objects = result;
            Distribution tmpDist(pasteRegion, currentlySelectedRegion.selectedRegion.width); // this will automatically generate the histograms for us

            // --- d) calculate acceptance rate based on example histogram 
            //float FXP = calculateFX(tmpDist, tmpObj.category);
            //float FX = exampleFX[findIndexGivenCategory(tmpObj.category)];
            //float acceptanceRate = (FX / FXP) * (result.size() / pasteRegion.getArea());

            // TO DO: only comparing HOUSE vs HOUSE right now
            float oldRatio = 0;
            float newRatio = 0;
            for (int x = 0; x < NUM_BUCKETS; x++) {
                if (this->currentlySelectedRegion.histograms[0][0].second[x] == 0) {
                    oldRatio += oldDist.histograms[0][0].second[x];
                    newRatio += tmpDist.histograms[0][0].second[x];
                }
                else {
                    if (oldDist.histograms[0][0].second[x] != 0) {
                        oldRatio += oldDist.histograms[0][0].second[x] / this->currentlySelectedRegion.histograms[0][0].second[x];
                    }
                    if (tmpDist.histograms[0][0].second[x] != 0) {
                        newRatio += tmpDist.histograms[0][0].second[x] / this->currentlySelectedRegion.histograms[0][0].second[x];
                    }
                }
            }
#if DEBUG
            printFloat(MString("Current Iteration: "), i);
            //printFloat(MString("(DEATH) Acceptance Rate: "), acceptanceRate);
            printFloat(MString("(DEATH) Old Ratio: "), oldRatio);
            printFloat(MString("(DEATH) New Ratio: "), newRatio);
#endif
            if (abs(oldRatio - 1) < abs(newRatio - 1) || result.size() < numElements - ELEMENT_BUFFER) {
                // we do not accept the death
                //printString(MString("(DEATH DENIED): "), MString(""));
                result.push_back(tmpObj);
            }
            else {
                // we accept the death
                float currentDensity = result.size() / pasteRegion.getArea();
                if (currentDensity / exampleDensity < 1) {
                    float probability2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                    if (probability2 < currentDensity / exampleDensity) {
                        //printString(MString("(DEATH ACCEPTED): "), MString(""));
                        continue;
                    }
                    else {
                        // we do not accep the new scene object
                        //printString(MString("(DEATH DENIED): "), MString(""));
                        result.push_back(tmpObj);
                    }
                }
                else {
                    //printString(MString("(DEATH ACCEPTED): "), MString(""));
                    continue;
                }
            }

            /*
            // --- e) accept or deny the death
            if (acceptanceRate > 1) {
                // we accept the new scene object
                printString(MString("(DEATH ACCEPTED) >1: "), MString(""));
                continue;
            }
            else {
                float probability2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                if (probability2 < acceptanceRate) {
                    // we accept the new scene object
                    printString(MString("(DEATH ACCEPTED) <1: "), MString(""));
                    continue;
                }
                else {
                    // we do not accep the new scene object
                    printString(MString("(DEATH DENIED) <1: "), MString(""));
                    result.push_back(tmpObj);
                }
            } */
        }
    
    }

    return result;
}

void WorldPalette::pasteDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos, int index) {

    // Do some error checks
    if (st == SelectionType::NONE) {
        printString(MString("Error: "), MString("Cannot paste in a none-type selection region"));
        return;
    }
    
    /* TO DO: Figure out how to check this
    if (!this->currentlySelectedRegion) {
        printString(MString("Error: "), MString("Need example selection first!"));
        return;
    } */

	// First delete the existing geometry within the region
	std::vector<SceneObject> geomToDelete;
	findSceneObjects(geomToDelete, st, w, h, min, max, pos); // find the objects within the selection region
	for (SceneObject geom : geomToDelete) {
		MGlobal::executeCommand("select -r " + geom.name); // Select the geometry
		MGlobal::executeCommand("delete"); // Delete the geometry
	}

	// Find the positions where the new geometry will be created
    std::vector<SceneObject> geomToPaste = metropolisHastingSampling(st, w, h, min, max, pos);
	
	// Use "executeCommand" to create the new geometry (for now this will output 3 spheres on a diagonal)
	for (SceneObject geom : geomToPaste) {
		MGlobal::executeCommand("polySphere -r 0.5 -n " + geom.name);
		// Get the world position of the geometry
		vec3 wpos = geom.position + pos;
		MGlobal::executeCommand((std::string("move -x ") + std::to_string(wpos[0]) + std::string(" -z ") + std::to_string(wpos[2])).c_str());
	}
}

void WorldPalette::updatePriorityOrder(std::vector<int>& newOrder) {
	for (int i = 0; i < newOrder.size(); ++i) {
		WorldPalette::priorityOrder[i] = static_cast<CATEGORY>(newOrder[i]);
	}
}

WorldPalette::~WorldPalette()
{}