#pragma once
#ifndef WPPLUGIN_H
#define WPPLUGIN_H
#include <maya/MArgList.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>
#include "Distribution.h"
#include "WorldPalette.h"
#include "helper.h"

WorldPalette worldPalette;

// custom Maya command
class WPPlugin : public MPxCommand
{

public:
	WPPlugin() {};
	virtual MStatus doIt(const MArgList& args);
	static void* creator();
	static MSyntax newSyntax();
	MStatus parseSyntax(const MArgList& argList, 
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
						bool& uninitializeTerrain,
						double& terrainWidth,
						double& terrainHeight,
						int& terrainSubWidth,
						int& terrainSubHeight,
						MString& geomToMove,
						bool& startMove,
						vec2& dpos,
						bool& saveBrush,
						vec2& brushPos,
						bool& releaseBrush,
						double& brushWidth);
};
#endif