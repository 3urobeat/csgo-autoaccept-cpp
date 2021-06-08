# csgo-autoaccept-cpp
This C++ script automatically accepts CS:GO matches for you by analyzing your screen every 4 seconds and moving your mouse when it detects the 'Accept' button.  
Since this script does not interfere with any game files it should be completely VAC safe.  

This script is for Linux only. If you are on Windows see [my other repository for a Windows version.](https://github.com/HerrEurobeat/csgo-autoaccept)  

![Screenshot](https://raw.githubusercontent.com/HerrEurobeat/csgo-autoaccept-cpp/master/.github/img/showcase.png)   


## Download
**Easy way:**  
Head over to the [release section](https://github.com/HerrEurobeat/csgo-autoaccept/releases/latest) and download the latest executable.  


**Harder way if you would like to compile it yourself:**  
Clone this repository using git or download it as a .zip.  
Make sure you have `g++` and `OpenCV` installed. Install g++ from your package manager and follow [this tutorial](https://docs.opencv.org/master/d7/d9f/tutorial_linux_install.html) to install & setup OpenCV.  

Then open a terminal in the folder containing the files that you cloned from this repository.  
To compile run this command: `g++ Main.cpp -o csgo-autoaccept-cpp -I /usr/local/include/opencv4 -lopencv_core -lopencv_imgproc -lX11 -lXtst -pthread`  
> Note: Make sure the path behind `-I` points correctly to your `OpenCV` installation. This should be the default path but check if it is the same case on your machine, otherwise the program won't compile.  
  
## Usage  
Open a terminal in the folder where you placed the `csgo-autoaccept-cpp` file.  
Then just type `./csgo-autoaccept-cpp` to start. You can stop the program by pressing `CTRL+C` or by closing the terminal.  

**Note: It is important that you run the executable from a terminal. By executing it with a double click it will run forever in the background and you'll only see it in your Task Manager/System Monitor or however it is called in your distribution.**

The script will start scanning your screen every 4 seconds.  
Open CS:GO and queue for a match. The script will automatically accept it for you.  
  
When you are in the loading screen you can close the terminal window.  
If not everyone accepted just leave the script open and it will continue scanning.  

> The game must be focused and visible. If you minimze the game the script won't work. If it doesn't recognize the button try changing your game brightness in the Video settings of CS:GO.  