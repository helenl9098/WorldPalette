#pragma once

#include "vec.h"
#include <vector>
#include "helper.h"
#include <map>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <string>

#define OFFSET 0.5
#define THRESHOLD 1e-3

struct TerrainTile
{
	// 4 corners of the tile (counterclockwise order)
	vec3 v1;
	vec3 v2;
	vec3 v3;
	vec3 v4;
};


class Terrain
{
	public:
		Terrain(); // default constructor
		Terrain(MString n, float w, float h, int sw, int sh);
		~Terrain();

		void fillTileMap(); // fill the tileMap

		vec2 findOverlappingTile(vec3 pos); // return the overlapping tile index for scene geometry position

		vec3 findSurfaceNormalAtPoint(vec3 pos); // returns the surface normal at given world position

		int findHeight(float& height, int& triIdx, vec2& coords, vec3 pos); // find the height of given position (returns 1: found it successfully, returns 0: unsuccessful)

		void updateSelectionRegion();

		static bool isPointInTriangle(vec2 p, vec2 p0, vec2 p1, vec2 p2); // checks if 2D point is within given triangle

		static double distance2D(vec2 p0, vec2 p1);

		bool isInitialized; // is this terrain initialized?
		std::vector<std::vector<TerrainTile>> tileMap; // height map for tiles
		MString name; // geom name
		float width; // terrain width
		float height; // terrain height
		int sub_width; // number of width subdivisions
		int sub_height; // number of height subdivisions
		vec2 offset; // how much has the terrain moved? (potentially change this to a transform matrix)
};
