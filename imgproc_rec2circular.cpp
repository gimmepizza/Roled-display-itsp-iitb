#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cmath>

#define pi 3.14159265
#define lli long long int

using namespace std;
using namespace cv;

// Implemented first only for static image

/* 
	Rectangular image scaled along width to match radius requirements,
	sliced into strips, and then into regions;
	after that, averaging followed by conversion to polar form.
*/

// Structures to hold data
struct RRegion{
	int section,region;
};
struct CRegion{
	int sector,region;
};

// Declaration of global variables
const string default_image_path="/home/yash/Desktop/test.jpg";
const int no_of_leds=32;
int no_of_sectors=255;
const int start_led=15;
int radius,xcenter,ycenter;

/* Function declarations */
Mat modify_and_pixellate(Mat);
Mat createCircularImage(vector< vector< vector<int> > >);
RRegion calcRectangularRegionNumber(int ,int ,int ,int );
CRegion calcCircularRegionNumber(int ,int );
// function to calculate the distance between two pixels
double distanceFromCenter(int x,int y)
{
	return sqrt((x-xcenter)*(x-xcenter)+(y-ycenter)*(y-ycenter));
}
int toInt(string);


int main(int argc, char const *argv[])
{
	system("clear");
	string path; 
	cout<<"RECTANGULAR TO CIRCULAR CONVERSION\n";
	do{
		// setting up the image path from the user
		cout<<"\nEnter complete path of the image OR type \'def\' to use default image: \n";
		cin>>path;
		if(path=="def")
		{
			
			path=default_image_path;
			break;

		}
		else
		{

			Mat image=imread(path,CV_LOAD_IMAGE_COLOR);
			if(image.empty()) path="invalid";

		}

	}while(path=="invalid");

	// at this point, we have the correct path
	Mat frame=imread(path,CV_LOAD_IMAGE_COLOR);

	// ensuring that the width of each section is >0
	if(frame.cols<no_of_sectors) no_of_sectors=frame.cols;
	imshow("Selected image",frame);
	waitKey(0);
	imshow("Resultant image",modify_and_pixellate(frame));
	waitKey(0);

	return 0;
}

Mat modify_and_pixellate(Mat pixels)
{
	// storing the details of the image
	radius=(int)((pixels.rows*no_of_leds)/(no_of_leds-start_led));

	// vector to store the B,G,R values and the number of pixels
	vector< vector< vector<lli> > > data_of_regions(no_of_sectors,vector< vector<lli> >(no_of_leds, vector<lli>(4, 1)));

	// Parsing the image array
	for(int y=0;y<pixels.rows;y++)
	{
		for(int x=0;x<pixels.cols;x++)
		{
			Vec3b pixel=pixels.at<Vec3b>(Point(x,y));
			
			RRegion temp=calcRectangularRegionNumber(x,y,pixels.rows,pixels.cols);
			// accumulating B,G,R values in data_of_regions     			
			data_of_regions[temp.section][temp.region][0]+=pixel[0];
			data_of_regions[temp.section][temp.region][1]+=pixel[1];
			data_of_regions[temp.section][temp.region][2]+=pixel[2];
    		// incrementing the number of pixels in that region
			data_of_regions[temp.section][temp.region][3]++;
			
		}
	}

	// Vector storing the final data to be sent to Arduino
	vector< vector< vector<int> > > averaged_pixels(no_of_sectors,vector< vector<int> >(no_of_leds,vector<int>(3,0)));

	// Averaging out the color in all regions - data stored in B,G,R format
	for(int i=0;i<no_of_sectors;i++)
	{
		for(int j=0;j<no_of_leds;j++)
		{
			lli cnt = data_of_regions[i][j][3];
    		averaged_pixels[i][j][0]=(int)(data_of_regions[i][j][0]/cnt); // averaged blue color
    		averaged_pixels[i][j][1]=(int)(data_of_regions[i][j][1]/cnt); // averaged green color
    		averaged_pixels[i][j][2]=(int)(data_of_regions[i][j][2]/cnt); // averaged red color
    	}
    }

    // Generating the final image in circular form
    cout<<"Transformation complete.\n";
    return createCircularImage(averaged_pixels);
}

Mat createCircularImage(vector< vector< vector<int> > > averaged_pixels)
{
	Mat res_image(2*radius,2*radius,CV_8UC3,Scalar(255,255,255));
	
	xcenter=ycenter=radius;
	for(int y=0;y<res_image.rows;y++)
	{
		for(int x=0;x<res_image.cols;x++)
		{
			if(distanceFromCenter(x,y)<radius)
			{

				CRegion temp=calcCircularRegionNumber(x,y);
				res_image.at<Vec3b>(Point(x,y))=Vec3b(averaged_pixels[temp.sector][temp.region][0],averaged_pixels[temp.sector][temp.region][1],averaged_pixels[temp.sector][temp.region][2]);
				
			}
		}
	}
	// Conversion of grayscale image to binary (black and white) image
	Mat temp;
	cvtColor(res_image,temp,CV_RGB2GRAY);
	temp=temp>170;

	// Storing data in data_mono.txt
	ofstream fout("data_mono.txt"); 
	fout<<"[";
	for(int i=0;i<no_of_sectors;i++)
	{
		string binaryRes="";
		for(int j=0;j<no_of_leds;j++)
		{
					// storing format is in the order of the leds i.e. the first digit denotes the state of first led from the center and so on
			binaryRes+=(averaged_pixels[i][j][0]*0.07+averaged_pixels[i][j][1]*0.72+averaged_pixels[i][j][2]*0.21<128)?"0":"1";
		}
		fout<<toInt(binaryRes);
		if(i!=no_of_sectors-1) fout<<",";
	}
	fout<<"]";
	fout.close();
	//return (res_image>127);
	return temp;

}


RRegion calcRectangularRegionNumber(int x,int y,int h,int w)
{
	int delta_height=(int)(h/(no_of_leds-start_led));
	int delta_width=(int)(w/no_of_sectors);
	RRegion res;
	res.section=(int)(x/delta_width);
	res.region=(int)(((h-y)/delta_height)+start_led);
	// applying constraints
	res.section=(res.section>=no_of_sectors)? (no_of_sectors-1) : res.section;
	res.region=(res.region>=no_of_leds)? (no_of_leds-1) : res.region;

	return res;
}
CRegion calcCircularRegionNumber(int x,int y)
{
	// calculating distance from the center
	double dist=distanceFromCenter(x,y);
	// calculating angle from base line
	double angle=atan2(y-ycenter,x-xcenter);
	// converting angle to degrees [0,360]
	angle = (angle<0)? angle*180.00/pi + 360.00 : angle*180.00/pi;

	CRegion v;
	// calculating sector number
	double delta_theta=360.0/no_of_sectors;
	v.sector=(int)(angle/delta_theta);
	// calculating region number
	double delta_radius=radius/no_of_leds;
	v.region=(int)(dist/delta_radius);

	// applying constraints
	v.sector=(v.sector>=no_of_sectors)? (no_of_sectors-1) : v.sector;
	v.region=(v.region>=no_of_leds)? (no_of_leds-1) : v.region;

	return v;

}

int toInt(string bin)
{
	int cntr=bin.length()-1,pow=1,res=0;
	while(cntr>=0)
	{
		res+=((int)(bin[cntr])-(int)('0'))*pow;
		pow*=2;
		cntr--;
	}
	return res;
}