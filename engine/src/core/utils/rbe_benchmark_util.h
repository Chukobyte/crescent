#pragma once

#include "time.h"

typedef struct RBEBenchmarkItem {
    float startTime;
    float endTime;
    float elapsedTime;
    bool isRunning;
} RBEBenchmarkItem;

void rbe_benchmark_start(RBEBenchmarkItem* benchmarkItem) {
    benchmarkItem->startTime   = (float) clock() / CLOCKS_PER_SEC;
    benchmarkItem->endTime     = 0.0f;
    benchmarkItem->elapsedTime = 0.0f;
    benchmarkItem->isRunning   = true;
}

float rbe_benchmark_stop(RBEBenchmarkItem* benchmarkItem) {
    benchmarkItem->endTime     = (float) clock() / CLOCKS_PER_SEC;
    benchmarkItem->elapsedTime = benchmarkItem->endTime - benchmarkItem->startTime;
    return benchmarkItem->elapsedTime;
}

/*
// USAGE EXAMPLE
RBEBenchmarkItem benchmarkItem;
rbe_benchmark_start(&benchmarkItem);
// DO STUFF ...
rbe_logger_debug("elapsedTime = %f", rbe_benchmark_stop(&benchmarkItem));
*/