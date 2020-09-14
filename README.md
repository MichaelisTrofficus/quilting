# Image Quilting for Texture Synthesis

This is my own implementation of the **quilting algorithm** in C++ programming language using the OpenCV library.  The algorithm is pretty simple, but it produces good results. In the following image (which comes from the original paper), you can see, in summary, how quilting works.

![enter image description here](https://lh3.googleusercontent.com/proxy/PvirojwkVAMb8uVbkWS9ce66kx5pyAlTBqmEPvNFz5QXCuzMqsslkNC2vnxXl7AZ5egnDzIinMFPv4prj-H6vHOWmvG5qr7rWbMg4q21QUwYMucNlqC1FYkJWgJS9Cw8keQLLRAvFFTfRpMjVZM7ADzmIxqNzmMqjreGQh2Ls1TLSw)

In a), we can see the naïve approach, that is, a **random placement of blocks** coming from the input texture.

 In b), we place neighboring blocks constrained by overlap (we choose those blocks which have the **minimum quadratic error in the region of overlap**).

 In c), we apply a **mincut algorithm**, to obtain a minimum error cut in the region of overlap between neighboring blocks. **Properly, this is the quilting algorithm**.

You can check the details of the implementation (I did it for educational purposes, so don't expect highest efficiency) in the source files. To conclude, I will show some interesting results.

### x2 synthesis scale

![](https://github.com/MichaelisTrofficus/quilting/blob/master/input/im2.png)  ![](https://github.com/MichaelisTrofficus/quilting/blob/master/output/im2.jpg)

![](https://github.com/MichaelisTrofficus/quilting/blob/master/input/im3.png)  ![](https://github.com/MichaelisTrofficus/quilting/blob/master/output/im3.jpg)

![](https://github.com/MichaelisTrofficus/quilting/blob/master/input/im4.png)  ![](https://github.com/MichaelisTrofficus/quilting/blob/master/output/im4.jpg)

![](https://github.com/MichaelisTrofficus/quilting/blob/master/input/im5.png)  ![](https://github.com/MichaelisTrofficus/quilting/blob/master/output/im5.jpg)

![](https://github.com/MichaelisTrofficus/quilting/blob/master/input/im6.png)  ![](https://github.com/MichaelisTrofficus/quilting/blob/master/output/im6.jpg)


### x4 synthesis scale

![](https://github.com/MichaelisTrofficus/quilting/blob/master/input/im7.png)  ![](https://github.com/MichaelisTrofficus/quilting/blob/master/output/im7.jpg)

![](https://github.com/MichaelisTrofficus/quilting/blob/master/input/im8.png)  ![](https://github.com/MichaelisTrofficus/quilting/blob/master/output/im8.jpg)

![](https://github.com/MichaelisTrofficus/quilting/blob/master/input/im9.png)  ![](https://github.com/MichaelisTrofficus/quilting/blob/master/output/im9.jpg)

![](https://github.com/MichaelisTrofficus/quilting/blob/master/input/im10.png)  ![](https://github.com/MichaelisTrofficus/quilting/blob/master/output/im10.jpg)


