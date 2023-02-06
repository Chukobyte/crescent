#include "se_array_list.h"
#include "../utils/se_assert.h"

#include <string.h>
#include <stdbool.h>

inline void array_list_delete_all_nodes(SEArrayList* list);
inline SEArrayListNode* array_list_create_node(SEArrayList* list, void* value, SEArrayListNode* next);
inline void* array_list_duplicate_node_value(SEArrayList* list, SEArrayListNode* node);

// Public functions
SEArrayList* se_array_list_create(size_t valueSize) {
    SEArrayList* list = SE_MEM_ALLOCATE(SEArrayList);
    list->valueSize = valueSize;
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
    return list;
}

void se_array_list_destroy(SEArrayList* list) {
    array_list_delete_all_nodes(list);
    SE_MEM_FREE(list);
}

void se_array_list_append(SEArrayList* list, void* value) {
    SE_ASSERT(list != NULL);
    SE_ASSERT(value != NULL);
    SEArrayListNode* prevTail = list->tail;
    list->tail = array_list_create_node(list, value, NULL);
    if (prevTail != NULL) {
        prevTail->next = list->tail;
    }
    // If no head, set the head to tail.
    if (list->head == NULL) {
        list->head = list->tail;
    }
    list->size++;
}

void se_array_list_prepend(SEArrayList* list, void* value) {
    SE_ASSERT(list != NULL);
    SE_ASSERT(value != NULL);
    list->head = array_list_create_node(list, value, list->head);
    // If no tail, set the tail to head.
    if (list->tail == NULL) {
        list->tail = list->head;
    }
    list->size++;
}

void se_array_list_insert(SEArrayList* list, void* value, size_t index) {
    SE_ASSERT(list != NULL);
    SE_ASSERT(value != NULL);
    // Check if at beginning or end of list, then just call those functions if so...
    if (index > list->size) {
        // Index is out of bounds, can return error if needed...
        return;
    } else if (index == 0) {
        se_array_list_prepend(list, value);
        return;
    } else if (index == list->size) {
        se_array_list_append(list, value);
        return;
    }
    // Now we know it's at the middle of the list, iterate through nodes
    size_t nodeIndex = 1;
    SEArrayListNode* prevNode = list->head;
    SEArrayListNode* currentIndexNode = list->head->next; // Get the next value since the head exists and was checked
    while (currentIndexNode != NULL) {
        // We found the index node, now insert
        if (index == nodeIndex) {
            SEArrayListNode* currentIndexNodeNext = currentIndexNode->next;
            prevNode->next = array_list_create_node(list, value, currentIndexNodeNext);
            list->size++;
            return;
        }
        prevNode = currentIndexNode;
        currentIndexNode = currentIndexNode->next;
        nodeIndex++;
    }
    SE_ASSERT_FMT(false, "Error with inserting into array list!");
}

void* se_array_list_get(SEArrayList* list, size_t index) {
    if (index < list->size) {
        size_t nodeIndex = 0;
        SEArrayListNode* node = list->head;
        while (node != NULL) {
            if (index == nodeIndex) {
                return node->value;
            }
            node = node->next;
            nodeIndex++;
        }
    }
    return NULL;
}

void* se_array_list_get_front(SEArrayList* list) {
    return list->head->value;
}

void* se_array_list_get_back(SEArrayList* list) {
    return list->tail->value;
}

void* se_array_list_pop(SEArrayList* list, size_t index) {
    if (list->size > 0 && index < list->size) {
        if (index == 0) {
            return se_array_list_pop_front(list);
        } else if (index == list->size - 1) {
            return se_array_list_pop_back(list);
        }

        // Search for node
        size_t nodeIndex = 1;
        SEArrayListNode* nodeBeforePopped = list->head->next;
        while (nodeBeforePopped != NULL) {
            if (index - 1 == nodeIndex) {
                break;
            }
            nodeBeforePopped = nodeBeforePopped->next;
            nodeIndex++;
        }
        // Connect the previous node with the node that's being popped's next node
        SEArrayListNode* nodeToPop = nodeBeforePopped->next;
        nodeBeforePopped->next = nodeToPop->next;
        list->size--;
        void* poppedValue = array_list_duplicate_node_value(list, nodeToPop);
        SE_MEM_FREE(nodeToPop->value);
        SE_MEM_FREE(nodeToPop);
        return poppedValue;
    }
    return NULL;
}

