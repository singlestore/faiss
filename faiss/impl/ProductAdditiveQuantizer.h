/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <cstdint>
#include <vector>

#include <faiss/Index.h>
#include <faiss/impl/AdditiveQuantizer.h>
#include <faiss/impl/LocalSearchQuantizer.h>
#include <faiss/impl/ResidualQuantizer.h>

namespace faiss {

/** Product Additive Quantizers
 *
 * The product additive quantizer is a variant of AQ and PQ.
 *
 * 1. It first splits the vector space into multiple orthogonal sub-spaces just
 * like PQ does.
 * 2. And then it quantizes each sub-space by an independent additive quantizer.
 *
 */
struct ProductAdditiveQuantizer : AdditiveQuantizer {
    size_t nsplits; ///< number of sub-vectors we split a vector into

    std::vector<AdditiveQuantizer*> quantizers;

    /** Construct a product additive quantizer.
     *
     * The ownership of the additive quantizers passed in is not
     * transferred to the ProductAdditiveQuantizer object. Users
     * are responsible to take care of it.
     *
     * @param d      dimensionality of the input vectors
     * @param aqs    sub-additive quantizers
     * @param search_type  AQ search type
     */
    ProductAdditiveQuantizer(
            size_t d,
            const std::vector<AdditiveQuantizer*>& aqs,
            Search_type_t search_type = ST_decompress);

    ProductAdditiveQuantizer();

    void init(
            size_t d,
            const std::vector<AdditiveQuantizer*>& aqs,
            Search_type_t search_type);

    ///< Train the product additive quantizer
    void train(size_t n, const float* x) override;

    /** Encode a set of vectors
     *
     * @param x      vectors to encode, size n * d
     * @param codes  output codes, size n * code_size
     * @param centroids  centroids to be added to x, size n * d
     */
    void compute_codes(
            const float* x,
            uint8_t* codes,
            size_t n,
            const float* centroids = nullptr) const override;

    /** Decode a set of vectors in non-packed format
     *
     * @param codes  codes to decode, size n * ld_codes
     * @param x      output vectors, size n * d
     */
    void decode_unpacked(
            const int32_t* codes,
            float* x,
            size_t n,
            int64_t ld_codes = -1) const override;

    /** Decode a set of vectors
     *
     * @param codes  codes to decode, size n * code_size
     * @param x      output vectors, size n * d
     */
    void decode(const uint8_t* codes, float* x, size_t n) const override;

    /** Compute inner-product look-up tables. Used in the search functions.
     *
     * @param xq     query vector, size (n, d)
     * @param LUT    look-up table, size (n, total_codebook_size)
     * @param alpha  compute alpha * inner-product
     * @param ld_lut  leading dimension of LUT
     */
    void compute_LUT(
            size_t n,
            const float* xq,
            float* LUT,
            float alpha = 1.0f,
            long ld_lut = -1) const override;
};

/** Product Local Search Quantizer
 */
struct ProductLocalSearchQuantizer : ProductAdditiveQuantizer {
    std::vector<LocalSearchQuantizer> lsqs;

    /** Construct a product LSQ object.
     *
     * @param d   imensionality of the input vectors
     * @param nsplits  number of sub-vectors we split a vector into
     * @param Msub     number of codebooks of each LSQ
     * @param nbits    bits for each step
     * @param search_type  AQ search type
     */
    ProductLocalSearchQuantizer(
            size_t d,
            size_t nsplits,
            size_t Msub,
            size_t nbits,
            Search_type_t search_type = ST_decompress);

    ProductLocalSearchQuantizer();

    LocalSearchQuantizer* subquantizer(size_t m);
};

/** Product Residual Quantizer
 */
struct ProductResidualQuantizer : ProductAdditiveQuantizer {
    std::vector<ResidualQuantizer> rqs;

    /** Construct a product RQ object.
     *
     * @param d   imensionality of the input vectors
     * @param nsplits  number of sub-vectors we split a vector into
     * @param Msub     number of codebooks of each RQ
     * @param nbits    bits for each step
     * @param search_type  AQ search type
     */
    ProductResidualQuantizer(
            size_t d,
            size_t nsplits,
            size_t Msub,
            size_t nbits,
            Search_type_t search_type = ST_decompress);

    ProductResidualQuantizer();

    ResidualQuantizer* subquantizer(size_t s);
};

}; // namespace faiss