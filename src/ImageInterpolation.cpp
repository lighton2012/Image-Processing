#include "ImageInterpolation.h"
#include "ImageFilter.h"
#include "ColorSpaces.h"
#include <math.h>


void sampleAndHold(const uchar input[], int xSize, int ySize, uchar output[], int newXSize, int newYSize)
{
	/* YUV420 format is used */
	uchar* yVal = new uchar[xSize * ySize];
	char* uVal = new char[xSize * ySize / 4];
	char* vVal = new char[xSize * ySize / 4];

	RGBtoYUV420(input, xSize, ySize, yVal, uVal, vVal);

	uchar* yValNew = new uchar[newXSize * newYSize];
	char* uValNew = new char[newXSize * newYSize / 4];
	char* vValNew = new char[newXSize * newYSize / 4];

	/* Scale factors for new image*/
	double scaleFactorY = (double)newYSize / ySize;
	double scaleFactorX = (double)newXSize / xSize;

	for (int i = 0; i < newYSize; i++) {
		for (int j = 0; j < newXSize; j++) {
			int closestValueY = (i - 1) / scaleFactorY + 1;
			int closestValueX = (j - 1) / scaleFactorX + 1;
			
			yValNew[i * newXSize + j] = yVal[closestValueY * xSize + closestValueX];

			if (i < (newYSize / 2) && j < (newXSize / 2)) {
				uValNew[i * newXSize / 2 + j] = uVal[closestValueY * xSize / 2 + closestValueX];
				vValNew[i * newXSize / 2 + j] = vVal[closestValueY * xSize / 2 + closestValueX];
			}
		}
	}

	YUV420toRGB(yValNew, uValNew, vValNew, newXSize, newYSize, output);

	delete[]yVal;
	delete[]uVal;
	delete[]vVal;
	delete[]yValNew;
	delete[]uValNew;
	delete[]vValNew;
}

void bilinearInterpolate(const uchar input[], int xSize, int ySize, uchar output[], int newXSize, int newYSize)
{
	double distY;
	double distX;
	int firstNewY;
	int firstNewX;
	int secondNewY;
	int secondNewX;

	/* YUV420 format is used */
	uchar* yVal = new uchar[xSize * ySize];
	char* uVal = new char[xSize * ySize / 4];
	char* vVal = new char[xSize * ySize / 4];

	RGBtoYUV420(input, xSize, ySize, yVal, uVal, vVal);

	uchar* yValNew = new uchar[newXSize * newYSize];
	char* uValNew = new char[newXSize * newYSize / 4];
	char* vValNew = new char[newXSize * newYSize / 4];

	/* Scale factors for new image*/
	double scaleFactorY = (double)newYSize / ySize;
	double scaleFactorX = (double)newXSize / xSize;

	/* YUV420 format is used */
	for (int i = 0; i < newYSize; i++) {
		for (int j = 0; j < newXSize; j++) {
			distY = i / scaleFactorY - floor(i / scaleFactorY);
			distX = j / scaleFactorX - floor(j / scaleFactorX);

			firstNewY = i / scaleFactorY;
			firstNewX = j / scaleFactorX;

			/* Protection from calculating interpolation value out of borders */
			secondNewY = firstNewY < ySize - 1 ? firstNewY + 1 : firstNewY;
			secondNewX = firstNewX < xSize - 1 ? firstNewX + 1 : firstNewX;

			yValNew[i * newXSize + j] = 
				(1 - distY) * (1 - distX) * yVal[firstNewY * xSize + firstNewX] 
				+ (1 - distY) * distX * yVal[firstNewY * xSize + secondNewX] 
				+ distY * (1 - distX) * yVal[secondNewY * xSize + firstNewX] 
				+ distY * distX * yVal[secondNewY * xSize + secondNewX];
				
			
			if (i < (newYSize / 2) && j < (newXSize / 2)) {
				secondNewY = firstNewY < ySize / 2 - 1 ? firstNewY + 1 : firstNewY;
				secondNewX = firstNewX < xSize / 2- 1 ? firstNewX + 1 : firstNewX;
				
				uValNew[i * newXSize / 2 + j] = 
					(1 - distY) * (1 - distX) * uVal[firstNewY * xSize / 2 + firstNewX]
					+ (1 - distY) * distX * uVal[firstNewY * xSize / 2 + secondNewX]
					+ distY * (1 - distX) * uVal[secondNewY * xSize / 2 + firstNewX]
					+ distY * distX * uVal[secondNewY * xSize / 2 + secondNewX];
				
				vValNew[i * newXSize / 2 + j] =
					(1 - distY) * (1 - distX) * vVal[firstNewY * xSize / 2 + firstNewX]
					+ (1 - distY) * distX * vVal[firstNewY * xSize / 2 + secondNewX]
					+ distY * (1 - distX) * vVal[secondNewY * xSize / 2 + firstNewX]
					+ distY * distX * vVal[secondNewY * xSize / 2 + secondNewX];
			}
		}
	}

	YUV420toRGB(yValNew, uValNew, vValNew, newXSize, newYSize, output);

	delete[]yVal;
	delete[]uVal;
	delete[]vVal;
	delete[]yValNew;
	delete[]uValNew;
	delete[]vValNew;
}

