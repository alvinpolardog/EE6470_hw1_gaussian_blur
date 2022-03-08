# EE6470 HW1 Report
吳哲廷 學號:110061590

Link to [Github Repo](https://github.com/alvinpolardog/EE6470_hw1_gaussian_blur)
##

### Part 1: Gaussian Blur with FIFO channels
The first part of HW1 requires altering the provided Sobel Filter code to instead apply a Gaussian blur to the image.
Compared to regular blurring, gaussian blur is smoother since it uses a mask that exponentially decreases as it is further from the center.
In my code, I used the 3x3 mask:
 ```
    1, 2, 1
    2, 4, 2
    1, 2, 1
```
and a multiplier factor of 1/16 to avoid brightness oversaturation.

Compared to the original code for Sobel Filter, the number of mask needed was reduced from 2 to 1,
but instead of applying the mask onto only the gray channel, that is, the average of the three color channels,
Gaussian filter needs to be applied to each of the red, green, and blue channels respectively.
This means that the original single FIFO thread for returning the resulting value was no longer enough, and it was
expanded to three individual channel that each transmit one color back to the testbench module.

The rest of the module was mostly untouched, since the 'convolution' for both types of filter is relatively identical.

##  
##  
  
### Part 2: Data movement with row buffers
While the original code can implement Gaussian blur with no issue, it is considerably wasteful in its data transmission.
In the second part of HW1, instead of transmitting the necessary 9 pixels for each pixel in the resulting image,
the new version of the thread send one row of the original image at a time. when enough pixels has been sent,
one full row of the filtered image is generated and sent back.

In this implementation, the testbench first send three rows of data and begin to wait for the return results.
After being notified via an event, the results are read and written onto its respective spots on the target bitmap.
On the side of the Gaussian filter module, each new row of data is saved onto an unsigned char array, and after three rows,
the oldest of which is updated with the newly received row. When there is at least three row of data saved, one full row
of the filtered image is calculated using the method in part 1, and returned to the testbench module.

Unlike the original thread, pixels that are reused is kept in a buffer instead of being immediately discarded, thus allowing
a much lower amount of data transfer:

Original Thread:
`9 (pixels per computation) * 256 * 256 (image size) = 589824`

New Thread:
`258 * 258 (image size + mask padding) = 66564`

Which is a data transfer reduction of  `(589824 - 66564) / 589824` ≈ 88% !


Original Image:

![original image](https://github.com/alvinpolardog/EE6470_hw1_gaussian_blur/blob/main/lena_std_short.bmp)

Filtered Images:

![filtered image](https://github.com/alvinpolardog/EE6470_hw1_gaussian_blur/blob/main/out.bmp)
