#pragma once
#include <cstdint>

int randomizer(int min, int max);

// Générateur xorshift64* inliné : bien plus rapide que mt19937 dans les boucles chaudes
extern uint64_t rngState;

inline uint32_t random32() {
	rngState ^= rngState >> 12;
	rngState ^= rngState << 25;
	rngState ^= rngState >> 27;
	return static_cast<uint32_t>((rngState * 0x2545F4914F6CDD1Dull) >> 32);
}

// Entier uniforme dans [0, n[ sans division (n > 0)
inline uint32_t randomBelow(uint32_t n) {
	return static_cast<uint32_t>((static_cast<uint64_t>(random32()) * n) >> 32);
}

// --- Parallélisme (OpenMP si disponible, sinon exécution séquentielle) ---
#ifdef _OPENMP
#include <omp.h>
inline int nbThreads() { return omp_get_max_threads(); }
inline int numThread() { return omp_get_thread_num(); }
#else
inline int nbThreads() { return 1; }
inline int numThread() { return 0; }
#endif

// Générateur local à un bloc : chaque bloc d'entités a son propre état, donc pas de partage entre threads
struct Xorshift {
	uint64_t s;

	Xorshift(uint64_t graine, uint64_t bloc)
	{
		uint64_t z = graine + (bloc + 1) * 0x9E3779B97F4A7C15ull; // splitmix64
		z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
		z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
		s = (z ^ (z >> 31)) | 1;
	}

	uint32_t suivant()
	{
		s ^= s >> 12;
		s ^= s << 25;
		s ^= s >> 27;
		return static_cast<uint32_t>((s * 0x2545F4914F6CDD1Dull) >> 32);
	}

	uint32_t sous(uint32_t n) { return static_cast<uint32_t>((static_cast<uint64_t>(suivant()) * n) >> 32); }
};
