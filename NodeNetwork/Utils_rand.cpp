#include "Utils_rand.h"

// Глобальные переменные для генерации случайных чисел
// random_device - генерирует случайные числа
// mt19937 - генератор псевдослучайных чисел
std::random_device rd;
std::mt19937 gen(rd());