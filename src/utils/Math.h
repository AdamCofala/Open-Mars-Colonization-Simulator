#include <cmath>


float calc_jitter(int x, int y) {
	int noise = ((x * 1619 + y * 31337) ^ (x + y * 7)) & 0xFF;
	return (noise % 13) - 6; // Jitter in range [-6, 6]
}