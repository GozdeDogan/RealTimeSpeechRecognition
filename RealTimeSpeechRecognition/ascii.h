#pragma once

//#ifndef ASCII_H
//#define ASCII_H

#include <string>
#include <vector>

using namespace std;

using iterator_t = vector<double>::const_iterator;
string dump_histogram(const iterator_t&, const iterator_t&);
string dump_log_histogram(const iterator_t&, const iterator_t&);
string dump_aerial_histogram(const iterator_t&, const iterator_t&);

//#endif /* ASCII_H */

