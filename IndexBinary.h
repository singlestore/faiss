/**
 * Copyright (c) 2018-present, Facebook, Inc.
 * All rights reserved.
 *
 * This source code is licensed under the BSD+Patents license found in the
 * LICENSE file in the root directory of this source tree.
 */

// Copyright 2004-present Facebook. All Rights Reserved
// -*- c++ -*-

#ifndef FAISS_INDEX_BINARY_H
#define FAISS_INDEX_BINARY_H

#include <cstdio>
#include <typeinfo>
#include <string>
#include <sstream>
#include "MetricType.h"

namespace faiss {


/// Forward declarations see AuxIndexStructures.h
struct IDSelector;
struct RangeSearchResult;

/** Abstract structure for a binary index.
 *
 * Supports adding vertices and searching them.
 *
 * Currently only asymmetric queries are supported:
 * database-to-database queries are not implemented.
 */
struct IndexBinary {
  typedef long idx_t;    ///< all indices are this type

  int d;                 ///< vector dimension
  idx_t ntotal;          ///< total nb of indexed vectors
  bool verbose;          ///< verbosity level

  /// set if the Index does not require training, or if training is done already
  bool is_trained;

  /// type of metric this index uses for search
  MetricType metric_type;

  explicit IndexBinary(idx_t d = 0, MetricType metric = METRIC_L2)
      : d(d),
        ntotal(0),
        verbose(false),
        is_trained(true),
        metric_type(metric) {}

  virtual ~IndexBinary();


  /** Perform training on a representative set of vectors
   *
   * @param n      nb of training vectors
   * @param x      training vecors, size n * d / 8
   */
  virtual void train(idx_t n, const uint8_t *x);

  /** Add n vectors of dimension d to the index.
   *
   * Vectors are implicitly assigned labels ntotal .. ntotal + n - 1
   * This function slices the input vectors in chuncks smaller than
   * blocksize_add and calls add_core.
   * @param x      input matrix, size n * d / 8
   */
  virtual void add(idx_t n, const uint8_t *x) = 0;

  /** Same as add, but stores xids instead of sequential ids.
   *
   * The default implementation fails with an assertion, as it is
   * not supported by all indexes.
   *
   * @param xids if non-null, ids to store for the vectors (size n)
   */
  virtual void add_with_ids(idx_t n, const uint8_t *x, const long *xids);

  /** query n vectors of dimension d to the index.
   *
   * return at most k vectors. If there are not enough results for a
   * query, the result array is padded with -1s.
   *
   * @param x           input vectors to search, size n * d / 8
   * @param labels      output labels of the NNs, size n*k
   * @param distances   output pairwise distances, size n*k
   */
  virtual void search(idx_t n, const uint8_t *x, idx_t k,
                      float *distances, idx_t *labels) const = 0;

  /** query n vectors of dimension d to the index.
   *
   * return all vectors with distance < radius. Note that many
   * indexes do not implement the range_search (only the k-NN search
   * is mandatory).
   *
   * @param x           input vectors to search, size n * d / 8
   * @param radius      search radius
   * @param result      result table
   */
  virtual void range_search(idx_t n, const uint8_t *x, float radius,
                            RangeSearchResult *result) const;

  /** return the indexes of the k vectors closest to the query x.
   *
   * This function is identical as search but only return labels of neighbors.
   * @param x           input vectors to search, size n * d / 8
   * @param labels      output labels of the NNs, size n*k
   */
  void assign(idx_t n, const uint8_t *x, idx_t *labels, idx_t k = 1);

  /// removes all elements from the database.
  virtual void reset() = 0;

  /** removes IDs from the index. Not supported by all indexes
   */
  virtual long remove_ids(const IDSelector& sel);

  /** Reconstruct a stored vector (or an approximation if lossy coding)
   *
   * this function may not be defined for some indexes
   * @param key         id of the vector to reconstruct
   * @param recons      reconstucted vector (size d)
   */
  virtual void reconstruct(idx_t key, uint8_t *recons) const;


  /** Reconstruct vectors i0 to i0 + ni - 1
   *
   * this function may not be defined for some indexes
   * @param recons      reconstucted vector (size ni * d)
   */
  virtual void reconstruct_n(idx_t i0, idx_t ni, uint8_t *recons) const;

  /** Similar to search, but also reconstructs the stored vectors (or an
   * approximation in the case of lossy coding) for the search results.
   *
   * If there are not enough results for a query, the resulting arrays
   * is padded with -1s.
   *
   * @param recons      reconstructed vectors size (n, k, d)
   **/
  virtual void search_and_reconstruct(idx_t n, const uint8_t *x, idx_t k,
                                      float *distances, idx_t *labels,
                                      uint8_t *recons) const;

  /** Computes a residual vector after indexing encoding.
   *
   * The residual vector is the difference between a vector and the
   * reconstruction that can be decoded from its representation in
   * the index. The residual can be used for multiple-stage indexing
   * methods, like IndexIVF's methods.
   *
   * @param x           input vector, size d
   * @param residual    output residual vector, size d
   * @param key         encoded index, as returned by search and assign
   */
  void compute_residual(const uint8_t *x, uint8_t *residual, idx_t key) const;

  /** Display the actual class name and some more info */
  void display() const;
};


}  // namespace faiss

#endif  // FAISS_INDEX_BINARY_H
