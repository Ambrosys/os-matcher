#pragma once

#include <optional>
#include <unordered_set>

namespace AppComponents::Common::Filter::Routing::Generic {

/**
 * Find previous sampling point (starting at \p samplingPoint) that was not skipped.
 */
std::optional<size_t> findPreviousAllowed(size_t samplingPoint, size_t min, std::unordered_set<size_t> const & skippedSamplingPoints);

/**
 * Find next sampling point (starting at \p samplingPoint) samplingPoint that was not skipped.
 */
std::optional<size_t> findNextAllowed(size_t samplingPoint, size_t max, std::unordered_set<size_t> const & skippedSamplingPoints);

}  // namespace AppComponents::Common::Filter::Routing::Generic
