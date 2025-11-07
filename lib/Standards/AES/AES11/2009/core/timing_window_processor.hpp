// Timing Window Processor - DES-C-003
// Implements windowed timing statistics (mean, variance, count, stability flag)
// using Welford's online algorithm; hardware-agnostic and testable.

#ifndef AES_AES11_2009_CORE_TIMING_WINDOW_PROCESSOR_HPP
#define AES_AES11_2009_CORE_TIMING_WINDOW_PROCESSOR_HPP

#include <cstddef>
#include <cstdint>
#include <vector>
#include <cmath>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace core {

class TimingWindowProcessor {
public:
    struct Metrics {
        size_t count;
        double mean;
        double variance; // population variance
        bool stable;     // variance below threshold
    };

    explicit TimingWindowProcessor(size_t capacity, double varianceThreshold)
        : _capacity(capacity), _varianceThreshold(varianceThreshold) {
        _samples.reserve(capacity);
        resetAggregates();
    }

    void addSample(double value) {
        if (_samples.size() == _capacity) {
            // Remove oldest -> adjust aggregates (recompute for simplicity first iteration)
            // For performance later consider a ring buffer with incremental removal math.
            _samples.erase(_samples.begin());
        }
        _samples.push_back(value);
        recomputeAggregates();
    }

    Metrics metrics() const { return {_samples.size(), _mean, _variance, _variance < _varianceThreshold}; }

    void clear() {
        _samples.clear();
        resetAggregates();
    }

private:
    void resetAggregates() {
        _mean = 0.0;
        _variance = 0.0;
    }

    void recomputeAggregates() {
        // Welford pass (recompute to keep logic simple & correct initially)
        double mean = 0.0;
        double M2 = 0.0;
        size_t n = 0;
        for (double x : _samples) {
            n++;
            double delta = x - mean;
            mean += delta / n;
            double delta2 = x - mean;
            M2 += delta * delta2;
        }
        _mean = (n > 0) ? mean : 0.0;
        _variance = (n > 0) ? (M2 / n) : 0.0; // population variance
    }

    size_t _capacity;
    double _varianceThreshold;
    std::vector<double> _samples;
    double _mean;
    double _variance;
};

} // namespace core
} // namespace _2009
} // namespace AES11
} // namespace AES

#endif // AES_AES11_2009_CORE_TIMING_WINDOW_PROCESSOR_HPP
