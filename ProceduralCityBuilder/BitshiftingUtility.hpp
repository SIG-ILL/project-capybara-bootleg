#pragma once

#include <array>

namespace pcb {
	template <typename T, size_t N> T littleToBigEndian(const std::array<char, N> source) {
		T returnValue = 0;

		for (int i = source.size() - 1; i >= 0; i--) {
			returnValue |= static_cast<unsigned char>(source[i]) << (8 * i);
		}

		return returnValue;
	}

	template <typename T, size_t N> std::vector<unsigned char> bigToLittleEndian(T source) {
		std::vector<unsigned char> returnValue(N);

		const int BYTE_MASK = 0b11111111;
		const int BITS_PER_BYTE = 8;
		for (int i = 0; i < N; i++) {
			int shiftingAmount = BITS_PER_BYTE * i;
			returnValue[i] = static_cast<unsigned char>((source & (BYTE_MASK << shiftingAmount)) >> shiftingAmount);
		}

		return returnValue;
	}
}