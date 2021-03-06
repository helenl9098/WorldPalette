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

// Scene OBJ name flags
#define kSetTreeOBJFlag "-sto"
#define kSetTreeOBJFlagLong "-setTreeOBJ"
#define kSetShrubOBJFlag "-sso"
#define kSetShrubOBJFlagLong "-setShrubOBJ"
#define kSetRockOBJFlag "-sro"
#define kSetRockOBJFlagLong "-setRockOBJ"
#define kSetGrassOBJFlag "-sgo"
#define kSetGrassOBJFlagLong "-setGrassOBJ"

// Height visualization/terrain flags
#define kTerrainNameFlag "-tn"
#define kTerrainNameFlagLong "-terrainName"
#define kUnitializeTerrainFlag "-ut"
#define kUnitializeTerrainFlagLong "-uninitializeTerrain"
#define kTerrainWidthFlag "-tw"
#define kTerrainWidthFlagLong "-terrainWidth"
#define kTerrainHeightFlag "-th"
#define kTerrainHeightFlagLong "-terrainHeight"
#define kTerrainSubdivisionWidthFlag "-tsw"
#define kTerrainSubdivisionWidthFlagLong "-terrainSubWidth"
#define kTerrainSubdivisionHeightFlag "-tsh"
#define kTerrainSubdivisionHeightFlagLong "-terrainSubHeight"
#define kUpdateSceneGeometryHeightFlag "-ugh"
#define kUpdateSceneGeometryHeightFlagLong "-uGeomHeight"

// Move editing flags
#define kStartMoveFlag "-sm"
#define kStartMoveFlagLong "-startMove"
#define kChangeInXFlag "-dx"
#define kChangeInXFlagLong "-dxpos"
#define kChangeInZFlag "-dz"
#define kChangeInZFlagLong "-dzpos"

// Resize editing flags
#define kStartResizeFlag "-sr"
#define kStartResizeFlagLong "-startResize"
#define kChangeInWidthFlag "-dw"
#define kChangeInWidthFlagLong "-dWidth"
#define kChangeInHeightFlag "-dh"
#define kChangeInHeightFlagLong "-dHeight"

// Brush/eraser editing flags
#define kSaveBrushStrokeFlag "-sb"
#define kSaveBrushStrokeFlagLong "-saveBrush"
#define kSaveBrushStrokePositionFlag "-sbp"
#define kSaveBrushStrokePositionFlagLong "-saveBrushPos"
#define kReleaseBrushFlag "-rb"
#define kReleaseBrushFlagLong "-releaseBrush"
#define kBrushWidthFlag "-bw"
#define kBrushWidthFlagLong "-brushWidth"
#define kReleaseEraserFlag "-re"
#define kReleaseEraserFlagLong "-releaseEraser"

// Clear flags
#define kClearSelectionFlag "-csl"
#define kClearSelectionFlagLong "-clearSelection"

// Undo flags
#define kMoveUndoFlag "-umv"
#define kMoveUndoFlagLong "-undoMove"
#define kResizeUndoFlag "-urs"
#define kResizeUndoFlagLong "-undoResize"
#define kPasteUndoFlag "-upt"
#define kPasteUndoFlagLong "-undoPaste"
#define kBrushUndoFlag "-ubr"
#define kBrushUndoFlagLong "-undoBrush"
#define kClearUndoFlag "-ucr"
#define kClearUndoFlagLong "-undoClear"
#define kEraseUndoFlag "-uer"
#define kEraseUndoFlagLong "-undoErase"

