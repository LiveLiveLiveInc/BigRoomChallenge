// main.cpp

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using std::cout;
using std::endl;
using std::string;

#define YUV2RGB(y, u, v, r, g, b)        \
    r = y + ((v * 1436) >> 10);          \
    g = y - ((u * 352 + v * 731) >> 10); \
    b = y + ((u * 1814) >> 10);          \
    r = r < 0 ? 0 : r;                   \
    g = g < 0 ? 0 : g;                   \
    b = b < 0 ? 0 : b;                   \
    r = r > 255 ? 255 : r;               \
    g = g > 255 ? 255 : g;               \
    b = b > 255 ? 255 : b

#define CLAMP(x, low, high) (x)<(low) ? (low) : (x)>(high) ? (high) : (x)

string type2str(int type)
{
    string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch (depth)
    {
    case CV_8U:
        r = "8U";
        break;
    case CV_8S:
        r = "8S";
        break;
    case CV_16U:
        r = "16U";
        break;
    case CV_16S:
        r = "16S";
        break;
    case CV_32S:
        r = "32S";
        break;
    case CV_32F:
        r = "32F";
        break;
    case CV_64F:
        r = "64F";
        break;
    default:
        r = "User";
        break;
    }

    r += "C";
    r += (chans + '0');

    return r;
}

void uyvy2rgb(unsigned char *yuv, unsigned char *bgr, int w, int h)
{
    int i, j;
    register int y0, y1, u, v;
    register int r, g, b;
    int NumPixels = w * h;

    for (i = 0, j = 0; i < (NumPixels << 1); i += 4, j += 6)
    {
        u = (unsigned char)yuv[i + 0] - 128;
        y0 = (unsigned char)yuv[i + 1];
        v = (unsigned char)yuv[i + 2] - 128;
        y1 = (unsigned char)yuv[i + 3];
        YUV2RGB(y0, u, v, r, g, b);
        bgr[j + 0] = b;
        bgr[j + 1] = g;
        bgr[j + 2] = r;
        YUV2RGB(y1, u, v, r, g, b);
        bgr[j + 3] = b;
        bgr[j + 4] = g;
        bgr[j + 5] = r;
    }
}

void bgr2uyvy(unsigned char *in_bgr, int w, int h, unsigned char *out_yuv)
{
    unsigned char *ptrBgr, *ptrUyvy;
    unsigned char r1, g1, b1, y1, u1, v1;
    unsigned char r2, g2, b2, y2, u2, v2;
    int i = 0, j = 0, flag = 0;

    memset(out_yuv, 0, w * h * 2);

    ptrUyvy = out_yuv;

    for (j = 0; j < h; j++)
    {
        ptrBgr = in_bgr + w * j * 3;
        for (i = 0; i < w; i += 2)
        {
            b1 = (*ptrBgr++);
            g1 = (*ptrBgr++);
            r1 = (*ptrBgr++);
            b2 = (*ptrBgr++);
            g2 = (*ptrBgr++);
            r2 = (*ptrBgr++);

            y1 = (unsigned char)((66 * r1 + 129 * g1 + 25 * b1 + 128) >> 8) + 16;
            u1 = (unsigned char)((-38 * r1 - 74 * g1 + 112 * b1 + 128) >> 8) + 128;
            v1 = (unsigned char)((112 * r1 - 94 * g1 - 18 * b1 + 128) >> 8) + 128;
            y2 = (unsigned char)((66 * r2 + 129 * g2 + 25 * b2 + 128) >> 8) + 16;
            u2 = (unsigned char)((-38 * r2 - 74 * g2 + 112 * b2 + 128) >> 8) + 128;
            v2 = (unsigned char)((112 * r2 - 94 * g2 - 18 * b2 + 128) >> 8) + 128;

            (*ptrUyvy++) = (u1 + u2) / 2;
            (*ptrUyvy++) = y1;
            (*ptrUyvy++) = (v1 + v2) / 2;
            (*ptrUyvy++) = y2;
        }
    }
}