uchar cubicInterpolate(uchar p[4], double x) {
	int res = p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));

	if (res > 255) {
		res = 255;
	}
	else if (res < 0) {
		res = 0;
	}

	return res;
}

char cubicInterpolate(char p[4], double x) {
	int res = p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));

	if (res > 127) {
		res = 127;
	}
	else if (res < -127) {
		res = -127;
	}

	return (char)res;
}

void bicubicInterpolate(const uchar input[], int xSize, int ySize, uchar output[], int newXSize, int newYSize)
{
	uchar hBicubicInterpolateY[4];
	uchar vBicubicInterpolateY[4];
	char hBicubicInterpolateU[4];
	char vBicubicInterpolateU[4];
	char hBicubicInterpolateV[4];
	char vBicubicInterpolateV[4];

	double distY;
	double distX;
	int firstNewY;
	int firstNewX;

	uchar* yVal = new uchar[xSize * ySize];
	char* uVal = new char[xSize * ySize / 4];
	char* vVal = new char[xSize * ySize / 4];

	RGBtoYUV420(input, xSize, ySize, yVal, uVal, vVal);

	uchar* yValNew = new uchar[newXSize * newYSize];
	char* uValNew = new char[newXSize * newYSize / 4];
	char* vValNew = new char[newXSize * newYSize / 4];

	/* Scale factors for new image*/
	double scaleFactorY = (double)newYSize / ySize;
	double scaleFactorX = (double)newXSize / xSize;

	/* Extend borders so that last pixels are calculated correctly */
	uchar *yExt = new uchar[(xSize + 4) * (ySize + 4)]();
	char *uExt = new char[(xSize + 16) * (ySize + 16) / 4]();
	char *vExt = new char[(xSize + 16) * (ySize + 16) / 4]();

	extendBorders(yVal, xSize, ySize, yExt, 2);
	extendBorders(uVal, xSize / 2, ySize / 2, uExt, 2);
	extendBorders(vVal, xSize / 2, ySize / 2, vExt, 2);

	/* YUV420 format is used */
	for (int i = 0; i < newYSize; i++) {
		for (int j = 0; j < newXSize; j++) {
			distY = i / scaleFactorY - floor(i / scaleFactorY);
			distX = j / scaleFactorX - floor(j / scaleFactorX);

			firstNewY = i / scaleFactorY;
			firstNewX = j / scaleFactorX;

			int vIndex = 0;
			for (int hIndex = firstNewY - 1; hIndex < firstNewY + 3; hIndex++, vIndex++) {
				hBicubicInterpolateY[0] = yExt[hIndex * (xSize + 4) + firstNewX - 1];
				hBicubicInterpolateY[1] = yExt[hIndex * (xSize + 4) + firstNewX];
				hBicubicInterpolateY[2] = yExt[hIndex * (xSize + 4) + firstNewX + 1];
				hBicubicInterpolateY[3] = yExt[hIndex * (xSize + 4) + firstNewX + 2];

				vBicubicInterpolateY[vIndex] = cubicInterpolate(hBicubicInterpolateY, distX);

				if (i < newYSize / 2 && j < newXSize / 2) {
					hBicubicInterpolateU[0] = uExt[hIndex * (xSize / 2 + 4) + firstNewX - 1];
					hBicubicInterpolateU[1] = uExt[hIndex * (xSize / 2 + 4) + firstNewX];
					hBicubicInterpolateU[2] = uExt[hIndex * (xSize / 2 + 4) + firstNewX + 1];
					hBicubicInterpolateU[3] = uExt[hIndex * (xSize / 2 + 4) + firstNewX + 2];

					vBicubicInterpolateU[vIndex] = cubicInterpolate(hBicubicInterpolateU, distX);

					hBicubicInterpolateV[0] = vExt[hIndex * (xSize / 2 + 4) + firstNewX - 1];
					hBicubicInterpolateV[1] = vExt[hIndex * (xSize / 2 + 4) + firstNewX];
					hBicubicInterpolateV[2] = vExt[hIndex * (xSize / 2 + 4) + firstNewX + 1];
					hBicubicInterpolateV[3] = vExt[hIndex * (xSize / 2 + 4) + firstNewX + 2];

					vBicubicInterpolateV[vIndex] = cubicInterpolate(hBicubicInterpolateV, distX);
				}
			}

			yValNew[i * newXSize + j] = cubicInterpolate(vBicubicInterpolateY, distY);
			
			if (i < newYSize / 2 && j < newXSize / 2) {
				uValNew[i * newXSize / 2 + j] = cubicInterpolate(vBicubicInterpolateU, distY);
				vValNew[i * newXSize / 2  + j] = cubicInterpolate(vBicubicInterpolateV, distY);
			}
		}
	}

	YUV420toRGB(yValNew, uValNew, vValNew, newXSize, newYSize, output);

	delete[]yVal;
	delete[]uVal;
	delete[]vVal;
	delete[]yValNew;
	delete[]uValNew;
	delete[]vValNew;
	delete[]yExt;
	delete[]uExt;
	delete[]vExt;
}

