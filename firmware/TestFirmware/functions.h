
byte mmNormalization(byte range_l, byte range_r, byte val, byte max_val, byte min_val) {
  return range_l + (val - min_val) * (range_r - range_l) / (max_val - min_val);
}
