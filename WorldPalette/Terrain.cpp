#include "Terrain.h"

bool Terrain::isPointInTriangle(vec2 p, vec2 p0, vec2 p1, vec2 p2)
{
	// Source: GeeksForGeeks (https://www.geeksforgeeks.org/check-whether-a-given-point-lies-inside-a-triangle-or-not/)

	float A = fabsf((p0[0] * (p1[1] - p2[1]) + p1[0] * (p2[1] - p0[1]) + p2[0] * (p0[1] - p1[1])) / 2.0);
	float A1 = fabsf((p[0] * (p1[1] - p2[1]) + p1[0] * (p2[1] - p[1]) + p2[0] * (p[1] - p1[1])) / 2.0);
	float A2 = fabsf((p0[0] * (p[1] - p2[1]) + p[0] * (p2[1] - p0[1]) + p2[0] * (p0[1] - p[1])) / 2.0);
	float A3 = fabsf((p0[0] * (p1[1] - p[1]) + p1[0] * (p[1] - p0[1]) + p[0] * (p0[1] - p1[1])) / 2.0);
	return fabsf(A - (A1 + A2 + A3)) < THRESHOLD;
}

double Terrain::distance2D(vec2 p0, vec2 p1)
{
	double dist = sqrt((p1[0] - p0[0]) * (p1[0] - p0[0]) + (p1[1] - p0[1]) * (p1[1] - p0[1]));
	// For debugging
	//printVec3(MString("sel point: "), vec3(p0[0], 0, p0[1]));
	//printVec3(MString("tile point: "), vec3(p1[0], 0, p1[1]));
	//printFloat(MString("distance: "), dist);
	return dist;
}

Terrain::Terrain() : width(0), height(0), sub_width(0), sub_height(0)
{

}

Terrain::Terrain(MString n, float w, float h, int sw, int sh) : name(n), width(w), height(h), sub_width(sw), sub_height(sh), offset(vec2(0, 0))
{
	// Allocate space for tile points
	for (int i = 0; i < sw; ++i) {
		std::vector<TerrainTile> row;
		int to_resize = sh;
		row.resize(to_resize);
		tileMap.push_back(row);
	}

	// Fill the height map per tile point
	fillTileMap();
	printString(MString("Created Terrain Object"), MString(""));
}

Terrain::~Terrain()
{

}

void Terrain::fillTileMap()
{
	for (int col = 0; col < (sub_height); ++col) {
		for (int row = 0; row < (sub_width); ++row) {
			MDoubleArray posArr1(3, 0);
			int idx = col * (sub_width + 1) + row;
			MGlobal::executeCommand("pointPosition " + name + ".vtx[" + idx + "]", posArr1);
			MDoubleArray posArr2(3, 0);
			idx += 1;
			MGlobal::executeCommand("pointPosition " + name + ".vtx[" + idx + "]", posArr2);
			MDoubleArray posArr3(3, 0);
			idx += (sub_width + 1);
			MGlobal::executeCommand("pointPosition " + name + ".vtx[" + idx + "]", posArr3);
			MDoubleArray posArr4(3, 0);
			idx -= 1;
			MGlobal::executeCommand("pointPosition " + name + ".vtx[" + idx + "]", posArr4);

			TerrainTile tile = {};
			tile.v1 = vec3(posArr1[0], posArr1[1], posArr1[2]);
			tile.v2 = vec3(posArr2[0], posArr2[1], posArr2[2]);
			tile.v3 = vec3(posArr3[0], posArr3[1], posArr3[2]);
			tile.v4 = vec3(posArr4[0], posArr4[1], posArr4[2]);
			tileMap[row][col] = tile;
		}
	}
}

