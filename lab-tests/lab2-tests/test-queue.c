#include <stdio.h>
#include <assert.h>
#include <queue.h>

struct node {
	LIST_ENTRY(node) links;
};

LIST_HEAD(head_node, node);

void test_insert_after() {
	struct head_node head;
	struct node nodes[3];
	LIST_INIT(&head);
	LIST_INSERT_HEAD(&head, &nodes[0], links);
	LIST_INSERT_AFTER(&nodes[0], &nodes[1], links);
	assert(head.lh_first == &nodes[0]);
	// assert(nodes[0].links.le_prev == NULL);
	assert(nodes[0].links.le_next == &nodes[1]);
	assert(nodes[1].links.le_prev == &nodes[0].links.le_next);
	assert(nodes[1].links.le_next == NULL);
	LIST_INSERT_AFTER(&nodes[0], &nodes[2], links);
	assert(head.lh_first == &nodes[0]);
	// assert(nodes[0].links.le_prev == NULL);
	assert(nodes[0].links.le_next == &nodes[2]);
	assert(nodes[1].links.le_prev == &nodes[2].links.le_next);
	assert(nodes[1].links.le_next == NULL);
	assert(nodes[2].links.le_prev == &nodes[0].links.le_next);
	assert(nodes[2].links.le_next == &nodes[1]);
}

void test_insert_tail() {
	struct head_node head;
	struct node nodes[3];
	LIST_INIT(&head);
	LIST_INSERT_TAIL(&head, &nodes[0], links);
	assert(head.lh_first == &nodes[0]);
	assert(nodes[0].links.le_prev == NULL);
	assert(nodes[0].links.le_next == NULL);
	LIST_INSERT_TAIL(&head, &nodes[1], links);
	assert(head.lh_first == &nodes[0]);
	assert(nodes[0].links.le_prev == NULL);
	assert(nodes[0].links.le_next == &nodes[1]);
	assert(nodes[1].links.le_prev == &nodes[0].links.le_next);
	assert(nodes[1].links.le_next == NULL);
	LIST_INSERT_TAIL(&head, &nodes[2], links);
	assert(head.lh_first == &nodes[0]);
	assert(nodes[0].links.le_prev == NULL);
	assert(nodes[0].links.le_next == &nodes[1]);
	assert(nodes[1].links.le_prev == &nodes[0].links.le_next);
	assert(nodes[1].links.le_next == &nodes[2]);
	assert(nodes[2].links.le_prev == &nodes[1].links.le_next);
	assert(nodes[2].links.le_next == NULL);
}

void test_queue() {
	test_insert_after();
	test_insert_tail();
}
