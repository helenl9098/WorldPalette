#include "WPPlugin.h"
#include <maya/MFnPlugin.h>
#include <maya/MSyntax.h>
#include <maya/MArgParser.h>
#include <maya/MArgDatabase.h>

#define kNameFlag "-n"
#define kNameFlagLong "-name"

// Selection region flags
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
#define kSelectionPaletteIndexFlag "-pi"
#define kSelectionPaletteIndexFlagLong "-paletteIdx"
#define kMoveSelectionRegionFlag "-msr"
#define kMoveSelectionRegionFlagLong "-moveSelRegion"

// Priority order flags
#define kPriorityOrderFlag "-po"
#define kPriorityOrderFlagLong "-priorityOrder"

// Generation flags
#define kIsGeneratingFlag "-ge" // true = use area for saving/generating, false = use area for pasting
#define kIsGeneratingFlagLong "-isGenerating"

// Height visualization/terrain flags
#define kTerrainNameFlag "-tn"
#define kTerrainNameFlagLong "-terrainName"
#define kTerrainWidthFlag "-tw"
#define kTerrainWidthFlagLong "-terrainWidth"
#define kTerrainHeightFlag "-th"
#define kTerrainHeightFlagLong "-terrainHeight"
#define kTerrainSubdivisionWidthFlag "-tsw"
#define kTerrainSubdivisionWidthFlagLong "-terrainSubWidth"
#define kTerrainSubdivisionHeightFlag "-tsh"
#define kTerrainSubdivisionHeightFlagLong "-terrainSubHeight"

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
	syntax.addFlag(kSelectionPaletteIndexFlag, kSelectionPaletteIndexFlagLong, MSyntax::kDouble);
	syntax.addFlag(kMoveSelectionRegionFlag, kMoveSelectionRegionFlagLong, MSyntax::kDouble);

	syntax.addFlag(kPriorityOrderFlag, kPriorityOrderFlagLong, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble); // UPDATE THIS IF MORE CATEGORIES ARE ADDED!!!

	syntax.addFlag(kIsGeneratingFlag, kIsGeneratingFlagLong, MSyntax::kBoolean);

	syntax.addFlag(kTerrainNameFlag, kTerrainNameFlagLong, MSyntax::kString);
	syntax.addFlag(kTerrainWidthFlag, kTerrainWidthFlagLong, MSyntax::kDouble);
	syntax.addFlag(kTerrainHeightFlag, kTerrainHeightFlagLong, MSyntax::kDouble);
	syntax.addFlag(kTerrainSubdivisionWidthFlag, kTerrainSubdivisionWidthFlagLong, MSyntax::kDouble);
	syntax.addFlag(kTerrainSubdivisionHeightFlag, kTerrainSubdivisionHeightFlagLong, MSyntax::kDouble);
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
							  int& paletteIdx,
	                          std::vector<int>& priOrder,
							  bool& isGenerating,
							  bool& moveSelRegion,
							  MString& terrainName,
							  double& terrainWidth,
							  double& terrainHeight,
							  int& terrainSubWidth,
							  int& terrainSubHeight)
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
	if (parser.isFlagSet(kSelectionPaletteIndexFlag))
	{
		int temp;
		stat = parser.getFlagArgument(kSelectionPaletteIndexFlag, 0, temp);
		paletteIdx = (int) temp;
	} else if (parser.isFlagSet(kSelectionPaletteIndexFlagLong))
	{
		int temp;
		stat = parser.getFlagArgument(kSelectionPaletteIndexFlagLong, 0, temp);
		paletteIdx = (int) temp;
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
	}  else if (parser.isFlagSet(kPriorityOrderFlagLong))
	{
		int temp = 0;
		stat = parser.getFlagArgument(kPriorityOrderFlagLong, 0, temp);
		priOrder.push_back((int) temp);
		stat = parser.getFlagArgument(kPriorityOrderFlagLong, 1, temp);
		priOrder.push_back((int) temp);
		stat = parser.getFlagArgument(kPriorityOrderFlagLong, 2, temp);
		priOrder.push_back((int) temp);
	}
	if (parser.isFlagSet(kIsGeneratingFlag)) 
	{
		stat = parser.getFlagArgument(kIsGeneratingFlag, 0, isGenerating);
	} else if (parser.isFlagSet(kIsGeneratingFlagLong)) 
	{
		stat = parser.getFlagArgument(kIsGeneratingFlagLong, 0, isGenerating);
	}
	if (parser.isFlagSet(kMoveSelectionRegionFlag))
	{
		stat = parser.getFlagArgument(kMoveSelectionRegionFlag, 0, moveSelRegion);
	} else if (parser.isFlagSet(kMoveSelectionRegionFlagLong))
	{
		stat = parser.getFlagArgument(kMoveSelectionRegionFlagLong, 0, moveSelRegion);
	}
	if (parser.isFlagSet(kTerrainNameFlag))
	{
		stat = parser.getFlagArgument(kTerrainNameFlag, 0, terrainName);
	} else if (parser.isFlagSet(kTerrainNameFlagLong))
	{
		stat = parser.getFlagArgument(kTerrainNameFlagLong, 0, terrainName);
	}
	if (parser.isFlagSet(kTerrainWidthFlag))
	{
		stat = parser.getFlagArgument(kTerrainWidthFlag, 0, terrainWidth);
	} else if (parser.isFlagSet(kTerrainWidthFlagLong)) 
	{
		stat = parser.getFlagArgument(kTerrainWidthFlagLong, 0, terrainWidth);
	}
	if (parser.isFlagSet(kTerrainHeightFlag))
	{
		stat = parser.getFlagArgument(kTerrainHeightFlag, 0, terrainHeight);
	} else if (parser.isFlagSet(kTerrainHeightFlagLong))
	{
		stat = parser.getFlagArgument(kTerrainHeightFlagLong, 0, terrainHeight);
	}	
	if (parser.isFlagSet(kTerrainSubdivisionWidthFlag))
	{
		int temp;
		stat = parser.getFlagArgument(kTerrainSubdivisionWidthFlag, 0, temp);
		terrainSubWidth = (int) temp;
	} else if (parser.isFlagSet(kTerrainSubdivisionWidthFlagLong))
	{
		int temp;
		stat = parser.getFlagArgument(kTerrainSubdivisionWidthFlagLong, 0, temp);
		terrainSubWidth = (int) temp;
	}
	if (parser.isFlagSet(kTerrainSubdivisionHeightFlag))
	{
		int temp;
		stat = parser.getFlagArgument(kTerrainSubdivisionHeightFlag, 0, temp);
		terrainSubHeight = (int)temp;
	} else if (parser.isFlagSet(kTerrainSubdivisionHeightFlagLong))
	{
		int temp;
		stat = parser.getFlagArgument(kTerrainSubdivisionHeightFlagLong, 0, temp);
		terrainSubHeight = (int)temp;
	}
	return stat;
}

