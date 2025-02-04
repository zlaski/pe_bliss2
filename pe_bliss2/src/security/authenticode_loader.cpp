#include "pe_bliss2/security/authenticode_loader.h"

#include <array>
#include <exception>
#include <string>
#include <vector>

#include "buffers/input_memory_buffer.h"

#include "pe_bliss2/pe_error.h"
#include "pe_bliss2/security/byte_range_types.h"

#include "simple_asn1/crypto/pkcs7/authenticode/oids.h"
#include "simple_asn1/crypto/pkcs7/authenticode/spec.h"
#include "simple_asn1/der_decode.h"

namespace
{

struct authenticode_directory_loader_error_category : std::error_category
{
	const char* name() const noexcept override
	{
		return "authenticode_directory_loader";
	}

	std::string message(int ev) const override
	{
		using enum pe_bliss::security::authenticode_loader_errc;
		switch (static_cast<pe_bliss::security::authenticode_loader_errc>(ev))
		{
		case unsupported_certificate_type:
			return "Unsupported security certificate type";
		case unable_to_read_der:
			return "Unable to read the PKCS7 Authenticode DER";
		case buffer_is_not_contiguous:
			return "PKCS7 Authenticode DER buffer is not contiguous";
		default:
			return {};
		}
	}
};

const authenticode_directory_loader_error_category authenticode_directory_loader_error_category_instance;

} //namespace

namespace pe_bliss::security
{

std::error_code make_error_code(authenticode_loader_errc e) noexcept
{
	return { static_cast<int>(e), authenticode_directory_loader_error_category_instance };
}

template<typename RangeType>
std::vector<authenticode_pkcs7<span_range_type>> load_nested_signatures(
	const pkcs7::attribute_map<RangeType>& unauthenticated_attributes)
{
	std::vector<authenticode_pkcs7<span_range_type>> result;

	const auto nested_signatures = unauthenticated_attributes.get_attributes(
		asn1::crypto::pkcs7::authenticode::oid_nested_signature_attribute);
	result.reserve(nested_signatures.size());
	for (const auto& nested_signature : nested_signatures)
	{
		buffers::input_memory_buffer buf(nested_signature.data(), nested_signature.size());
		result.emplace_back(load_authenticode_signature<span_range_type>(buf));
	}

	return result;
}

template<typename RangeType>
authenticode_pkcs7<RangeType> load_authenticode_signature(
	buffers::input_buffer_interface& buffer)
{
	authenticode_pkcs7<RangeType> result;
	const auto size = buffer.size();
	const auto* data = buffer.get_raw_data(0, size);
	try
	{
		if constexpr (std::is_same_v<RangeType, span_range_type>)
		{
			if (!data)
				throw pe_error(authenticode_loader_errc::buffer_is_not_contiguous);

			//TODO: check for trailing garbage (nullbytes in the end are allowed)
			asn1::der::decode<asn1::spec::crypto::pkcs7::authenticode::content_info>(
				data, data + size, result.get_content_info());
		}
		else
		{
			if (data)
			{
				//TODO: check for trailing garbage
				asn1::der::decode<asn1::spec::crypto::pkcs7::authenticode::content_info>(
					data, data + size, result.get_content_info());
			}
			else
			{
				std::vector<std::byte> copied_data;
				copied_data.resize(size);
				buffer.read(0, size, copied_data.data());
				//TODO: check for trailing garbage
				asn1::der::decode<asn1::spec::crypto::pkcs7::authenticode::content_info>(
					copied_data.cbegin(), copied_data.cend(), result.get_content_info());
			}
		}
	}
	catch (const asn1::parse_error&)
	{
		std::throw_with_nested(pe_error(authenticode_loader_errc::unable_to_read_der));
	}

	return result;
}

template<typename RangeType>
[[nodiscard]]
authenticode_pkcs7<RangeType> load_authenticode_signature(
	buffers::input_buffer_interface& buffer,
	const detail::security::win_certificate& certificate_info)
{
	if (certificate_info.certificate_type != detail::security::win_cert_type_pkcs_signed_data)
		throw pe_error(authenticode_loader_errc::unsupported_certificate_type);

	return load_authenticode_signature<RangeType>(buffer);
}

template authenticode_pkcs7<span_range_type> load_authenticode_signature(
	buffers::input_buffer_interface& buffer);
template authenticode_pkcs7<vector_range_type> load_authenticode_signature(
	buffers::input_buffer_interface& buffer);

template authenticode_pkcs7<span_range_type> load_authenticode_signature(
	buffers::input_buffer_interface& buffer,
	const detail::security::win_certificate& certificate_info);
template authenticode_pkcs7<vector_range_type> load_authenticode_signature(
	buffers::input_buffer_interface& buffer,
	const detail::security::win_certificate& certificate_info);

template std::vector<authenticode_pkcs7<span_range_type>> load_nested_signatures<span_range_type>(
	const pkcs7::attribute_map<span_range_type>& unauthenticated_attributes);
template std::vector<authenticode_pkcs7<span_range_type>> load_nested_signatures<vector_range_type>(
	const pkcs7::attribute_map<vector_range_type>& unauthenticated_attributes);

} //namespace pe_bliss::security
