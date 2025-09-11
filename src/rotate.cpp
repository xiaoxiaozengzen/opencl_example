#include <iostream>
#include <fstream>
#include <sstream>

#include <CL/cl.h>

/**
 * ========== 图像旋转原理 ==========
 * 图像旋转定义为：将图像绕某个点旋转一定的角度。通常是指绕图像的中心点以逆时针方向旋转。
 * 假设图像的左上角为(l, t)，右下角为(r, b)，中心点为(cx, cy)，旋转角度为angle。
 * 对于图像中的每个像素点(x, y)，其旋转后的新坐标(x', y')可以通过以下公式计算得到：
 *   x' = (x - cx) * cos(angle) - (y - cy) * sin(angle) + cx
 *   y' = (x - cx) * sin(angle) + (y - cy) * cos(angle) + cy
 */

/**
 * @brief 图像旋转函数
 * @note OpenCL C Kernel 代码见 rotate.cl
 */
void rotate(
      unsigned char* inbuf,
      unsigned char* outbuf,
      int w, int h,
      float sinTheta,
      float cosTheta)
{
   int i, j;
   int xc = w/2;
   int yc = h/2;
   for(i = 0; i < h; i++)
   {
     for(j=0; j< w; j++)
     {
       int xpos =  (j-xc)*cosTheta - (i - yc) * sinTheta + xc;
       int ypos =  (j-xc)*sinTheta + (i - yc) * cosTheta + yc;
       if(xpos>=0&&ypos>=0&&xpos<w&&ypos<h)
          outbuf[ypos*w + xpos] = inbuf[i*w+j];
     }
   }
}


/**
 * 使用OpenCL进行编程的一般流程：
 *  - Platform
 *    - 1. 查询并选择一个Platform
 *    - 2. 在Platform上创建一个Context
 *    - 3. 在Context上查询并创建一个或多个device
 *  - Running Time
 *    - 4. 加载OpenCL内核程序并创建一个Program对象
 *    - 5. 为指定的device编译Program中的kernel
 *    - 6. 创建指定名字的kernel对象
 *    - 7. 为kernel创建内存对象
 *    - 8. 设置kernel参数
 *    - 9. 在指定的device上创建一个Command Queue
 *    - 10. 将要执行的kernel加入Command Queue
 *    - 11. 读取kernel执行结果，返回给host
 *    - Cleanup
 * 
 */