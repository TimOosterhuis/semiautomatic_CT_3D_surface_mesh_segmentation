#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <assert.h>
#include <algorithm>
#include <iostream>

#include <dirent.h>

#include <Magick++.h>

//#include <dcmtk\config\osconfig.h> 
//#include <dcmtk\dcmimgle\dcmimage.h>
//#include <dcmtk\ofstd\ofcond.h>

#pragma comment(lib, "CORE_RL_Magick++_.lib")
#pragma comment(lib, "CORE_RL_MagickCore_.lib")
//#pragma comment(lib, )
//#pragma comment(lib, "ofstd.lib")
//#pragma comment(lib, "oflog.lib")
//#pragma comment(lib, "dcmdata.lib")
//#pragma comment(lib, "dcmimgle.lib")

#pragma once

class Scan
{
	unsigned int width, height, depth;
	int *values;
	public:
	Scan (unsigned int, unsigned int, unsigned int);
	Scan (std::string);
	bool hasEnding(std::string const &, std::string const &);
	int valueAt(unsigned int, unsigned int, unsigned int);
	void setValue(unsigned int, unsigned int, unsigned int, int);
	void createExample();
	void prettyPrint();
	void basicPrint();
	int getMax();
	int getMin();
	unsigned int getWidth();
	unsigned int getHeight();
	unsigned int getDepth();
	bool equals(Scan);
	Scan copy();
    void getCube(int [8], unsigned int, unsigned int, unsigned int);
	void freeValues();
};
