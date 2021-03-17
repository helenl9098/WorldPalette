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

        
        MStringArray* objectNames = new MStringArray;

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
                    MString name = dagPath.fullPathName();
                    objectNames->append(name);

                    printString("Mesh + Transform Option: ", name);

                    // 1. ignore the selection region

                    // 2. for all geometry, find its transform/position
                    MFnTransform fnTransform(dagPath);
                    MVector trans = fnTransform.getTranslation(MSpace::kWorld, &stat);
                    printVec3(MString("Translation Vector: "), vec3(trans[0], trans[1], trans[2]));

                    // 3. check if position is inside bounding box

                    // 4. if so, we create a scene object struct

                    // 5. fill in scene object struct with type & position

                    // 6. push scene object back in vector

                    continue;
                }
                else if (dagPath.hasFn(MFn::kMesh))
                {
                    MString name = dagPath.fullPathName();
                    objectNames->append(name);

                    printString("Just Mesh Option: ", name);
                    continue;

                    /*
                    // We have a mesh so create a vertex and polygon table
                    // for this object.
                    //

                    MFnMesh fnMesh(dagPath);
                    int vtxCount = fnMesh.numVertices();
                    int polygonCount = fnMesh.numPolygons();
                    // we do not need this call anymore, we have the shape.
                    // dagPath.extendToShape();
                    MString name = dagPath.fullPathName();
                    objectNames->append(name);
                    objectNodeNamesArray.append(fnMesh.name());

                    vertexCounts.append(vtxCount);
                    polygonCounts.append(polygonCount);

                    objectCount++; */
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