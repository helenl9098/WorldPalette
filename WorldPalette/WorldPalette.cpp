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

    printString(MString("Finding Scene Objects.."), MString(""));

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

void WorldPalette::pasteDistribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos, int index) {
	// First delete the existing geometry within the region
	std::vector<SceneObject> geomToDelete;
	findSceneObjects(geomToDelete, st, w, h, min, max, pos); // find the objects within the selection region
	for (SceneObject geom : geomToDelete) {
		MGlobal::executeCommand("select -r " + geom.name); // Select the geometry
		MGlobal::executeCommand("delete"); // Delete the geometry
	}

	// Find the positions where the new geometry will be created (THIS IS HARDCODED ATM)
    // This is currently using a "geomId" counter to give unique names to each pasted new geometry
    // Not sure if it's necessary though since Maya might be handling that already
    // TODO : Use Metropolis-Hasting to generate the geometry to paste
	std::vector<SceneObject> geomToPaste;
	for (int i = -1; i < 2; ++i) {
		MString name((std::string("NewSphere") + std::to_string(geomId)).c_str());
		geomToPaste.push_back(SceneObject(LAYER::VEGETATION, CATEGORY::HOUSE, DATATYPE::DISTRIBUTION, vec3(i, 0, i), name));
		geomId++;
	}
	
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