vec2 Terrain::findOverlappingTile(vec3 pos)
{
	// TODO: Bring pos to terrain space (for now we don't need this assuming we don't allow terrain transformation)
	vec3 posT = pos;

	// Find the tile row
	float hw = width / 2;
	if (pos[0] >= -hw && pos[0] <= hw) {
		int row = (int)((pos[0] + hw) * sub_width / width);
		if (row == sub_width) {
			row = sub_width - 1; // Handle the case where pos[0] == hw
		}
		float hh = height / 2;
		if (pos[2] >= -hh && pos[2] < hh) {
			int col = (int)((hh - pos[2]) * sub_height / height);
			if (col == sub_height) {
				col = sub_height - 1; // Handle the case where pos[2] == -hh
			}
			return vec2(row, col);
		}
		else {
			// Position is out of bounds
			return vec2(0, -1);
		}
	}
	else {
		// Position is out of bounds
		return vec2(-1, -1);
	}
}

vec3 Terrain::findSurfaceNormalAtPoint(vec3 pos)
{
	// Find the height of the position on the terrain
	float height = 0;
	int triangle = 0;
	vec2 coords;
	int res = findHeight(height, triangle, coords, pos);
	if (res) {
		// Get the tile coordinates
		TerrainTile tile = tileMap[coords[0]][coords[1]];
		// Compute the surface normal for the 3 triangles formed by barycentric coordinates
		vec3 posOnTile = vec3(pos[0], height - OFFSET, pos[2]); // We don't want to use the offset here
		if (triangle == 1) {
			// Compute from v1, v2, v3
			vec3 n1 = ((posOnTile - tile.v1).Cross(posOnTile - tile.v2)).Normalize();
			vec3 n2 = ((posOnTile - tile.v2).Cross(posOnTile - tile.v3)).Normalize();
			vec3 n3 = ((posOnTile - tile.v3).Cross(posOnTile - tile.v1)).Normalize();
			return ((n1 + n2 + n3) / 3).Normalize(); // take the average of 3 normals
		}
		else if (triangle == 2) {
			// Compute from v3, v4, v1
			vec3 n1 = ((posOnTile - tile.v3).Cross(posOnTile - tile.v4)).Normalize();
			vec3 n2 = ((posOnTile - tile.v4).Cross(posOnTile - tile.v1)).Normalize();
			vec3 n3 = ((posOnTile - tile.v1).Cross(posOnTile - tile.v3)).Normalize();
			return ((n1 + n2 + n3) / 3).Normalize(); // take the average of 3 normals
		}
		else {
			// Something went wrong - debug this
			printString(MString("Invalid triangle index for surface normal computation!"), MString(""));
			return vec3(0, 0, 0);
		}
	}
	else {
		// if res = 0 something is wrong
		printString(MString("Cannot compute the surface normal at this position."), MString(""));
		return vec3(0, 0, 0);
	}
}

