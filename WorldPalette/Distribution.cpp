#include "Distribution.h"

SelectedRegion::SelectedRegion(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos)
	: selectionType(st), position(pos), width(w), height(h), minBounds(min), maxBounds(max), radius(w)
{
	if (st != SelectionType::NONE) {
		findSceneObjects();
	}
}

float SelectedRegion::getArea() {
	if (selectionType == SelectionType::PLANAR) {
		return width * 2.0f * height * 2.0f;
	} else if (selectionType == SelectionType::RADIAL) {
		return M_PI * radius * radius;
	}
}

/*
* Goes through all the scene objects and finds the ones that are currently in the selected region
* Looked at : https://download.autodesk.com/us/maya/2010help/api/obj_export_8cpp-example.html
*/
void SelectedRegion::findSceneObjects() {
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
                    // We want only the shape, 
                    // not the transform-extended-to-shape.
                    MString name = dagPath.partialPathName();
                    //printString("Mesh + Transform Option: ", name);

                    // 1. ignore the selection region
                    bool objectInRegion = false;
                    if (name == MString("selectionRegion")) {
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
                        obj.position = vec3(trans[0], trans[1], trans[2]);

                        // TO DO: CHANGE DATA TYPES LATER
                        obj.layer = LAYER::VEGETATION;
                        obj.datatype = DATATYPE::DISTRIBUTION;
                        obj.category = CATEGORY::HOUSE;
                        obj.name = name;

                        // 6. push scene object back in vector
                        this->objects.push_back(obj);

                        printString("Adding this to the objects vector: ", name);
                        printVec3(MString("Located at: "), vec3(trans[0], trans[1], trans[2]));
                    }

                    continue;
                }
            }
        }
}

SelectedRegion::~SelectedRegion() {}

// default constructor that makes an empty distribution
Distribution::Distribution() 
	: selectedRegion(SelectedRegion(SelectionType::NONE, 0, 0, vec3(0, 0, 0), vec3(0, 0 , 0), vec3(0, 0, 0))), empty(true)
{}

Distribution::Distribution(SelectedRegion r) 
	: selectedRegion(r), empty(false)
{
	if (r.selectionType == SelectionType::NONE) {
		empty = true;
	}
}

Distribution::Distribution(SelectionType st, float w, float h, vec3 min, vec3 max, vec3 pos)
	: selectedRegion(SelectedRegion(st, w, h, min, max, pos)), empty(false)
{
	if (st == SelectionType::NONE) {
		empty = true;
	}
}

Distribution::~Distribution() {}