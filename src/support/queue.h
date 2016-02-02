#pragma once

#include <pebble.h>

typedef struct QueueInstance QueueInstance;

QueueInstance* queue_create_instance(void);
void queue_destroy_instance(QueueInstance *instance);
int queue_length(QueueInstance* instance);
void queue_add(QueueInstance* instance, void* object);
void* queue_head(QueueInstance* instance);
void queue_remove(QueueInstance* instance);
void queue_clear(QueueInstance* instance);
