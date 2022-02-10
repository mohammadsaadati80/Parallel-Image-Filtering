#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <chrono>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <pthread.h>

#include <math.h>
#include <iomanip> 
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>

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

const int MAX_THREAD_NUMBERS = 20; 

long double avrRed[MAX_THREAD_NUMBERS] = {0};
long double avrGreen[MAX_THREAD_NUMBERS] = {0};
long double avrBlue[MAX_THREAD_NUMBERS] = {0};

int cur = 0;

struct thread_data {
   int  start;
   int  finish;
   int cur;
};

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

void *applyCrossFilter2(void *threadarg)
{
	struct thread_data *my_data;
   	my_data = (struct thread_data *) threadarg; 
	
	for (int i = (int)my_data->start; i < (int)my_data->finish; i++)
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
	pthread_exit(NULL);
}

void *applyOverallAverageFilter2(void *threadarg)
{
	struct thread_data *my_data;
   	my_data = (struct thread_data *) threadarg;
	long double avrRedz = 0;
	long double avrGreenz = 0;
	long double avrBluez = 0;
	for(int i=0; i < MAX_THREAD_NUMBERS; i++)
	{
		avrRedz += avrRed[i];
		avrGreenz += avrGreen[i];
		avrBluez += avrBlue[i];
	}

	long double size = rows * cols;
  	avrRedz /= size;
  	avrGreenz /= size;
  	avrBluez /= size;
	for (int i = (int)my_data->start; i < (int)my_data->finish; i++)
	{
		for (int j = cols - 1; j >= 0; j--)
			for (int k = 0; k < 3; k++)
			{
				switch (k)
				{
				case 0:
					red[i][j] = (0.4*red[i][j] + 0.6*avrRedz);
					break;
				case 1:
					green[i][j] = (0.4*green[i][j] + 0.6*avrGreenz);
					break;
				case 2:
					blue[i][j] = (0.4*blue[i][j] + 0.6*avrBluez);
					break;
				}
			}
	}
	pthread_exit(NULL);
}

void *applySepiaFilter2(void *threadarg)
{
	struct thread_data *my_data;
   	my_data = (struct thread_data *) threadarg;

	for (int i = (int)my_data->start; i < (int)my_data->finish; i++)
	{
		for (int j = cols - 1; j >= 0; j--)
			for (int k = 0; k < 3; k++)
			{
				switch (k)
				{
				case 0:
					redCopy[i][j] = red[i][j];
					red[i][j] = std::min(double(255),(0.393*redCopy[i][j] + 0.769*greenCopy[i][j] + 0.189*blueCopy[i][j]));
          			avrRed[(int)my_data->cur] += red[i][j];
					break;
				case 1:
					greenCopy[i][j] = green[i][j];
					green[i][j] = std::min(double(255),(0.349*redCopy[i][j] + 0.686*greenCopy[i][j] + 0.168*blueCopy[i][j]));
          			avrGreen[(int)my_data->cur] += green[i][j];
					break;
				case 2:
					blueCopy[i][j] = blue[i][j];
					blue[i][j] = std::min(double(255),(0.272*redCopy[i][j] + 0.534*greenCopy[i][j] + 0.131*blueCopy[i][j]));
          			avrBlue[(int)my_data->cur] += blue[i][j];
					break;
				}
			}
	}
	pthread_exit(NULL);
}

void *applySmoothingFilter2(void *threadarg)
{
	struct thread_data *my_data;
   	my_data = (struct thread_data *) threadarg;
	
	for (int i = (int)my_data->start; i < (int)my_data->finish; i++)
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
	pthread_exit(NULL);
}

