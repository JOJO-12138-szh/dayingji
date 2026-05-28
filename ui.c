#include "printer.h"

// 显示菜单
void displayMenu() {
    printf("==================== 菜单 ====================\n");
    printf("1. 提交打印任务\n");
    printf("2. 执行打印（取出最高优先级任务）\n");
    printf("3. 查看所有等待任务\n");
    printf("4. 撤销指定任务（按ID）\n");
    printf("5. 调整任务优先级（新功能）\n");  // 新增
    printf("6. 查看任务数量\n");
    printf("7. 查看队列容量\n");
    printf("8. 清空所有任务\n");
    printf("0. 退出系统\n");
    printf("==============================================\n");
}

// 获取并验证优先级
int getPriority() {
    int priority;
    printf("请输入优先级(%d-%d, %d最高): ", MIN_PRIORITY, MAX_PRIORITY, MIN_PRIORITY);
    scanf("%d", &priority);
    if (priority < MIN_PRIORITY) priority = MIN_PRIORITY;
    if (priority > MAX_PRIORITY) priority = MAX_PRIORITY;
    return priority;
}

// 获取并验证页数
int getPageNum() {
    int pageNum;
    printf("请输入页数: ");
    scanf("%d", &pageNum);
    if (pageNum < 1) {
        printf("页数不能小于1，已自动设置为1\n");
        pageNum = 1;
    }
    if (pageNum > 10000) {
        printf("页数超过10000，已自动设置为10000\n");
        pageNum = 10000;
    }
    return pageNum;
}

// 提交任务界面
void submitTaskUI(MinHeap* heap, int* taskCounter) {
    char userName[MAX_USERNAME], fileName[MAX_FILENAME];
    int pageNum, priority;
    
    printf("\n========== 提交新任务 ==========\n");
    printf("请输入用户名: ");
    scanf("%s", userName);
    printf("请输入文件名: ");
    scanf("%s", fileName);
    pageNum = getPageNum();
    priority = getPriority();
    
    insertTask(heap, (*taskCounter)++, userName, fileName, pageNum, priority);
    printf("================================\n");
}

// 撤销任务界面
void cancelTaskUI(MinHeap* heap) {
    int taskId;
    printf("请输入要撤销的任务ID: ");
    scanf("%d", &taskId);
    if (cancelTask(heap, taskId)) {
        printf("撤销成功！\n");
    } else {
        printf("未找到该任务！\n");
    }
}

// 显示调度规则
void showScheduleRules() {
    printf("\n========== 调度规则说明 ==========\n");
    printf("任务执行顺序由以下规则决定（按优先级从高到低）：\n");
    printf("1. 用户优先级（数值越小越优先，1为最高）\n");
    printf("2. 页数（页数越少越优先，提高小任务响应速度）\n");
    printf("3. 提交时间（先提交的先执行）\n");
    printf("===================================\n\n");
}
// 在 ui.c 中添加新函数
void adjustPriorityUI(MinHeap* heap) {
    int taskId, newPriority;
    
    printf("\n========== 调整任务优先级 ==========\n");
    printf("请输入要调整的任务ID: ");
    scanf("%d", &taskId);
    printf("请输入新的优先级(%d-%d, %d最高): ", MIN_PRIORITY, MAX_PRIORITY, MIN_PRIORITY);
    scanf("%d", &newPriority);
    
    if (adjustPriority(heap, taskId, newPriority)) {
        printf("优先级调整成功！\n");
    } else {
        printf("优先级调整失败！\n");
    }
    printf("=====================================\n");
}