void imageRotate(uchar input[], int xSize, int ySize, uchar output[], int m, int n, double angle)
{
	int newPosX;
	int	newPosY;

	/* YUV420 format is used */
	uchar* yVal = new uchar[xSize * ySize];
	char* uVal = new char[xSize * ySize / 4];
	char* vVal = new char[xSize * ySize / 4];

	RGBtoYUV420(input, xSize, ySize, yVal, uVal, vVal);

	uchar* yValNew = new uchar[xSize * ySize];
	char* uValNew = new char[xSize * ySize / 4];
	char* vValNew = new char[xSize * ySize / 4];

	double theta = 3.14 * angle / 180;

	for (int i = 0; i < ySize; i++)
	{
		for (int j = 0; j < xSize; j++)
		{
			newPosY = round(i * cos(theta) + j * sin(theta) - m * sin(theta) - n * cos(theta) + n);
			newPosX = round(j * cos(theta) - i * sin(theta) - m * cos(theta) + n * sin(theta) + m);

			if (newPosY < 0 || newPosY > ySize - 1 || newPosX < 0 || newPosX > xSize - 1) {
				yValNew[i * xSize + j] = 0;
			}
			else {
				yValNew[i * xSize + j] = yVal[newPosY * xSize + newPosX];
			}

			
			if (i < ySize / 2 && j < xSize / 2) {
				newPosY = round(i * cos(theta) + j * sin(theta) - m / 2 * sin(theta) - n / 2 * cos(theta) + n / 2);
				newPosX = round(j * cos(theta) - i * sin(theta) - m / 2 * cos(theta) + n / 2 * sin(theta) + m / 2);

				if (newPosY < 0 || newPosY > ySize / 2 - 1 || newPosX < 0 || newPosX > xSize / 2- 1) {
					uValNew[i * xSize / 2 + j] = 0;
					vValNew[i * xSize / 2 + j] = 0;
				}
				else {
					uValNew[i * xSize / 2 + j] = uVal[newPosY * xSize / 2 + newPosX];
					vValNew[i * xSize / 2 + j] = vVal[newPosY * xSize / 2 + newPosX];
				}
			}
			
		}
	}

	YUV420toRGB(yValNew, uValNew, vValNew, xSize, ySize, output);

	delete[]yVal;
	delete[]uVal;
	delete[]vVal;
	delete[]yValNew;
	delete[]uValNew;
	delete[]vValNew;
}

