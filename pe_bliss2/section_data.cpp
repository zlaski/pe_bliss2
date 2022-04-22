#include "pe_bliss2/section_data.h"

#include <cstddef>
#include <memory>

#include "buffers/input_buffer_section.h"
#include "buffers/output_buffer_interface.h"
#include "pe_bliss2/section_header.h"
#include "pe_bliss2/pe_error.h"
#include "utilities/generic_error.h"
#include "utilities/safe_uint.h"

namespace pe_bliss
{

void section_data::deserialize(const section_header& header,
	const buffers::input_buffer_ptr& buffer,
	const section_data_load_options& options)
{
	auto raw_size = header.get_raw_size(options.section_alignment);

	utilities::safe_uint<std::size_t> buffer_pos(options.image_loaded_to_memory
		? header.base_struct()->virtual_address
		: header.get_pointer_to_raw_data());
	buffer_pos += options.image_start_buffer_pos;

	auto buffer_section = std::make_shared<buffers::input_buffer_section>(
		buffer, buffer_pos.value(), raw_size);
	buffer_section->set_relative_offset(0);
	ref_buffer::deserialize(buffer_section, options.copy_memory);
}

} //namespace pe_bliss
