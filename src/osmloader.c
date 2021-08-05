#include "osmloader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* global variables */
char *getNode_valString;

void getTagName(char*, char**);
void getBounds(char*, osm**);
static inline void getNode(char*, osm**, int);

void osmLoad(char* fileName, osm **mapData)
{
	*mapData = (osm*) malloc(sizeof(osm));
	osm *ptrMapData = *mapData;
	ptrMapData->numNodes = 0;
	ptrMapData->numWays = 0;
	ptrMapData->numTags = 0;
	ptrMapData->numRelations = 0;

	FILE* osm_file = fopen(fileName, "rb");
	if(osm_file == NULL)
	{
		printf("Could not open file \'%s\'\n", fileName);
		return;
	}

	getNode_valString = (char*) calloc(32, sizeof(char));
	char *lineBuffer = (char*) calloc(1024, sizeof(char)),
		  *tagStart,
	     *tagEnd,
		  *nameSeparator,
		  *tagName = (char*) calloc(32, sizeof(char)),
		  *keyStart,
		  *valueStart;
	
	int line = 0,
		 tagListPos = 0,
		 tabs = 0,
		 keyLength,
		 valueLength;

	// read a line
	fgets(lineBuffer, 1024, osm_file);

	// count all items in file
	printf("Counting nodes, ways and relations...\n");
	while(!feof(osm_file))
	{
		if(tagStart = strstr(lineBuffer, "<"))
		{
			// if next char is '/' this is a tag ending
			if(tagStart[1] == '/')
			{
				;//printf("Tag ending: %s", lineBuffer);
			} else {
				getTagName(tagStart, &tagName);
				if(!strcmp(tagName, "bounds"))
				{
					getBounds(tagStart, &ptrMapData
			);
				} else
				if(!strcmp(tagName, "node"))
				{
					ptrMapData
			->numNodes++;
				} else
				if(!strcmp(tagName, "way"))
				{
					ptrMapData
			->numWays++;					
				} else
				if(!strcmp(tagName, "relation"))
				{
					ptrMapData
			->numRelations++;
				}
			}
		} else
			printf("Broken line: %s", lineBuffer);
		
		// read new line here to avoid missing feof()
		fgets(lineBuffer, 512, osm_file);
	}

	printf("Nodes: %d | Ways: %d | Relations: %d\n", ptrMapData->numNodes, ptrMapData->numWays, ptrMapData->numRelations);
	
	// allocate memory for structs
	ptrMapData->nodes = (node*) malloc(sizeof(node) * ptrMapData->numNodes);
	ptrMapData->ways = (way*) malloc(sizeof(way) * ptrMapData->numWays);
	ptrMapData->relations = (relation*) malloc(sizeof(relation) * ptrMapData->numRelations);
	
	int nodeCount = 0,
		 wayCount = 0,
		 relationCount = 0,
		 progCnt = 0;

	rewind(osm_file);
	fgets(lineBuffer, 1024, osm_file);
	while(!feof(osm_file))
	{		
		if(tagStart = strstr(lineBuffer, "<"))
		{
			// if next char is '/' this is a tag ending
			if(tagStart[1] == '/')
			{
				;//printf("Tag ending: %s", lineBuffer);
			} else {
				getTagName(tagStart, &tagName);
				if(!strcmp(tagName, "node"))
				{
					if(progCnt++ == 5000)
					{
						progCnt = 0;
						printf("^%c[2K\r", 27);
						printf("loading... (%.5f%%)", ((float)nodeCount / (float)ptrMapData->numNodes * 100.0f));
					}
					getNode(tagStart, &ptrMapData, nodeCount);
					nodeCount++;
				} else
				if(!strcmp(tagName, "way"))
				{	
					wayCount++;
				} else
				if(!strcmp(tagName, "relation"))
				{
					relationCount++;
				}
			}
		} else
			printf("Broken line: %s", lineBuffer);
		
		// read new line here to avoid missing feof()
		fgets(lineBuffer, 1024, osm_file);
	}
	printf("^%c[2K\r", 27); // clear console line
	printf("loading... (%.5f%%)", ((float)nodeCount / (float)ptrMapData->numNodes * 100.0f));
	
	// free used memory
	free(lineBuffer);
	free(getNode_valString);
	free(tagName);
}
void getTagName(char *src, char **dest)
{
	while(*src == '<' || *src == '/')
		src++;
	memset(*dest, 0, 32);
	strncpy(*dest, src, strstr(src, " ") - src);
}

static inline void getNode(char *src, osm **osmData, int index)
{
	node *nptr = &(*osmData)->nodes[index];
	
	// id
	char *valueStart = strstr(src, "id=") + 4;
	int valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	nptr->id = atoi(getNode_valString);
	
	// lat
	valueStart = strstr(src, "lat=") + 5;
	valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	nptr->latitude = 2 * ((atof(getNode_valString) - (*osmData)->minLat) / (*osmData)->deltaLat) - 1;
	
	// lon
	valueStart = strstr(src, "lon=") + 5;
	valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	nptr->longitude = 2 * ((atof(getNode_valString) - (*osmData)->minLon) / (*osmData)->deltaLon) - 1;
}

void getBounds(char *src, osm **in)
{
	char *valueStart = strstr(src, "minlat=") + 8;
	int valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	(*in)->minLat = atof(getNode_valString);	
	memset(getNode_valString, 0, valLength);
 
	valueStart = strstr(src, "maxlat=") + 8;
	valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	(*in)->maxLat = atof(getNode_valString);
	memset(getNode_valString, 0, valLength);

	valueStart = strstr(src, "minlon=") + 8;
	valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	(*in)->minLon = atof(getNode_valString);
 	memset(getNode_valString, 0, valLength);

	valueStart = strstr(src, "maxlon=") + 8;
	valLength = strstr(valueStart, "\"") - valueStart;
	strncpy(getNode_valString, valueStart, valLength);
	(*in)->maxLon = atof(getNode_valString);
 	memset(getNode_valString, 0, valLength);

	(*in)->deltaLat = (*in)->maxLat - (*in)->minLat;
	(*in)->deltaLon = (*in)->maxLon - (*in)->minLon;

	printf("minLat: %f | minLon %f | maxLat %f | maxLon %f | dLat %f | dLon %f\n", (*in)->minLat, (*in)->minLon, (*in)->maxLat, (*in)->maxLon, (*in)->deltaLat, (*in)->deltaLon);
}

void osmFree(osm **osmData)
{
	printf("Cleaning up osmloader memory...\n");
	// TODO: IMPLEMENT! -> clean up all memory used by osm struct and its children
	printf("\tCleaning ways...\n");
	free((*osmData)->ways);
	printf("\tCleaning nodes...\n");
	free((*osmData)->nodes);
	printf("\tCleaning relations...\n");
	free((*osmData)->relations);

	free(*osmData);
	printf("\tDone!\n");
}