#pragma once

#include "buffers/input_buffer_interface.h"
#include "pe_bliss2/core/optional_header_validator.h"
#include "pe_bliss2/dos/dos_header_validator.h"
#include "pe_bliss2/image/image.h"
#include "utilities/static_class.h"

namespace pe_bliss
{
class error_list;
} //namespace pe_bliss

namespace pe_bliss::image
{

struct image_load_options
{
	bool allow_virtual_headers = false;
	bool validate_sections = true;
	bool load_section_data = true;
	bool validate_size_of_image = true;
	bool image_loaded_to_memory = false;
	bool eager_section_data_copy = false;
	bool eager_dos_stub_data_copy = false;
	bool validate_image_base = true;
	bool validate_size_of_optional_header = true;
	bool load_overlay = true;
	bool eager_overlay_data_copy = false;
	bool load_full_headers_buffer = true;
	bool eager_full_headers_buffer_copy = false;
	dos::dos_header_validation_options dos_header_validation{};
	core::optional_header_validation_options optional_header_validation{};
};

struct image_load_result
{
	image result;
	bool is_partial = true;
};

class image_loader final : public utilities::static_class
{
public:
	[[nodiscard]]
	static image_load_result load(const buffers::input_buffer_ptr& buffer,
		const image_load_options& options, error_list& errors);
};

} //namespace pe_bliss::image
