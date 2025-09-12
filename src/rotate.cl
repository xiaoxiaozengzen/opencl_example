// pragma OPENCL EXTENSION cl_amd_printf : enable
kernel void image_rotate(
      global int * src_data,
      global int * dest_data, int W, int H, float sinTheta, float cosTheta )
{
   // get_global_id 用于获取当前线程的全局坐标，决定每个线程处理的数据块。
   const int ix = get_global_id(0);
   const int iy = get_global_id(1);
   int xc = W/2;
   int yc = H/2;
   int xpos =  ( ix-xc)*cosTheta - (iy-yc)*sinTheta+xc;
   int ypos =  (ix-xc)*sinTheta + ( iy-yc)*cosTheta+yc;
   if ((xpos>=0) && (xpos< W)   && (ypos>=0) && (ypos< H))
      dest_data[ypos*W+xpos]= src_data[iy*W+ix];
}