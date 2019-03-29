// HelloWorld.cpp : Defines the entry pofloat for the console application.
//

#include "stdafx.h"

#include <opencv2/opencv.hpp>

#include <iostream>
#include <string>
#include <tbb/tbb.h>

using namespace cv;
using namespace std;

	//values for coordinates and colors of a pixel changing line; image coordinates are inclusive
struct pixelChangingValues{
	float b = 0, g = 0, r = 0;	//color channel values
	int y = 0, x = 0;	//current coordinates of pixels to change
	int initY = y, initX = x;	//initial coordinates for loops
	//the coordinates of where to start and end the the creation of the image on a window
	int imageMaxX = 0, imageStartX = 0, imageMaxY = 0, imageStartY = 0;
	// values to subtract from the x and y coordinates when referencing the coordinates in the actual hidden image
	int subtractedX = 0, subtractedY = 0;

	//overriding = operator
	pixelChangingValues& operator=(const pixelChangingValues&changer) {
		b = changer.b;
		g = changer.g;
		r = changer.r;
		y = changer.y;
		x = changer.x;
		initY = changer.initY;
		initX = changer.initX;
		return *this;
	}
};

void changeColor(float &, float &, float &);
void changePixelsMulti(bool, pixelChangingValues &);
void changePixels(bool, pixelChangingValues &);
void changePixelsNoise(pixelChangingValues &);
void changePixelsMatrix(pixelChangingValues &);
void changePixelsMurica(pixelChangingValues &);
void muricaHelper(pixelChangingValues &);
void changePixelsDecay(pixelChangingValues &);
void changePixelsHiddenPic(pixelChangingValues &, const Mat &);
void hiddenPicHelper(const pixelChangingValues &, const Mat &);
Mat createHiddenImage(string fileName, pixelChangingValues &);

bool bLowering, gLowering, rLowering;
// Create the image (y, x)
Mat mainWindow(1000, 1700, CV_8UC3, Scalar(0, 0, 0));
//width of color bands
int width = 10;

//Define the name of the window
String windowNameOfOriginalImage = "Image";

int main(int argc, char** argv)
{
	int lines = 4;
	pixelChangingValues changer1;
	pixelChangingValues changer2;
	pixelChangingValues changer3;
	pixelChangingValues changer4;
	pixelChangingValues changer5;
	pixelChangingValues changer6;	//hidden pic one

	Mat hidden = createHiddenImage("Penguins Stretch.jpg", changer6);
	// Check for failure
	if (hidden.empty())
	{
		cout << "Could not open or find the image" << endl;
		cin.get(); //wait for any key press
		return -1;
	}

	// Create the window with the above name
	namedWindow(windowNameOfOriginalImage);

	srand(time(NULL));	//seeding the random number generator

	while (true)
	{
		//show the frame in the created window
		imshow(windowNameOfOriginalImage, mainWindow);

		//change color values of the image matrix
		//std::vector<bool> input = { true, false };
		//tbb::parallel_for(size_t(0), input.size(), size_t(1), [=](size_t i) {
		//	changePixelsMulti(input[i]);
		//});
		//y += width;

		//changePixels(true);
		//changePixels(false);
		changePixelsNoise(changer1);

		//first few if statements for the first round about until all lines are in
		if (lines == 3) {
			changePixelsNoise(changer2);
		}
		else if (lines == 2) {
			changePixelsNoise(changer2);
			changePixelsNoise(changer3);
		}
		else if(lines == 1) {
			changePixelsNoise(changer2);
			changePixelsNoise(changer3);
			changePixelsNoise(changer4);
		}
		else if (lines == 0) {
			changer6 = changer5;
			changePixelsNoise(changer2);
			changePixelsNoise(changer3);
			changePixelsNoise(changer4);
			changePixelsHiddenPic(changer6, hidden);
			changePixelsDecay(changer5);
		}

		//wait for for 1 ms until any key is pressed.  
		//If the 'Esc' key is pressed (27), break the while loop.
		//If the any other key is pressed, continue the loop 
		//If any key is not pressed withing 10 ms, continue the loop
		if (waitKey(1) == 27)
		{
			cout << "Esc key is pressed by user. Stoppig the video" << endl;
			break;
		}

		//change b g r values
		//changeColor(b, g, r);

		//making sure x and y are within bounds of the window
		/*if (y >= 1000) {
			y = 0;
			x += width;
			if (x >= 1650) {
				x = 0;
			}
			initX = x;
		}
		initY = y;*/
		if (lines > 0) {
			if (lines == 4 && changer1.y >= 200) {
				lines--;
			}
			else if (lines == 3 && changer2.y >= 200) {
				lines--;
			}
			else if (lines == 2 && changer3.y >= 200) {
				lines--;
			}
			else if (lines == 1 && changer4.y >= 200) {
				lines--;
			}
		}
		if (changer1.y >= 1000) {
			changer1.y = 0;
			changer1.initY = changer1.y;
		}
		if (changer2.y >= 1000) {
			changer2.y = 0;
			changer2.initY = changer2.y;
		}
		if (changer3.y >= 1000) {
			changer3.y = 0;
			changer3.initY = changer3.y;
		}
		if (changer4.y >= 1000) {
			changer4.y = 0;
			changer4.initY = changer4.y;
		}
		if (changer5.y >= 1000) {
			changer5.y = 0;
			changer5.initY = changer5.y;
		}
	}
	cv::destroyAllWindows(); //Destroy all opened windows

	return 0;
}

