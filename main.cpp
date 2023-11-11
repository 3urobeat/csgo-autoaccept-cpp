/*
 * File: Main.cpp
 * Project: csgo-autoaccept-cpp
 * Created Date: 04.06.2021 17:00:05
 * Author: 3urobeat
 *
 * Last Modified: 11.11.2023 19:08:00
 * Modified By: 3urobeat
 *
 * Copyright (c) 2021 3urobeat <https://github.com/3urobeat>
 *
 * This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>.
 */


#include <iostream>
#include <chrono>
#include <thread>
#include <opencv2/opencv.hpp> // Call before xlib. Issue: https://github.com/opencv/opencv/issues/7113 (we are in 2021 and this issue from 2016 still seems to exist? or am I at fault?)
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/XTest.h> // Used to simulate mouse click

using namespace cv;
using namespace std;


// Some global variables
int      screen, i, x, y, width, height;
Display* display;
Window   root;


// Function that will get executed every checkInterval ms to check the screen for the 'Accept' button
void intervalEvent()
{
    cout << "\r[" << i << "] Searching..." << flush; // Print and let it replace itself

    //auto startTime = chrono::steady_clock::now(); // Only needed for testing to measure time this interval takes
    bool breakLoop = false;
    int  matches   = 0;

    XImage *img  = XGetImage(display, root, x, y, width, height, AllPlanes, ZPixmap); // Make a screenshot
    Mat    cvImg = Mat(height, width, CV_8UC4, img->data);                            // Convert XImage into OpenCV Mat

    cvtColor(cvImg, cvImg, COLOR_BGR2HSV);                             // Convert color scheme to HSV which made the recognition in CS better (inRange() just simply didn't recognize any color in the cs window but worked fine everywhere else)
    inRange(cvImg, Scalar(60, 140, 155), Scalar(70, 150, 175), cvImg); // Apply color mask to filter 'Accept' button (lower & upper threshold)


    // Iterate through every pixel in the Mat
    for (int row = 0; row < cvImg.rows && !breakLoop; row++) // Iterate over all rows in Mat
    {
        for (int col = 0; col < cvImg.cols && !breakLoop; col++) // Iterate over all coloumns in this row of the Mat
        {
            if (cvImg.at<uchar>(row, col) == 255) // Since inRange() makes matching pixels white we search for Mat entries with the value 255
            {
                matches++; // Increment number if this is a match

                if (matches >= 9000) // If we got 9000 matching pixels then it surely is the Accept button
                {
                    cout << "\r--------------------------------------------" << endl;
                    cout << "[" << i << "] Found button! Accepting match..." << endl;
                    cout << "\nIf everyone accepted and you are in the loading screen then please close this window.\nI will otherwise continue searching.\n" << endl;

                    // Set cursor position and click
                    XWarpPointer(display, None, root, 0, 0, 0, 0, col, row); // Update cursor position
                    XTestFakeButtonEvent(display, 1, True, CurrentTime);     // Press and release left click
                    XTestFakeButtonEvent(display, 1, False, CurrentTime);
                    XFlush(display); // Thanks for mentioning that I need to flush the output buffer: https://stackoverflow.com/a/2433488/12934162

                    // Stop loop prematurely
                    breakLoop = true;
                    break;
                }
            }
        }
    }


    // Clean up when we are done with this screenshot
    //auto endTime = chrono::steady_clock::now(); // A few messages only needed for testing
    //cout << "\nMatches: " << matches << endl;
    //cout << "This iteration took " << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() << "ms.\n" << endl;

    XDestroyImage(img); // Release allocated memory on the last iteration (otherwise we create a memory leak)
}


int main() // Entry point
{
    const string version    = "1.1";
    const int checkInterval = 4000;

    cout << "\ncsgo-autoaccept-cpp script version " + version + " by 3urobeat" << endl;
    cout << "---------------------------------------------------" << endl;
    cout << "\nChecking for 'Accept' window every " + to_string(checkInterval / 1000) + " seconds..." << endl;


    // Establish connection to the X11 server https://stackoverflow.com/questions/24988164/c-fast-screenshots-in-linux-for-use-with-opencv & https://stackoverflow.com/questions/4049877/how-to-save-ximage-as-bitmap
    display = XOpenDisplay(0);
    screen  = XDefaultScreen(display);
    root    = RootWindow(display, screen);

    x, y = 0;
    width  = XDisplayWidth(display, screen); // This seems to return both monitors combined. If this impacts the scanning speed severely this needs to be fixed (seems to be fine)
    height = XDisplayHeight(display, screen);


    // Run intervalEvent() every checkInterval ms
    while (true) {
        intervalEvent();

        i++; // Increase counter

        auto x = chrono::steady_clock::now() + chrono::milliseconds(checkInterval);
        this_thread::sleep_until(x);
    }
}
