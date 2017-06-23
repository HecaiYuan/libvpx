/*
 *  Copyright (c) 2017 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <arm_neon.h>

#include "./vpx_dsp_rtcd.h"
#include "./vpx_config.h"
#include "vpx_dsp/arm/mem_neon.h"

static INLINE tran_low_t sum_int16x8(const int16x8_t a) {
  const int32x4_t b = vpaddlq_s16(a);
  const int64x2_t c = vpaddlq_s32(b);
  const int32x2_t d = vadd_s32(vreinterpret_s32_s64(vget_low_s64(c)),
                               vreinterpret_s32_s64(vget_high_s64(c)));
#if CONFIG_VP9_HIGHBITDEPTH
  return vget_lane_s32(d, 0);
#else
  return vget_lane_s16(vreinterpret_s16_s32(d), 0);
#endif
}

void vpx_fdct4x4_1_neon(const int16_t *input, tran_low_t *output, int stride) {
  int16x4_t a0, a1, a2, a3;
  int16x8_t b0, b1;
  int16x8_t c;

  a0 = vld1_s16(input);
  input += stride;
  a1 = vld1_s16(input);
  input += stride;
  a2 = vld1_s16(input);
  input += stride;
  a3 = vld1_s16(input);

  b0 = vcombine_s16(a0, a1);
  b1 = vcombine_s16(a2, a3);

  c = vaddq_s16(b0, b1);

  output[0] = sum_int16x8(c) << 1;
  output[1] = 0;
}

void vpx_fdct8x8_1_neon(const int16_t *input, tran_low_t *output, int stride) {
  int r;
  int16x8_t sum = vld1q_s16(&input[0]);
  for (r = 1; r < 8; ++r) {
    const int16x8_t input_00 = vld1q_s16(&input[r * stride]);
    sum = vaddq_s16(sum, input_00);
  }

  output[0] = sum_int16x8(sum);
  output[1] = 0;
}
