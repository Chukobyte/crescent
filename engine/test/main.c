#include "unity.h"

#include <stdbool.h>

#include "../src/core/node_event.h"

void setUp() {}
void tearDown() {}

void cre_node_event_test(void);

int main(int argv, char** args) {
    UNITY_BEGIN();
    RUN_TEST(cre_node_event_test);
    return UNITY_END();
}

// Node event test
void cre_node_event_test(void) {
    // Simple testing one event
    const Entity eventEntity = 1;
    const Entity observerEntity = 2;
    const char* eventId = "walk";

    node_event_create_event(eventEntity, eventId);

    TEST_ASSERT_TRUE(true);
}
