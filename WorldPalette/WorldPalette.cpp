#include "WorldPalette.h"
#include <algorithm>
#include "vec.h"

std::vector<CATEGORY> WorldPalette::priorityOrder = { CATEGORY::HOUSE, CATEGORY::TREE, CATEGORY::ROCK }; // default order
Terrain WorldPalette::terrain = Terrain(); // default initialization
std::vector<vec3> WorldPalette::brushStrokes; // initially an empty list

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

float WorldPalette::calculateRatio(Distribution& tmpDist) {
    float newRatio = 0;
    for (int i = 0; i < currentlySelectedRegion.histograms.size(); i++) {
        for (int j = 0; j < currentlySelectedRegion.histograms[i].size(); j++) {
            for (int x = 0; x < NUM_BUCKETS; x++) {
                if (this->currentlySelectedRegion.histograms[i][j].second[x] == 0) {
                    if (tmpDist.histograms[i][j].second[x] != 0) {
                        newRatio += tmpDist.histograms[i][j].second[x] * 20.0;
                    }
                }
                else {
                    newRatio += abs(this->currentlySelectedRegion.histograms[i][j].second[x] - tmpDist.histograms[i][j].second[x]);
                }
            }
        }
    }
    return newRatio;
}

std::vector<SceneObject> WorldPalette::metropolisHastingSampling(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos) {

    float buffer = 1.0; // we don't want to generate meshes right at the edge

    // 1. Create a empty selected region that will represent our area to paste into
    SelectedRegion pasteRegion(SelectionType::NONE, w, h, min, max, pos);
    pasteRegion.selectionType = st;

    // 2. Declare a vector of scene objects that will store the generated meshes
    std::vector<SceneObject> result;


    // 3. Initialize a random distribution depending on the density of the currently selected sample
    for (std::pair<CATEGORY, std::vector<SceneObject>> category : currentlySelectedRegion.sceneObjects) {

        float density = category.second.size() / this->currentlySelectedRegion.selectedRegion.getArea();
        int numElements = std::max(5.0f, ceil(density * pasteRegion.getArea()));
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
            CATEGORY assignedCat = category.first;
            result.push_back(SceneObject(getLayer(assignedCat), assignedCat, getType(assignedCat), randLocalPos, name));
        }
    }

    int numElements = result.size();

    // 5. Go through a fixed number of iterations
    pasteRegion.objects = result;
    Distribution oldDist(pasteRegion, currentlySelectedRegion.selectedRegion.width); // calculate the histograms for the current distribution
    float oldRatio = calculateRatio(oldDist);

    for (int i = 0; i < NUM_ITERS; i++) {

        // 6. For each iteration, decide whether element birth or death
        float probability = rand() % 100;
        if (probability < 50) {

            // Element Birth
            // --- a) add element at random location
            vec3 randLocation = getRandPosInRegion(st, w - buffer, h - buffer, pos);

            // --- b) assign a random category to it
            MString name((std::string("NewSphere") + std::to_string(result.size())).c_str());


            int randIndex = rand() % currentlySelectedRegion.selectedRegion.objects.size();
            CATEGORY assignedCategory = currentlySelectedRegion.selectedRegion.objects[randIndex].category;


            //auto it = currentlySelectedRegion.sceneObjects.begin();
            //std::advance(it, rand() % currentlySelectedRegion.sceneObjects.size());
            //CATEGORY assignedCategory = it->first;

            result.push_back(SceneObject(getLayer(assignedCategory), assignedCategory, getType(assignedCategory), randLocation, name));

            // --- c) generate new histogram with this new element
            pasteRegion.objects = result;
            Distribution tmpDist(pasteRegion, currentlySelectedRegion.selectedRegion.width); // this will automatically generate the histograms for us
            float newRatio = calculateRatio(tmpDist);
#if DEBUG
            printFloat(MString("Current Iteration: "), i);
            printFloat(MString("(BIRTH) Old Ratio: "), oldRatio);
            printFloat(MString("(BIRTH) New Ratio: "), newRatio);
#endif
            if (oldRatio < newRatio || result.size() > numElements + floor(numElements * 0.2)) {
                // we do not accept the new scene object
                result.pop_back();
            }
            else {
                SceneObject last = result[result.size() - 1]; // most recently added scene object
                if (last.category == CATEGORY::TREE && WorldPalette::terrain.isInitialized) {
                    // Check for surface normal if the scene object is a tree
                    vec3 worldPos = vec3(last.position[0], 0, last.position[2]) + pos;
                    vec3 surfaceNormal = WorldPalette::terrain.findSurfaceNormalAtPoint(worldPos);
                    float diffuseTerm = Dot(surfaceNormal.Normalize(), vec3(0, 1, 0));
                    if (diffuseTerm < NORM_FACTOR) {
                        // We do not accept the tree if terrain slope is too high at its location
                        result.pop_back();
                    }
                    else {
                        // We accept the tree
                        oldRatio = newRatio;
                        continue;
                    }
                } else {
                    // We accept the new scene object
                    oldRatio = newRatio;
                    continue;
                }
            }
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
                float newRatio = calculateRatio(tmpDist);
#if DEBUG
                printFloat(MString("Current Iteration: "), i);
                printFloat(MString("(DEATH) Old Ratio: "), oldRatio);
                printFloat(MString("(DEATH) New Ratio: "), newRatio);
#endif
                if (oldRatio < newRatio || result.size() < numElements - floor(numElements * 0.2)) {
                    // we do not accept the death
                    result.push_back(tmpObj);
                }
                else {
                    // we accept the death
                    oldRatio = newRatio;
                    continue;
                }
        }
    
    }

    printFloat(MString("Final: "), oldRatio);

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
	
	for (SceneObject geom : geomToPaste) {
		// Get the world position of the geometry
		vec3 wpos = geom.position + pos;
        // Find the height of the geometry
        float height = 0.f;
        int triIdx = 0;
        vec2 coords;
        int res = WorldPalette::terrain.findHeight(height, triIdx, coords, wpos);
        if (res) {
            wpos[1] = height;
        }
        std::string com = ","; // comma
        MGlobal::executeCommand((std::string("addSceneGeometryAtLoc(") + std::to_string(static_cast<std::underlying_type<CATEGORY>::type>(geom.category)) + com 
                                + std::to_string(wpos[0]) + com + std::to_string(wpos[1]) + com + std::to_string(wpos[2]) + std::string(")")).c_str());
	}
}

