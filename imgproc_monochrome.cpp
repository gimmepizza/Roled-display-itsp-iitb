#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <cmath>
#include <ctype.h>


/* SUPPORTED MOTION OF MOTOR : ANTICLOCKWISE WITH INITIAL POSITION AS HORIZONTAL RIGHT */

using namespace std; 
using namespace cv;

#define pi 3.14159265
#define lli long long int 

// Structure to store regions' data
struct Region
{
	int sector; int region;
};
struct RRegion{
	int section,region;
};
struct CRegion{
	int sector,region;
};

// Declaration of general global variables
int no_of_leds=16;
int no_of_sectors=255;
int start_led=5;
const string default_image_path="/home/yash/Documents/ITSP\ 2k17/images/bm.png";

// Declaration of dynamic global variables
int xcenter;
int ycenter;
double radius;
Mat frame;

// Function to create the resulting image
Mat createResult(vector< vector< vector<int> > >,Mat);
// Function to pixellate an image after circular cropping
Mat pixellate(Mat);
// Function to convert binary string to an integer
int toInt(string);
// Function to pixellate an image after rec-to-circular conversion
Mat modify_and_pixellate(Mat);
// Function to create a circular image given the data of its sectors
Mat createCircularImage(vector< vector< vector<int> > >);
// Function to calculate rectangular region in which a point lies
RRegion calcRectangularRegionNumber(int ,int ,int ,int );
// Function to calculate circular region in which a point lies
CRegion calcCircularRegionNumber(int ,int );

// Function to calculate the distance between pixel and specified center
double distanceFromCenter(int x,int y)
{
	return sqrt((x-xcenter)*(x-xcenter)+(y-ycenter)*(y-ycenter));
}

