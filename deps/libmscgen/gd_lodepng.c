#include "gd.h"
#include "lodepng.h"

BGD_DECLARE(void) gdImagePng (gdImagePtr im, FILE * outFile)
{
  unsigned char *buffer;
  size_t bufferSize;
  int **ptpixels = im->tpixels;
  unsigned char *pixelBuffer = (unsigned char *)malloc(3*im->sx*im->sy);
  unsigned char *pOut = pixelBuffer;
  for (int y = 0; y < im->sy; y++)
  {
    int *pThisRow = *ptpixels++;
    for (int x = 0; x < im->sx; x++)
    {
      const int thisPixel = *pThisRow++;
      *pOut++ = gdTrueColorGetRed(thisPixel);
      *pOut++ = gdTrueColorGetGreen(thisPixel);
      *pOut++ = gdTrueColorGetBlue(thisPixel);
    }
  }
  // TODO: convert ptPixels into pixelBuffer...
  LodePNG_Encoder encoder;
  LodePNG_Encoder_init(&encoder);
  encoder.infoPng.color.colorType = 2; // 2=RGB 24 bit
  encoder.infoRaw.color.colorType = 2; // 2=RGB 24 bit
  LodePNG_encode(&encoder, &buffer, &bufferSize, pixelBuffer, im->sx, im->sy);
  // write bufferSize bytes from buffer into outFile
  fwrite(buffer,1,bufferSize,outFile);
  LodePNG_Encoder_cleanup(&encoder);
  free(buffer);
  free(pixelBuffer);
}