// Plugin doIt function
MStatus WPPlugin::doIt(const MArgList& argList)
{
	MStatus status;

	// parse files
	MString name("");

	// Selection region stuff
	SelectionType seltype = SelectionType::NONE;
	double width = 0; // only in one direction, so the true width is twice as long
	double height = 0; // only in one direction, so the true height is twice as long
	vec3 center;
	vec3 minBound;
	vec3 maxBound;
	int paletteIdx;
	bool isSelRegionMoving = false; // did we move the selection region? (this is for updating region heights)

	std::vector<int> priOrder;
	bool isGenerating = false; // is the passed in area for generating?

	// Terrain stuff
	MString terrainName("");
	double terrainWidth = 0;
	double terrainHeight = 0;
	int terrainSubWidth = 0;
	int terrainSubHeight = 0;
	parseSyntax(argList, name, seltype, width, height, center, minBound, maxBound, 
				paletteIdx, priOrder, isGenerating, isSelRegionMoving,
				terrainName, terrainWidth, terrainHeight, terrainSubWidth, terrainSubHeight); // get all the arguments

	// Update priority order (if needed)
	if (priOrder.size() == WorldPalette::priorityOrder.size()) {
		worldPalette.updatePriorityOrder(priOrder);
	}

	if (terrainWidth != 0 && terrainHeight != 0 && terrainSubWidth != 0 && terrainSubHeight != 0) {
		// Initialize terrain
		terrain = Terrain(terrainName, terrainWidth, terrainHeight, terrainSubWidth, terrainSubHeight);
	}

	if (isSelRegionMoving) {
		// Update selection region
		terrain.updateSelectionRegion();
	}

	// checking arguments
#if DEBUG
	printFloat(MString("Width Argument: "), width);
	printFloat(MString("Height Argument: "), height);
	printVec3(MString("Min Argument: "), minBound);
	printVec3(MString("Max Argument: "), maxBound);
	printVec3(MString("Center Argument: "), center);
#endif

	// Plugin's functionality
	// TO DO: We need to do some error checks! (like if w is < 0 etc)

	// Check if we're saving/generating or pasting a distribution
	if (isGenerating) {
		if (width > 0) {
			// Saving distribution
			worldPalette.saveDistribution(seltype, width, height, minBound, maxBound, center, paletteIdx);
		} else {
			// Updating currently selected distribution
			if (paletteIdx >= 0 && paletteIdx < worldPalette.maxPaletteSize) {
				worldPalette.setCurrentDistribution(worldPalette.palette[paletteIdx]);
			}
		}
	}
	else {
		if (width > 0) {
			// Delete existing geometry in the region and paste new geometry
			worldPalette.pasteDistribution(seltype, width, height, minBound, maxBound, center, paletteIdx);
		}
	}

	//MString caption("Processed Selection!");
	//MString order((std::string("1st: ") + std::to_string((int)WorldPalette::priorityOrder[0]) + std::string(" 2nd: ") + std::to_string((int)WorldPalette::priorityOrder[1]) + std::string(" 3rd: ") + std::to_string((int)WorldPalette::priorityOrder[2])).c_str());
	//MString messageBoxCommand = ("confirmDialog -title \"" + caption + "\" -message \"" + order + "\" -button \"Ok\" -defaultButton \"Ok\"");
	//MGlobal::executeCommand(messageBoxCommand);
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

	// Create a WorldPalette directory under default workspace
	MString wsp_path;
	MGlobal::executeCommand("workspace -q -fullName;", wsp_path);
	MString w_palette_path = wsp_path + MString("/WorldPalette");
	MGlobal::executeCommand("sysFile -makeDir \"" + w_palette_path + "\"");

	// Copy the scene obj and mtl files
	MGlobal::executeCommand("sysFile -makeDir \"" + w_palette_path + "/objects\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/objects/tree.obj\" " + "\"" + loadPath + "/scene_objects/tree.obj\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/objects/tree.mtl\" " + "\"" + loadPath + "/scene_objects/tree.mtl\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/objects/big_rock.obj\" " + "\"" + loadPath + "/scene_objects/big_rock.obj\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/objects/big_rock.mtl\" " + "\"" + loadPath + "/scene_objects/big_rock.mtl\"");

	// Copy the terrain files
	MGlobal::executeCommand("sysFile -makeDir \"" + w_palette_path + "/terrains\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/terrains/terrain.obj\" " + "\"" + loadPath + "/scene_objects/terrain.obj\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/terrains/terrain.mtl\" " + "\"" + loadPath + "/scene_objects/terrain.mtl\"");

	// Copy the icon files
	MGlobal::executeCommand("sysFile -makeDir \"" + w_palette_path + "/icons\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/icons/tree.png\" " + "\"" + loadPath + "/icons/tree.png\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/icons/big_rock.png\" " + "\"" + loadPath + "/icons/big_rock.png\"");

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