#include <iostream>
#include <vector>
#include "Point.h"
#include <fstream>

typedef unsigned char BYTE; //define BYTE

class Hough{
private:
    int r_size;
    int xSize;
    int ySize;
    
    std::vector< Point > threshAcc(int); //Threshold the accumulator
    std::vector< std::vector< Point > > getPoints(); //Get Points on lines
    bool PointInImage(Point, int, int); //To find whether a point is in the image
    
    
public:
    
    std::vector< Point > polarPoints;//Polar line coordinates
    unsigned char* acc; //The accumulator
    
    Hough(int, int, int, int); //constructor
    
    void fillAcc(int, int); //To fill in the accumulator
    std::vector< std::vector< Point > > drawLines(); //To draw in the lines found in the output image
    void savePoints(std::string, std::vector< std::vector< Point > > edgePoints); //save the points on the lines
    
};
