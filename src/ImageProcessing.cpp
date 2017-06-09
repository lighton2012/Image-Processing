
#include "ImageProcessing.h"
#include "ImageInterpolation.h"

#include <QDebug>

void imageProcessingFun(const QString& progName, QImage* const outImgs, const QImage* const inImgs, const QVector<double>& params) 
{
	int X_SIZE = inImgs->width();
	int Y_SIZE = inImgs->height();

	/* NOTE: Calculate output image resolution and construct output image object */
	int X_SIZE_NEW;
	int Y_SIZE_NEW;

	if (progName != "Rotation" && progName != "Rotation Bilinear") {
		X_SIZE_NEW = divisibleByN(X_SIZE * params[1], 4);
		Y_SIZE_NEW = divisibleByN(Y_SIZE * params[0], 4);
	
		new (outImgs) QImage(X_SIZE_NEW, Y_SIZE_NEW, inImgs->format());
	}
	else {
		new (outImgs) QImage(X_SIZE, Y_SIZE, inImgs->format());
	}

	//outImgs = new QImage(X_SIZE_NEW, Y_SIZE_NEW, inImgs->format);

	if(progName == "Sample and hold") 
	{	
		/* Input image data in RGB format can be obtained with inImgs->bits() */
		/* Vertical scale factor is params[0] */
		/* Horizontal scale factor is params[1] */

		/* Perform Sample and hold interpolation  */
	
		sampleAndHold(inImgs->bits(), X_SIZE, Y_SIZE, outImgs->bits(), X_SIZE_NEW, Y_SIZE_NEW);

	}
	else if (progName == "Bilinear") 
	{
		/* Input image data in RGB format can be obtained with inImgs->bits() */
		/* Vertical scale factor is params[0] */
		/* Horizontal scale factor is params[1] */

		/* Perform Bilinear interpolation  */

		bilinearInterpolate(inImgs->bits(), X_SIZE, Y_SIZE, outImgs->bits(), X_SIZE_NEW, Y_SIZE_NEW);
	}
	else if (progName == "Bicubic")
	{
		/* Input image data in RGB format can be obtained with inImgs->bits() */
		/* Vertical scale factor is params[0] */
		/* Horizontal scale factor is params[1] */

		/* Perform Bicubic interpolation  */
		bicubicInterpolate(inImgs->bits(), X_SIZE, Y_SIZE, outImgs->bits(), X_SIZE_NEW, Y_SIZE_NEW);
	}
	else if(progName == "Rotation") 
	{	
		/* Input image data in RGB format can be obtained with inImgs->bits() */
		/* Rotation angle in degrees is params[0]*/
		/* Center of rotation coordinates are (XSIZE/2, YSIZE/2) */

		/* TO DO: Perform image rotation */
		
		imageRotate((uchar*)inImgs->bits(), X_SIZE, Y_SIZE, outImgs->bits(), X_SIZE / 2, Y_SIZE / 2, params[0]);
	}
	else if (progName == "Rotation Bilinear") 
	{
		/* Input image data in RGB format can be obtained with inImgs->bits() */
		/* Rotation angle in degrees is params[0]*/
		/* Center of rotation coordinates are (XSIZE/2, YSIZE/2) */

		/* TO DO: Perform image rotation with bilinear interpolation */
		
		imageRotateBilinear(inImgs->bits(), X_SIZE, Y_SIZE, outImgs->bits(), X_SIZE / 2, Y_SIZE / 2, params[0]);
	}

}

