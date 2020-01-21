// RealTimeSpeechRecognition.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <deque>
#include <future>
#include <iostream>
#include <iterator>
#include <mutex>
#include <numeric>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include "fourier.h"

using namespace std;

// Structure of a WAV
struct ac44 {
    uint32_t riff_id{};
    uint32_t riff_size{};
    uint32_t wave_tag{};
    uint32_t format_id{};
    uint32_t format_size{};
    uint16_t format_tag{};
    uint16_t channels{};
    uint32_t sample_rate{};
    uint32_t bytes_per_second{};
    uint16_t block_align{};
    uint16_t bit_depth{};
    uint32_t data_id{};
    uint32_t data_size{};
};

// Read WAV header from a stream and return sample rate
ac44 get_meta(istream& in)
{
    cout << "in: " << &in << endl;
    cout << "Get_Meta_1" << endl;
    ac44 meta;
    assert(sizeof(meta) == 44);

    cout << "meta: " << &meta << endl;
    cout << "Get_Meta_2" << endl;

    // Check file is good and read the meta data
    if (in.good()) {
        cout << "Get_Meta_if_1" << endl;
        char* temp = reinterpret_cast<char*>(&meta);
        cout << "temp: " << &temp << "\ttemp size: " << sizeof(temp) << "\tmeta size: " << sizeof(meta) << endl;
        in.read(reinterpret_cast<char*>(&meta), sizeof(meta));
        cout << "Get_Meta_if_2" << endl;
    }

    cout << "Get_Meta_3" << endl;

    return meta;
}

int main(int argc, char** argv) {

    // Define source of audio
    istream& in = cin;

    cout << "MAIN_1" << endl;

    cout << "in: " << &in << endl;
    // Get the meta data
    const auto& sample_rate = get_meta(in).sample_rate;

    cout << "MAIN_2" << endl;

    // Create a large block of samples
    const size_t duration{ 30 };

    cout << "MAIN_3" << endl;

    vector<int16_t> samples(sample_rate * duration);

    cout << "MAIN_4" << endl;

    // Split into blocks
    const size_t blocks_per_second{ 5 };

    cout << "MAIN_5" << endl;

    const size_t block_size{ sample_rate / blocks_per_second };

    cout << "MAIN_6" << endl;

    atomic<int16_t> latest;
    mutex quick, slow;
    atomic<bool> shutdown;

    cout << "MAIN_7" << endl;

    const auto quick_loop = [&]()
    {
        while (!shutdown.load())
        {
            // Wait until some samples are available
            quick.lock();

            // Prepare to copy the latest block
            const auto i = latest.load();
            const size_t start = i * block_size;
            const size_t end = (i + 1) * block_size;
            const vector<int16_t> s(&samples[start], &samples[end]);

            // quick.unlock();

            // Do Fourier analysis
            const auto f = get_fourier(s);

            // Calculate frequency of max bin
            const auto peak_bin_it = max_element(f.cbegin(), f.cend());
            const size_t bin = distance(f.cbegin(), peak_bin_it);

            // Size of Fourier response
            const auto bins = f.size();

            const double freq = 1.0 * sample_rate * bin / bins;

            // if (freq > 0)
            cout << bin << "\t" << freq << "\n";
        }
    };

    const auto slow_loop = [&]()
    {
        while (!shutdown.load())
        {
            // Wait until some samples are available
            slow.lock();

            // Prepare to copy the latest block
            const auto i = latest.load();
            const size_t start = i * block_size;
            const size_t end = (i + 1) * block_size;
            const vector<int16_t> s(&samples[start], &samples[end]);

            // Do Fourier analysis
            const auto f = get_fourier(s);

            // Calculate frequency of max bin
            // const auto peak_bin = distance(f.cbegin(), max_element(f.cbegin(), f.cend()));

            // // Size of Fourier response
            // const auto bins = f.size();

            // cout << sample_rate * peak_bin / bins << " slow\n";
        }
    };

    // Get ready to release thread
    shutdown.store(false);
    slow.lock();
    quick.lock();

    thread t1(quick_loop);
    thread t2(slow_loop);

    for (size_t i = 0; i < samples.size() / block_size; ++i) {
        // Read a block of samples
        cin.read(reinterpret_cast<char*>(&samples[i * block_size]), block_size * sizeof(int16_t));

        // Store current index and release the processing
        latest.store(i);
        quick.unlock();

        if (!((i + 1) % blocks_per_second))
            slow.unlock();
    }

    // Wait for threads to return
    shutdown.store(true);
    t1.join();
    t2.join();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
