#include "pe_bliss2/resources/bitmap.h"

#include <cassert>

#include "buffers/buffer_copy.h"
#include "buffers/output_buffer_interface.h"

namespace pe_bliss::resources
{

void bitmap::serialize(buffers::output_buffer_interface& output,
	bool write_virtual_part)
{
	assert(!!buf_);

	fh_.serialize(output, write_virtual_part);
	ih_.serialize(output, write_virtual_part);
	buffers::copy(*buf_, output, write_virtual_part
		? buf_->size() : buf_->physical_size());
}

} //namespace pe_bliss::resources
