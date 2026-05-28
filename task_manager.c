#include "printer.h"

// 创建临时堆用于排序输出
void buildTempHeap(Task** temp, int size) {
    for (int i = size/2 - 1; i >= 0; i--) {
        int j = i;
        while (2 * j + 1 < size) {
            int left = 2 * j + 1;
            int right = 2 * j + 2;
            int smaller = left;
            
            if (right < size && compare(temp[right], temp[left]) < 0) {
                smaller = right;
            }
            
            if (compare(temp[smaller], temp[j]) >= 0) break;
            
            Task* t = temp[j];
            temp[j] = temp[smaller];
            temp[smaller] = t;
            j = smaller;
        }
    }
}

// 查看所有任务
void viewAllTasks(MinHeap* heap) {
    if (isEmpty(heap)) {
        printf("队列为空！\n");
        return;
    }
    
    // 动态分配临时数组
    Task** temp = (Task**)malloc(sizeof(Task*) * heap->size);
    if (temp == NULL) {
        printf("内存分配失败！\n");
        return;
    }
    
    for (int i = 0; i < heap->size; i++) {
        temp[i] = heap->heap[i];
    }
    
    buildTempHeap(temp, heap->size);
    
    printf("\n========== 等待任务列表 ==========\n");
    printf("排序规则: 优先级(数值小优先) > 页数(少优先) > 提交时间\n");
    printf("当前队列: %d/%d\n", heap->size, heap->capacity);
    printf("%-8s %-12s %-15s %-8s %-8s %-20s\n", 
           "ID", "用户", "文件名", "页数", "优先级", "提交时间");
    printf("--------------------------------------------------------------------------------\n");
    
    int remaining = heap->size;
    while (remaining > 0) {
        Task* t = temp[0];
        char timeStr[64];
        strftime(timeStr, sizeof(timeStr), "%H:%M:%S", localtime(&t->submitTime));
        
        printf("%-8d %-12s %-15s %-8d %-8d %-20s\n",
               t->taskId, t->userName, t->fileName, t->pageNum, t->priority, timeStr);
        
        temp[0] = temp[--remaining];
        
        int i = 0;
        while (2 * i + 1 < remaining) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;
            int smaller = left;
            
            if (right < remaining && compare(temp[right], temp[left]) < 0) {
                smaller = right;
            }
            
            if (compare(temp[smaller], temp[i]) >= 0) break;
            
            Task* t = temp[i];
            temp[i] = temp[smaller];
            temp[smaller] = t;
            i = smaller;
        }
    }
    printf("================================================================================\n\n");
    
    free(temp);
}

// 撤销任务
int cancelTask(MinHeap* heap, int taskId) {
    for (int i = 0; i < heap->size; i++) {
        if (heap->heap[i]->taskId == taskId) {
            printf("撤销任务: ID=%d, 用户=%s, 文档=%s, 页数=%d, 优先级=%d\n",
                   heap->heap[i]->taskId, heap->heap[i]->userName, 
                   heap->heap[i]->fileName, heap->heap[i]->pageNum, heap->heap[i]->priority);
            free(heap->heap[i]);
            heap->heap[i] = heap->heap[--heap->size];
            
            // 调整被移动过来的节点
            if (i > 0 && compare(heap->heap[i], heap->heap[(i-1)/2]) < 0) {
                int j = i;
                while (j > 0 && compare(heap->heap[j], heap->heap[(j-1)/2]) < 0) {
                    Task* t = heap->heap[j];
                    heap->heap[j] = heap->heap[(j-1)/2];
                    heap->heap[(j-1)/2] = t;
                    j = (j-1)/2;
                }
            } else {
                int j = i;
                while (2*j+1 < heap->size) {
                    int left = 2*j+1;
                    int right = 2*j+2;
                    int smaller = left;
                    
                    if (right < heap->size && compare(heap->heap[right], heap->heap[left]) < 0) {
                        smaller = right;
                    }
                    
                    if (compare(heap->heap[smaller], heap->heap[j]) >= 0) break;
                    
                    Task* t = heap->heap[j];
                    heap->heap[j] = heap->heap[smaller];
                    heap->heap[smaller] = t;
                    j = smaller;
                }
            }
            return 1;
        }
    }
    return 0;
}

// 清空所有任务
void clearAll(MinHeap* heap) {
    for (int i = 0; i < heap->size; i++) {
        free(heap->heap[i]);
    }
    heap->size = 0;
    printf("已清空所有任务！\n");
}