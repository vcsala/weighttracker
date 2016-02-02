#include <pebble.h>

#include "queue.h"

typedef struct Queue Queue;

struct Queue {
  Queue* next;
  void* object;
};

struct QueueInstance {
  Queue* head;
  Queue* tail;
  int size;
};

static Queue* create_list_item(void* object);

QueueInstance* queue_create_instance(void) {
  QueueInstance* instance = malloc(sizeof(QueueInstance));

  if (instance != NULL) {
    instance->head = NULL;
    instance->tail = NULL;
    instance->size = 0;
  }

  return instance;
}

void queue_destroy_instance(QueueInstance *instance) {
  if (instance == NULL) {
    return;
  }

  queue_clear(instance);
  free(instance);
}

int queue_length(QueueInstance* instance) {
  if (instance == NULL) {
    return 0;
  }

  return instance->size;
}

void queue_add(QueueInstance* instance, void* object) {
  if (NULL == instance) {
    return;
  }

  Queue* child = create_list_item(object);

  if (NULL == instance->head) {
    instance->head = child;
    instance->tail = child;
    instance->size = 1;
  } else {
    Queue* tail = instance->tail;
    tail->next = child;
    instance->tail = child;
    instance->size++;
  }
}

void* queue_head(QueueInstance* instance) {
  if (NULL == instance) {
    return NULL;
  }

  if (instance->head == NULL) {
    return NULL;
  }

  return instance->head->object;
}

void queue_remove(QueueInstance* instance) {
  if (NULL == instance) {
    return;
  }

  Queue* head = instance->head;

  if (head == NULL) {
    return;
  } else {
    if (instance->size == 1) {
      instance->head = NULL;
      instance->tail = NULL;
      instance->size = 0;
    } else {
      instance->head = head->next;
      instance->size--;
    }
  }

  if (head != NULL) {
    if (head->object != NULL) {
      free(head->object);
      free(head);
    }
  }
}

void queue_clear(QueueInstance* instance) {
  if (NULL == instance) {
    return;
  }

  while (instance->head != NULL) {
    queue_remove(instance);
  }
}

//----------------------------------------------------------------------------//

// Create a new Queue item with the specified object.
static Queue* create_list_item(void* object) {
  Queue* item = (Queue *)malloc(sizeof(Queue));

  if (item != NULL) {
    item->next = NULL;
    item->object = object;
  }

  return item;
}
