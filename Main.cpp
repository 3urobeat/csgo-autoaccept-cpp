#include <iostream>
#include <chrono>
#include <thread>
#include <opencv2/opencv.hpp> //call before xlib. Issue: https://github.com/opencv/opencv/issues/7113 (we are in 2021 and this issue from 2016 still seems to exist? or am I at fault?)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h> //used to simulate mouse click

using namespace cv;
using namespace std;


//Some global variables
int      screen, i, x, y, width, height;
Display* display;
Window   root;


//Function that will get executed every checkInterval ms to check the screen for the 'Accept' button
void intervalEvent()
{
    cout << "\r[" << i << "] Searching..." << flush; //Print and let it replace itself

    //auto startTime = chrono::steady_clock::now(); //only needed for testing to measure time this interval takes
    bool breakLoop = false;
    int  matches   = 0;

    XImage *img  = XGetImage(display, root, x, y, width, height, AllPlanes, ZPixmap); //make a screenshot
    Mat    cvImg = Mat(height, width, CV_8UC4, img->data); //convert XImage into OpenCV Mat

    cvtColor(cvImg, cvImg, COLOR_BGR2HSV); //convert color scheme to HSV which made the recognition in CS better (inRange() just simply didn't recognize any color in the cs window but worked fine everywhere else)
    inRange(cvImg, Scalar(60, 140, 155), Scalar(70, 150, 175), cvImg); //apply color mask to filter 'Accept' button (lower & upper threshold)

    
    //function to print result messages and to clean up the memory (Nested functions: https://stackoverflow.com/a/4324780/12934162)
    auto cleanUp = [&] ()
    {
        //auto endTime = chrono::steady_clock::now(); //a few messages only needed for testing
        //cout << "\nMatches: " << matches << endl;
        //cout << "This iteration took " << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() << "ms.\n" << endl;

        XDestroyImage(img); //release allocated memory on the last iteration (otherwise we create a memory leak)
    };


    //Iterate through every pixel in the Mat
    for (int row = 0; row < cvImg.rows && !breakLoop; row++) //iterate over all rows in Mat
    {
        for (int col = 0; col < cvImg.cols && !breakLoop; col++) //iterate over all coloumns in this row of the Mat
        {
            if (cvImg.at<uchar>(row, col) == 255) //since inRange() makes matching pixels white we search for Mat entries with the value 255
            {
                matches++; //increment number if this is a match

                if (matches >= 9000) //if we got 9000 matching pixels then it surely is the Accept button
                {
                    cout << "\r--------------------------------------------" << endl;
                    cout << "[" << i << "] Found button! Accepting match..." << endl;
                    cout << "\nIf everyone accepted and you are in the loading screen then please close this window.\nI will otherwise continue searching.\n" << endl;
                    
                    XWarpPointer(display, None, root, 0, 0, 0, 0, col, row); //update cursor position
                    XTestFakeButtonEvent(display, 1, True, CurrentTime); //press and release left click
                    XTestFakeButtonEvent(display, 1, False, CurrentTime);
                    XFlush(display); //thanks for mentioning that I need to flush the output buffer: https://stackoverflow.com/a/2433488/12934162

                    cleanUp();
                    breakLoop = true; //prevent next iteration from running
                    break;
                }
            }

            if ((row >= cvImg.rows - 1 && col >= cvImg.cols - 1) || breakLoop) cleanUp(); //Check for the last iteration
        }
    }
}


int main() //Entry point
{
    const string version    = "1.1";
    const int checkInterval = 4000;

    cout << "\ncsgo-autoaccept-cpp script version " + version + " by 3urobeat" << endl;
    cout << "---------------------------------------------------" << endl;
    cout << "\nChecking for 'Accept' window every " + to_string(checkInterval / 1000) + " seconds..." << endl;


    display = XOpenDisplay(0); //Establish connection to the X11 server https://stackoverflow.com/questions/24988164/c-fast-screenshots-in-linux-for-use-with-opencv & https://stackoverflow.com/questions/4049877/how-to-save-ximage-as-bitmap
    screen  = XDefaultScreen(display);
    root    = RootWindow(display, screen);

    x, y = 0;
    width  = XDisplayWidth(display, screen); //this seems to return both monitors combined. If this impacts the scanning speed severely this needs to be fixed (seems to be fine)
    height = XDisplayHeight(display, screen);

    //run intervalEvent() every checkInterval ms
    while (true) {
        intervalEvent();

        i++; //increase counter
        
        auto x = chrono::steady_clock::now() + chrono::milliseconds(checkInterval);
        this_thread::sleep_until(x);
    }
}