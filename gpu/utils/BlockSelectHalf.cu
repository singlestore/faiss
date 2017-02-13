
/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the CC-by-NC license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

// Copyright 2004-present Facebook. All Rights Reserved.
#include "blockselect/BlockSelectImpl.cuh"

namespace faiss { namespace gpu {

#ifdef FAISS_USE_FLOAT16

// warp Q to thread Q:
// 1, 1
// 32, 2
// 64, 3
// 128, 3
// 256, 4
// 512, 8
// 1024, 8

BLOCK_SELECT_DECL(half, true, 1);
BLOCK_SELECT_DECL(half, true, 32);
BLOCK_SELECT_DECL(half, true, 64);
BLOCK_SELECT_DECL(half, true, 128);
BLOCK_SELECT_DECL(half, true, 256);
BLOCK_SELECT_DECL(half, true, 512);
BLOCK_SELECT_DECL(half, true, 1024);

BLOCK_SELECT_DECL(half, false, 1);
BLOCK_SELECT_DECL(half, false, 32);
BLOCK_SELECT_DECL(half, false, 64);
BLOCK_SELECT_DECL(half, false, 128);
BLOCK_SELECT_DECL(half, false, 256);
BLOCK_SELECT_DECL(half, false, 512);
BLOCK_SELECT_DECL(half, false, 1024);

void runBlockSelect(Tensor<half, 2, true>& in,
                  Tensor<half, 2, true>& outK,
                  Tensor<int, 2, true>& outV,
                  bool dir, int k, cudaStream_t stream) {
  FAISS_ASSERT(k <= 1024);

  if (dir) {
    if (k == 1) {
      BLOCK_SELECT_CALL(half, true, 1);
    } else if (k <= 32) {
      BLOCK_SELECT_CALL(half, true, 32);
    } else if (k <= 64) {
      BLOCK_SELECT_CALL(half, true, 64);
    } else if (k <= 128) {
      BLOCK_SELECT_CALL(half, true, 128);
    } else if (k <= 256) {
      BLOCK_SELECT_CALL(half, true, 256);
    } else if (k <= 512) {
      BLOCK_SELECT_CALL(half, true, 512);
    } else if (k <= 1024) {
      BLOCK_SELECT_CALL(half, true, 1024);
    }
  } else {
    if (k == 1) {
      BLOCK_SELECT_CALL(half, false, 1);
    } else if (k <= 32) {
      BLOCK_SELECT_CALL(half, false, 32);
    } else if (k <= 64) {
      BLOCK_SELECT_CALL(half, false, 64);
    } else if (k <= 128) {
      BLOCK_SELECT_CALL(half, false, 128);
    } else if (k <= 256) {
      BLOCK_SELECT_CALL(half, false, 256);
    } else if (k <= 512) {
      BLOCK_SELECT_CALL(half, false, 512);
    } else if (k <= 1024) {
      BLOCK_SELECT_CALL(half, false, 1024);
    }
  }
}

#endif

} } // namespace
