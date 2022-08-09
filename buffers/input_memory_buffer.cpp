#include "buffers/input_memory_buffer.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <system_error>

#include "utilities/generic_error.h"
#include "utilities/math.h"

namespace buffers
{
input_memory_buffer::input_memory_buffer(const std::byte* memory,
	std::size_t size) noexcept
	: memory_(memory)
	, size_(size)
{
	assert(!size_ || memory_);
}

std::size_t input_memory_buffer::size()
{
	return size_;
}

std::size_t input_memory_buffer::read(std::size_t pos,
	std::size_t count, std::byte* data)
{
	if (!count)
		return 0u;

	if (!utilities::math::is_sum_safe(pos, count) || pos + count > size_)
		throw std::system_error(utilities::generic_errc::buffer_overrun);

	std::memcpy(data, memory_ + pos, count);
	return count;
}
} //namespace buffers
