#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <chrono>
#include <algorithm>
#include <vector>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using namespace std::chrono;
using namespace std;

#pragma pack(1)
#pragma once

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

int rows;
int cols;
#define WHITE 255

#define MAX_SIZE 1000
unsigned char red[MAX_SIZE][MAX_SIZE];
unsigned char green[MAX_SIZE][MAX_SIZE];
unsigned char blue[MAX_SIZE][MAX_SIZE];

unsigned char redCopy[MAX_SIZE][MAX_SIZE];
unsigned char greenCopy[MAX_SIZE][MAX_SIZE];
unsigned char blueCopy[MAX_SIZE][MAX_SIZE];

long double avrRed = 0;
long double avrGreen = 0;
long double avrBlue = 0;

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
	for (int i = 0; i < rows; i++)
	{
		count += extra;
		for (int j = cols - 1; j >= 0; j--)
			for (int k = 0; k < 3; k++)
			{
				switch (k)
				{
				case 0:
					red[i][j] = fileReadBuffer[end - count]; // fileReadBuffer[end - count] is the red value
					redCopy[i][j] = red[i][j];
					count++;
					break;
				case 1:
					green[i][j] = fileReadBuffer[end - count]; // fileReadBuffer[end - count] is the green value
					greenCopy[i][j] = green[i][j];
					count++;
					break;
				case 2:
					blue[i][j] = fileReadBuffer[end - count]; // fileReadBuffer[end - count] is the blue value
					blueCopy[i][j] = blue[i][j];
					count++;
					break;
					// go to the next position in the buffer
				}
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
					fileBuffer[bufferSize - count] = red[i][j]; // write red value in fileBuffer[bufferSize - count]
					count++;
					break;
				case 1:
					fileBuffer[bufferSize - count] = green[i][j]; // write green value in fileBuffer[bufferSize - count]
					count++;
					break;
				case 2:
					fileBuffer[bufferSize - count] = blue[i][j]; // write blue value in fileBuffer[bufferSize - count]
					count++;
					break;
					// go to the next position in the buffer
				}
			}
	}
	write.write(fileBuffer, bufferSize);
}

void applyCrossFilter()
{
	for (int i = 0; i < rows - 1; i++)
	{
		for (int k = 0; k < 3; k++)
		{
			switch (k)
			{
			case 0:
				red[i][i] = WHITE;
				red[i][rows - 1 - i] = WHITE;
				red[i][i + 1] = WHITE;
				red[i + 1][i] = WHITE;
				red[i][rows - 1 - i - 1] = WHITE;
				red[i + 1][rows - 1 - i] = WHITE;
				break;
			case 1:
				green[i][i] = WHITE;
				green[i][rows - 1 - i] = WHITE;
				green[i][i + 1] = WHITE;
				green[i + 1][i] = WHITE;
				green[i][rows - 1 - i - 1] = WHITE;
				green[i + 1][rows - 1 - i] = WHITE;
				break;
			case 2:
				blue[i][i] = WHITE;
				blue[i][rows - 1 - i] = WHITE;
				blue[i][i + 1] = WHITE;
				blue[i + 1][i] = WHITE;
				blue[i][rows - 1 - i - 1] = WHITE;
				blue[i + 1][rows - 1 - i] = WHITE;
				break;
			}
		}
	}
	int i = rows - 1;
	red[i][i] = WHITE;
	red[i][rows - 1 - i] = WHITE;
	green[i][i] = WHITE;
	green[i][rows - 1 - i] = WHITE;
	blue[i][i] = WHITE;
	blue[i][rows - 1 - i] = WHITE;
}

