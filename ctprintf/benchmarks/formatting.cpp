#include <benchmark/benchmark.h>

#include <ctprintf/format.hpp>

#include <array>
#include <cstddef>
#include <cstdio>

namespace {

class Buffer {
  public:
    void clear() { size_ = 0; }

    void put(char character) { characters_[size_++] = character; }

  private:
    std::array<char, 128> characters_{};
    std::size_t size_ = 0;
};

void benchmark_ctprintf_hexadecimal(benchmark::State &state)
{
    constexpr unsigned int value = 0xDEADBEEFU;
    Buffer output;

    for (auto _ : state) {
        output.clear();
        ctprintf::format(output, "value=%08x", value);
        benchmark::DoNotOptimize(output);
        benchmark::ClobberMemory();
    }
}

void benchmark_snprintf_hexadecimal(benchmark::State &state)
{
    constexpr unsigned int value = 0xDEADBEEFU;
    std::array<char, 128> output{};

    for (auto _ : state) {
        int result = std::snprintf(output.data(), output.size(), "value=%08x", value);
        benchmark::DoNotOptimize(result);
        benchmark::DoNotOptimize(output);
        benchmark::ClobberMemory();
    }
}

void benchmark_ctprintf_mixed(benchmark::State &state)
{
    constexpr unsigned int identifier = 0x42U;
    constexpr int status = -17;
    constexpr const char *name = "sensor";
    Buffer output;

    for (auto _ : state) {
        output.clear();
        ctprintf::format(output, "id=%08x status=%+d name=%s", identifier, status, name);
        benchmark::DoNotOptimize(output);
        benchmark::ClobberMemory();
    }
}

void benchmark_snprintf_mixed(benchmark::State &state)
{
    constexpr unsigned int identifier = 0x42U;
    constexpr int status = -17;
    constexpr const char *name = "sensor";
    std::array<char, 128> output{};

    for (auto _ : state) {
        int result = std::snprintf(output.data(), output.size(), "id=%08x status=%+d name=%s",
                                   identifier, status, name);
        benchmark::DoNotOptimize(result);
        benchmark::DoNotOptimize(output);
        benchmark::ClobberMemory();
    }
}

} // namespace

BENCHMARK(benchmark_ctprintf_hexadecimal);
BENCHMARK(benchmark_snprintf_hexadecimal);
BENCHMARK(benchmark_ctprintf_mixed);
BENCHMARK(benchmark_snprintf_mixed);

BENCHMARK_MAIN();
