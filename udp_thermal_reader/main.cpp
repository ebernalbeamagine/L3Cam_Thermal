#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdint.h>
#include <unistd.h>

#include <pthread.h>

#include <iostream>

pthread_t stream_thread;

bool g_listening = false;

void saveBinaryThermalData(float *data, int size, uint32_t timestamp)
{
    //!TODO: modify this path to match a folder in your sistem where the binary files with thermal data will be saved,
    //! KEEP THE TIMESTAMP PART to save multiple frames with the timestamp as the name
    std::string name = "/home/caf/Escritorio/thermal_binary/" + std::to_string(timestamp) + ".bin";

    FILE *file_handler = std::fopen(name.c_str(), "wb");

    //! write all data
    std::fwrite(data, size, 1, file_handler);

    std::fclose(file_handler);
}

void *ImageThread(void *functionData)
{

    struct sockaddr_in m_socket;
    int m_socket_descriptor;           // Socket descriptor
    std::string m_address = "0.0.0.0"; // Local address of the network interface port connected to the L3CAM
    int m_udp_port = 6031;             // For Thermal raw data it's 6031

    socklen_t socket_len = sizeof(m_socket);
    char *buffer;
    buffer = (char *)malloc(64000);

    uint16_t m_image_height;
    uint16_t m_image_width;
    uint8_t m_image_channels;
    uint32_t m_timestamp;
    int m_image_data_size;
    bool m_is_reading_image = false;
    char *m_image_buffer = NULL;
    int bytes_count = 0;

    if ((m_socket_descriptor = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        std::cout << "Error opening socket\n";
        pthread_exit(0);
    }

    memset((char *)&m_socket, 0, sizeof(struct sockaddr_in));
    m_socket.sin_addr.s_addr = inet_addr((char *)m_address.c_str());
    m_socket.sin_family = AF_INET;
    m_socket.sin_port = htons(m_udp_port);

    if (inet_aton((char *)m_address.c_str(), &m_socket.sin_addr) == 0)
    {
        std::cout << "inet_aton() failed\n";
        pthread_exit(0);
    }

    if (bind(m_socket_descriptor, (struct sockaddr *)&m_socket, sizeof(struct sockaddr_in)) == -1)
    {
        std::cout << "Could not bind port to socket\n";
        close(m_socket_descriptor);
        pthread_exit(0);
    }

    int rcvbufsize = 134217728;
    if (0 != setsockopt(m_socket_descriptor, SOL_SOCKET, SO_RCVBUF, (char *)&rcvbufsize, sizeof(rcvbufsize)))
    {
        std::cout << "Error setting size to socket\n";
        pthread_exit(0);
    }

    // 1 second timeout for socket
    struct timeval read_timeout;
    read_timeout.tv_sec = 1;
    setsockopt(m_socket_descriptor, SOL_SOCKET, SO_RCVTIMEO, &read_timeout, sizeof read_timeout);


    float *thermal_data_pointer = NULL;
    int float_pointer_cnt = 0;

    while (1)
    {
        int size_read = recvfrom(m_socket_descriptor, buffer, 64004, 0, (struct sockaddr *)&m_socket, &socket_len);
        
        if (size_read == 9) // Header
        {
            memcpy(&m_image_height, &buffer[1], 2);
            memcpy(&m_image_width, &buffer[3], 2);
            memcpy(&m_timestamp, &buffer[5], 4);

            if (thermal_data_pointer != NULL)
            {
                free(thermal_data_pointer);
                thermal_data_pointer = NULL;
            }

            m_image_data_size = m_image_height * m_image_width * sizeof(float);


            thermal_data_pointer = (float *)malloc(m_image_data_size);

            m_is_reading_image = true;
            bytes_count = 0;
            float_pointer_cnt = 0;
        }
        else if (size_read == 1) // End, send image
        {
            m_is_reading_image = false;
            bytes_count = 0;
            float_pointer_cnt = 0;

            saveBinaryThermalData(thermal_data_pointer, m_image_data_size, m_timestamp);
        }
        else if (size_read > 0 && m_is_reading_image)
        {
            memcpy(&thermal_data_pointer[float_pointer_cnt], buffer, size_read);
            bytes_count += size_read;
            float_pointer_cnt += (size_read / 4);
        }
    }

    free(buffer);
    if (thermal_data_pointer != NULL)
    {
        free(thermal_data_pointer);
    }

    shutdown(m_socket_descriptor, SHUT_RDWR);
    close(m_socket_descriptor);

    pthread_exit(0);
}

int main(int argc, char **argv)
{

    pthread_create(&stream_thread, NULL, &ImageThread, NULL);

    while (1)
    {
        usleep(2000000);
    }

    return 0;
}