void* se_array_list_pop_front(SEArrayList* list) {
    if (list->size > 0) {
        SEArrayListNode* nodeToPop = list->head;
        list->head = nodeToPop->next;
        list->size--;
        if (list->size <= 1) {
            list->tail = list->head;
        }
        void* poppedValue = array_list_duplicate_node_value(list, nodeToPop);
        SE_MEM_FREE(nodeToPop->value);
        SE_MEM_FREE(nodeToPop);
        return poppedValue;
    }
    return NULL;
}

void* se_array_list_pop_back(SEArrayList* list) {
    if (list->size == 1) {
        return se_array_list_pop_front(list);
    } else if (list->size == 2) {
        SEArrayListNode* nodeToPop = list->tail;
        list->tail = list->head;
        list->size--;
        void* poppedValue = array_list_duplicate_node_value(list, nodeToPop);
        SE_MEM_FREE(nodeToPop);
        return poppedValue;
    } else if (list->size >= 3) {
        SEArrayListNode* nodeToPop = list->head->next;
        SEArrayListNode* nodePrev = list->head;
        while (true) {
            if (nodeToPop->next == NULL) {
                break;
            }
            nodePrev = nodePrev->next;
            nodeToPop = nodeToPop->next;
        }
        // Now that we have node to pop and previous node, pop it
        nodePrev->next = nodeToPop->next;
        list->size--;
        void* poppedValue = array_list_duplicate_node_value(list, nodeToPop);
        SE_MEM_FREE(nodeToPop->value);
        SE_MEM_FREE(nodeToPop);
        return poppedValue;
    }
    return NULL;
}

bool se_array_list_remove(SEArrayList* list, void* value) {
    SEArrayListNode* currentNode = list->head;
    SEArrayListNode* prevNode = NULL;
    while (currentNode != NULL) {
        if (memcmp(currentNode->value, value, list->valueSize) == 0) {
            SEArrayListNode* nodeToDelete = currentNode;
            // First entry
            if (prevNode == NULL) {
                list->head = currentNode->next;
                if (list->size - 1 == 1) {
                    list->tail = list->head;
                }
            } else {
                prevNode->next = currentNode->next;
                // If at end, set tail
                if (prevNode->next == NULL) {
                    list->tail = prevNode;
                }
            }

            list->size--;
            SE_MEM_FREE(nodeToDelete->value);
            SE_MEM_FREE(nodeToDelete);
            return true;
        }
        prevNode = currentNode;
        currentNode = currentNode->next;
    }
    return false;
}

bool se_array_list_is_empty(SEArrayList* list) {
    return list->size == 0;
}

void se_array_list_sort(SEArrayList* list, SeArrayListSortFunc isGreaterThanFunc) {
    if (list->head == NULL) {
        return;
    }
    for (SEArrayListNode* current = list->head; current->next != NULL; current = current->next) {
        for (SEArrayListNode* next = current->next; next != NULL; next = next->next) {
            if (isGreaterThanFunc(current, next)) {
                void* temp = current->value;
                current->value = next->value;
                next->value = temp;
            }
        }
    }
}

void se_array_list_clear(SEArrayList* list) {
    array_list_delete_all_nodes(list);
}

// Private functions
SEArrayListNode* array_list_create_node(SEArrayList* list, void* value, SEArrayListNode* next) {
    SEArrayListNode* node = SE_MEM_ALLOCATE(SEArrayListNode);
    node->value = SE_MEM_ALLOCATE_SIZE(list->valueSize);
    memcpy(node->value, value, list->valueSize);
    node->next = next;
    return node;
}

void* array_list_duplicate_node_value(SEArrayList* list, SEArrayListNode* node) {
    void* dupValue = SE_MEM_ALLOCATE_SIZE(list->valueSize);
    memcpy(dupValue, node->value, list->valueSize);
    return dupValue;
}

void array_list_delete_all_nodes(SEArrayList* list) {
    SEArrayListNode* nodeToDelete = list->head;
    while (nodeToDelete != NULL) {
        SEArrayListNode* nextNode = nodeToDelete->next;
        SE_MEM_FREE(nodeToDelete->value);
        SE_MEM_FREE(nodeToDelete);
        nodeToDelete = nextNode;
    }
    list->size = 0;
    list->head = NULL;
    list->tail = NULL;
}