// Function to calculate the sector and region number for a given pixel
Region calcRegionNumber(int x,int y)
{
	// calculating distance from the center
	double dist=distanceFromCenter(x,y);
	// calculating angle from base line
	double angle=atan2(y-ycenter,x-xcenter);
	// converting angle to degrees [0,360]
	angle = (angle<0)? angle*180.00/pi + 360.00 : angle*180.00/pi;

	Region v;
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
int main(int argc, char const *argv[])
{
	system("clear");
	string mode="image";
	cout<<"\t\tMONOCHROME DISPLAY - BINARY output\n";
	cout<<"Please select the mode :\n\t1. Static image\n\t2. Text\nYour choice (1 or 2): ";
	int choice; cin>>choice;
	mode=(choice==1)? "image" : (choice==2)? "text" : "fail";
	if(mode=="fail")
	{ 
		cout<<"Wrong option!";
		return -1;
	}
	else if(mode=="image")
	{
		system("clear");
		bool pass=false;
		// Accepting image from the user
		string image_path;
		do
		{
			std::cout<<"Enter the complete path of the image - \nType \"default\" (sans quotes) for using default image :\n";
			std::cin>>image_path;
			if(image_path=="default")
			{
				image_path=default_image_path;
				// Storing the specified image as a matrix of pixels
				frame = imread(image_path, CV_LOAD_IMAGE_COLOR); 
				pass=true;
			}
			else
			{

    		// Storing the specified image as a matrix of pixels
				frame = imread(image_path, CV_LOAD_IMAGE_COLOR); 

				if(! frame.data)
				{
					cout<<"Could not find OR load specified image.\nPlease try again.\n";
					pass=false;
				}
				else
				{
	    		// Displaying the selected image
					imshow("Selected image : Press any key to continue.",frame);
					waitKey(0);
					cout<<"Was that the image you wanted? Type 'y' OR 'n'.\n";
					char ans; cin>>ans;
					pass=(ans=='y')? true : false;
				}
			}
		}while(pass==false);

		imshow("Resulting image",pixellate(frame));
		waitKey(0);
	}
	else if(mode=="text")
	{ 
		system("clear");
		cout<<"Enter the text you want to display \n[max. 20 characters]\t[type '.' to end] :\n";

		// Code for video display
		/*
		int choice1; cin>>choice1;
		switch(choice1)
		{
			case 1: 
			{
				VideoCapture cap(0); // open the default camera
    			if(!cap.isOpened())  // check if we succeeded
    			{ 
    				cout<<"Error in opening camera!\n"; 
    				return -1;
    			}

    			for(int count=0;count<200;count++)
    			{
    				Mat frame;
        			cap >> frame; // get a new frame from camera
        			imshow("Pixellated video - 23 seconds",pixellate(frame));
        			waitKey(1);

        		}
        		cap.release();
        		waitKey(0);
        		break;
        	}
        	case 2: 
        	{
        		cout<<"Enter the exact path of the video file:\n";
        		string path; cin>>path;
        		VideoCapture vid_file;
        		if(!vid_file.open(path))
        		{
        			cout<<"Error in opening file.\n"; 
        			return -1; 
        		}
        		else
        		{
        			for(;;)
        			{
        				Mat frame;
        				vid_file >> frame;
        				if(frame.empty())
        				{
        					break;
        				}
        				else
        				{
        					imshow("Pixellated video", pixellate(frame));
        					waitKey(1);
        				}
        			}
        			vid_file.release();
        			waitKey(0);
        		}
        		break;
        	}
        	default: 
        	{
        		cout<<"Wrong option!\n";
        		return -1;
        	}
        }
        */
        char text[21]; 
        int j=0;
        for(;j<20;j++) 
        {
        	cin.get(text[j]);
        	if(text[j]=='.') 
        	{
        		text[j]='\0';
        		break;
        	}
        	else if(text[j]=='\n')
        	{
        		j--;
        	}
        }
        if(j==20) text[j]='\0';
        if(text[0]=='\0') { text[0]=' '; text[1]='\0'; }
        
        cout<<"\nText selected: "<<text<<"\n";
        int pos=0,length=0;
        for(;text[length]!='\0';length++);
        double size=51,scale=2.5;
        
        
        double x_pos=(1020-length*size)/2.0;

        while(pos<length)
        {
        	text[pos]=toupper(text[pos]);
        	pos++;
        }
        Mat text_img(100,1020,CV_8UC3,Scalar(255,255,255));
        putText(text_img,text,Point(x_pos,70),FONT_HERSHEY_COMPLEX,scale,Scalar(0,0,0),3);
        imshow("Entered text",text_img);
        waitKey(0);
        imshow("Preview",modify_and_pixellate(text_img));
        waitKey(0);

    }

    cout<<"Thank you!\n";
    return 0;
}

Mat pixellate(Mat pixels)
{
	// Defining vectors to store image components     
	vector< vector< vector<lli> > > data_of_regions(no_of_sectors,vector< vector<lli> >(no_of_leds, vector<lli>(4, 1)));


    // Defining the bounding circle
	xcenter=(int)(pixels.cols+1)/2;
	ycenter=(int)(pixels.rows+1)/2;
	radius=min(xcenter,ycenter);

    // Parsing the image array
	for(int y=0;y<pixels.rows;y++)
	{
		for(int x=0;x<pixels.cols;x++)
		{
			if(distanceFromCenter(x,y)<radius)
			{
				Vec3b pixel=pixels.at<Vec3b>(Point(x,y));
				Region temp=calcRegionNumber(x,y);
				// accumulating B,G,R values in data_of_regions     			
				data_of_regions[temp.sector][temp.region][0]+=pixel[0];
				data_of_regions[temp.sector][temp.region][1]+=pixel[1];
				data_of_regions[temp.sector][temp.region][2]+=pixel[2];
    			// incrementing the number of pixels in that region
				data_of_regions[temp.sector][temp.region][3]++;
			}
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


    cout<<"Averaging complete.\n";
    // recreating the averaged out image
    return createResult(averaged_pixels,pixels);
    
}


Mat createResult(vector< vector< vector<int> > > averaged_pixels, Mat org_img)
{
	
	Mat res_image=org_img;
	for(int y=0;y<res_image.rows;y++)
	{
		for(int x=0;x<res_image.cols;x++)
		{
			if(distanceFromCenter(x,y)<radius)
			{

				Region temp=calcRegionNumber(x,y);
				res_image.at<Vec3b>(Point(x,y))=Vec3b(averaged_pixels[temp.sector][temp.region][0],averaged_pixels[temp.sector][temp.region][1],averaged_pixels[temp.sector][temp.region][2]);
				
			}
			else
				res_image.at<Vec3b>(Point(x,y))=Vec3b(0,0,0);

		}
	}

	// Conversion of grayscale image to binary (black and white) image
	Mat temp;
	cvtColor(res_image,temp,CV_RGB2GRAY);
	temp=temp>127;

	// Storing the generated result in a text file which can be accessed later
	ofstream fout("data_mono.txt"); // static mode implies the file has to be overwritten
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
	    
	return temp;
}

Mat modify_and_pixellate(Mat pixels)
{
	// storing the details of the image
	radius=(unsigned int)((pixels.rows*no_of_leds)/(no_of_leds-start_led));
	
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