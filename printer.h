#ifndef PRINTER_H
#define PRINTER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define INITIAL_SIZE 10      // 初始容量
#define GROWTH_FACTOR 2      // 扩容因子

#define MAX_USERNAME 20
#define MAX_FILENAME 30
#define MIN_PRIORITY 1
#define MAX_PRIORITY 10

// 打印任务信息
typedef struct Task {
    int taskId;
    char userName[MAX_USERNAME];
    char fileName[MAX_FILENAME];
    int pageNum;
    int priority;
    time_t submitTime;
} Task;

// 动态数组实现的最小堆
typedef struct {
    Task** heap;     // 动态分配的数组指针
    int size;        // 当前元素个数
    int capacity;    // 当前容量
} MinHeap;

// 比较函数声明
int compare(Task* a, Task* b);

// 堆操作函数
void initHeap(MinHeap* heap);
int isEmpty(MinHeap* heap);
int getTaskCount(MinHeap* heap);
int getCapacity(MinHeap* heap);      
void insertTask(MinHeap* heap, int taskId, char* userName, char* fileName, 
                int pageNum, int priority);
void printTask(MinHeap* heap);
void viewAllTasks(MinHeap* heap);
int cancelTask(MinHeap* heap, int taskId);
void clearAll(MinHeap* heap);
void destroyHeap(MinHeap* heap);     

int adjustPriority(MinHeap* heap, int taskId, int newPriority);

#endif