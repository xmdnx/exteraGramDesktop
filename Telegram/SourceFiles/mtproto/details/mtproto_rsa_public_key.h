/*
This file is part of exteraGram Desktop,
the unofficial app based on Telegram Desktop.

For license and copyright information please follow this link:
https://github.com/xmdnx/exteraGramDesktop/blob/dev/LEGAL
*/
#pragma once

#include "base/bytes.h"

namespace MTP::details {

// this class holds an RSA public key and can encrypt fixed-size messages with it
class RSAPublicKey final {
public:
	RSAPublicKey() = default;
	RSAPublicKey(bytes::const_span nBytes, bytes::const_span eBytes);
	RSAPublicKey(RSAPublicKey &&other) = default;
	RSAPublicKey(const RSAPublicKey &other) = default;
	RSAPublicKey &operator=(RSAPublicKey &&other) = default;
	RSAPublicKey &operator=(const RSAPublicKey &other) = default;

	// key in "-----BEGIN RSA PUBLIC KEY----- ..." format
	// or in "-----BEGIN PUBLIC KEY----- ..." format
	explicit RSAPublicKey(bytes::const_span key);

	[[nodiscard]] bool empty() const;
	[[nodiscard]] bool valid() const;
	[[nodiscard]] uint64 fingerprint() const;
	[[nodiscard]] bytes::vector getN() const;
	[[nodiscard]] bytes::vector getE() const;

	// data has exactly 256 chars to be encrypted
	[[nodiscard]] bytes::vector encrypt(bytes::const_span data) const;

	// data has exactly 256 chars to be decrypted
	[[nodiscard]] bytes::vector decrypt(bytes::const_span data) const;

	// data has lequal than 215 chars to be decrypted
	[[nodiscard]] bytes::vector encryptOAEPpadding(bytes::const_span data) const;

private:
	class Private;
	std::shared_ptr<Private> _private;

};

} // namespace MTP::details
