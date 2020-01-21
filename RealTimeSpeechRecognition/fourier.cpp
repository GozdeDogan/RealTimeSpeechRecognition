#include "fourier.h"
#include <algorithm>
#include <cassert>
#include <complex>
#include <iterator>

using namespace std;

// Initialse twiddle matrix
vector<complex<double>> fourier_init() {

    // Declare container for twiddle matrix
    vector<complex<double>> twiddle;

    // Only need to calculate first half as the second is a mirror image
    twiddle.reserve(bins<size_t> * bins<size_t> / 2);

    const double pi{ 3.1415926535897932385 };

    // Populate twiddle matrix
    using namespace complex_literals;
    for (size_t k = 0; k < bins<size_t> / 2; ++k)
        for (size_t n = 0; n < bins<size_t>; ++n)
            twiddle.push_back(exp(2 * pi * static_cast<double>(k)* static_cast<double>(n) / bins<double>));

    return twiddle;
}

// Calculate Fourier response for a container of samples
vector<double> get_fourier(const vector<int16_t>& samples) {

    assert(samples.size() >= bins<size_t> &&
        "Fourier bins greater than samples size");

    // Initialise twiddle matrix on first call
    static const auto& twiddle = fourier_init();

    // Initialise results container and reserve enough space for the bins
    vector<double> fourier;
    fourier.reserve(bins<size_t> / 2);

    // Calculate Fourier response
    for (size_t k = 0; k < bins<size_t> / 2; ++k) {

        // We want to calculate the sum of all responses
        complex<double> sum{};

        // Calculate response for each sample
        for (size_t n = 0; n < bins<size_t>; ++n)
            sum += twiddle[(k * bins<size_t>) + n] *
            complex<double>(samples.at(n), 0);

        // Store the absolute value of the complex sum
        fourier.emplace_back(abs(sum));
    }

    return fourier;
}
