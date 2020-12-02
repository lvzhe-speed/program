/*=============================================================================
#       COPYRIGHT NOTICE
#       Copyright (c) 2019
#       All rights reserved
#
#       @author       :lvzhe
#       @mail         :1750895316@qq.com
#       @file         :opencv_faceCheck.cpp
#       @date         :2020/03/06 15:52
#       @algorithm    :
=============================================================================*/

#include "opencv2/objdetect.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <stdio.h>
#include <iostream>
#include <wiringPi.h>
using namespace std;
using namespace cv;

#define PwmMaxValue (250)
#define PwmMinValue (50)


/** Function Headers */
void detectAndDisplay();
void steeringPwmInit(void);
void steeringControlDir(Point face_center,Point frame_center,int face_size);


/** Global variables */
Mat frame;
int pitchPwmPin=18;
int yawPwmPin=13;
String face_cascade_name;
CascadeClassifier face_cascade;
String window_name = "Capture - Face detection";

/** @function main */
int main(int argc, const char **argv)
{
    face_cascade_name = "xml/haarcascade_frontalface_alt.xml";
    VideoCapture capture;
    //-- 1. Load the cascades
    if (!face_cascade.load(face_cascade_name))
    {
        printf("--(!)Error loading face cascade\n");
        return -1;
    }
	steeringPwmInit();
    //-- 2. Read the video stream
    capture.open(0);
	capture.set(CV_CAP_PROP_FRAME_WIDTH,640);//最大
 	capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    if (!capture.isOpened())
    {
        printf("--(!)Error opening video capture\n");
        return -1;
    }

    while (capture.read(frame))
    {
        if (frame.empty())
        {
            printf(" --(!) No captured frame -- Break!");
            break;
        }
		Size dsize = Size(frame.cols*0.5, frame.rows*0.5);
        resize(frame, frame, dsize);
        //-- 3. Apply the classifier to the frame
        detectAndDisplay();
        if (waitKey(10) == 27)
        {
            break;
        } // escape
    }
    return 0;
}

/** @function detectAndDisplay */
void detectAndDisplay()
{
    std::vector<Rect> faces;
    Mat frame_gray;

    cvtColor(frame, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);
     Point frame_center(frame.size().width/2,frame.size().height/2); 
    //-- Detect faces
    face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(60, 60));
    for (size_t i = 0; i < faces.size(); i++)
    {
        Point face_center(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);
        rectangle(frame,faces[static_cast<int>(i)],Scalar(255,0,0),2,8,0);
		steeringControlDir(face_center,frame_center,faces[i].area());
		// std::cout << "face size:" << faces[i].size()<<endl; 
    }
    //-- Show what you got
    imshow(window_name, frame);
}


/** @function steeringPwmInit */

void steeringPwmInit(void)
{
	wiringPiSetupGpio();
	pinMode(pitchPwmPin,PWM_OUTPUT);
	pinMode(yawPwmPin,PWM_OUTPUT);
	pwmSetMode(PWM_MODE_MS);

	pwmSetClock(192);
	pwmSetRange(2000);
}


int pitch_value=150;
int yaw_value=150;
float kx=0.2;
float ky=0.3;


/** @function steeringControlDir */
void steeringControlDir(Point face_center,Point frame_center,int face_size)
{
	int frame_size=frame.cols*frame.rows;
	int k =frame_size/face_size;
	float dx,dy;
	// std::cout << "k:" << k<<endl;
		if(k>=7){//如果整个图像相对人脸的大小来说很大，就要限制舵机的摆幅
			dx=(frame_center.x-face_center.x)*kx/k;
			dy=(frame_center.y-face_center.y)*ky/k;
		
			yaw_value+=dx;
			if(yaw_value >= PwmMaxValue)
			yaw_value=PwmMaxValue;
			else if(yaw_value <= PwmMinValue)
			yaw_value=PwmMinValue;	
			pwmWrite(yawPwmPin,(int)yaw_value);
		
			pitch_value+=dy;
			if(pitch_value >= PwmMaxValue)
			pitch_value=PwmMaxValue;
			else if(pitch_value <= PwmMinValue)
			pitch_value=PwmMinValue;	
			pwmWrite(pitchPwmPin,(int)pitch_value);
		}else {//如果整个图像相对人脸的大小来说很小，就要限制舵机的摆动的频率
			dx=(frame_center.x-face_center.x)*0.05;
			dy=(frame_center.y-face_center.y)*0.05;
			if(abs((int)dx)>=3){
				yaw_value+=dx;
			if(yaw_value >= PwmMaxValue)
			yaw_value=PwmMaxValue;
			else if(yaw_value <= PwmMinValue)
			yaw_value=PwmMinValue;	
			pwmWrite(yawPwmPin,(int)yaw_value);
			}
			
			if(abs((int)dy)>=2)
			{
				pitch_value+=dy;
		if(pitch_value >= PwmMaxValue)
			pitch_value=PwmMaxValue;
		else if(pitch_value <= PwmMinValue)
			pitch_value=PwmMinValue;	
			pwmWrite(pitchPwmPin,(int)pitch_value);
			}
		
		}
		


}