//changes the values of the b, g, r color values by different increments
void changeColor(float &b, float &g, float &r) {
	if ((b > 0 && bLowering) || b >= 255) {
		b -= 10;
		bLowering = true;
	}
	else {
		b += 10;
		bLowering = false;
	}

	if ((g > 0 && gLowering) || g >= 255) {
		g -= .2;
		gLowering = true;
	}
	else {
		g += .2;
		gLowering = false;
	}


	if ((r > 0 && rLowering) || r >= 255) {
		r -= 1;
		rLowering = true;
	}
	else {
		r += 1;
		rLowering = false;
	}
}

//change pixels to new colors (Multithreading version)
void changePixelsMulti(bool side, pixelChangingValues &changer1) {
	//after this, x and y are the same because only tempx and tempy are changed
	if (side) {
		int tempy = changer1.y, tempx = changer1.x;
		for (tempy; tempy < changer1.initY + width; tempy++) {
			for (tempx; tempx < changer1.initX + width; tempx++) {
				mainWindow.at<Vec3b>(tempy, tempx) = Vec3b(changer1.b, changer1.g, changer1.r);	//(y, x)
			}
			tempx = changer1.initX;
		}
	}//x and y are not modified by this
	else {
		int endY = 999 - changer1.y, endX = 1699 - changer1.x;
		for (endY; endY > 999 - changer1.initY - width; endY--) {
			for (endX; endX > 1699 - changer1.initX - width; endX--) {
				mainWindow.at<Vec3b>(endY, endX) = Vec3b(changer1.b, changer1.g, changer1.r);	//(y, x)
			}
			endX = 1699 - changer1.initX;
		}
	}
}

//change pixels to new colors
void changePixels(bool side, pixelChangingValues &changer1) {
	//after this, y is width above initY
	if (side) {
		for (changer1.y; changer1.y < changer1.initY + width; changer1.y++) {
			for (changer1.x; changer1.x < changer1.initX + width; changer1.x++) {
				mainWindow.at<Vec3b>(changer1.y, changer1.x) = Vec3b(changer1.b, changer1.g, changer1.r);	//(y, x)
			}
			changer1.x = changer1.initX;
		}
	}//x and y are not modified by this
	else {
		int endY = 999 - changer1.y + width, endX = 1699 - changer1.x;
		for (endY; endY > 999 - changer1.initY - width; endY--) {
			for (endX; endX > 1699 - changer1.initX - width; endX--) {
				mainWindow.at<Vec3b>(endY, endX) = Vec3b(changer1.b, changer1.g, changer1.r);	//(y, x)
			}
			endX = 1699 - changer1.initX;
		}
	}
}

//noise filter
void changePixelsNoise(pixelChangingValues &changer1) {
	for (changer1.x = 0; changer1.x < 1699; changer1.x += 1) {
		for (int tinyX = changer1.x; tinyX < changer1.x + 1; tinyX++) {
			changer1.b = rand() % 256;
			changer1.g = rand() % 256;
			changer1.r = rand() % 256;
				for (changer1.y = changer1.initY; changer1.y < changer1.initY + 1; changer1.y++) {
					if (changer1.y < 999) {
						mainWindow.at<Vec3b>(changer1.y, tinyX) = Vec3b(changer1.b, changer1.g, changer1.r);	//(y, x)
					}
				}
		}
	}
	changer1.initY = changer1.y;
}

