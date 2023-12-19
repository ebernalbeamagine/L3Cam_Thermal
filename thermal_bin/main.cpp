#include <iostream>
#include <fstream>

#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>

int main()
{
    // Read the file
    int size = 240 * 320;

    //!TODO: Modify this path to read a binary file saved with thermal information
    std::string file_name = "/home/caf/Escritorio/thermal_binary/130424317.bin";

    FILE *thermal_file = std::fopen(file_name.c_str(), "rb");

    float thermal_data[size];

    std::fread(&thermal_data[0], size * sizeof(float), 1, thermal_file);

    std::fclose(thermal_file);

    cv::Mat float_image = cv::Mat(240, 320, CV_32FC1, &thermal_data[0]);

    double minVal, maxVal;
    minMaxLoc(float_image, &minVal, &maxVal);

    std::cout << "min value" << minVal << " max value " << maxVal << "\n";

    uint8_t thermal_color[size];

    for (int i = 0; i < size; ++i)
    {
        if(i < 20){
            std::cout<<thermal_data[i]<<"\n";
        }
        float val = thermal_data[i];
        float norm = (val - minVal) / (maxVal - minVal);
        norm *= 255.0;
        thermal_color[i] = (uint8_t)norm;
    }

    cv::Mat image = cv::Mat(240, 320, CV_8UC1, &thermal_color[0]);
    //!TODO: modify this path to save the image file in your system
    cv::imwrite("/home/caf/Escritorio/thermal_colored.png", image);

}