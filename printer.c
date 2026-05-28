#include "printer.h"

// 比较两个任务（优先级 > 页数 > 提交时间）
int compare(Task* a, Task* b) {
    if (a->priority != b->priority) {
        return a->priority - b->priority;
    }
    if (a->pageNum != b->pageNum) {
        return a->pageNum - b->pageNum;
    }
    return (int)difftime(a->submitTime, b->submitTime);
}

// 交换两个任务指针
void swap(Task** a, Task** b) {
    Task* temp = *a;
    *a = *b;
    *b = temp;
}

// 动态扩容
int expandHeap(MinHeap* heap) {
    int newCapacity = heap->capacity * GROWTH_FACTOR;
    Task** newHeap = (Task**)realloc(heap->heap, sizeof(Task*) * newCapacity);
    
    if (newHeap == NULL) {
        printf("内存分配失败，无法扩容！\n");
        return 0;
    }
    
    heap->heap = newHeap;
    heap->capacity = newCapacity;
    printf("堆已自动扩容：%d → %d\n", heap->capacity / GROWTH_FACTOR, heap->capacity);
    return 1;
}

// 初始化堆
void initHeap(MinHeap* heap) {
    heap->capacity = INITIAL_SIZE;
    heap->size = 0;
    heap->heap = (Task**)malloc(sizeof(Task*) * heap->capacity);
    if (heap->heap == NULL) {
        printf("内存分配失败！\n");
        exit(1);
    }
}

// 判断是否为空
int isEmpty(MinHeap* heap) {
    return heap->size == 0;
}

// 获取任务数量
int getTaskCount(MinHeap* heap) {
    return heap->size;
}

// 获取当前容量
int getCapacity(MinHeap* heap) {
    return heap->capacity;
}

// 下沉调整
void heapify(MinHeap* heap, int i) {
    while (2 * i + 1 < heap->size) {
        int left = 2 * i + 1;
        int right = 2 * i + 2;
        int smaller = left;
        
        if (right < heap->size && compare(heap->heap[right], heap->heap[left]) < 0) {
            smaller = right;
        }
        
        if (compare(heap->heap[smaller], heap->heap[i]) >= 0) break;
        
        swap(&heap->heap[i], &heap->heap[smaller]);
        i = smaller;
    }
}

// 上浮调整
void siftUp(MinHeap* heap, int i) {
    while (i > 0 && compare(heap->heap[i], heap->heap[(i - 1) / 2]) < 0) {
        swap(&heap->heap[i], &heap->heap[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

// 插入任务
void insertTask(MinHeap* heap, int taskId, char* userName, char* fileName, 
                int pageNum, int priority) {
    // 检查是否需要扩容
    if (heap->size >= heap->capacity) {
        if (!expandHeap(heap)) {
            printf("队列扩容失败，无法插入任务！\n");
            return;
        }
    }
    
    Task* newTask = (Task*)malloc(sizeof(Task));
    if (newTask == NULL) {
        printf("内存分配失败！\n");
        return;
    }
    
    newTask->taskId = taskId;
    strcpy(newTask->userName, userName);
    strcpy(newTask->fileName, fileName);
    newTask->pageNum = pageNum;
    newTask->priority = priority;
    newTask->submitTime = time(NULL);
    
    int i = heap->size++;
    heap->heap[i] = newTask;
    siftUp(heap, i);
    
    printf("任务提交成功！[ID:%d, 优先级:%d, 页数:%d] (当前队列:%d/%d)\n", 
           taskId, priority, pageNum, heap->size, heap->capacity);
}

// 执行打印（取出最高优先级任务）
void printTask(MinHeap* heap) {
    if (isEmpty(heap)) {
        printf("队列为空，无任务可打印！\n");
        return;
    }
    
    Task* task = heap->heap[0];
    
    printf("\n========== 正在打印 ==========\n");
    printf("任务ID: %d\n", task->taskId);
    printf("用户: %s\n", task->userName);
    printf("文档: %s\n", task->fileName);
    printf("页数: %d\n", task->pageNum);
    printf("优先级: %d (1=最高)\n", task->priority);
    printf("提交时间: %s", ctime(&task->submitTime));
    printf("================================\n\n");
    
    // 取出堆顶
    heap->heap[0] = heap->heap[--heap->size];
    heapify(heap, 0);
    
    free(task);
    
    // 可选：当使用率低于25%时缩容（节省内存）
    if (heap->size > INITIAL_SIZE && heap->size <= heap->capacity / 4) {
        int newCapacity = heap->capacity / 2;
        Task** newHeap = (Task**)realloc(heap->heap, sizeof(Task*) * newCapacity);
        if (newHeap != NULL) {
            heap->heap = newHeap;
            heap->capacity = newCapacity;
            printf("堆已自动缩容：%d → %d\n", heap->capacity * 2, heap->capacity);
        }
    }
}

// 完全销毁堆（释放所有内存）
void destroyHeap(MinHeap* heap) {
    clearAll(heap);
    free(heap->heap);
    heap->heap = NULL;
    heap->size = 0;
    heap->capacity = 0;
}

// 调整指定任务的优先级
int adjustPriority(MinHeap* heap, int taskId, int newPriority) {
    // 参数验证
    if (newPriority < MIN_PRIORITY) newPriority = MIN_PRIORITY;
    if (newPriority > MAX_PRIORITY) newPriority = MAX_PRIORITY;
    
    // 查找目标任务
    int targetIndex = -1;
    for (int i = 0; i < heap->size; i++) {
        if (heap->heap[i]->taskId == taskId) {
            targetIndex = i;
            break;
        }
    }
    
    if (targetIndex == -1) {
        printf("未找到任务 ID: %d\n", taskId);
        return 0;
    }
    
    Task* task = heap->heap[targetIndex];
    int oldPriority = task->priority;
    
    // 如果优先级没变化，直接返回
    if (oldPriority == newPriority) {
        printf("优先级未发生变化\n");
        return 1;
    }
    
    // 更新优先级
    task->priority = newPriority;
    printf("任务 ID:%d 优先级已调整: %d → %d\n", taskId, oldPriority, newPriority);
    
    // 根据优先级变化方向调整堆
    if (newPriority < oldPriority) {
        // 优先级提高（数值变小），需要向上调整
        printf("优先级提高，向上调整...\n");
        siftUp(heap, targetIndex);
    } else {
        // 优先级降低（数值变大），需要向下调整
        printf("优先级降低，向下调整...\n");
        heapify(heap, targetIndex);
    }
    
    return 1;
}