#include <cmath>

#include "GaussFilter.h"

GaussFilter::GaussFilter(sc_module_name n) : sc_module(n) {
  SC_THREAD(do_filter);
  sensitive << i_clk.pos();
  dont_initialize();
  reset_signal_is(i_rst, false);
}

// gaussian mask
const int mask[MASK_N][MASK_X][MASK_Y] = {{{1, 2, 1},
                                          {2, 4, 2},
                                          {1, 2, 1}}};
const int factor = 16;
const int image_size = 256;

unsigned char red[3][image_size + 2];
unsigned char green[3][image_size + 2];
unsigned char blue[3][image_size + 2];
int row_sent = 0;

void GaussFilter::do_filter() {
  while (true) {
    row_sent++;
    for (int i = 0; i < (image_size + 2); i++){
      red[(row_sent-1)%3][i] = i_r.read();
      green[(row_sent-1)%3][i] = i_g.read();
      blue[(row_sent-1)%3][i] = i_b.read();
    }
    if (row_sent < 3){
      wait(10);
      continue;
    }


    // compute full row of gaussian blur using previous three rows
    for (int w = 1; w <image_size+1; w++){
      for (unsigned int i = 0; i < MASK_N; ++i) {
        o_red[i] = 0;
        o_green[i] = 0;
        o_blue[i] = 0;
      }
      for (unsigned int v = 0; v < MASK_Y; ++v) {
        for (unsigned int u = 0; u < MASK_X; ++u) {
          for (unsigned int i = 0; i != MASK_N; ++i) {
            o_red[i] += red[(row_sent-(MASK_Y-v))%3][w + (u-1)] * mask[i][u][v] / factor;
            o_green[i] += green[(row_sent-(MASK_Y-v))%3][w + (u-1)] * mask[i][u][v] / factor;
            o_blue[i] += blue[(row_sent-(MASK_Y-v))%3][w + (u-1)] * mask[i][u][v] / factor;
          }
        }
      }
      // printf("%d, %d, %d \n",o_red[0],o_green[0],o_blue[0]);
      o_result_r.write(o_red[0]);
      o_result_g.write(o_green[0]);
      o_result_b.write(o_blue[0]);
    
    }
    wait(10); //emulate module delay

    
    /*     
    double total = 0;
    for (unsigned int i = 0; i != MASK_N; ++i) {
      total += val[i];
    } 
    int result = (int)(std::sqrt(total));
    */

 
  }
}