//matrix symbols filter
void changePixelsMatrix(pixelChangingValues &changer1) {
	for (changer1.x = 0; changer1.x < 1699; changer1.x += 20) {	//looping through first row
		for (int tinyX = changer1.x; tinyX < changer1.x + 3; tinyX++) {	//looping through each section in the 1st row
			changer1.b = 0;
			changer1.g = rand() % 256;
			changer1.r = 0;
			//if (rand() % 2 == 1) {
			for (changer1.y = changer1.initY; changer1.y < changer1.initY + (rand() % 15); changer1.y++) {	//looping down each section, sorta random height
				if (changer1.y < 999) {
					mainWindow.at<Vec3b>(changer1.y, tinyX) = Vec3b(changer1.b, changer1.g, changer1.r);	//(y, x)
				}
			}
			//}
		}
	}
	changer1.initY = changer1.y;
}

//red white blue filter
void changePixelsMurica(pixelChangingValues &changer1) {
	for (changer1.x = 0; changer1.x < 1699; changer1.x += 20) {	//looping through first row
		for (int tinyX = changer1.x; tinyX < changer1.x + 3; tinyX++) {	//looping through each section in the 1st row
			changer1.b = rand() % 256;
			changer1.g = rand() % 256;
			changer1.r = rand() % 256;
			//if (rand() % 2 == 1) {
			for (changer1.y = changer1.initY; changer1.y < changer1.initY + (rand() % 15); changer1.y++) {	//looping down each section, sorta random height
				if (changer1.y < 999) {
					mainWindow.at<Vec3b>(changer1.y, tinyX) = Vec3b(changer1.b, changer1.g, changer1.r);	//(y, x)
				}
			}
			//}
		}
	}
	changer1.initY = changer1.y;
}

//chooses between red white and blue
void muricaHelper(pixelChangingValues &changer) {
	int num = rand() % 3;
	if (num == 0) {
		changer.b = 0;
		changer.g = 0;
		changer.r = 250;
	}
	else if (num == 1) {
		changer.b = 255;
		changer.g = 255;
		changer.r = 255;
	}
	else if (num == 2) {
		changer.b = 200;
		changer.g = 0;
		changer.r = 0;
	}
}

//replaces pixels with color black
void changePixelsDecay(pixelChangingValues &changer1) {
	changer1.b = 0;
	changer1.g = 0;
	changer1.r = 0;
	for (changer1.x = 0; changer1.x < 1699; changer1.x += 1) { //looping through first row
		for (int tinyX = changer1.x; tinyX < changer1.x + 1; tinyX++) {	//looping through each section in the 1st row
			if (rand() % 6 == 0) {	//controls how much is decayed
				for (changer1.y = changer1.initY; changer1.y < changer1.initY + (rand() % 15); changer1.y++) {//looping down each section
					if (changer1.y < 999) {
						mainWindow.at<Vec3b>(changer1.y, tinyX) = Vec3b(changer1.b, changer1.g, changer1.r);	//(y, x)
					}
				}
			}
		}
	}
	changer1.initY = changer1.y;	//initY value moved down after loop is done
}

//reveals a hidden picture, 1 row at a time
void changePixelsHiddenPic(pixelChangingValues &changer, const Mat &hidden) {
	for (changer.x = 0; changer.x < 1699; changer.x += 1) {	//going through the row that was just decayed
		hiddenPicHelper(changer, hidden);
	}
}

//determines whether the current pixel is part of the hidden image and displays it if true
void hiddenPicHelper(const pixelChangingValues &changer, const Mat &hidden) {
	//checking if the x and y coordinate are where a pixel of the image should be
	if (changer.x > changer.imageStartX && changer.x < changer.imageMaxX && changer.y > changer.imageStartY && changer.y < changer.imageMaxY) {
		//getting the pixel color from the hidden image
		Vec3b hiddenColor = hidden.at<Vec3b>(changer.y - changer.subtractedY, changer.x - changer.subtractedX);
		mainWindow.at<Vec3b>(changer.y, changer.x) = hiddenColor;	//(y, x)
	}
}

/*
	creates the hidden image Mat object from a file and positions it in the center
		@image -- the hidden image
		@fileName -- the file name
		@changer -- the pixelChangingValues struct that will be asscociated with this image (w/ its image Coordinate values)
		@returns -- the hidden image Null or not
*/
Mat createHiddenImage(string fileName, pixelChangingValues &changer)
{
	Mat image = imread(fileName);

	//if it isn't empty, then we can center it
	if (!image.empty()) {
		changer.imageStartX = changer.subtractedX = (mainWindow.cols - image.cols) / 2;
		changer.imageMaxX = changer.imageStartX + image.cols - 1;
		changer.imageStartY = changer.subtractedY = (mainWindow.rows - image.rows) / 2;
		changer.imageMaxY = changer.imageStartY + image.rows - 1;
	}

	return image;
}