void WorldPalette::moveDistribution(float dx, float dz) {
    // TO DO: Change this so it's not manually set
    //currentlySelectedRegion.selectedRegion.position += vec3(dx, 0, dz);
    for (SceneObject o : currentlySelectedRegion.selectedRegion.objects) {

        MGlobal::executeCommand("select -r " + o.name); // Select the geometry
        MGlobal::executeCommand((std::string("move -r ") + std::to_string(dx) + std::string(" ") + std::to_string(0.f) + std::string(" ") + std::to_string(dz)).c_str()); // Select the geometry
        MGlobal::executeCommand((std::string("showHidden ") + o.name.asChar()).c_str());

        // account for surface normal: if surface normal is too steep, we 
        // Find the height of the geometry
        vec3 wpos = currentlySelectedRegion.selectedRegion.position + o.position;
        float height = 0.f;
        int triIdx = 0;
        vec2 coords;
        int res = WorldPalette::terrain.findHeight(height, triIdx, coords, wpos);
        if (res) {
            // set the y
            //setAttr pSphere1.translateY 6
            MGlobal::executeCommand((std::string("setAttr ") + o.name.asChar() + std::string(".translateY ") + std::to_string(height)).c_str());
        }

        // hide the object if the surface normal is too steep and object is a tree
        if (o.category == CATEGORY::TREE && WorldPalette::terrain.isInitialized) {
            vec3 worldPos = vec3(currentlySelectedRegion.selectedRegion.position + o.position) + vec3(0, height, 0);
            vec3 surfaceNormal = WorldPalette::terrain.findSurfaceNormalAtPoint(worldPos); // returns the surface normal at given world position
            float diffuseTerm = Dot(surfaceNormal.Normalize(), vec3(0, 1, 0));
            printFloat(MString("Diffuse Term: "), diffuseTerm);
            if (diffuseTerm < NORM_FACTOR) {
                printString(MString("Hiding Object: "), o.name);
                MGlobal::executeCommand((std::string("hide ") + o.name.asChar()).c_str());
            }
        }
    }
}

void WorldPalette::updatePriorityOrder(std::vector<int>& newOrder) {
	for (int i = 0; i < newOrder.size(); ++i) {
		WorldPalette::priorityOrder[i] = static_cast<CATEGORY>(newOrder[i]);
	}
}

WorldPalette::~WorldPalette()
{}