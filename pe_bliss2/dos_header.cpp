#include "pe_bliss2/dos_header.h"

#include <exception>
#include <system_error>

#include "pe_bliss2/pe_error.h"
#include "utilities/math.h"

namespace
{

struct dos_header_error_category : std::error_category
{
	const char* name() const noexcept override
	{
		return "dos_header";
	}

	std::string message(int ev) const override
	{
		using enum pe_bliss::dos_header_errc;
		switch (static_cast<pe_bliss::dos_header_errc>(ev))
		{
		case invalid_dos_header_signature:
			return "Invalid DOS header signature";
		case unaligned_e_lfanew:
			return "PE header signature is not DWORD-aligned";
		case invalid_e_lfanew:
			return "Too big or too small e_lfanew value";
		case unable_to_read_dos_header:
			return "Unable to read DOS header";
		default:
			return {};
		}
	}
};

const dos_header_error_category dos_header_error_category_instance;

} //namespace

namespace pe_bliss
{

std::error_code make_error_code(dos_header_errc e) noexcept
{
	return { static_cast<int>(e), dos_header_error_category_instance };
}

void dos_header::deserialize(buffers::input_buffer_interface& buf,
	bool allow_virtual_memory)
{
	try
	{
		base_struct().deserialize(buf, allow_virtual_memory);
	}
	catch (...)
	{
		std::throw_with_nested(pe_error(
			dos_header_errc::unable_to_read_dos_header));
	}
}

void dos_header::serialize(buffers::output_buffer_interface& buf,
	bool write_virtual_part) const
{
	base_struct().serialize(buf, write_virtual_part);
}

pe_error_wrapper dos_header::validate(
	const dos_header_validation_options& options) const noexcept
{
	pe_error_wrapper result;
	if (options.validate_magic && (result = validate_magic()))
		return result;

	if (options.validate_e_lfanew && (result = validate_e_lfanew()))
		return result;

	return result;
}

pe_error_wrapper dos_header::validate_magic() const noexcept
{
	if (base_struct()->e_magic != mz_magic_value)
		return dos_header_errc::invalid_dos_header_signature;
	return {};
}

pe_error_wrapper dos_header::validate_e_lfanew() const noexcept
{
	auto e_lfanew = base_struct()->e_lfanew;

	if (!utilities::math::is_aligned<std::uint32_t>(e_lfanew))
		return dos_header_errc::unaligned_e_lfanew;

	if (e_lfanew < min_e_lfanew || e_lfanew > max_e_lfanew)
		return dos_header_errc::invalid_e_lfanew;

	return {};
}

} //namespace pe_bliss
