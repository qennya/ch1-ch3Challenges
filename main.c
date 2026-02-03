#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *value;            // heap-allocated string
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    Node *tail;
    size_t size;
} DList;

// --- Helpers ---

static char *heap_strdup(const char *s) {
    if (!s) return NULL;
    size_t n = strlen(s) + 1;
    char *copy = (char *)malloc(n);
    if (!copy) return NULL;
    memcpy(copy, s, n);
    return copy;
}

static Node *make_node(const char *value) {
    Node *node = (Node *)malloc(sizeof(Node));
    if (!node) return NULL;

    node->value = heap_strdup(value);
    if (!node->value) {
        free(node);
        return NULL;
    }

    node->prev = NULL;
    node->next = NULL;
    return node;
}

// --- API ---

void dlist_init(DList *list) {
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

void dlist_print(const DList *list) {
    printf("List(size=%zu): ", list->size);
    for (Node *cur = list->head; cur != NULL; cur = cur->next) {
        printf("\"%s\"", cur->value);
        if (cur->next) printf(" <-> ");
    }
    printf("\n");
}

int dlist_insert_front(DList *list, const char *value) {
    Node *node = make_node(value);
    if (!node) return 0;

    node->next = list->head;
    node->prev = NULL;

    if (list->head) {
        list->head->prev = node;
    } else {
        // empty list: head and tail become this node
        list->tail = node;
    }
    list->head = node;
    list->size++;
    return 1;
}

int dlist_insert_back(DList *list, const char *value) {
    Node *node = make_node(value);
    if (!node) return 0;

    node->prev = list->tail;
    node->next = NULL;

    if (list->tail) {
        list->tail->next = node;
    } else {
        // empty list
        list->head = node;
    }
    list->tail = node;
    list->size++;
    return 1;
}

Node *dlist_find(const DList *list, const char *value) {
    if (!value) return NULL;

    for (Node *cur = list->head; cur != NULL; cur = cur->next) {
        if (strcmp(cur->value, value) == 0) {
            return cur; // pointer to node
        }
    }
    return NULL;
}

// delete by node pointer (O(1))
int dlist_delete_node(DList *list, Node *node) {
    if (!list || !node) return 0;

    if (node->prev) node->prev->next = node->next;
    else list->head = node->next; // node was head

    if (node->next) node->next->prev = node->prev;
    else list->tail = node->prev; // node was tail

    free(node->value);
    free(node);
    list->size--;
    return 1;
}

// delete first node whose string matches (O(n))
int dlist_delete_value(DList *list, const char *value) {
    Node *node = dlist_find(list, value);
    return dlist_delete_node(list, node);
}

void dlist_clear(DList *list) {
    Node *cur = list->head;
    while (cur) {
        Node *next = cur->next;
        free(cur->value);
        free(cur);
        cur = next;
    }
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

// --- Tests ---

static void test_basic() {
    DList list;
    dlist_init(&list);

    printf("\n== Insert ==\n");
    dlist_insert_back(&list, "apple");
    dlist_insert_back(&list, "banana");
    dlist_insert_front(&list, "cherry");
    dlist_insert_back(&list, "banana"); // duplicate on purpose
    dlist_print(&list); // cherry <-> apple <-> banana <-> banana

    printf("\n== Find ==\n");
    Node *b = dlist_find(&list, "banana");
    if (b) printf("Found: %s (node=%p)\n", b->value, (void *)b);
    else printf("banana not found\n");

    printf("\n== Delete by value (first match) ==\n");
    dlist_delete_value(&list, "banana");
    dlist_print(&list); // should remove first banana

    printf("\n== Delete by node pointer ==\n");
    Node *c = dlist_find(&list, "cherry");
    dlist_delete_node(&list, c);
    dlist_print(&list);

    printf("\n== Delete missing ==\n");
    int ok = dlist_delete_value(&list, "durian");
    printf("delete durian returned %d\n", ok);
    dlist_print(&list);

    printf("\n== Clear ==\n");
    dlist_clear(&list);
    dlist_print(&list);
}

int main(void) {
    test_basic();
    return 0;
}
