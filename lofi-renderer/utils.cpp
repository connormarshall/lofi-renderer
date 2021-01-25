//	Abbreviation for convenience
typedef unsigned int u32;

// Clamps v between min and max. Inline, meaning it replaces each call with the function itself.
inline int clamp(int min, int v, int max) {
	if (v < min) v = min;
	if (v > max) v = max;
	return v;

}