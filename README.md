## L3CAM THERMOGRAPHY DATA

This repo contains sample code to read thermography data from L3Cam devices, saves the data in a binary file, reads the data and saves it in an image using openCV.

**NOTICE**

Please modify all the paths where **TODO:** is indicated.

### udp_thermal_reader

This folder contains the sample code to read the thermography data fomr the L3Cam device and saves it in a binary file.
Each frame is saved with the name **timestamp.bin** allowing the code to save more than one frame per execution.

The frame is saved without any headers information as an example.

This code creates a thread and opens an UDP socket to port **6031**, then in an infinte loop reads the thermal data.
The thermal data is send as chunks of ~8000 Bytes, this thread reads all the data and copies it in a float pointer, finally this pointer is written in a binary file.

### thermal_bin

This folder contains the sample code to read the binary file saved with the code executed in **udp_thermal_reader**, it shows how to read the binary data as 
float values and using openCV the thermografy data is normalized from 0 to 255 and saved to disk with the name **thermal_colored.png**



