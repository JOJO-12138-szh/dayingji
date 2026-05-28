#include "printer.h"

// 函数声明
void displayMenu();
void submitTaskUI(MinHeap *heap, int *taskCounter);
void cancelTaskUI(MinHeap *heap);
void showScheduleRules();
void adjustPriorityUI(MinHeap *heap);

int main()
{
    MinHeap printerQueue;
    initHeap(&printerQueue);

    int choice, taskCounter = 1;

    printf("========== 打印机任务调度系统 ==========\n");
    printf("初始队列容量: %d (可自动扩容)\n", INITIAL_SIZE);
    showScheduleRules();

    while (1)
    {
        displayMenu();
        printf("请选择: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            submitTaskUI(&printerQueue, &taskCounter);
            break;
        case 2:
            printTask(&printerQueue);
            break;
        case 3:
            viewAllTasks(&printerQueue);
            break;
        case 4:
            cancelTaskUI(&printerQueue);
            break;
        case 5: // 新增的优先级调整功能
            adjustPriorityUI(&printerQueue);
            break;
        case 6:
            printf("当前排队任务数: %d\n", getTaskCount(&printerQueue));
            break;
        case 7:
            printf("当前队列容量: %d (使用率: %.1f%%)\n",
                   getCapacity(&printerQueue),
                   (float)getTaskCount(&printerQueue) / getCapacity(&printerQueue) * 100);
            break;
        case 8:
            clearAll(&printerQueue);
            break;
        case 0:
            destroyHeap(&printerQueue);
            printf("感谢使用打印机调度系统！\n");
            return 0;
        default:
            printf("无效选项，请重新选择！\n");
        }
        printf("\n");
    }

    return 0;
}