void resizeBilinearPixels(const unsigned char *src, unsigned char *dest, int sourceWidth, int sourceHeight, int targetWidth, int targetHeight)
{
    float a, b;
    float red, green, blue, alpha;
    float dx, dy;
    float rx, ry;
    int x, y;
    int index0, index1, index2, index3;

    dx = ((float)sourceWidth) / targetWidth;
    dy = ((float)sourceHeight) / targetHeight;
    for (y = 0, ry = 0; y < targetHeight - 1; y++, ry += dy)
    {
        b = ry - (int)ry;
        for (x = 0, rx = 0; x < targetWidth - 1; x++, rx += dx)
        {
            a = rx - (int)rx;
            index0 = (int)ry * sourceWidth + (int)rx;
            index1 = index0 + 1;
            index2 = index0 + sourceWidth;
            index3 = index0 + sourceWidth + 1;

            red = src[index0 * 3] * (1.0f - a) * (1.0f - b);
            green = src[index0 * 3 + 1] * (1.0f - a) * (1.0f - b);
            blue = src[index0 * 3 + 2] * (1.0f - a) * (1.0f - b);

            red += src[index1 * 3] * (a) * (1.0f - b);
            green += src[index1 * 3 + 1] * (a) * (1.0f - b);
            blue += src[index1 * 3 + 2] * (a) * (1.0f - b);

            red += src[index2 * 3] * (1.0f - a) * (b);
            green += src[index2 * 3 + 1] * (1.0f - a) * (b);
            blue += src[index2 * 3 + 2] * (1.0f - a) * (b);

            red += src[index3 * 3] * (a) * (b);
            green += src[index3 * 3 + 1] * (a) * (b);
            blue += src[index3 * 3 + 2] * (a) * (b);

            red = CLAMP(red, 0, 255);
            green = CLAMP(green, 0, 255);
            blue = CLAMP(blue, 0, 255);

            dest[(y * targetWidth + x) * 3] = (unsigned char)red;
            dest[(y * targetWidth + x) * 3 + 1] = (unsigned char)green;
            dest[(y * targetWidth + x) * 3 + 2] = (unsigned char)blue;
        }
        index0 = (int)ry * targetWidth + (int)rx;
        index1 = index0;
        index2 = index0 + sourceWidth;
        index3 = index0 + sourceWidth;

        red = src[index0 * 3] * (1.0f - a) * (1.0f - b);
        green = src[index0 * 3 + 1] * (1.0f - a) * (1.0f - b);
        blue = src[index0 * 3 + 2] * (1.0f - a) * (1.0f - b);

        red += src[index1 * 3] * (a) * (1.0f - b);
        green += src[index1 * 3 + 1] * (a) * (1.0f - b);
        blue += src[index1 * 3 + 2] * (a) * (1.0f - b);

        red += src[index2 * 3] * (1.0f - a) * (b);
        green += src[index2 * 3 + 1] * (1.0f - a) * (b);
        blue += src[index2 * 3 + 2] * (1.0f - a) * (b);

        red += src[index3 * 3] * (a) * (b);
        green += src[index3 * 3 + 1] * (a) * (b);
        blue += src[index3 * 3 + 2] * (a) * (b);

        red = CLAMP(red, 0, 255);
        green = CLAMP(green, 0, 255);
        blue = CLAMP(blue, 0, 255);

        dest[(y * targetWidth + x) * 3] = (unsigned char)red;
        dest[(y * targetWidth + x) * 3 + 1] = (unsigned char)green;
        dest[(y * targetWidth + x) * 3 + 2] = (unsigned char)blue;
    }
    index0 = (int)ry * sourceWidth + (int)rx;
    index1 = index0;
    index2 = index0 + sourceWidth;
    index3 = index0 + sourceWidth;

    for (x = 0, rx = 0; x < targetWidth - 1; x++, rx += dx)
    {
        a = rx - (int)rx;
        index0 = (int)ry * sourceWidth + (int)rx;
        index1 = index0 + 1;
        index2 = index0;
        index3 = index0;

        red = src[index0 * 3] * (1.0f - a) * (1.0f - b);
        green = src[index0 * 3 + 1] * (1.0f - a) * (1.0f - b);
        blue = src[index0 * 3 + 2] * (1.0f - a) * (1.0f - b);

        red += src[index1 * 3] * (a) * (1.0f - b);
        green += src[index1 * 3 + 1] * (a) * (1.0f - b);
        blue += src[index1 * 3 + 2] * (a) * (1.0f - b);

        red += src[index2 * 3] * (1.0f - a) * (b);
        green += src[index2 * 3 + 1] * (1.0f - a) * (b);
        blue += src[index2 * 3 + 2] * (1.0f - a) * (b);

        red += src[index3 * 3] * (a) * (b);
        green += src[index3 * 3 + 1] * (a) * (b);
        blue += src[index3 * 3 + 2] * (a) * (b);

        red = CLAMP(red, 0, 255);
        green = CLAMP(green, 0, 255);
        blue = CLAMP(blue, 0, 255);

        dest[(y * targetWidth + x) * 3] = (unsigned char)red;
        dest[(y * targetWidth + x) * 3 + 1] = (unsigned char)green;
        dest[(y * targetWidth + x) * 3 + 2] = (unsigned char)blue;
    }

    dest[(y * targetWidth + x) * 3] = src[((sourceHeight - 1) * sourceWidth + sourceWidth - 1) * 3];
    dest[(y * targetWidth + x) * 3 + 1] = src[((sourceHeight - 1) * sourceWidth + sourceWidth - 1) * 3 + 1];
    dest[(y * targetWidth + x) * 3 + 2] = src[((sourceHeight - 1) * sourceWidth + sourceWidth - 1) * 3 + 2];
}

