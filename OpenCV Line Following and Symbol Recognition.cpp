// Include files for required libraries
#include <stdio.h>
#include <unistd.h>
#include "opencv_aee.hpp"
#include "main.hpp"     // You can use this file for declaring defined values and functions

extern "C"{
#include "pi2c.h"
}
#define I2C_DEVICE_ADDRESS 0X04
#include <iostream>
using namespace cv;
using namespace std;
int colourUsed = 2;
int lowH = 0, highH = 0, lowS = 0, highS = 0, lowV = 0, highV = 0;
//int lowHp=0, highHp=0, lowSp=0, highSp=0,lowVp=0, highVp=0;


Pi2c car(0x22);

void setup(void)
{
    setupCamera(320, 240);  // Enable the camera for OpenCV
    // resizeCamera(320,20);
}

int main( int argc, char** argv )
{

    setup();    // Call a setup function to prepare IO and devices


//    namedWindow("Photo");   // Create a GUI window called photoa
//
//    namedWindow("HSV Tester");   // Create a GUI window called photo
//
//        // Initialise some variables for HSV limits
//
//    createTrackbar("Low Hue", "HSV Tester", &lowHp, 179, NULL);      // Create trackbar controls for each HSV limit
//    createTrackbar("High Hue", "HSV Tester", &highHp, 179, NULL);
//
//    createTrackbar("Low Sat", "HSV Tester", &lowSp, 255, NULL);
//    createTrackbar("High Sat", "HSV Tester", &highSp, 255, NULL);
//
//    createTrackbar("Low Value", "HSV Tester", &lowVp, 255, NULL);
//    createTrackbar("High Value", "HSV Tester", &highVp, 255, NULL);
//

 Mat startemplate = imread("/home/b18/Desktop/Star (Green Line).png");
        Mat circletemplate = imread("/home/b18/Desktop/Circle (Red Line).png");
        Mat triangletemplate = imread("/home/b18/Desktop/Triangle (Blue Line).png");
        Mat umbrellatemplate = imread("/home/b18/Desktop/Umbrella (Yellow Line).png");
        Mat starhsv, circlehsv, trianglehsv, umbrellahsv, rotatedimg, rotatedhsv;

        cvtColor(startemplate,starhsv,COLOR_BGR2HSV);
        cvtColor(circletemplate,circlehsv,COLOR_BGR2HSV);
        cvtColor(triangletemplate,trianglehsv,COLOR_BGR2HSV);
        cvtColor(umbrellatemplate,umbrellahsv,COLOR_BGR2HSV);
        inRange(starhsv,Scalar(150,100,110),Scalar(160,255,255),starhsv);
        inRange(circlehsv,Scalar(150,100,110),Scalar(160,255,255),circlehsv);
        inRange(trianglehsv,Scalar(150,100,110),Scalar(160,255,255),trianglehsv);
        inRange(umbrellahsv,Scalar(150,100,110),Scalar(160,255,255),umbrellahsv);

        int flagstop=0;
        int  flagrev=0;
    while(1)    // Main loop to perform image processing
    {


        Mat frame;
        Mat transformed;

        while(frame.empty())
            frame = captureFrame(); // Capture a frame from the camera and store in a new matrix variable

        imshow("Photo", frame); //Display the image in the window


        int key = waitKey(1);   // Wait 1ms for a keypress (required to update windows)

        Mat cframe = frame(Rect(0,0,320,20));
        int thresholdvalue = 30;
int iofmaxarea=-1;


        Mat symbolframeHSV;
        Mat symbolframeContours;
        Mat symbolgrayscale;
        cvtColor(frame,symbolframeHSV,COLOR_BGR2HSV);
        inRange(symbolframeHSV,Scalar(117,23,0),Scalar(170,255,255),symbolframeHSV);

        //cvtColor(symbolframeHSV,symbolframeHSV,COLOR_GRAY2BGR);

        Mat dilated;
        dilate(symbolframeHSV, dilated, getStructuringElement(MORPH_RECT, Size(3, 3)));

        int numofpink =countNonZero(symbolframeHSV);
        // Create a integer to store the result
        //numNonZero = countNonZero(frame); // Count the number of non-zero pixels
        if( numofpink>=860)
        {
            flagstop=(flagstop+1);
        }
        else
        {
            flagstop=0;
        }
        cout<<"pink"<<numofpink<<endl;

        // Morphological Opening
      //  morphologyEx(symbolframeHSV, morphed, MORPH_CLOSE, getStructuringElement(MORPH_RECT, Size(5, 5)));


      //  imshow("HSV Star", starhsv);


        vector< vector<Point> > contourspink; // Declare space for contours
        vector<Vec4i> hierarchypink;              // Declare space for hierarchy


        findContours(dilated, contourspink, hierarchypink, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
        vector< vector<Point> > approxedcontours (contourspink.size());
//
        int maximumArea2 = 0;
        int iofmaxarea2 = -1;
        for(int i = 0; i < contourspink.size(); i++) // Loop through the contours
        {
            approxPolyDP(contourspink[i],approxedcontours[i], 10, true);

            int area2 = contourArea(contourspink[i]);
            if (area2 > maximumArea2){
                maximumArea2 = area2;
                iofmaxarea2 = i;
            }




        }

        if (iofmaxarea2>0){
            drawContours(frame, approxedcontours, iofmaxarea2, Scalar(0,0,255), 2, LINE_8,noArray(), 0,Point()); // Draw each in red
            transformed =transformPerspective(approxedcontours[iofmaxarea2], symbolframeHSV, 350,350);



            rotate(transformed,transformed,ROTATE_180);
            //cvtColor(transformed,transformed,COLOR_BGR2HSV);
            //        inRange(rotatedhsv,Scalar(120,0,61),Scalar(174,230,148),rotatedhsv);




            if (transformed.cols > 0 && transformed.rows > 0){
//                cout<<starhsv.rows<<endl;
//                cout<<starhsv.cols<<endl;
//                        imshow("rotatehsv",transformed);
//                        imshow("starhsv",starhsv);
//                Mat xyzconcat;
//                vconcat(transformed,starhsv,xyzconcat);
//
//                imshow("tra and star", xyzconcat);
                float starg = compareImages(transformed, starhsv);
                float circleg = compareImages(transformed, circlehsv);
                float triangleg = compareImages(transformed, trianglehsv);
                float umbrellag = compareImages(transformed, umbrellahsv);

                            if (starg>thresholdvalue){
                                colourUsed = 3;
                                cframe = frame(Rect(0,0,320,240));
                            }
                            else if (circleg>thresholdvalue){
                                colourUsed = 4;
                                cframe = frame(Rect(0,0,320,240));
                            }
                            else if (triangleg>thresholdvalue){
                                colourUsed = 1;
                                cframe = frame(Rect(0,0,320,240));
                            }
                            else if (umbrellag>thresholdvalue){
                                colourUsed = 5;
                                cframe = frame(Rect(0,0,320,240));
                            }
//                            else{
//                           // default back to black
//                                colourUsed = 2;
//                                cframe = frame(Rect(0,0,320,40));
                        //}
                       // cout<<"compare"<<triangleg<<endl;

             //   cout<<"Colour used is"<<colourUsed;

                //imshow("transformed image", rotatedimg); //You forgot to declare any values for rotatedimg
               imshow("transformed", transformed);
            }
            //
        }



       imshow("Frame with Contours Symbol", frame);



        switch(colourUsed)
        {
            case 1:

                cframe = frame(Rect(0,0,320,240));
                lowH = 90, highH =179, lowS = 154, highS=255, lowV = 0, highV = 255;
                break; //blue
            case 2:

                cframe = frame(Rect(0,0,320,20));
                lowH = 0, highH = 179, lowS = 0, highS = 255, lowV = 0, highV = 43;
                break;//black
            case 3:

                cframe = frame(Rect(0,0,320,240));
                lowH = 68, highH = 103, lowS = 186, highS = 255, lowV = 60, highV = 255;
                break; //green
            case 4:

                cframe = frame(Rect(0,0,320,240));
                lowH = 169, highH = 179, lowS = 90, highS = 255, lowV = 0, highV = 255;
                break; //red

        }

        Mat frameHSV, frameContours, grayscale, equalised;

        Mat thresholded;
        cvtColor(cframe,frameHSV,COLOR_BGR2HSV);
        inRange(frameHSV,Scalar(lowH,lowS,lowV),Scalar(highH,highS,highV),frameHSV);
        if (countNonZero(frameHSV)<=40){

       flagrev = 1;

       }
       else{

       flagrev = 0;
       }
        cvtColor(frameHSV,frameHSV,COLOR_GRAY2BGR);
       cvtColor(frameHSV, grayscale, COLOR_BGR2GRAY); // Convert to grayscale

       imshow("HSV TEST",frameHSV);





        vector< vector<Point> > contours; // Declare space for contours
        vector<Vec4i> hierarchy;              // Declare space for hierarchy

        findContours(grayscale, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
        int maximumArea = 0;
        int area;
       // imshow("greyscale", grayscale);
        for (int i = 0; i < contours.size(); i++) { // Loop through all contours
        area = contourArea(contours[i]);
            if (area > maximumArea){
                maximumArea = area;
                iofmaxarea = i;
            }

        }

        // Create a integer to store the result
        //numNonZero = countNonZero(frame); // Count the number of non-zero pixels

        //cout<<"Line"<<numofline<<endl;

        if (iofmaxarea >= 0){
            drawContours(cframe, contours, iofmaxarea, Scalar(0, 0, 255), 2, LINE_8, noArray(), 0, Point());
            Point regionCentre = findContourCentre(contours[iofmaxarea]); // Calculate the centre point
            printf("Contour centre: x = %dpx, y = %dpx\n", regionCentre.x,regionCentre.y);

            char Data[4];

            Data[0] = (regionCentre.x >> 8) & 0xFF;

            Data[1]= regionCentre.x & 0xFF;

            Data[2]=flagstop;
            Data[3]=flagrev;



            car.i2cWrite(Data,4);
           // cout<<"  area  "<<area<<endl;


            //if(wiringPiI2CWrite(fd, byte1) < 0 || wiringPiI2CWrite(fd, byte0) < 0){

            //printf("error sending \n");
            //return -1;
            //}

        }
      //  areaold=areanew;
       // areanew=area;


        int minareaforcolour = 10; //THIS VALUE NEEDS TO BE TUNED IN ORDER TO MAKE IT SO THAT THE CAR GOES BACK TO THE BLACK LINE WHEN ONLY A SMALL AMOUNT OF THE COLOURED LINE CAN BE SEEN.
        if (maximumArea<=minareaforcolour){

            colourUsed = 2; //SETTING IT BACK TO BLACK
          //  cout<<"colour is black";


        }
        imshow("symbol",dilated);

        imshow("Frame with Contours", cframe);

        key = (key==255) ? -1 : key;    // Check if the ESC key has been pressed
        if (key == 27)
            break;
    }

    closeCV();  // Disable the camera and close any windows


    return 0;
}
