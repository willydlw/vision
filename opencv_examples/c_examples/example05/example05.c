/** Filename: example05.c
*
*   Description: opens a color image file named in argv[1].
*
*   This example demonstrates how to access the pixel data,
*   use a formula to convert the color image data to grayscale
*   and then compare the results to OpenCV's cvtColor function.
*
*/

#include <stdio.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>


int main(int argc, char** argv){

    /** Two arguments must be passed to the program.
    *   argv[0] is the name of the executable
    *   argv[1] is the name of the image file to open
    */
    if(argc < 2){
        printf("Usage: ./example05 imageName");
        return -1;
    }

    /** Load the input image.
    *   Note: loading the image with this C function dynamically creates memory for the image data.
    *   Later we will need to release the image to free this memory.
    */
    IplImage* colorimg = cvLoadImage(argv[1],1);

    /** The pointer will be NULL if the image was not correctly opened and loaded into memory */
    if(colorimg == NULL){

        printf("File %s not opened, program ending\n", argv[1]);
        return -1;
    }

    /** We want to convert our color image to a grayscale image. First, let's declare two
    *   IplImage pointers for our grayscale images. We will create one grayscale image with
    *   OpenCV's cvCvtColor function and a second grayscale image by accessing and manipulating
    *   the pixel data ourselves. */
    IplImage *grayimg, *mygrayimg;

    /** Next, we need to allocate memory to store the grayscale images. Use the function
    *
    *   IplImage* cvCreateImage(CvSize size, int depth, int channels)
    *
    *   Parameters:
    *       size – Image width and height
    *       depth – Bit depth of image elements. See IplImage for valid depths.
    *       channels – Number of channels per pixel. See IplImage for details.
    *
    *   This function creates memory to store all the data members of the IplImage struct
    *   type, including memory for the pixel data.
    *
    *   What is the size of the grayscale image? It will be the same pixel width and height
    *   as the color image. The data type CvSize is a struct that stores the size of an
    *   image or rectangle. It has two data members, width and height. To create a variable of
    *   type CvSize, we can call the function CvSize cvSize(int width, int height)
    *
    */

    /** Remember, colorimg is a pointer to an IplImage struct. To access the struct's data
    *   members, use the notation -> */
    CvSize s = cvSize(colorimg->width, colorimg->height);

    /** The grayscale image depth will be the same as the color image's depth. Depth refers to
    *   the number of bits of the image pixels. Typically, we will work with 8 bit unsigned
    *   integers. OpenCV's enumeration for this depth type is IPL_DEPTH_8U.
    */
    int d = colorimg->depth;

    /** Now, let's call the cvCreateImage function, passing our size and depth parameters.
    *   The channel parameter will be 1. Grayscale only has 1 intensity channel for pixel values.
    */
    grayimg = cvCreateImage(s, d, 1);


    /** Let's verify memory was allocated. We will terminate the program in this case.
    *   We need to release the memory allocated for the colorimg before terminating the program */
    if(grayimg == NULL){
        printf("No memory allocated for grayimg\n");
        cvReleaseImage(&colorimg);                      /// Prevent memory leak
        return -1;
    }

    /** Create another image to store our grayscale results */
    mygrayimg = cvCreateImage(s, d, 1);

    if(mygrayimg == NULL){
        printf("No memory allocated for mygrayimg\n");
        cvReleaseImage(&colorimg);                      /// Prevent memory leak
        cvReleaseImage(&grayimg);
        return -1;
    }

    /** At this point, neither grayimg nor mygrayimg has garbage pixel data. cvCreateImage
    *   simply allocated memory to store the data, but did not initialize the pixel data */

    /** The next step is calling the cvtColor function to access the color image pixel data,
    *   convert these pixels to grayscale and store those grayscale pixels in grayimg data.
    *
    *   We will use OpenCV's convert color function
    *
    *   void cvCvtColor(const CvArr* src, CvArr* dst, int code)
    *
    *   Parameters:
    *   src – input image: 8-bit unsigned, 16-bit unsigned ( CV_16UC... ), or single-precision floating-point.
    *   dst – output image of the same size and depth as src.
    *   code – color space conversion code
    *
    *   We will use the code CV_BGR2GRAY whichs converts an BGR image 2 GRAY
    *   Note that the default color format in OpenCV is often referred to as RGB but it is actually BGR
    *   (the bytes are reversed). So the first byte in a standard (24-bit) color image will be an 8-bit
    *   Blue component, the second byte will be Green, and the third byte will be Red. The fourth, fifth,
    *   and sixth bytes would then be the second pixel (Blue, then Green, then Red), and so on.
    *
    *   How does OpenCV convert color to gray?   Y = 0.299 * R + 0.587 * G + 0.114 B
    *
    */

    cvCvtColor(colorimg, grayimg, CV_BGR2GRAY);


    /** Now, let's create a grayscale image by accessing the color image pixel data and then
    *   use OpenCV's formula to convert color to gray
    *
    *   The IplImage struct data member char* imageData contains a pointer to the first row of
    *   image data. The data member height tells us how many rows of pixels, and width tells
    *   us how many columns of pixels. Because we are using pointers to access the data, we
    *   need to know the widthstep to always correctly access the data.
    *
    *   We think of the pixel data as a two dimensional array. In order to be efficient in accessing
    *   the data, sometimes extra bytes are appended to each row. This byte aligns the data.
    *   widthStep contains the number of bytes between points in the same column and successive
    *   rows. The width is not sufficient to calculate the distance because each row may be aligned
    *   with a certain number of bytes to achieve faster processing. This means there may be some
    *   gaps between row i and row i+1.
    *
    *   Example: image is 2 rows, 5 columns. Height is 2, width is 5, widthStep is 16 bytes.
    *
    *   Each pixel requires 24 bits of storage. 5 columns x 24 bits = 120 bits. It is more efficient
    *   to byte align this to 32 bit address alignment. 32 * 4 = 128 bits, 128 is 0x80. Next row
    *   starts at address 0x80. Need to fill the first row with 1 extra byte (8 bits). widthStep is
    *   16 bytes. Data is 3 bytes (rgb) * 5 columns = 15 bytes.
    *
    *   Address (hexadicmal)
    *   0001        0019        0031        0049        0061        0079
    *   pixel(0,0)  pixel(0,1)  pixel(0,2)  pixel(0,3)  pixel(0,4)  garbage
    *
    *   0080        0098        00B0        00C8        00E0        0100
    *   pixel(1,0)  pixel(1,1)  pixel(1,2)  pixel(1,3)  pixel(1,4)  garbage
    */


    /// Let's display some information about the color image
    printf("\nImage: %s, height: %d, width: %d, widthStep: %d\n", argv[1], colorimg->height,
            colorimg->width, colorimg->widthStep);

    printf("colorimg->width * 24: %d bits, widthStep * 8: %d bits\n", colorimg->width * 24,
            colorimg->widthStep * 8);

    int row, col;
    unsigned char blue, green, red, gray;

    unsigned char* colorData = (unsigned char*)colorimg->imageData;
    unsigned char* grayData = (unsigned char*)mygrayimg->imageData;

    int colorstep = colorimg->widthStep/sizeof(uchar);
    int graystep = mygrayimg->widthStep/sizeof(uchar);
    for(row = 0; row < colorimg->height; ++row){
        for(col = 0; col < colorimg->width; ++col){
            /// remember color data is BGR. First byte is blue, second byte is green, ...
            blue = colorData[row * colorstep + col];
            green = colorData[row * colorstep + col + 1];
            red = colorData[row * colorstep + col + 2];

            /// calculate gray = 0.299 * R + 0.587 * G + 0.114 B
            gray = 0.299 * red + 0.587 * green + 0.114 * blue;

            /// store in mygrayimg data
            grayData[row * graystep + col] = gray;
        }
    }

    /** Display the images */
    cvNamedWindow("color", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO);
    cvNamedWindow("gray", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO);
    cvNamedWindow("mygray", CV_WINDOW_NORMAL | CV_WINDOW_KEEPRATIO);

    /** Let's tell OpenCV where to display the window on our screen with the function
    *
    *   void cvMoveWindow(const char* name, int x, int y)¶
    *
    *   Parameters:
    *   winname – Window name
    *   x – The new x-coordinate of the window
    *   y – The new y-coordinate of the window
    */
    cvMoveWindow("color", 100, 100);
    cvMoveWindow("gray", 500, 100);
    cvMoveWindow("mygray", 500, 500);

    cvShowImage("color", colorimg);
    cvShowImage("gray", grayimg);
    cvShowImage("mygray", grayimg);

    cvWaitKey(0);         /// Display images until user presses a key

    /** Free memory, or as the OpenCV book says: Don't be a piggy, clean up */
    cvReleaseImage(&colorimg);
    cvReleaseImage(&colorimg);
    cvDestroyAllWindows();


    return 0;
}
