#pragma once
#ifndef WPPLUGIN_H
#define WPPLUGIN_H
#include <maya/MArgList.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>
// custom Maya command
class WPPlugin : public MPxCommand
{

enum class SelectionType {
	NONE,
	PLANAR,
	RADIAL,
};

public:
	WPPlugin() {};
	virtual MStatus doIt(const MArgList& args);
	static void* creator();
	static MSyntax newSyntax();
	MStatus parseSyntax(const MArgList& argList, 
		                MString& name, 
		                WPPlugin::SelectionType& type, 
		                double& width, 
		                double& height,
					    double3& center,
		                double3& minBound, 
		                double3& maxBound);

};
#endif