int Terrain::findHeight(float& height, int& triIdx, vec2& coords, vec3 pos)
{
	// Get the tile coordinates
	coords = findOverlappingTile(pos);
	if (coords[0] == -1) {
		// X out of bounds
		return 0;
	}
	else if (coords[1] == -1) {
		// Z out of bounds
		return 0;
	}
	TerrainTile tile = tileMap[coords[0]][coords[1]];

	// If the point is exactly on a tile point simply get the height
	if (Terrain::distance2D(vec2(pos[0], pos[2]), vec2(tile.v1[0], tile.v1[2])) < THRESHOLD) {
		triIdx = 1; // Intersection found with 1st triangle
		height = tile.v1[1] + OFFSET;
		return 1;
	}
	if (Terrain::distance2D(vec2(pos[0], pos[2]), vec2(tile.v2[0], tile.v2[2])) < THRESHOLD) {
		triIdx = 1; // Intersection found with 1st triangle
		height = tile.v2[1] + OFFSET;
		return 1;
	}
	if (Terrain::distance2D(vec2(pos[0], pos[2]), vec2(tile.v3[0], tile.v3[2])) < THRESHOLD) {
		triIdx = 1; // Intersection found with 1st triangle
		height = tile.v3[1] + OFFSET;
		return 1;
	}
	if (Terrain::distance2D(vec2(pos[0], pos[2]), vec2(tile.v4[0], tile.v4[2])) < THRESHOLD) {
		triIdx = 2; // Intersection found with 2nd triangle
		height = tile.v4[1] + OFFSET;
		return 1;
	}

	// Find which half plane (triangle) the position intersects

	// Triangle 1: v1, v2, v3
	bool doesIsect = Terrain::isPointInTriangle(vec2(pos[0], pos[2]), vec2(tile.v1[0], tile.v1[2]),
												vec2(tile.v2[0], tile.v2[2]), vec2(tile.v3[0], tile.v3[2]));
	if (doesIsect) {
		triIdx = 1; // Intersection found with 1st triangle
		// Find the y-coordinate of intersection
		// Get the plane equation
		vec3 cp = (tile.v2 - tile.v1) ^ (tile.v3 - tile.v1); // cross product
		cp.Normalize(); // normalize in place
		// Find missing variable in equation
		float d = -cp[0] * tile.v1[0] - cp[1] * tile.v1[1] - cp[2] * tile.v1[2];
		// Solve equation for y
		height = ((-cp[0] * pos[0] - cp[2] * pos[2] - d) / cp[1]) + OFFSET;
		return 1;
	}
	else {
		// Triangle 2: v3, v4, v1
		doesIsect = Terrain::isPointInTriangle(vec2(pos[0], pos[2]), vec2(tile.v3[0], tile.v3[2]),
											   vec2(tile.v4[0], tile.v4[2]), vec2(tile.v1[0], tile.v1[2]));
		if (doesIsect) {
			triIdx = 2; // Intersection found with 2nd triangle
			// Find the y-coordinate of intersection
			// Get the plane equation
			vec3 cp = (tile.v4 - tile.v3) ^ (tile.v1 - tile.v3); // cross product
			cp.Normalize(); // normalize in place
			// Find missing variable in equation
			float d = -cp[0] * tile.v3[0] - cp[1] * tile.v3[1] - cp[2] * tile.v3[2];
			// Solve equation for y
			height = ((-cp[0] * pos[0] - cp[2] * pos[2] - d) / cp[1]) + OFFSET;
			return 1;
		}
		else {
			// Something went wrong
			printString(MString("Selection region height: "), MString("Something went wrong."));
			// Print more information
			printVec3(MString("Current point: "), pos);
			printVec2(MString("Tile coordinates: "), coords);
			printVec3(MString("Tile v1: "), tile.v1);
			printVec3(MString("Tile v2: "), tile.v2);
			printVec3(MString("Tile v3: "), tile.v3);
			printVec3(MString("Tile v4: "), tile.v4);
			printString(MString(""), MString("\n"));
			return 0;
		}
	}
}

void Terrain::updateSelectionRegion()
{
	// Get the number of vertices of the selection region
	MIntArray numPoints;
	MGlobal::executeCommand("polyEvaluate -v selectionRegion", numPoints);

	// For each vertex, get its position and find its height on the terrain
	for (int i = 0; i < numPoints[0]; ++i) {
		MDoubleArray posArr(3, 0);
		MGlobal::executeCommand((std::string("pointPosition selectionRegion.vtx[") + std::to_string(i) + std::string("]")).c_str(), posArr);
		// We only need the height info, but we still need to create triIdx and coords because of method signature
		float height = 0.f;
		int triIdx = 0;
		vec2 coords;
		int res = findHeight(height, triIdx, coords, vec3(posArr[0], posArr[1], posArr[2]));
		// Test the normal function
		//vec3 norm = findSurfaceNormalAtPoint(vec3(posArr[0], posArr[1], posArr[2]));
		//printVec2(MString("Point: "), vec2(posArr[0], posArr[2]));
		//printVec3(MString("Surface normal: "), norm);
		if (res) {
			MGlobal::executeCommand((std::string("select -r selectionRegion.vtx[") + std::to_string(i) + std::string("]")).c_str());
			MGlobal::executeCommand((std::string("move -y ") + std::to_string(height)).c_str());
			MGlobal::executeCommand("select -cl");
		}
	}
}

