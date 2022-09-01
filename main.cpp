#include <iostream>
#include <filesystem>
#include <windows.h>
#include <cstdio>
#include "./bitmap_image.hpp"
#include <ctime>
#include <sys/types.h>
#include <algorithm>
#include <chrono>
#include <ratio>

#define AUDIOTIME 203 // Segundos
#define TOTALFRAMES 6116

using namespace std;

const string brightnessLevels = " .-+*wGHM#&%";
bool directoryFlag = false;
bool FPSCreated = false;

string loadFrame(const char *filePath)
{
    bitmap_image image(filePath);

    if (!image)
    {
        printf("Error - Failed to open: input.bmp\n");
        return "1";
    }

    string result = "";
    uint8_t pixelBright = 0;
    const unsigned int height = image.height();
    const unsigned int width = image.width();

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            rgb_t colour;

            image.get_pixel(x, y, colour);

            // Promedio de colores = brillo
            pixelBright = (colour.red + colour.green + colour.blue) / 3;
            result += (pixelBright <= 23) ? brightnessLevels[0] : (pixelBright <= 46 && pixelBright > 23) ? brightnessLevels[1]
                                                              : (pixelBright <= 69 && pixelBright > 46)   ? brightnessLevels[2]
                                                              : (pixelBright <= 92 && pixelBright > 69)   ? brightnessLevels[3]
                                                              : (pixelBright <= 116 && pixelBright > 92)  ? brightnessLevels[4]
                                                              : (pixelBright <= 139 && pixelBright > 116) ? brightnessLevels[5]
                                                              : (pixelBright <= 162 && pixelBright > 139) ? brightnessLevels[6]
                                                              : (pixelBright <= 185 && pixelBright > 162) ? brightnessLevels[7]
                                                              : (pixelBright <= 208 && pixelBright > 185) ? brightnessLevels[8]
                                                              : (pixelBright <= 232 && pixelBright > 208) ? brightnessLevels[9]
                                                              : (pixelBright > 232)                       ? brightnessLevels[10]
                                                                                                          : brightnessLevels[11];
        }
        result += "\n";
    }

    return result;
}

void setCursorPosition(int x, int y)
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, coord);
}


void run(string result[])
{
    int audio = system("start /MIN ffplay.exe -loglevel 0 tmp\\audio.wav -autoexit -nodisp");
    int actualFrameFlag = 0;
    Sleep(3500);
    chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
    cout.flush();
    while (!audio)
    {
        chrono::high_resolution_clock::time_point t2 = chrono::high_resolution_clock::now();
        chrono::duration<double> time_span = chrono::duration_cast<chrono::duration<double>>(t2 - t1);
        int frame = floor((time_span.count() / AUDIOTIME) * TOTALFRAMES);
        setCursorPosition(0,0);
        cout << result[frame];
        // Sleep(42);
        // system("cls")
    }
}

int main()
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    string width{0}, height{0};
    string videoName = "sample";

    cout << "Cual es el nombre del video? ";
    cin >> videoName;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width = to_string(csbi.srWindow.Right - csbi.srWindow.Left);
    height = to_string((csbi.srWindow.Bottom - csbi.srWindow.Top) - 1);
    const string commandVideo = "ffmpeg.exe -loglevel panic -i ./" + videoName + ".mp4 -vf scale=" + width + ":" + height + " tmp\\frames\\%0d.bmp ";
    const string commandAudio = "ffmpeg.exe -loglevel panic -i ./" + videoName + ".mp4  tmp\\audio.wav";

    cout << "Cleaning up..." << endl;

    if (filesystem::exists("./tmp"))
    {
        if (filesystem::exists("./tmp/frames"))
        {

            filesystem::remove_all("./tmp/frames");
        }
        filesystem::create_directory("./tmp/frames/");

        if (filesystem::exists("./tmp/audio.wav"))
        {
            filesystem::remove("./tmp/audio.wav");
        }
    }
    else
    {
        filesystem::create_directory("./tmp");
        filesystem::create_directory("./tmp/frames");
    }

    cout << "Extracting frames..." << endl;

    system(commandVideo.c_str());

    cout << "Extracting sound..." << endl;
    system(commandAudio.c_str());

    if (!FPSCreated)
    {
        string result[TOTALFRAMES];
        int i = 0;

        vector<int> filesSorted;

        for (auto &entry : filesystem::directory_iterator("./tmp/frames"))
        {

            // entry.path().filename() => Retorna el NOMBRE del archivo ("dsa.txt")
            filesSorted.push_back(stoi(entry.path().filename()));
        }
        sort(filesSorted.begin(), filesSorted.end());

        for (auto &frame : filesSorted)
        {
            string directory = "./tmp/frames/" + to_string(frame) + ".bmp";
            result[i] = loadFrame(directory.c_str());
            i++;
        }

        system("cls");
        run(result);
    }
}