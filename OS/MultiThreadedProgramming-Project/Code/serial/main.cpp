#include <iostream>
#include <unistd.h>
#include <fstream>
#include <chrono>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;

#pragma pack(1)
//#pragma once

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

typedef struct tagBITMAPFILEHEADER
{
  WORD bfType;
  DWORD bfSize;
  WORD bfReserved1;
  WORD bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
  DWORD biSize;
  LONG biWidth;
  LONG biHeight;
  WORD biPlanes;
  WORD biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG biXPelsPerMeter;
  LONG biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

#define OUTPUT_File "output.bmp"

int rows;
int cols;

uint8_t** red_pixels;
uint8_t** green_pixels;
uint8_t** blue_pixels;

bool fillAndAllocate(char *&buffer, const char *fileName, int &rows, int &cols, int &bufferSize)
{
  std::ifstream file(fileName);

  if (file)
  {
    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    return 1;
  }
  else
  {
    cout << "File" << fileName << " doesn't exist!" << endl;
    return 0;
  }
}

void getPixlesFromBMP24(int end, int rows, int cols, char *fileReadBuffer)
{
  int count = 1;
  int extra = cols % 4;
  unsigned char red_pixel;
  unsigned char green_pixel;
  unsigned char blue_pixel;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          red_pixel = fileReadBuffer[end - count];
          red_pixels[i][j] = (uint8_t) red_pixel;
          break;
        case 1:
          green_pixel = fileReadBuffer[end - count];
          green_pixels[i][j] = (uint8_t) green_pixel;
          break;
        case 2:
          blue_pixel = fileReadBuffer[end - count];
          blue_pixels[i][j] = (uint8_t) blue_pixel;
          break;
        }
        count++;
      }
  }
}

void writeOutBmp24(char *fileBuffer, const char *nameOfFileToCreate, int bufferSize)
{
  std::ofstream write(nameOfFileToCreate);
  if (!write)
  {
    cout << "Failed to write " << nameOfFileToCreate << endl;
    return;
  }
  int count = 1;
  int extra = cols % 4;
  for (int i = 0; i < rows; i++)
  {
    count += extra;
    for (int j = cols - 1; j >= 0; j--)
      for (int k = 0; k < 3; k++)
      {
        switch (k)
        {
        case 0:
          fileBuffer[bufferSize - count] = red_pixels[i][j];
          break;
        case 1:
          fileBuffer[bufferSize - count] = green_pixels[i][j];
          break;
        case 2:
          fileBuffer[bufferSize - count] = blue_pixels[i][j];
          break;
        }
        count ++;
      }
  }
  write.write(fileBuffer, bufferSize);
}

void* filterSmoothing(void* argument) {
    int* id = ((int*) argument);
    uint8_t** color = (*id == 1) ? red_pixels : (*id == 2) ? green_pixels : blue_pixels; 
    uint8_t new_color[rows][cols];
    for (int i = 1; i < rows - 1; i++){
        for (int j = 1; j < cols - 1; j++){
        new_color[i][j] = (color[i - 1][j - 1] + color[i - 1][j]
                            + color[i - 1][j + 1] + color[i][j - 1]
                            + color[i][j + 1] + color[i][j] + color[i + 1][j - 1]
                            + color[i + 1][j] + color[i + 1][j + 1]) / 9;    
        }
    }

    for (int i = 1; i < rows - 1; i++){
        for (int j = 1; j < cols - 1; j++){
        color[i][j] = new_color[i][j];
        }
    }
    return (void*) 0;
}

