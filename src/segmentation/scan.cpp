#include "scan.h"

Scan::Scan (unsigned int w, unsigned int h, unsigned int d)
{
	width = w;
	height = h;
	depth = d;
	values =  (int*) calloc(d*h*w, sizeof(int));
}

int Scan::valueAt(unsigned int x, unsigned int y, unsigned int z)
{
	return values[height * width * z + width * y + x];
}

void Scan::setValue(unsigned int x, unsigned int y, unsigned int z, int w)
{
	values[height * width * z + width * y + x] = w;
}

bool Scan::hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

Scan::Scan(std::string dirPath)
{
	//printf("wearehere\n");
  DIR *dir;
  struct dirent *ent;
  int frames = 0;
  if ((dir = opendir (dirPath.c_str())) != NULL)
  {
  while ((ent = readdir (dir)) != NULL) {
    if(hasEnding(ent->d_name, ".dcm"))
    {
	  //OFCondition sometihng;
	  //DicomImage *image = new DicomImage(ent->d_name);
      frames++;
	  //printf("%d ", frames);
  	}
  }
  //printf("\n");
    closedir (dir);
  } else {
    /* could not open directory */
    perror ("could not open directory");
    //return EXIT_FAILURE;
  }
  if ((dir = opendir (dirPath.c_str())) != NULL)
  {
  depth = frames;
  frames = 0;
  bool init_hw = false;
  Magick::InitializeMagick("D:\\Programs\\ImageMagick-7.0.7-Q16");
  printf("we have advanced MAGIC!\n");
  while ((ent = readdir (dir)) != NULL) {
    if(hasEnding(ent->d_name, ".dcm"))
    {
	  //printf("trying to read scan:\n");
	  //printf(ent->d_name);
	  //printf("\n");
	  std::string file_name = ent->d_name;
	  std::string file_path = dirPath + file_name;
	  //printf(file_path.c_str());
	  //printf("\n");
	  Magick::Image dcmImg;
	  //printf("canilive?\n");
	  try {
		  dcmImg.read(file_path.c_str());
		  //dcmImg.read("D:\\JC_12151463_0298.dcm");
		  //dcmImg.read("D:\\gray.png");
	  } catch(std::exception &error_)
	  {
		  std::cout << "Caught exception: " << error_.what() << std::endl;
		  return;
	  }
	  //printf("first %d\n", init_hw);
      if (!init_hw) //first frame
      {
      	width = dcmImg.columns();
      	height = dcmImg.rows();
		printf("w %d h %d\n", width, height);
      	//assert(dcmImg.getFrames() == 1);
      	values =  (int*) calloc(depth*height*width, sizeof(int));
      	init_hw = true;
	  }
	  unsigned char *pixels = new unsigned char[width*height];
	  dcmImg.write(0, 0, width, height, "I", Magick::CharPixel, pixels);
	  //MagickCore::Quantum *pixels = dcmImg.getPixels(0, 0, width, height);

  	  for (unsigned int i = 0; i < height; i++)
  	  {
  	    for (unsigned int j = 0; j < width; j++)
	    {
	      //Magick::Quantum *pixel = pixels+i*width + j;
		  unsigned char *pixel = pixels + i*width + j;
	      setValue(j, i, frames, (int)*pixel); //col is assumed to be grayscale
	    }
      }
	  frames++;
	}
  }
    closedir (dir);
  } else {
    /* could not open directory */
    perror ("could not open directory the second time");
    //return EXIT_FAILURE;
  }
}

