#include "WPPlugin.h"
#include <maya/MFnPlugin.h>
#include <maya/MSyntax.h>
#include <maya/MArgParser.h>
#include <maya/MArgDatabase.h>

#define kNameFlag "-n"
#define kNameFlagLong "-name"

// Selection flags
#define kSelectionTypeFlag "-st"
#define kSelectionTypeFlagLong "-selType"
#define kSelectionWidthFlag "-w" // this is the radius for radial selection
#define kSelectionWidthFlagLong "-width"
#define kSelectionHeightFlag "-h"
#define kSelectionHeightFlagLong "-height"
#define kSelectionWorldPositionFlag "-wp"
#define kSelectionWorldPositionFlagLong "-worldpos"
#define kSelectionMinBoundFlag "-mib"
#define kSelectionMinBoundFlagLong "-minBound"
#define kSelectionMaxBoundFlag "-mab"
#define kSelectionMaxBoundFlagLong "-maxBound"

// Priority order flags
#define kPriorityOrderFlag "-po"
#define kPriorityOrderFlagLong "-priorityOrder"

// define EXPORT for exporting dll functions
#define EXPORT _declspec(dllexport)
// Maya Plugin creator function

void* WPPlugin::creator()
{
	return new WPPlugin;
}

MSyntax WPPlugin::newSyntax()
{
	MSyntax syntax;
	syntax.addFlag(kNameFlag, kNameFlagLong, MSyntax::kString);
	syntax.addFlag(kSelectionTypeFlag, kSelectionTypeFlagLong, MSyntax::kDouble);
	syntax.addFlag(kSelectionWidthFlag, kSelectionWidthFlagLong, MSyntax::kDouble);
	syntax.addFlag(kSelectionHeightFlag, kSelectionHeightFlagLong, MSyntax::kDouble);
	syntax.addFlag(kSelectionWorldPositionFlag, kSelectionWorldPositionFlagLong, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag(kSelectionMinBoundFlag, kSelectionMinBoundFlagLong, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag(kSelectionMaxBoundFlag, kSelectionMaxBoundFlagLong, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag(kPriorityOrderFlag, kPriorityOrderFlagLong, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble); // UPDATE THIS IF MORE CATEGORIES ARE ADDED!!!
	return syntax;
}

MStatus WPPlugin::parseSyntax(const MArgList& argList, 
	                          MString& name, 
	                          SelectionType& type,
	                          double& width,
	                          double& height,
							  vec3& center,
	                          vec3& minBound,
	                          vec3& maxBound,
	                          std::vector<int>& priOrder)
{
	MStatus stat = MS::kSuccess;
	MArgDatabase parser(newSyntax(), argList, &stat);
	if (!stat)
		stat.perror("parsing failed");

	if (parser.isFlagSet(kNameFlag))
	{
		stat = parser.getFlagArgument(kNameFlag, 0, name);
	} else if (parser.isFlagSet(kNameFlagLong))
	{
		stat = parser.getFlagArgument(kNameFlag, 0, name);
	}
	if (parser.isFlagSet(kSelectionTypeFlag))
	{
		int temp;
		stat = parser.getFlagArgument(kSelectionTypeFlag, 0, temp);
		type = (SelectionType) temp;
	} else if (parser.isFlagSet(kSelectionTypeFlagLong))
	{
		int temp;
		stat = parser.getFlagArgument(kSelectionTypeFlagLong, 0, temp);
		type = (SelectionType) temp;
	}
	if (parser.isFlagSet(kSelectionWidthFlag))
	{
		stat = parser.getFlagArgument(kSelectionWidthFlag, 0, width);
	} else if (parser.isFlagSet(kSelectionWidthFlagLong))
	{
		stat = parser.getFlagArgument(kSelectionWidthFlagLong, 0, width);
	}
	if (parser.isFlagSet(kSelectionHeightFlag))
	{
		stat = parser.getFlagArgument(kSelectionHeightFlag, 0, height);
	} else if (parser.isFlagSet(kSelectionHeightFlagLong))
	{
		stat = parser.getFlagArgument(kSelectionHeightFlagLong, 0, height);
	}
	if (parser.isFlagSet(kSelectionWorldPositionFlag))
	{
		stat = parser.getFlagArgument(kSelectionWorldPositionFlag, 0, center[0]);
		stat = parser.getFlagArgument(kSelectionWorldPositionFlag, 1, center[1]);
		stat = parser.getFlagArgument(kSelectionWorldPositionFlag, 2, center[2]);
	} else if (parser.isFlagSet(kSelectionWorldPositionFlagLong))
	{
		stat = parser.getFlagArgument(kSelectionWorldPositionFlagLong, 0, center[0]);
		stat = parser.getFlagArgument(kSelectionWorldPositionFlagLong, 1, center[1]);
		stat = parser.getFlagArgument(kSelectionWorldPositionFlagLong, 2, center[2]);
	}
	if (parser.isFlagSet(kSelectionMinBoundFlag))
	{
		stat = parser.getFlagArgument(kSelectionMinBoundFlag, 0, minBound[0]);
		stat = parser.getFlagArgument(kSelectionMinBoundFlag, 1, minBound[1]);
		stat = parser.getFlagArgument(kSelectionMinBoundFlag, 2, minBound[2]);
	} else if (parser.isFlagSet(kSelectionMinBoundFlagLong))
	{
		stat = parser.getFlagArgument(kSelectionMinBoundFlagLong, 0, minBound[0]);
		stat = parser.getFlagArgument(kSelectionMinBoundFlagLong, 1, minBound[1]);
		stat = parser.getFlagArgument(kSelectionMinBoundFlagLong, 2, minBound[2]);
	}
	if (parser.isFlagSet(kSelectionMaxBoundFlag))
	{
		stat = parser.getFlagArgument(kSelectionMaxBoundFlag, 0, maxBound[0]);
		stat = parser.getFlagArgument(kSelectionMaxBoundFlag, 1, maxBound[1]);
		stat = parser.getFlagArgument(kSelectionMaxBoundFlag, 2, maxBound[2]);
	} else if (parser.isFlagSet(kSelectionMaxBoundFlagLong))
	{
		stat = parser.getFlagArgument(kSelectionMaxBoundFlagLong, 0, maxBound[0]);
		stat = parser.getFlagArgument(kSelectionMaxBoundFlagLong, 1, maxBound[1]);
		stat = parser.getFlagArgument(kSelectionMaxBoundFlagLong, 2, maxBound[2]);
	}
	if (parser.isFlagSet(kPriorityOrderFlag))
	{
		int temp = 0;
		stat = parser.getFlagArgument(kPriorityOrderFlag, 0, temp);
		priOrder.push_back((int) temp);
		stat = parser.getFlagArgument(kPriorityOrderFlag, 1, temp);
		priOrder.push_back((int) temp);
		stat = parser.getFlagArgument(kPriorityOrderFlag, 2, temp);
		priOrder.push_back((int) temp);
	} else if (parser.isFlagSet(kPriorityOrderFlagLong))
	{
		int temp = 0;
		stat = parser.getFlagArgument(kPriorityOrderFlagLong, 0, temp);
		priOrder.push_back((int) temp);
		stat = parser.getFlagArgument(kPriorityOrderFlagLong, 1, temp);
		priOrder.push_back((int) temp);
		stat = parser.getFlagArgument(kPriorityOrderFlagLong, 2, temp);
		priOrder.push_back((int) temp);
	}
	return stat;
}

// Plugin doIt function
MStatus WPPlugin::doIt(const MArgList& argList)
{
	MStatus status;

	// parse files
	MString name("");
	SelectionType seltype;
	double width; // only in one direction, so the true width is twice as long
	double height; // only in one direction, so the true height is twice as long
	vec3 center;
	vec3 minBound;
	vec3 maxBound;
	std::vector<int> priOrder;
	parseSyntax(argList, name, seltype, width, height, center, minBound, maxBound, priOrder); // get all the arguments
	if (priOrder.size() == WorldPalette::priorityOrder.size()) {
		worldPalette.updatePriorityOrder(priOrder);
	}

	// checking arguments
	/*printFloat(MString("Width Argument: "), width);
	printFloat(MString("Height Argument: "), height);
	printVec3(MString("Min Argument: "), minBound);
	printVec3(MString("Max Argument: "), maxBound);
	printVec3(MString("Center Argument: "), center);*/

	// Plugin's functionality
	//worldPalette.setCurrentDistribution(seltype, width, height, minBound, maxBound, center);

	// Check that scene objects are found
	//printString(MString("The first object in the list is: "), worldPalette.currentlySelectedRegion.selectedRegion.objects[0].name);

	MString caption("Processed Selection!");
	MString order((std::string("1st: ") + std::to_string((int)WorldPalette::priorityOrder[0]) + std::string(" 2nd: ") + std::to_string((int)WorldPalette::priorityOrder[1]) + std::string(" 3rd: ") + std::to_string((int)WorldPalette::priorityOrder[2])).c_str());
	MString messageBoxCommand = ("confirmDialog -title \"" + caption + "\" -message \"" + order + "\" -button \"Ok\" -defaultButton \"Ok\"");
	MGlobal::executeCommand(messageBoxCommand);
	return status;
}

// Initialize Maya Plugin upon loading
EXPORT MStatus initializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj, "CIS660", "1.0", "Any");

	status = plugin.registerCommand("WorldPalette", WPPlugin::creator, WPPlugin::newSyntax);
	if (!status)
		status.perror("registerCommand failed");
	
	// Load MEL
	MString loadPath = plugin.loadPath();
	MGlobal::executeCommand("source \"" + loadPath + "/WorldPalette.mel\";");

	return status;
}
// Cleanup Plugin upon unloading
EXPORT MStatus uninitializePlugin(MObject obj)
{
	MStatus status;
	MFnPlugin plugin(obj);
	status = plugin.deregisterCommand("WorldPalette");
	if (!status)
		status.perror("deregisterCommand failed");
	return status;
}