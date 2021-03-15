#pragma once
#ifndef WPPLUGIN_H
#define WPPLUGIN_H
#include <maya/MArgList.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>
#include "Distribution.h"
#include "helper.h"

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
		                vec3& maxBound);
};
#endif