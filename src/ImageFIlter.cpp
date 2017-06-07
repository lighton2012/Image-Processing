#include "ImageFilter.h"


void convolve2D (uchar Y_buff[], int xSize, int ySize, double filterCoeff, int N)
{
	//TO DO
}

void extendBorders(uchar input[], int xSize, int ySize, uchar output[], int delta)
{
	for (int i = 0; i<ySize; i++)
	{
		memcpy(&output[(i + delta)*(xSize + 2 * delta) + delta], &input[i*(xSize)], xSize);
		if (delta != 0)
		{
			memset(&output[(i + delta)*(xSize + 2 * delta)], input[i*(xSize)], delta);
			memset(&output[(i + delta)*(xSize + 2 * delta) + xSize + delta], input[i*xSize + xSize - 1], delta);
		}
	}

	for (int i = 0; i<delta; i++)
	{
		memcpy(&output[i*(xSize + 2 * delta)], &output[(delta)*(xSize + 2 * delta)], xSize + 2 * delta);
		memcpy(&output[(i + ySize + delta)*(xSize + 2 * delta)], &output[(delta + ySize - 1)*(xSize + 2 * delta)], xSize + 2 * delta);
	}

}

void extendBorders(char input[], int xSize, int ySize, char output[], int delta)
{
	for (int i = 0; i<ySize; i++)
	{
		memcpy(&output[(i + delta)*(xSize + 2 * delta) + delta], &input[i*(xSize)], xSize);
		if (delta != 0)
		{
			memset(&output[(i + delta)*(xSize + 2 * delta)], input[i*(xSize)], delta);
			memset(&output[(i + delta)*(xSize + 2 * delta) + xSize + delta], input[i*xSize + xSize - 1], delta);
		}
	}

	for (int i = 0; i<delta; i++)
	{
		memcpy(&output[i*(xSize + 2 * delta)], &output[(delta)*(xSize + 2 * delta)], xSize + 2 * delta);
		memcpy(&output[(i + ySize + delta)*(xSize + 2 * delta)], &output[(delta + ySize - 1)*(xSize + 2 * delta)], xSize + 2 * delta);
	}

}
	
void performNFFilter (uchar input[], int xSize, int ySize)
{
	//TO DO
}

void performVFFilter (uchar input[], int xSize, int ySize)
{
	//TO DO
}

void performSuccessiveVFFilter (uchar input[], int xSize, int ySize, int stages)
{
	//TO DO
}

void performSobelEdgeDetection(uchar input[], int xSize, int ySize, uchar threshold)
{
	//TO DO
}

void performNFplusSobelEdgeDetection(uchar input[], int xSize, int ySize, int stages, uchar threshold)
{
	//TO DO
}