void filterSepia(uint8_t**& input_red, uint8_t**& input_green, uint8_t**& input_blue) {

  int8_t output_red[rows][cols];
  int8_t output_green[rows][cols];
  int8_t output_blue[rows][cols];
  for (int i = 0; i < rows; i++){
    for (int j = 0; j < cols; j++){
      output_red[i][j] = std::min<int>(255, input_red[i][j] * 0.393 + input_green[i][j] * 0.769 + input_blue[i][j] * 0.189);
      output_green[i][j] = std::min<int>(255, input_red[i][j] * 0.349 + input_green[i][j] * 0.686 + input_blue[i][j] * 0.168);
      output_blue[i][j] = std::min<int>(255, input_red[i][j] * 0.272 + input_green[i][j] * 0.534 + input_blue[i][j] * 0.131);

    }
  }

  for (int i = 0; i < rows; i++){
    for (int j = 0; j < cols; j++){
      input_red[i][j] = output_red[i][j];
      input_green[i][j] = output_green[i][j];
      input_blue[i][j] = output_blue[i][j];

    }
  }
}

void* filterWashOut(void* argument) {
    int* id = ((int*) argument);
    uint8_t** color = (*id == 1) ? red_pixels : (*id == 2) ? green_pixels : blue_pixels; 
    int8_t avg = 0;
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
        avg += color[i][j];
        }
    }
    avg /= rows * cols;

    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
        color[i][j] = color[i][j] * 0.4 + avg * 0.6;
        }
    }
    return (void*) 0;
}

void filterDrawX(uint8_t**& color) {

  for (int i = 0; i < rows; i++){
    color[i][i] = 255;
    color[std::max<int> (0, i - 1)][i] = 255;
    color[i][std::max<int> (0, i - 1)] = 255;

    color[i][cols - 1 - i] = 255;
    color[std::max<int> (0, i - 1)][cols - 1 - i] = 255;
    color[i][std::min<int> (cols - 1, cols - i)] = 255;
  }
  
}

void applyFilters() {
  int RED_ID = 1;
  int GREEN_ID = 2;
  int BLUE_ID = 3;

  //smoothing filter
  auto start = std::chrono::high_resolution_clock::now();
  filterSmoothing((void*) &RED_ID);
  filterSmoothing((void*) &GREEN_ID);
  filterSmoothing((void*) &BLUE_ID);
  auto end = std::chrono::high_resolution_clock::now();
  cout << "Smoothing time: " << 
    std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    << " milliseconds" << endl;

  //sepia filter
  start = std::chrono::high_resolution_clock::now();
  filterSepia(red_pixels, green_pixels, blue_pixels);
  end = std::chrono::high_resolution_clock::now();
  cout << "Sepia time: " << 
    std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    << " milliseconds" << endl;

  //wash out filter
  start = std::chrono::high_resolution_clock::now();
  filterWashOut((void*) &RED_ID);
  filterWashOut((void*) &GREEN_ID);
  filterWashOut((void*) &BLUE_ID);
  end = std::chrono::high_resolution_clock::now();
  cout << "WashOut time: " << 
  std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
  << " milliseconds" << endl;

  //draw an x
  start = std::chrono::high_resolution_clock::now();
  filterDrawX(red_pixels);
  filterDrawX(green_pixels);
  filterDrawX(blue_pixels);
  end = std::chrono::high_resolution_clock::now();
  cout << "drawX time: " << 
  std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
  << " milliseconds" << endl;
}

void allocate_rgb_space(uint8_t**& color) {
  color = new uint8_t*[rows];
  for (int i = 0; i < rows; i++)
    color[i] = new uint8_t[cols];
}

void allocate_space() {
  allocate_rgb_space(red_pixels);
  allocate_rgb_space(green_pixels);
  allocate_rgb_space(blue_pixels);
}


int main(int argc, char *argv[])
{
  auto start = std::chrono::high_resolution_clock::now();

  char *fileBuffer;
  int bufferSize;
  char *fileName = argv[1];
  if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize))
  {
    cout << "File read error" << endl;
    return 1;
  }
  else{
    allocate_space();
  }
  getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer);

  applyFilters();

  writeOutBmp24(fileBuffer, OUTPUT_File, bufferSize);
 
  auto end = std::chrono::high_resolution_clock::now();

  cout << "Execution time: " << 
    std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    << " milliseconds" << endl;
  return 0;
}