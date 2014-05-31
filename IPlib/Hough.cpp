#include "Hough.h"
#include <math.h>

#define PI 3.14159265

Hough::Hough(int theta, int r, int width, int height){
    
    acc = (unsigned char*)calloc(width*height, sizeof(unsigned char)); //Create the accumulator and fill with 0s
    
    xSize = width; //set image width
    ySize = height;//set image height
    r_size = r;//set maximum r value
}

void Hough::fillAcc(int x, int y){
    int yCoord = 550 - y; //pixel locations are referenced from top left but origin in hough transform is bottom left
    for(int theta = 0; theta < 180; theta++){ //for all angles from 0 to 179
        double r = (double)x*cos((double)theta * PI/180) + (double)yCoord*sin((double)theta * PI/180); // r = x.cos(theta) + y.sin(theta)
        acc[(int)((round(r+r_size))*180) + theta] += 1; //increase the value in the accumulator at that point
    }
}

std::vector<Point> Hough::threshAcc(int thresh){ //Threshold the accumulator
    std::vector< Point > polarPoints;
    for(int i = 0; i < 180*r_size*2; i++){
        int theta = i % 180;
        int r = (i - theta) / 180;
        
        if(acc[i] > thresh){ //if above threshold
            Point temp((theta), (r - r_size));
            polarPoints.push_back(temp); //keep the polar coordinate
        }
        else{
            acc[i] = 0;
        }
    }
    return polarPoints;
}

bool Hough::PointInImage(Point P, int width, int height){ //check whether a point falls in the bounds of image
    if((P.x >= 0)&&(P.x<width)&&(P.y >= 0)&&(P.y < height)){
        return true;
    }
    else{
        return false;
    }
}

std::vector< std::vector< Point > > Hough::getPoints(){ //to find the points on lines in the image
    std::vector< std::vector< Point > > edgePoints;
    //save the accumulator
    FILE *fp = fopen("accumulator.raw", "wb");
    if (fp) {
        
        fwrite(acc, (180) * (r_size*2), 1, fp);
        fclose(fp);
        std::cout<<"Accumulator saved"<<std::endl;
    }
    else{
        std::cout<<"Could not save file"<<std::endl;
    }
    
    polarPoints = threshAcc(200);//threshold and get polar coords
    
    for(int i = 0; i < polarPoints.size(); i++){
        
        Point p1, p2, p3, p4; //Where the lines cross points where x = 0, x = width, y = 0, y = height
        
        //x = r - y.sin(theta) / cos(theta)    y = r - x.cos(theta) / sin(theta)
        p1.x = 0;
        p1.y = (polarPoints[i].y - p1.x * cos(polarPoints[i].x*(PI/180))) /sin(polarPoints[i].x*(PI/180));
        
        p2.x = xSize - 1;
        p2.y = (polarPoints[i].y - p2.x * cos(polarPoints[i].x*(PI/180))) /sin(polarPoints[i].x*(PI/180));
        
        p3.y = 0;
        p3.x = (polarPoints[i].y - p3.y * sin(polarPoints[i].x*(PI/180))) /cos(polarPoints[i].x*(PI/180));
        
        p4.y = ySize - 1;
        p4.x = (polarPoints[i].y - p4.y * sin(polarPoints[i].x*(PI/180))) /cos(polarPoints[i].x*(PI/180));
        
        std::vector< Point > temp_vec;
        
        //if the points are in the image save the points
        if(PointInImage(p1, xSize, ySize)){
            temp_vec.push_back(p1);
        }
        if(PointInImage(p2, xSize, ySize)){
            temp_vec.push_back(p2);
        }
        if(PointInImage(p3, xSize, ySize)){
            temp_vec.push_back(p3);
        }
        if(PointInImage(p4, xSize, ySize)){
            temp_vec.push_back(p4);
        }
        
        if(temp_vec.size() != 0) {
            edgePoints.push_back(temp_vec);
        }
        
    }
    return edgePoints;
}

std::vector< std::vector< Point > > Hough::drawLines(){ //draw the lines found
    BYTE *lines_buffer = (unsigned char*)calloc(xSize*ySize, sizeof(unsigned char)); //image full of 0s
    
    std::vector< std::vector< Point > > edgePoints = getPoints();

    for(int i = 0; i<xSize*ySize; i++){
        int x = i%xSize;
        int y = ySize - ((i - x)/xSize);
        
    
        for(int j = 0; j<edgePoints.size(); j++){
            if(((x>=edgePoints[j][0].x)&&(x<=edgePoints[j][edgePoints[j].size()-1].x))||((x<=edgePoints[j][0].x)&&(x>=edgePoints[j][edgePoints[j].size()-1].x))){//if between two points
                int lineY = (polarPoints[j].y - x*cos(polarPoints[j].x*(PI/180)))/sin(polarPoints[j].x*(PI/180)); //given x find y on line
                
                if(abs(lineY - y) < 1){ //to account for rounding errors
                    double a = 255;
                    lines_buffer[i] = a; //make pixel white
                }
            }
        }
    }
    
    //Save image with lines on.
    FILE *fp = fopen("output.raw", "wb");
    if (fp) {
        
        fwrite(lines_buffer, (xSize) * (ySize), 1, fp);
        fclose(fp);
        std::cout<<"Output image saved"<<std::endl;
    }
    else{
        std::cout<<"Could not save file"<<std::endl;
    }
    
    return edgePoints;
}

void Hough::savePoints(std::string filename, std::vector< std::vector< Point > > edgePoints){ //save the points on the lines
    std::ofstream myfile (filename);
    if (myfile.is_open())
    {
        //save points for each line on a line
        for(int i = 0; i < edgePoints.size(); i++){
            for(int j = 0; j < edgePoints[i].size(); j++){
                myfile << edgePoints[i][j].x<<" "<<edgePoints[i][j].y<<" ";
            }
            myfile << "\n";
        }
        myfile.close();
        std::cout<<"Line Points saved"<<std::endl;
    }
    else std::cout << "Unable to open file";
}