// Palette flags
#define kPaletteSaveFlag "-ps"
#define kPaletteSaveFlagLong "-paletteSave"
#define kPaletteLoadFlag "-pl"
#define kPaletteLoadFlagLong "-paletteLoad"
#define kPaletteClearFlag "-pc"
#define kPaletteClearFlagLong "-paletteClear"

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

	syntax.addFlag(kPriorityOrderFlag, kPriorityOrderFlagLong, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble, MSyntax::kDouble); // UPDATE THIS IF MORE CATEGORIES ARE ADDED!!!

	syntax.addFlag(kIsGeneratingFlag, kIsGeneratingFlagLong, MSyntax::kBoolean);

	syntax.addFlag(kSetTreeOBJFlag, kSetTreeOBJFlagLong, MSyntax::kString);
	syntax.addFlag(kSetShrubOBJFlag, kSetShrubOBJFlagLong, MSyntax::kString);
	syntax.addFlag(kSetRockOBJFlag, kSetRockOBJFlagLong, MSyntax::kString);
	syntax.addFlag(kSetGrassOBJFlag, kSetGrassOBJFlagLong, MSyntax::kString);

	syntax.addFlag(kTerrainNameFlag, kTerrainNameFlagLong, MSyntax::kString);
	syntax.addFlag(kUnitializeTerrainFlag, kUnitializeTerrainFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kTerrainWidthFlag, kTerrainWidthFlagLong, MSyntax::kDouble);
	syntax.addFlag(kTerrainHeightFlag, kTerrainHeightFlagLong, MSyntax::kDouble);
	syntax.addFlag(kTerrainSubdivisionWidthFlag, kTerrainSubdivisionWidthFlagLong, MSyntax::kDouble);
	syntax.addFlag(kTerrainSubdivisionHeightFlag, kTerrainSubdivisionHeightFlagLong, MSyntax::kDouble);
	syntax.addFlag(kUpdateSceneGeometryHeightFlag, kUpdateSceneGeometryHeightFlagLong, MSyntax::kString);

	syntax.addFlag(kStartMoveFlag, kStartMoveFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kChangeInXFlag, kChangeInXFlagLong, MSyntax::kDouble);
	syntax.addFlag(kChangeInZFlag, kChangeInZFlagLong, MSyntax::kDouble);

	syntax.addFlag(kStartResizeFlag, kStartResizeFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kChangeInWidthFlag, kChangeInWidthFlagLong, MSyntax::kDouble);
	syntax.addFlag(kChangeInHeightFlag, kChangeInHeightFlagLong, MSyntax::kDouble);

	syntax.addFlag(kSaveBrushStrokeFlag, kSaveBrushStrokeFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kSaveBrushStrokePositionFlag, kSaveBrushStrokePositionFlagLong, MSyntax::kDouble, MSyntax::kDouble);
	syntax.addFlag(kReleaseBrushFlag, kReleaseBrushFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kBrushWidthFlag, kBrushWidthFlagLong, MSyntax::kDouble);
	syntax.addFlag(kReleaseEraserFlag, kReleaseEraserFlagLong, MSyntax::kBoolean);

	syntax.addFlag(kClearSelectionFlag, kClearSelectionFlagLong, MSyntax::kBoolean);

	syntax.addFlag(kMoveUndoFlag, kMoveUndoFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kResizeUndoFlag, kResizeUndoFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kPasteUndoFlag, kPasteUndoFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kBrushUndoFlag, kBrushUndoFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kClearUndoFlag, kClearUndoFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kEraseUndoFlag, kEraseUndoFlagLong, MSyntax::kBoolean);

	syntax.addFlag(kPaletteSaveFlag, kPaletteSaveFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kPaletteLoadFlag, kPaletteLoadFlagLong, MSyntax::kBoolean);
	syntax.addFlag(kPaletteClearFlag, kPaletteClearFlagLong, MSyntax::kBoolean);

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
							  MString& treeOBJ,
							  MString& shrubOBJ,
							  MString& rockOBJ,
							  MString& grassOBJ,
							  bool& moveSelRegion,
							  MString& terrainName,
							  bool& uninitializeTerrain,
							  double& terrainWidth,
							  double& terrainHeight,
							  int& terrainSubWidth,
							  int& terrainSubHeight,
							  MString& geomToMove,
							  bool& startMove,
							  vec2& dpos,
							  bool& startResize,
							  vec2& dsize,
							  bool& saveBrush,
							  vec2& brushPos,
							  bool& releaseBrush,
							  double& brushWidth,
							  bool& releaseEraser,
							  bool& clearSelection,
							  bool& undoMove,
							  bool& undoResize,
							  bool& undoPaste,
							  bool& undoBrush,
							  bool& undoClear,
							  bool& undoErase, 
							  bool& paletteSave,
							  bool& paletteLoad, 
	                          bool& paletteClear)
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
		stat = parser.getFlagArgument(kNameFlagLong, 0, name);
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
		stat = parser.getFlagArgument(kPriorityOrderFlag, 3, temp);
		priOrder.push_back((int)temp);
	}  else if (parser.isFlagSet(kPriorityOrderFlagLong))
	{
		int temp = 0;
		stat = parser.getFlagArgument(kPriorityOrderFlagLong, 0, temp);
		priOrder.push_back((int) temp);
		stat = parser.getFlagArgument(kPriorityOrderFlagLong, 1, temp);
		priOrder.push_back((int) temp);
		stat = parser.getFlagArgument(kPriorityOrderFlagLong, 2, temp);
		priOrder.push_back((int) temp);
		stat = parser.getFlagArgument(kPriorityOrderFlagLong, 3, temp);
		priOrder.push_back((int)temp);
	}
	if (parser.isFlagSet(kIsGeneratingFlag)) 
	{
		stat = parser.getFlagArgument(kIsGeneratingFlag, 0, isGenerating);
	} else if (parser.isFlagSet(kIsGeneratingFlagLong)) 
	{
		stat = parser.getFlagArgument(kIsGeneratingFlagLong, 0, isGenerating);
	}
	if (parser.isFlagSet(kSetTreeOBJFlag))
	{
		stat = parser.getFlagArgument(kSetTreeOBJFlag, 0, treeOBJ);
	}
	else if (parser.isFlagSet(kSetTreeOBJFlagLong))
	{
		stat = parser.getFlagArgument(kSetTreeOBJFlagLong, 0, treeOBJ);
	}
	if (parser.isFlagSet(kSetShrubOBJFlag))
	{
		stat = parser.getFlagArgument(kSetShrubOBJFlag, 0, shrubOBJ);
	}
	else if (parser.isFlagSet(kSetShrubOBJFlagLong))
	{
		stat = parser.getFlagArgument(kSetShrubOBJFlagLong, 0, shrubOBJ);
	}
	if (parser.isFlagSet(kSetRockOBJFlag))
	{
		stat = parser.getFlagArgument(kSetRockOBJFlag, 0, rockOBJ);
	}
	else if (parser.isFlagSet(kSetRockOBJFlagLong))
	{
		stat = parser.getFlagArgument(kSetRockOBJFlagLong, 0, rockOBJ);
	}
	if (parser.isFlagSet(kSetGrassOBJFlag))
	{
		stat = parser.getFlagArgument(kSetGrassOBJFlag, 0, grassOBJ);
	}
	else if (parser.isFlagSet(kSetGrassOBJFlagLong))
	{
		stat = parser.getFlagArgument(kSetGrassOBJFlagLong, 0, grassOBJ);
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
	if (parser.isFlagSet(kUnitializeTerrainFlag)) {
		stat = parser.getFlagArgument(kUnitializeTerrainFlag, 0, uninitializeTerrain);
	} else if (parser.isFlagSet(kUnitializeTerrainFlagLong)) {
		stat = parser.getFlagArgument(kUnitializeTerrainFlagLong, 0, uninitializeTerrain);
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
	if (parser.isFlagSet(kUpdateSceneGeometryHeightFlag))
	{
		stat = parser.getFlagArgument(kUpdateSceneGeometryHeightFlag, 0, geomToMove);
	} else if (parser.isFlagSet(kUpdateSceneGeometryHeightFlagLong)) {
		stat = parser.getFlagArgument(kUpdateSceneGeometryHeightFlagLong, 0, geomToMove);
	}
	if (parser.isFlagSet(kStartMoveFlag)) {
		bool tmpMove = startMove;
		stat = parser.getFlagArgument(kStartMoveFlag, 0, startMove);
		if (startMove != tmpMove) {
			//worldPalette.moveDistributionSave();
		}
	}
	else if (parser.isFlagSet(kStartMoveFlagLong)) {
		bool tmpMove = startMove;
		stat = parser.getFlagArgument(kStartMoveFlagLong, 0, startMove);
		if (startMove != tmpMove) {
			//worldPalette.moveDistributionSave();
		}
	}
	if (parser.isFlagSet(kChangeInXFlag)) {
		stat = parser.getFlagArgument(kChangeInXFlag, 0, dpos[0]);
	} else if (parser.isFlagSet(kChangeInXFlagLong)) {
		stat = parser.getFlagArgument(kChangeInXFlagLong, 0, dpos[0]);
	}
	if (parser.isFlagSet(kChangeInZFlag)) {
		stat = parser.getFlagArgument(kChangeInZFlag, 0, dpos[1]);
	} else if (parser.isFlagSet(kChangeInZFlagLong)) {
		stat = parser.getFlagArgument(kChangeInZFlagLong, 0, dpos[1]);
	}
	if (parser.isFlagSet(kStartResizeFlag)) {
		stat = parser.getFlagArgument(kStartResizeFlag, 0, startResize);
	}
	else if (parser.isFlagSet(kStartResizeFlagLong)) {
		stat = parser.getFlagArgument(kStartResizeFlagLong, 0, startResize);
	}
	if (parser.isFlagSet(kChangeInWidthFlag)) {
		stat = parser.getFlagArgument(kChangeInWidthFlag, 0, dsize[0]);
	}
	else if (parser.isFlagSet(kChangeInWidthFlagLong)) {
		stat = parser.getFlagArgument(kChangeInWidthFlagLong, 0, dsize[0]);
	}
	if (parser.isFlagSet(kChangeInHeightFlag)) {
		stat = parser.getFlagArgument(kChangeInHeightFlag, 0, dsize[1]);
	}
	else if (parser.isFlagSet(kChangeInHeightFlagLong)) {
		stat = parser.getFlagArgument(kChangeInHeightFlagLong, 0, dsize[1]);
	}
	if (parser.isFlagSet(kSaveBrushStrokeFlag)) {
		stat = parser.getFlagArgument(kSaveBrushStrokeFlag, 0, saveBrush);
	} else if (parser.isFlagSet(kSaveBrushStrokeFlagLong)) {
		stat = parser.getFlagArgument(kSaveBrushStrokeFlagLong, 0, saveBrush);
	}
	if (parser.isFlagSet(kSaveBrushStrokePositionFlag)) {
		stat = parser.getFlagArgument(kSaveBrushStrokePositionFlag, 0, brushPos[0]);
		stat = parser.getFlagArgument(kSaveBrushStrokePositionFlag, 1, brushPos[1]);
	} else if (parser.isFlagSet(kSaveBrushStrokePositionFlagLong)) {
		stat = parser.getFlagArgument(kSaveBrushStrokePositionFlagLong, 0, brushPos[0]);
		stat = parser.getFlagArgument(kSaveBrushStrokePositionFlagLong, 1, brushPos[1]);
	}
	if (parser.isFlagSet(kReleaseBrushFlag)) {
		stat = parser.getFlagArgument(kReleaseBrushFlag, 0, releaseBrush);
	} else if (parser.isFlagSet(kReleaseBrushFlagLong)) {
		stat = parser.getFlagArgument(kReleaseBrushFlagLong, 0, releaseBrush);
	}
	if (parser.isFlagSet(kBrushWidthFlag)) {
		stat = parser.getFlagArgument(kBrushWidthFlag, 0, brushWidth);
	} else if (parser.isFlagSet(kBrushWidthFlagLong)) {
		stat = parser.getFlagArgument(kBrushWidthFlagLong, 0, brushWidth);
	}
	if (parser.isFlagSet(kReleaseEraserFlag)) {
		stat = parser.getFlagArgument(kReleaseEraserFlag, 0, releaseEraser);
	}
	else if (parser.isFlagSet(kReleaseEraserFlagLong)) {
		stat = parser.getFlagArgument(kReleaseEraserFlagLong, 0, releaseEraser);
	}
	if (parser.isFlagSet(kClearSelectionFlag)) {
		stat = parser.getFlagArgument(kClearSelectionFlag, 0, clearSelection);
	}
	else if (parser.isFlagSet(kClearSelectionFlagLong)) {
		stat = parser.getFlagArgument(kClearSelectionFlagLong, 0, clearSelection);
	}
	if (parser.isFlagSet(kMoveUndoFlag)) {
		stat = parser.getFlagArgument(kMoveUndoFlag, 0, undoMove);
	}
	else if (parser.isFlagSet(kMoveUndoFlagLong)) {
		stat = parser.getFlagArgument(kMoveUndoFlagLong, 0, undoMove);
	}
	if (parser.isFlagSet(kResizeUndoFlag)) {
		stat = parser.getFlagArgument(kResizeUndoFlag, 0, undoResize);
	}
	else if (parser.isFlagSet(kResizeUndoFlagLong)) {
		stat = parser.getFlagArgument(kResizeUndoFlagLong, 0, undoResize);
	}
	if (parser.isFlagSet(kPasteUndoFlag)) {
		stat = parser.getFlagArgument(kPasteUndoFlag, 0, undoPaste);
	}
	else if (parser.isFlagSet(kPasteUndoFlagLong)) {
		stat = parser.getFlagArgument(kPasteUndoFlagLong, 0, undoPaste);
	}
	if (parser.isFlagSet(kBrushUndoFlag)) {
		stat = parser.getFlagArgument(kBrushUndoFlag, 0, undoBrush);
	}
	else if (parser.isFlagSet(kBrushUndoFlagLong)) {
		stat = parser.getFlagArgument(kBrushUndoFlagLong, 0, undoBrush);
	}
	if (parser.isFlagSet(kEraseUndoFlag)) {
		stat = parser.getFlagArgument(kEraseUndoFlag, 0, undoErase);
	}
	else if (parser.isFlagSet(kEraseUndoFlagLong)) {
		stat = parser.getFlagArgument(kEraseUndoFlagLong, 0, undoErase);
	}
	if (parser.isFlagSet(kClearUndoFlag)) {
		stat = parser.getFlagArgument(kClearUndoFlag, 0, undoClear);
	}
	else if (parser.isFlagSet(kClearUndoFlagLong)) {
		stat = parser.getFlagArgument(kClearUndoFlagLong, 0, undoClear);
	}

	if (parser.isFlagSet(kPaletteSaveFlag)) {
		stat = parser.getFlagArgument(kPaletteSaveFlag, 0, paletteSave);
	}
	else if (parser.isFlagSet(kPaletteSaveFlagLong)) {
		stat = parser.getFlagArgument(kPaletteSaveFlagLong, 0, paletteSave);
	}
	if (parser.isFlagSet(kPaletteLoadFlag)) {
		stat = parser.getFlagArgument(kPaletteLoadFlag, 0, paletteLoad);
	}
	else if (parser.isFlagSet(kPaletteLoadFlagLong)) {
		stat = parser.getFlagArgument(kPaletteLoadFlagLong, 0, paletteLoad);
	}
	if (parser.isFlagSet(kPaletteClearFlag)) {
		stat = parser.getFlagArgument(kPaletteClearFlag, 0, paletteClear);
	}
	else if (parser.isFlagSet(kPaletteClearFlagLong)) {
		stat = parser.getFlagArgument(kPaletteClearFlagLong, 0, paletteClear);
	}
	return stat;
}

// Plugin doIt function
MStatus WPPlugin::doIt(const MArgList& argList)
{
	MStatus status;

	/* 
	* Step 1: Parse command arguments
	*/

	MString name("");

	// Selection region stuff
	SelectionType seltype = SelectionType::NONE;
	double width = 0; // only in one direction, so the true width is twice as long
	double height = 0; // only in one direction, so the true height is twice as long
	vec3 center;
	vec3 minBound;
	vec3 maxBound;
	int paletteIdx = -1;
	bool isSelRegionMoving = false; // did we move the selection region? (this is for updating region heights)
	bool isGenerating = false; // is the passed in area for generating?

	// Scene OBJ stuff
	MString treeOBJ("");
	MString shrubOBJ("");
	MString rockOBJ("");
	MString grassOBJ("");

	// Priority order stuff
	std::vector<int> priOrder;

	// Terrain stuff
	MString terrainName("");
	bool uninitializeTerrain = false;
	double terrainWidth = 0;
	double terrainHeight = 0;
	int terrainSubWidth = 0;
	int terrainSubHeight = 0;
	MString geomToMove("");

	// Move editing stuff
	bool startMove = false;
	vec2 dpos = vec2(0, 0);

	// Resize editing stuff
	bool startResize = false;
	vec2 dsize = vec2(0, 0);

	// Brush editing stuff
	bool saveBrush = false;
	vec2 brushPos;
	bool releaseBrush = false;
	double brushWidth;
	bool releaseEraser = false;

	// Clear selection stuff
	bool clearSelection = false;

	// Undo stuff
	bool undoMove = false;
	bool undoResize = false;
	bool undoPaste = false;
	bool undoBrush = false;
	bool undoClear = false;
	bool undoErase = false;

	// palette stuff
	bool paletteSave = false; 
	bool paletteLoad = false;
	bool paletteClear = false;

	// Parse all the arguments
	parseSyntax(argList, name, seltype, width, height, center, minBound, maxBound, 
				paletteIdx, priOrder, isGenerating, 
				treeOBJ, shrubOBJ, rockOBJ, grassOBJ, isSelRegionMoving,
				terrainName, uninitializeTerrain, terrainWidth, terrainHeight, terrainSubWidth, terrainSubHeight, geomToMove, 
				startMove, dpos, startResize, dsize, saveBrush, brushPos, releaseBrush, brushWidth, releaseEraser, clearSelection,
				undoMove, undoResize, undoPaste, undoBrush, undoClear, undoErase, paletteSave, paletteLoad, paletteClear);

	/*
	* Step 2: Call needed WorldPalette operations
	*/

	// Update selection region 3D position (if needed)
	if (isSelRegionMoving) {
		// Update selection region
		WorldPalette::terrain.updateSelectionRegion();
	}

	// Update priority order (if needed)
	if (priOrder.size() == WorldPalette::priorityOrder.size()) {
		worldPalette.updatePriorityOrder(priOrder);
		return status;
	}

	// Update scene object names (if needed)
	if (treeOBJ.length() > 0) {
		WorldPalette::objNames[3] = treeOBJ;
	}
	if (shrubOBJ.length() > 0) {
		WorldPalette::objNames[2] = shrubOBJ;
	}
	if (rockOBJ.length() > 0) {
		WorldPalette::objNames[1] = rockOBJ;
	}
	if (grassOBJ.length() > 0) {
		WorldPalette::objNames[0] = grassOBJ;
	}

	// Initialize terrain (if needed)
	if (terrainWidth != 0 && terrainHeight != 0 && terrainSubWidth != 0 && terrainSubHeight != 0) {
		// Initialize terrain
		WorldPalette::terrain = Terrain(terrainName, terrainWidth, terrainHeight, terrainSubWidth, terrainSubHeight);
		return status;
	}
	// Uninitialize terrain (if needed)
	if (uninitializeTerrain) {
		WorldPalette::terrain = Terrain();
		return status;
	}

	// Move scene geometry to terrain height (if needed)
	if (geomToMove.length() > 0) {
		// Update the geometry height
		MDoubleArray posArr(3, 0);
		MGlobal::executeCommand("xform -q -t -ws " + geomToMove, posArr); // get position
		// We only need the height info, but we still need to create triIdx and coords because of method signature
		float height = 0.f;
		int triIdx = 0;
		vec2 coords;
		int res = WorldPalette::terrain.findHeight(height, triIdx, coords, vec3(posArr[0], posArr[1], posArr[2]));
		if (res) {
			MGlobal::executeCommand((std::string("move -y ") + std::to_string(height)).c_str());
		}
		return status;
	}

	// Start moving the distribution by first storing the geometry within it
	if (startMove) {
		worldPalette.moveDistributionSave();
		worldPalette.setCurrentDistribution(seltype, width, height, minBound, maxBound, center);
		return status;
	}
	// Update the geometry within selection region (if dpos is provided)
	if (dpos[0] != 0 || dpos[1] != 0) {
		// Now, move the distribution
		worldPalette.moveDistribution(dpos[0], dpos[1]);
		return status;
	}

	// Start resizing the distribution by first storing the geometry within it
	if (startResize) {
		worldPalette.setCurrentDistribution(seltype, width, height, minBound, maxBound, center);
		worldPalette.resizeDistributionSave(width);
		return status;
	}

	// Update the geometry within selection region (if dsize is provided)
	if (dsize[0] != 0 || dsize[1] != 0) {
		worldPalette.resizeDistribution(dsize[0], dsize[1]);
		return status;
	}

	// Save brush stroke position (if needed)
	if (saveBrush) {
		WorldPalette::brushStrokes.push_back(vec3(brushPos[0], 0, brushPos[1])); // push stroke center position
		//printVec2(MString("Saved stroke at position: "), brushPos);
	}

	// Use saved brush strokes, then reset the list
	if (releaseBrush) {
		worldPalette.brushDistribution(brushWidth);
		WorldPalette::brushStrokes.clear(); // empty the stroke list
		//printString(MString("Brush released!"), "");
	}

	// Use saved eraser strokes, then reset the list
	if (releaseEraser) {
		worldPalette.eraseDistribution(brushWidth);
		WorldPalette::brushStrokes.clear();
	}

	// Check if user wants to clear selection region
	if (clearSelection) {
		// First update the selection region
		worldPalette.setCurrentDistribution(seltype, width, height, minBound, maxBound, center);
		// Then clear the region
		worldPalette.clearDistribution();
		return status;
	}

	if (undoMove) {
		worldPalette.moveDistributionUndo();
		// Map selection region to terrain if there is one
		if (worldPalette.currentlySelectedRegion.selectedRegion.selectionType != SelectionType::NONE && worldPalette.terrain.isInitialized) {
			WorldPalette::terrain.updateSelectionRegion();
		}
		return status;
	}

	if (undoResize) {
		worldPalette.resizeDistributionUndo();
		// Map selection region to terrain if there is one
		if (worldPalette.currentlySelectedRegion.selectedRegion.selectionType != SelectionType::NONE && worldPalette.terrain.isInitialized) {
			WorldPalette::terrain.updateSelectionRegion();
		}
		return status;
	}

	if (undoPaste) {
		worldPalette.pasteDistributionUndo();
		return status;
	}

	if (undoBrush) {
		worldPalette.brushDistributionUndo();
		return status;
	}

	if (undoErase) {
		worldPalette.eraseDistributionUndo();
		return status;
	}

	if (undoClear) {
		worldPalette.clearDistributionUndo();
		return status;
	}

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
		if (width > 0 && dpos[0] == 0 && dpos[1] == 0) {
			// Delete existing geometry in the region and paste new geometry
			worldPalette.pasteDistribution(seltype, width, height, minBound, maxBound, center, paletteIdx);
		}
	}
	if (paletteSave) {
		printString("Saving Palette", "");
		worldPalette.savePalette();
		return status;
	}
	if (paletteLoad) {
		printString("Loading Palette", "");
		worldPalette.loadPalette();
		return status;
	}
	if (paletteClear) {
		printString("Clearing Palette", "");
		worldPalette.clearPalette();
		return status;
	}

	// checking arguments
#if DEBUG
	printFloat(MString("Width Argument: "), width);
	printFloat(MString("Height Argument: "), height);
	printVec3(MString("Min Argument: "), minBound);
	printVec3(MString("Max Argument: "), maxBound);
	printVec3(MString("Center Argument: "), center);
#endif
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
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/objects/grass.obj\" " + "\"" + loadPath + "/scene_objects/grass.obj\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/objects/grass.mtl\" " + "\"" + loadPath + "/scene_objects/grass.mtl\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/objects/shrub.obj\" " + "\"" + loadPath + "/scene_objects/shrub.obj\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/objects/shrub.mtl\" " + "\"" + loadPath + "/scene_objects/shrub.mtl\"");

	// Copy the icon files
	MGlobal::executeCommand("sysFile -makeDir \"" + w_palette_path + "/icons\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/icons/tree.png\" " + "\"" + loadPath + "/icons/tree.png\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/icons/big_rock.png\" " + "\"" + loadPath + "/icons/big_rock.png\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/icons/shrub.png\" " + "\"" + loadPath + "/icons/shrub.png\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/icons/grass.png\" " + "\"" + loadPath + "/icons/grass.png\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/icons/terrain.png\" " + "\"" + loadPath + "/icons/terrain.png\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/icons/terrain_delete.png\" " + "\"" + loadPath + "/icons/terrain_delete.png\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/icons/move.png\" " + "\"" + loadPath + "/icons/maya_icons/move.png\"");
	MGlobal::executeCommand("sysFile -copy \"" + w_palette_path + "/icons/resize.png\" " + "\"" + loadPath + "/icons/maya_icons/resize.png\"");

	// Execute MEL script
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