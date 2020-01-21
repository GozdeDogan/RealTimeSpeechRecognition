#pragma once

//#ifndef FOURIER_H
//#define FOURIER_H

#include <cstdint>
#include <vector>

using namespace std;

template <class T> constexpr T bins = 8 * 1024;
vector<double> get_fourier(const vector<int16_t>&);

//#endif /* FOURIER_H */

