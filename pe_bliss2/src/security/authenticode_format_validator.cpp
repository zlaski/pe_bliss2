#include "pe_bliss2/security/authenticode_format_validator.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <tuple>

#include "pe_bliss2/pe_error.h"
#include "pe_bliss2/security/byte_range_types.h"
#include "pe_bliss2/security/crypto_algorithms.h"
#include "pe_bliss2/security/pkcs7/pkcs7_format_validator.h"

#include "simple_asn1/crypto/crypto_common_types.h"
#include "simple_asn1/crypto/pkcs7/authenticode/oids.h"
#include "simple_asn1/der_decode.h"
#include "simple_asn1/spec.h"

namespace
{

//TODO
//1.3.6.1.4.1.311.3.3.1 - Timestamping signature(Ms-CounterSign)
//PKCS #9 v1 countersignature
//The timestamp certificate chain�including the root certificate�is added to the PKCS #7 SignedData certificates structure, although the root certificate is not required
//This is an unsigned attribute in an Authenticode signature that contains a PKCS#7
//that timestamps the Portable Executable(PE) file signed by the Authenticode signature.
//The following is the timestamp attribute's OID type:
//szOID_RSA_counterSign 1.2.840.113549.1.9.6

struct authenticode_format_validator_error_category : std::error_category
{
	const char* name() const noexcept override
	{
		return "authenticode_format_validator";
	}

	std::string message(int ev) const override
	{
		using enum pe_bliss::security::authenticode_format_validator_errc;
		switch (static_cast<pe_bliss::security::authenticode_format_validator_errc>(ev))
		{
		case non_matching_type_value_digest_algorithm:
			return "Digest algorithm of signer info does not match digest algorithm of DigestInfo";
		case invalid_content_info_oid:
			return "Invalid content info OID";
		case invalid_type_value_type:
			return "Invalid SpcAttributeTypeAndOptionalValue type";
		default:
			return {};
		}
	}
};

const authenticode_format_validator_error_category authenticode_format_validator_error_category_instance;

} //namespace

namespace pe_bliss::security
{

std::error_code make_error_code(authenticode_format_validator_errc e) noexcept
{
	return { static_cast<int>(e), authenticode_format_validator_error_category_instance };
}

template<typename RangeType>
void validate(const authenticode_pkcs7<RangeType>& signature,
	error_list& errors)
{
	pkcs7::validate(signature, errors);

	const auto& content_info = signature.get_content_info();
	if (content_info.data.digest_algorithms.size() != 1u
		|| content_info.data.signer_infos.size() != 1u)
	{
		errors.add_error(pkcs7::pkcs7_format_validator_errc::invalid_signer_count);
		return;
	}

	const auto& signed_data_content_info = content_info.data.content_info;
	if (!std::ranges::equal(signed_data_content_info.content_type.container,
		asn1::crypto::pkcs7::authenticode::oid_spc_indirect_data_content))
	{
		errors.add_error(authenticode_format_validator_errc::invalid_content_info_oid);
	}

	const auto& type_value = signed_data_content_info.content.type_value.value;
	if (!std::ranges::equal(type_value.type.container,
		asn1::crypto::pkcs7::authenticode::oid_spc_pe_image_data))
	{
		errors.add_error(authenticode_format_validator_errc::invalid_type_value_type);
	}

	if (!algorithm_id_equals(
		signed_data_content_info.content.digest.value.digest_algorithm,
		content_info.data.signer_infos[0].digest_algorithm))
	{
		errors.add_error(
			authenticode_format_validator_errc::non_matching_type_value_digest_algorithm);
	}
}

template<typename RangeType>
void validate(const pkcs7::attribute_map<RangeType>& authenticated_attributes,
	std::optional<asn1::utc_time>& signing_time, error_list& errors)
{
	pkcs7::validate_authenticated_attributes(authenticated_attributes, errors);

	try
	{
		if (auto signing_time_attr = authenticated_attributes.get_signing_time(); signing_time_attr)
		{
			signing_time = asn1::der::decode<asn1::utc_time, asn1::spec::utc_time<>>(
				signing_time_attr->begin(), signing_time_attr->end());
		}
	}
	catch (const pe_error&)
	{
		errors.add_error(pkcs7::pkcs7_format_validator_errc::invalid_signing_time);
	}
	catch (const asn1::parse_error&)
	{
		errors.add_error(pkcs7::pkcs7_format_validator_errc::invalid_signing_time);
	}

	try
	{
		if (auto content_type = authenticated_attributes.get_content_type(); content_type)
		{
			auto decoded_content_type = asn1::der::decode<asn1::crypto::object_identifier_type,
				asn1::spec::object_identifier<>>(content_type->begin(), content_type->end());
			if (!std::ranges::equal(decoded_content_type.container,
				asn1::crypto::pkcs7::authenticode::oid_spc_indirect_data_content))
			{
				errors.add_error(pkcs7::pkcs7_format_validator_errc::invalid_content_type);
			}
		}
	}
	catch (const pe_error&)
	{
		errors.add_error(pkcs7::pkcs7_format_validator_errc::invalid_content_type);
	}
	catch (const asn1::parse_error&)
	{
		errors.add_error(pkcs7::pkcs7_format_validator_errc::invalid_content_type);
	}
}

template void validate<span_range_type>(
	const authenticode_pkcs7<span_range_type>& signature,
	error_list& errors);
template void validate<vector_range_type>(
	const authenticode_pkcs7<vector_range_type>& signature,
	error_list& errors);

template void validate<span_range_type>(
	const pkcs7::attribute_map<span_range_type>& authenticated_attributes,
	std::optional<asn1::utc_time>& signing_time, error_list& errors);
template void validate<vector_range_type>(
	const pkcs7::attribute_map<vector_range_type>& authenticated_attributes,
	std::optional<asn1::utc_time>& signing_time, error_list& errors);

} //namespace pe_bliss::security
