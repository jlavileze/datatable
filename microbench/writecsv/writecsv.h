#ifndef WRITECSV_H
#define WRITECSV_H
#include <stdint.h>

extern double now();

#define NWRITERS 7
#define NA_I1 -128
#define NA_I2 -32768
#define NA_I4 INT32_MIN
#define NA_I8 INT64_MIN
#define NA_F4 NAN
#define NA_F8 ((double)NAN)


typedef struct Column {
  void *data;
  char *strbuf;
} Column;

typedef void (*kernel_fn)(char **output, Column *col, int64_t row);
typedef void (*wkernel_fn)(const char *filename, int64_t *data, int64_t nrows);

typedef struct Kernel {
  kernel_fn kernel;
  const char *name;
} Kernel;

typedef struct WKernel {
  wkernel_fn kernel;
  const char *name;
} WKernel;


typedef struct BenchmarkSuite {
  Column *column;
  char *output;
  Kernel *kernels;
} BenchmarkSuite;

void kernel_dragonflyD(char **pch, Column *col, int64_t row);

BenchmarkSuite prepare_bench_boolean(int64_t N);
BenchmarkSuite prepare_bench_int8(int64_t N);
BenchmarkSuite prepare_bench_int16(int64_t N);
BenchmarkSuite prepare_bench_int32(int64_t N);
BenchmarkSuite prepare_bench_float(int64_t N);
BenchmarkSuite prepare_bench_double(int64_t N);
BenchmarkSuite prepare_bench_string(int64_t N);

void test_write_methods(int B, int64_t N);

#endif