void Scan::createExample()
{
    assert(depth >= 4 && width >= 4 && height >= 4);
	setValue(0,0,0,0);
    setValue(1,0,0,0);
    setValue(0,1,0,0);
    setValue(1,1,0,0);
    setValue(0,0,1,0);
    setValue(1,0,1,0);
    setValue(0,1,1,0);
    setValue(1,1,1,2);

    setValue(0,0,2,0);
    setValue(1,0,2,0);
    setValue(0,1,2,0);
    setValue(1,1,2,2);
    setValue(0,0,3,0);
    setValue(1,0,3,0);
    setValue(0,1,3,0);
    setValue(1,1,3,0);

    setValue(2,2,0,0);
    setValue(3,2,0,0);
    setValue(2,3,0,0);
    setValue(3,3,0,0);
    setValue(2,2,1,2);
    setValue(3,2,1,0);
    setValue(2,3,1,0);
    setValue(3,3,1,0);

    setValue(2,2,2,2);
    setValue(3,2,2,0);
    setValue(2,3,2,0);
    setValue(3,3,2,0);
    setValue(2,2,3,0);
    setValue(3,2,3,0);
    setValue(2,3,3,0);
    setValue(3,3,3,0);

    setValue(0,2,0,0);
    setValue(1,2,0,0);
    setValue(0,3,0,0);
    setValue(1,3,0,0);
    setValue(2,0,1,0);
    setValue(3,0,1,0);
    setValue(2,1,1,2);
    setValue(3,1,1,0);

    setValue(0,2,2,0);
    setValue(1,2,2,2);
    setValue(0,3,2,0);
    setValue(1,3,2,0);
    setValue(2,0,3,0);
    setValue(3,0,3,0);
    setValue(2,1,3,0);
    setValue(3,1,3,0);

    setValue(2,0,0,0);
    setValue(3,0,0,0);
    setValue(2,1,0,0);
    setValue(3,1,0,0);
    setValue(0,2,1,0);
    setValue(1,2,1,2);
    setValue(0,3,1,0);
    setValue(1,3,1,0);

    setValue(2,0,2,0);
    setValue(3,0,2,0);
    setValue(2,1,2,2);
    setValue(3,1,2,0);
    setValue(0,2,3,0);
    setValue(1,2,3,0);
    setValue(0,3,3,0);
    setValue(1,3,3,0);
}

void Scan::basicPrint()
{
    for (unsigned int j = 0; j < width*height*depth; j++)
	{
		printf("%d ", values[j]);
	}
	printf("\n");
}

void Scan::prettyPrint()
{
    for (unsigned int j = 0; j < width; j++)
	{
		printf("----");
	}
	printf("\n");
    for (unsigned int i = 0; i < depth; i++)
	{
        for (unsigned int j = 0; j < height; j++)
		{
			printf("|");
            for (unsigned int k = 0; k < width; k++)
			{
				printf("%d", valueAt(k, j, i));
				if (k!=width-1)
				{
					printf(", ");
				}
			}
			printf("|\n");
		}
        for (unsigned int j = 0; j < width; j++)
		{
			printf("----");
		}
		printf("\n");
	}
	printf("\n");
}
	
int Scan::getMax()
{
	return *std::max_element(values, values + depth*height*width);
}

int Scan::getMin()
{
	return *std::min_element(values, values + depth*height*width);
}

unsigned int Scan::getWidth()
{
	return width;
}

unsigned int Scan::getHeight()
{
	return height;
}

unsigned int Scan::getDepth()
{
	return depth;
}

bool Scan::equals(Scan other)
{
	for (int i = 0; i < depth; i++)
	{
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < width; k++)
			{
				if (valueAt(k, j, i) != other.valueAt(k, j, i)) return false;
			}
		}
	}
	return true;
}

Scan Scan::copy()
{
	Scan res(width, height, depth);
	for (int i = 0; i < depth; i++)
	{
		for (int j = 0; j < height; j++)
		{
			for (int k = 0; k < width; k++)
			{
				res.setValue(k, j, i, valueAt(k, j, i));
			}
		}
	}
	return res;
}

void Scan::getCube(int cube[8], unsigned int x, unsigned int y, unsigned int z)
{  
  cube[0] = valueAt(x, y, z),
  cube[1] = valueAt(x + 1, y, z),
  cube[2] = valueAt(x + 1, y + 1, z),
  cube[3] = valueAt(x, y + 1, z),
  cube[4] = valueAt(x, y, z + 1),
  cube[5] = valueAt(x + 1, y, z + 1),
  cube[6] = valueAt(x + 1, y + 1, z + 1),
  cube[7] = valueAt(x, y + 1, z + 1);
  //return cube;
}

void Scan::freeValues()
{
	free(values);
}
