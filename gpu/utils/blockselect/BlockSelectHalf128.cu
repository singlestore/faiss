
/**
 * Copyright (c) 2015-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the CC-by-NC license found in the
 * LICENSE file in the root directory of this source tree. An additional grant
 * of patent rights can be found in the PATENTS file in the same directory.
 */

// Copyright 2004-present Facebook. All Rights Reserved.
#include "BlockSelectImpl.cuh"

namespace faiss { namespace gpu {

#ifdef FAISS_USE_FLOAT16
BLOCK_SELECT_IMPL(half, true, 128, 3);
BLOCK_SELECT_IMPL(half, false, 128, 3);
#endif

} } // namespace