void imageRotateBilinear(const uchar input[], int xSize, int ySize, uchar output[], int m, int n, double angle)
{
	double newPosX;
	double newPosY;
	int closestFirstX;
	int closestFirstY;
	int closestSecondX;
	int closestSecondY;
	double distX;
	double distY;

	/* YUV420 format is used */
	uchar* yVal = new uchar[xSize * ySize];
	char* uVal = new char[xSize * ySize / 4];
	char* vVal = new char[xSize * ySize / 4];

	RGBtoYUV420(input, xSize, ySize, yVal, uVal, vVal);

	uchar* yValNew = new uchar[xSize * ySize];
	char* uValNew = new char[xSize * ySize / 4];
	char* vValNew = new char[xSize * ySize / 4];

	uchar *yExt = new uchar[(xSize + 2) * (ySize + 2)];
	char *uExt = new char[(xSize + 8) * (ySize + 8) / 4];
	char *vExt = new char[(xSize + 8) * (ySize + 8) / 4];

	extendBorders(yVal, xSize, ySize, yExt, 1);
	extendBorders(uVal, xSize / 2, ySize / 2, uExt, 1);
	extendBorders(vVal, xSize / 2, ySize / 2, vExt, 1);

	double theta = 3.14 * angle / 180;

	for (int i = 0; i < ySize; i++)
	{
		for (int j = 0; j < xSize; j++)
		{
			newPosY = (i * cos(theta) + j * sin(theta) - m * sin(theta) - n * cos(theta) + n);
			newPosX = (j * cos(theta) - i * sin(theta) - m * cos(theta) + n * sin(theta) + m);

			closestFirstY = floor(newPosY);
			closestFirstX = floor(newPosX);
			closestSecondY = closestFirstY + 1;
			closestSecondX = closestFirstX + 1;

			distY = newPosY - closestFirstY;
			distX = newPosX - closestFirstX;

			if (closestFirstY < 0 || closestFirstY > ySize - 1 || closestFirstX < 0 || closestFirstX > xSize - 1) {
				yValNew[i * xSize + j] = 0;
			}
			else {
				yValNew[i * xSize + j] = 
					(1 - distY) * (1 - distX) * yExt[closestFirstY * (xSize + 2) + closestFirstX]
					+ (1 - distY) * distX * yExt[closestFirstY * (xSize + 2) + closestSecondX]
					+ distY * (1 - distX) * yExt[closestSecondY * (xSize + 2) + closestFirstX]
					+ distY * distX * yExt[closestSecondY * (xSize + 2) + closestSecondX];
			}
			
			if (i < ySize / 2 && j < xSize / 2) {
				newPosY = (i * cos(theta) + j * sin(theta) - m / 2 * sin(theta) - n / 2 * cos(theta) + n / 2);
				newPosX = (j * cos(theta) - i * sin(theta) - m / 2 * cos(theta) + n / 2 * sin(theta) + m / 2);

				closestFirstY = floor(newPosY);
				closestFirstX = floor(newPosX);
				closestSecondY = closestFirstY + 1;
				closestSecondX = closestFirstX + 1;

				distY = newPosY - closestFirstY;
				distX = newPosX - closestFirstX;

				if (closestFirstY < 0 || closestFirstY > ySize / 2 - 1 || closestFirstX < 0 || closestFirstX > xSize / 2- 1) {
					uValNew[i * xSize / 2 + j] = 0;
					vValNew[i * xSize / 2 + j] = 0;
				}
				else {
					uValNew[i * xSize / 2 + j] =
						(1 - distY) * (1 - distX) * uExt[closestFirstY * (xSize / 2 + 2) + closestFirstX]
						+ (1 - distY) * distX * uExt[closestFirstY * (xSize / 2 + 2) + closestSecondX]
						+ distY * (1 - distX) * uExt[closestSecondY * (xSize / 2 + 2) + closestFirstX]
						+ distY * distX * uExt[closestSecondY * (xSize / 2 + 2) + closestSecondX];

					vValNew[i * xSize / 2 + j] =
						(1 - distY) * (1 - distX) * vExt[closestFirstY * (xSize / 2 + 2) + closestFirstX]
						+ (1 - distY) * distX * vExt[closestFirstY * (xSize / 2 + 2) + closestSecondX]
						+ distY * (1 - distX) * vExt[closestSecondY * (xSize / 2 + 2) + closestFirstX]
						+ distY * distX * vExt[closestSecondY * (xSize / 2 + 2) + closestSecondX];
				}
			}
		}
	}

	YUV420toRGB(yValNew, uValNew, vValNew, xSize, ySize, output);

	delete[]yVal;
	delete[]uVal;
	delete[]vVal;
	delete[]yValNew;
	delete[]uValNew;
	delete[]vValNew;
	delete[]yExt;
	delete[]uExt;
	delete[]vExt;
}

int divisibleByN(int sizeNum, int n) {
	return sizeNum % n == 0 ? sizeNum : sizeNum + (n - (sizeNum % n));
}