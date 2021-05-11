#pragma once
template <typename T> T littleToBigEndian(char* source, int sourceLength) {
	T returnValue = 0;

	for (int i = sourceLength - 1; i >= 0; i--) {
		returnValue |= (unsigned char)(source[i] << (8 * i));
	}

	return returnValue;
}