#pragma once

#include "time.h"

typedef struct SEBenchmarkItem {
    float startTime;
    float endTime;
    float elapsedTime;
    bool isRunning;
} SEBenchmarkItem;

void se_benchmark_start(SEBenchmarkItem* benchmarkItem) {
    benchmarkItem->startTime = (float)clock() / CLOCKS_PER_SEC;
    benchmarkItem->endTime = 0.0f;
    benchmarkItem->elapsedTime = 0.0f;
    benchmarkItem->isRunning = true;
}

float se_benchmark_stop(SEBenchmarkItem* benchmarkItem) {
    benchmarkItem->endTime = (float)clock() / CLOCKS_PER_SEC;
    benchmarkItem->elapsedTime = benchmarkItem->endTime - benchmarkItem->startTime;
    return benchmarkItem->elapsedTime;
}

/*
// USAGE EXAMPLE
SEBenchmarkItem benchmarkItem;
se_benchmark_start(&benchmarkItem);
// DO STUFF ...
se_logger_debug("elapsedTime = %f", se_benchmark_stop(&benchmarkItem));
*/