void resizePixels(const unsigned char *in, unsigned char *out, int src_w, int src_h, int dst_w, int dst_h)
{
    int x_ratio = (int)((src_w << 16) / dst_w);
    int y_ratio = (int)((src_h << 16) / dst_h);
    int x2, y2;
    for (int i = 0; i < dst_h; i++)
    {
        for (int j = 0; j < dst_w; j++)
        {
            x2 = ((j * x_ratio) >> 16);
            y2 = ((i * y_ratio) >> 16);
            out[((i * dst_w) + j) * 3] = in[((y2 * src_w) + x2) * 3];
            out[((i * dst_w) + j) * 3 + 1] = in[((y2 * src_w) + x2) * 3 + 1];
            out[((i * dst_w) + j) * 3 + 2] = in[((y2 * src_w) + x2) * 3 + 2];
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        cout << "Expecting a image file to be passed to program" << endl;
        return -1;
    }

    //Load original image
    cv::Mat img = cv::imread(argv[1]);
    cv::namedWindow("original", cv::WINDOW_AUTOSIZE);
    cv::imshow("original", img);
    cout << "cols:" << img.cols << " rows:" << img.rows << " ch:" << img.channels() << " type:" << type2str(img.type()) << endl;

    //Create a buffer for the uyvy
    unsigned char *puyvy = (unsigned char *)malloc(img.cols * img.rows * 2);
    bgr2uyvy(img.data, img.cols, img.rows, puyvy);

    //Write uyvy to file. You can view it at https://rawpixels.net/
    FILE *pFile;
    pFile = fopen("file.uyvy", "wb");
    fwrite(puyvy, sizeof(char), img.cols * img.rows * 2, pFile);
    fclose(pFile);

    cv::Mat bgr(img.rows, img.cols, CV_8UC3);
    uyvy2rgb(puyvy, bgr.data, img.cols, img.rows);
    cv::namedWindow("uyvy2rgb", cv::WINDOW_AUTOSIZE);
    cv::imshow("uyvy2rgb", bgr);

    //Resize the original image
    int resize_w = img.cols / 2;
    int resize_h = img.rows / 2;
    cv::Mat imgResizePixels(resize_h, resize_w, CV_8UC3);
    resizePixels(img.data, imgResizePixels.data, img.cols, img.rows, imgResizePixels.cols, imgResizePixels.rows);
    cv::namedWindow("imgResizePixels", cv::WINDOW_AUTOSIZE);
    cv::imshow("imgResizePixels", imgResizePixels);

    //Resize the original image bilinear
    cv::Mat imgResizeBilinearPixels(resize_h, resize_w, CV_8UC3);
    resizeBilinearPixels(img.data, imgResizeBilinearPixels.data, img.cols, img.rows, imgResizeBilinearPixels.cols, imgResizeBilinearPixels.rows);
    cv::namedWindow("imgResizeBilinearPixels", cv::WINDOW_AUTOSIZE);
    cv::imshow("imgResizeBilinearPixels", imgResizeBilinearPixels);

    if (img.empty())
    {
        cout << "Not a valid image file" << endl;
        return -1;
    }

    cv::waitKey(0);
    cv::destroyAllWindows();

    return 0;
}
