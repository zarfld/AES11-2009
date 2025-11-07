// Synchronization Manager - DES-C-002
// Provides source selection with hysteresis and hooks for degradation/holdover.
// Hardware-agnostic; operates on abstract source metric inputs.

#ifndef AES_AES11_2009_SYNC_SYNCHRONIZATION_MANAGER_HPP
#define AES_AES11_2009_SYNC_SYNCHRONIZATION_MANAGER_HPP

#include <vector>
#include <cstddef>
#include <limits>

namespace AES {
namespace AES11 {
namespace _2009 {
namespace sync {

struct SourceMetrics {
    double stability;   // lower is better (e.g., variance)
    double quality;     // higher is better (composite score)
    bool   degraded;    // flag indicating current degradation
};

class SynchronizationManager {
public:
    explicit SynchronizationManager(double hysteresisMargin)
        : _hysteresisMargin(hysteresisMargin), _currentIndex(std::numeric_limits<size_t>::max()) {}

    // Select best source based on quality while applying hysteresis to avoid rapid churn.
    // Returns index of selected source.
    size_t select(const std::vector<SourceMetrics>& sources) {
        if (sources.empty()) return invalidIndex();

        size_t best = 0;
        double bestScore = score(sources[0]);
        for (size_t i = 1; i < sources.size(); ++i) {
            double sc = score(sources[i]);
            if (sc > bestScore) {
                bestScore = sc;
                best = i;
            }
        }

        if (_currentIndex == invalidIndex()) {
            _currentIndex = best;
            return _currentIndex;
        }

        // Hysteresis: retain current if its score is within margin of best
        double currentScore = score(sources[_currentIndex]);
        if (currentScore + _hysteresisMargin >= bestScore) {
            return _currentIndex; // hold current
        }

        _currentIndex = best;
        return _currentIndex;
    }

    size_t current() const { return _currentIndex; }

    static size_t invalidIndex() { return std::numeric_limits<size_t>::max(); }

private:
    double score(const SourceMetrics& m) const {
        // Basic composite: prioritize non-degraded, then quality, inverse stability.
        double base = m.quality - m.stability; // higher quality, lower stability value = better
        if (m.degraded) base -= 10.0; // penalty; future refinement may scale
        return base;
    }

    double _hysteresisMargin;
    size_t _currentIndex;
};

} // namespace sync
} // namespace _2009
} // namespace AES11
} // namespace AES

#endif // AES_AES11_2009_SYNC_SYNCHRONIZATION_MANAGER_HPP
