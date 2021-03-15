#include "WPPlugin.h"
#include <maya/MFnPlugin.h>
#include <maya/MSyntax.h>
#include <maya/MArgParser.h>
#include <maya/MArgDatabase.h>

// Argument Flags

#define kNameFlag "-n"
#define kNameFlagLong "-name"
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
#define kSelectionMaxBoundFlagLong "maxBound"

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
	return syntax;
}

MStatus WPPlugin::parseSyntax(const MArgList& argList, 
	                          MString& name, 
	                          WPPlugin::SelectionType& type,
	                          double& width,
	                          double& height,
							  double3& center,
	                          double3& minBound,
	                          double3& maxBound)
{
	MStatus stat = MS::kSuccess;
	MArgDatabase parser(newSyntax(), argList, &stat);
	if (!stat)
		stat.perror("parsing failed");

	if (parser.isFlagSet(kNameFlag))
	{
		stat = parser.getFlagArgument(kNameFlag, 0, name);
	}
	if (parser.isFlagSet(kNameFlagLong))
	{
		stat = parser.getFlagArgument(kNameFlag, 0, name);
	}
	if (parser.isFlagSet(kSelectionTypeFlag))
	{
		int temp;
		stat = parser.getFlagArgument(kSelectionTypeFlag, 0, temp);
		type = (WPPlugin::SelectionType) temp;
	}
	if (parser.isFlagSet(kSelectionTypeFlagLong))
	{
		int temp;
		stat = parser.getFlagArgument(kSelectionTypeFlagLong, 0, temp);
		type = (WPPlugin::SelectionType) temp;
	}
	if (parser.isFlagSet(kSelectionWidthFlag))
	{
		stat = parser.getFlagArgument(kSelectionWidthFlag, 0, width);
	}
	if (parser.isFlagSet(kSelectionWidthFlagLong))
	{
		stat = parser.getFlagArgument(kSelectionWidthFlagLong, 0, width);
	}
	if (parser.isFlagSet(kSelectionHeightFlag))
	{
		stat = parser.getFlagArgument(kSelectionHeightFlag, 0, height);
	}
	if (parser.isFlagSet(kSelectionHeightFlagLong))
	{
		stat = parser.getFlagArgument(kSelectionHeightFlagLong, 0, height);
	}
	if (parser.isFlagSet(kSelectionWorldPositionFlag))
	{
		stat = parser.getFlagArgument(kSelectionWorldPositionFlag, 0, center[0]);
		stat = parser.getFlagArgument(kSelectionWorldPositionFlag, 1, center[1]);
		stat = parser.getFlagArgument(kSelectionWorldPositionFlag, 2, center[2]);
	}
	if (parser.isFlagSet(kSelectionWorldPositionFlagLong))
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
	}
	if (parser.isFlagSet(kSelectionMinBoundFlagLong))
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
	}
	if (parser.isFlagSet(kSelectionMaxBoundFlagLong))
	{
		stat = parser.getFlagArgument(kSelectionMaxBoundFlagLong, 0, maxBound[0]);
		stat = parser.getFlagArgument(kSelectionMaxBoundFlagLong, 1, maxBound[1]);
		stat = parser.getFlagArgument(kSelectionMaxBoundFlagLong, 2, maxBound[2]);
	}
	return stat;
}

// Plugin doIt function
MStatus WPPlugin::doIt(const MArgList& argList)
{
	MStatus status;
	MGlobal::displayInfo("Hello World");

	// parse files
	MString name("");
	WPPlugin::SelectionType seltype;
	double width;
	double height;
	double3 center;
	double3 minBound;
	double3 maxBound;
	parseSyntax(argList, name, seltype, width, height, center, minBound, maxBound);

	// Plugin's functionality. Just a dialog box for now. 
	MString caption("Hello Maya");
	MString messageBoxCommand = ("confirmDialog -title \"" + caption + "\" -message \"" + "World Pos:" + center[0] + ", " + center[1] + ", " + center[2] + "\" -button \"Ok\" -defaultButton \"Ok\"");
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