void applyCrossFilter()
{
   int rc;
   pthread_t threads[MAX_THREAD_NUMBERS];
   pthread_attr_t attr;
   void *status;
   int step = (rows % MAX_THREAD_NUMBERS == 0) ? (rows / MAX_THREAD_NUMBERS) : (rows / MAX_THREAD_NUMBERS + 1);
   struct thread_data td[MAX_THREAD_NUMBERS];

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   for(int i = 0; i < MAX_THREAD_NUMBERS; i++ ) {
	  td[i].start = std::max(0, i*step);
      td[i].finish = std::min(rows - 1 , (i+1)*step);

      rc = pthread_create(&threads[i], &attr, applyCrossFilter2, (void *)&td[i] );
      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }

   // free attribute and wait for the other threads
   pthread_attr_destroy(&attr);
   for(int i = 0; i < MAX_THREAD_NUMBERS; i++ ) {
      rc = pthread_join(threads[i], &status);
      if (rc) {
         cout << "Error:unable to join," << rc << endl;
         exit(-1);
      }
   }
}

void applyOverallAverageFilter()
{
   int rc;
   pthread_t threads[MAX_THREAD_NUMBERS];
   pthread_attr_t attr;
   void *status;
   int step = (rows % MAX_THREAD_NUMBERS == 0) ? (rows / MAX_THREAD_NUMBERS) : (rows / MAX_THREAD_NUMBERS + 1);
   struct thread_data td[MAX_THREAD_NUMBERS];

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   for(int i = 0; i < MAX_THREAD_NUMBERS; i++ ) {
	  td[i].start = std::max(0, i*step);
      td[i].finish = std::min(rows , (i+1)*step);
	  
      rc = pthread_create(&threads[i], &attr, applyOverallAverageFilter2, (void *)&td[i] );
      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }

   // free attribute and wait for the other threads
   pthread_attr_destroy(&attr);
   for(int i = 0; i < MAX_THREAD_NUMBERS; i++ ) {
      rc = pthread_join(threads[i], &status);
      if (rc) {
         cout << "Error:unable to join," << rc << endl;
         exit(-1);
      }
   }
}

void applySepiaFilter()
{
   int rc;
   pthread_t threads[MAX_THREAD_NUMBERS];
   pthread_attr_t attr;
   void *status;
   int step = (rows % MAX_THREAD_NUMBERS == 0) ? (rows / MAX_THREAD_NUMBERS) : (rows / MAX_THREAD_NUMBERS + 1);
   struct thread_data td[MAX_THREAD_NUMBERS];

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   for(int i = 0; i < MAX_THREAD_NUMBERS; i++ ) {
	  td[i].start = std::max(0, i*step);
      td[i].finish = std::min(rows , (i+1)*step);
      td[i].cur = i;

      rc = pthread_create(&threads[i], &attr, applySepiaFilter2, (void *)&td[i]);
      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }

   // free attribute and wait for the other threads
   pthread_attr_destroy(&attr);
   for(int i = 0; i < MAX_THREAD_NUMBERS; i++ ) {
      rc = pthread_join(threads[i], &status);
      if (rc) {
         cout << "Error:unable to join," << rc << endl;
         exit(-1);
      }
   }
}

void applySmoothingFilter()
{		
   int rc;
   pthread_t threads[MAX_THREAD_NUMBERS];
   pthread_attr_t attr;
   void *status;
   int step = (rows % MAX_THREAD_NUMBERS == 0) ? (rows / MAX_THREAD_NUMBERS) : (rows / MAX_THREAD_NUMBERS + 1);
   struct thread_data td[MAX_THREAD_NUMBERS];

   // Initialize and set thread joinable
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

   for(int i = 0; i < MAX_THREAD_NUMBERS; i++ ) {
	  td[i].start = std::max(1, i*step);
      td[i].finish = std::min(rows - 1, (i+1)*step);
	  
      rc = pthread_create(&threads[i], &attr, applySmoothingFilter2, (void *)&td[i] );
      if (rc) {
         cout << "Error:unable to create thread," << rc << endl;
         exit(-1);
      }
   }

   // free attribute and wait for the other threads
   pthread_attr_destroy(&attr);
   for(int i = 0; i < MAX_THREAD_NUMBERS; i++ ) {
      rc = pthread_join(threads[i], &status);
      if (rc) {
         cout << "Error:unable to join," << rc << endl;
         exit(-1);
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
	cout << "parallel execution time: " << duration4.count()/1000 << " miliseconds" << endl;

	return 0;
}