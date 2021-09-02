# BigRoom Challenges You

## #1 Color Conversion
Please implement a color conversion algorithm between RGB and YUV with 422 subsampling (UYVY).   
RGB to YUV and YUV to RGB.  
The algorithm itself should not use any opencv. it should work on raw data buffers. opencv shoud be only used to load, and show results.  

### Recommended steps
- Use opencv to load an image from file or camera
- color convert the bgr cv::Mat to an UYVY cv::Mat (using your designed algorithm) 
- color convert the UYVY cv::Mat back to bgr
- display on screen using cv image show functions 
- it does not matter if it is internally on rgb or bgr, we don't care, if it shows colors properly on cv::imshow its ok.

### How to validate
UYVY is a pretty standard color format using by capture devices and web cameras.    
opencv should have a proper implementation for this YUV to RGB conversion cv::cvtColor(yuv, rgb, cv::COLOR_YUV2RGB_UYVY),   
this can be using to check if the RGB to YUV is correct. Once that is correct you also have the baseline to test YUV to RGB.   

### Nice To Have
- Hardware acceleration on one of the following opencl, opengl compute or metal.


## #2 Image resizing
Please implement an image RGB resizing algorithm using nearest neighbor.
If should support resizing to bigger or smaller resolutions that the original.
The algorithm itself should not use any opencv. it should work on raw data buffers. opencv shoud be only used to load, and show results.  

### Recommended steps
- Use opencv to load an image from file or camera
- resize the image to a different size like (640 480) or something that preserves aspect ration
- display on screen using cv image show functions 

### How to validate
Visual inspection

### Nice To Have
- Hardware acceleration on one of the following opencl, opengl compute or metal.
- Bilinear interpolation


### How to accept the challenge?   
1. checkout this repo
2. do your thing
3. make a pull request to the repo in the end

### Rules
1. C++ Only
2. Use windows or mac
3. Don't use external libraries other than opencv
4. Make sure we can build your code
5. That is it.
