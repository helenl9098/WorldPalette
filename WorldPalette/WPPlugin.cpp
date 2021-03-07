#include "WPPlugin.h"
#include <maya/MFnPlugin.h>
#include <maya/MSyntax.h>
#include <maya/MArgParser.h>
#include <maya/MArgDatabase.h>

#define kNameFlag "-n"
#define kNameFlagLong "-name"

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
	return syntax;
}

MStatus WPPlugin::parseSyntax(const MArgList& argList, MString& name)
{
	MStatus stat = MS::kSuccess;
	MArgDatabase parser(syntax(), argList, &stat);
	if (!stat)
		stat.perror("parsing failed");

	if (parser.isFlagSet(kNameFlag))
	{
		MString temp;
		stat = parser.getFlagArgument(kNameFlag, 0, temp);
		name = temp;
	}
	else if (parser.isFlagSet(kNameFlagLong))
	{
		MString temp;
		stat = parser.getFlagArgument(kNameFlagLong, 0, temp);
		name = temp;
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
	parseSyntax(argList, name);

	// Plugin's functionality. Just a dialog box for now. 
	MString caption("Hello Maya");
	MString text("A Text Box!");
	MString messageBoxCommand = ("confirmDialog -title \"" + caption + "\" -message \"" + text + "\" -button \"Ok\" -defaultButton \"Ok\""); \

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