void applyOverallAverageFilter()
{
	long double size = rows * cols;
  	avrRed /= size;
  	avrGreen /= size;
  	avrBlue /= size;
	for (int i = 0; i < rows; i++)
	{
		for (int j = cols - 1; j >= 0; j--)
			for (int k = 0; k < 3; k++)
			{
				switch (k)
				{
				case 0:
					red[i][j] = (0.4*red[i][j] + 0.6*avrRed);
					break;
				case 1:
					green[i][j] = (0.4*green[i][j] + 0.6*avrGreen);
					break;
				case 2:
					blue[i][j] = (0.4*blue[i][j] + 0.6*avrBlue);
					break;
				}
			}
	}
}

void applySepiaFilter()
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = cols - 1; j >= 0; j--)
			for (int k = 0; k < 3; k++)
			{
				switch (k)
				{
				case 0:
					redCopy[i][j] = red[i][j];
					red[i][j] = std::min(double(255),(0.393*redCopy[i][j] + 0.769*greenCopy[i][j] + 0.189*blueCopy[i][j]));
          			avrRed += red[i][j];
					break;
				case 1:
					greenCopy[i][j] = green[i][j];
					green[i][j] = std::min(double(255),(0.349*redCopy[i][j] + 0.686*greenCopy[i][j] + 0.168*blueCopy[i][j]));
          			avrGreen += green[i][j];
					break;
				case 2:
					blueCopy[i][j] = blue[i][j];
					blue[i][j] = std::min(double(255),(0.272*redCopy[i][j] + 0.534*greenCopy[i][j] + 0.131*blueCopy[i][j]));
          			avrBlue += blue[i][j];
					break;
				}
			}
	}
}

void applySmoothingFilter()
{
	for (int i = 1; i < rows - 1; i++)
	{
		for (int j = cols - 1 - 1; j >= 0 + 1; j--)
			for (int k = 0; k < 3; k++)
			{
				switch (k)
				{
				case 0:
					red[i][j] = (redCopy[i - 1][j - 1] + redCopy[i - 1][j] + redCopy[i - 1][j + 1] + redCopy[i][j - 1] + redCopy[i][j] + redCopy[i][j + 1] + redCopy[i + 1][j - 1] + redCopy[i + 1][j] + redCopy[i + 1][j + 1]) / 9;
					break;
				case 1:
					green[i][j] = (greenCopy[i - 1][j - 1] + greenCopy[i - 1][j] + greenCopy[i - 1][j + 1] + greenCopy[i][j - 1] + greenCopy[i][j] + greenCopy[i][j + 1] + greenCopy[i + 1][j - 1] + greenCopy[i + 1][j] + greenCopy[i + 1][j + 1]) / 9;
					break;
				case 2:
					blue[i][j] = (blueCopy[i - 1][j - 1] + blueCopy[i - 1][j] + blueCopy[i - 1][j + 1] + blueCopy[i][j - 1] + blueCopy[i][j] + blueCopy[i][j + 1] + blueCopy[i + 1][j - 1] + blueCopy[i + 1][j] + blueCopy[i + 1][j + 1]) / 9;
					break;
				}
			}
	}
}

void applyFilters(char *fileBuffer, int bufferSize)
{
	applySmoothingFilter();
  	applySepiaFilter();
  	applyOverallAverageFilter();
  	applyCrossFilter();
}

int main(int argc, char *argv[])
{
	auto start = high_resolution_clock::now();

	char *fileBuffer;
	int bufferSize;
	char *fileName = argv[1];

	if (!fillAndAllocate(fileBuffer, fileName, rows, cols, bufferSize))
	{
		cout << "File read error" << endl;
		return 1;
	}

	getPixlesFromBMP24(bufferSize, rows, cols, fileBuffer); // read input file
	applyFilters(fileBuffer, bufferSize); // apply filters
	writeOutBmp24(fileBuffer, "output.bmp", bufferSize); // write output file

	auto stop4 = high_resolution_clock::now();
	auto duration4 = duration_cast<microseconds>(stop4 - start);
	cout << "serial execution time: " << duration4.count()/1000 << " miliseconds" << endl;

	return 0;
}