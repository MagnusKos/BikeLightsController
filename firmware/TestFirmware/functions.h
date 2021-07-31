#pragma once

byte mmNormalization(byte range_l, byte range_r, byte val, byte max_val, byte min_val) { //min-max normalization function
  return range_l + (val - min_val) * (range_r - range_l) / (max_val - min_val);
}

void zeroArray(byte* arr, byte arr_size){  //function for zeroing of the uint8_t array
  for (int i=0; i < arr_size; i++)
    *(arr + i) = 0;
}
