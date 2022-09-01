
#  ASCII rendering in terminal

This project consists of rendering (in CMD) a video using a brightness scale with ASCII characters


## Deployment

You have to change the AUDIOTIME and TOTALFRAMES parameters at the top of the code.
You must put a video in the ROOT folder, this video is the one which you'll select in the terminal later on.

```bash
  $ g++ -g main.cpp -o main.exe
  $ ./main.exe
```
The first command compiles the code so you can execute the executable file
The second command executes the program.
In this code, the terminal width and height is selected when the program starts, so if you want to change the size of the video, Please restart the program
## Features

- Preatty fast rendering video (But not optimized yet!)
- ~~Optimized for fast rendering~~ (Comming soon)
- Render any video you want
