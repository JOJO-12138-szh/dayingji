# 打印机任务调度系统

## 一、需求分析（业务场景分析）

### 1.1 业务背景

在现代办公环境中，打印机作为共享资源，同一时刻只能处理一个打印任务。当多个用户同时提交打印任务时，需要合理的调度机制来管理任务队列，确保打印服务的有序进行。

### 1.2 功能需求

本系统需要实现以下核心功能：

| 序号 | 功能需求 | 描述 |
|:----:|----------|------|
| 1 | 提交打印任务 | 用户可提交包含用户名、文件名、页数、优先级的打印任务 |
| 2 | 执行打印 | 按照调度规则取出最优先任务进行打印 |
| 3 | 查看等待任务 | 显示当前所有排队任务及其排序情况 |
| 4 | 撤销任务 | 用户可取消已提交但未打印的任务 |
| 5 | 统计功能 | 查看当前排队任务数量 |
| 6 | 清空队列 | 一键清空所有等待任务 |
| 7 | 多用户并发提交 | 支持多个用户同时提交 |
| 8 | 调整优先级 | 支持调整已提交任务的优先级 |

### 1.3 非功能性需求

- **内存管理**：动态分配任务内存，避免内存泄漏

---

## 二、核心数据结构设计

### 2.1 数据结构选型

本系统采用**最小堆（Min-Heap）**作为核心数据结构，而非传统的链式队列。

### 2.2 任务结构体定义

```c
typedef struct Task {
    int taskId;              // 任务唯一标识
    char userName[20];       // 提交用户
    char fileName[30];       // 文档名称
    int pageNum;             // 打印页数
    int priority;            // 优先级（1最高，10最低）
    time_t submitTime;       // 提交时间戳
} Task;
2.3 最小堆结构体
c
typedef struct {
    Task** heap;     // 动态分配的数组指针
    int size;        // 当前元素个数
    int capacity;    // 当前容量
} MinHeap;
2.4 数据结构图示
text
堆内元素排序示例（按优先级从高到低）：

排名   任务ID   优先级   页数   提交时间      比较说明
1      001      1       5     09:00:00    最优：高优先级+少页数
2      002      1       10    08:59:00    优先级相同，页数比003少
3      003      1       10    09:01:00    优先级和页数相同，时间晚于002
4      004      1       15    09:00:00    优先级相同，页数多
5      005      2       3     09:00:00    优先级低（2>1），排在所有P=1后面
6      006      2       8     08:59:00    P=2中，页数少+时间早
7      007      2       8     09:02:00    P=2中，页数相同但时间晚
三、核心算法设计
3.1 多级优先级比较算法
本系统设计了三级比较规则，确保任务调度既考虑用户优先级，又兼顾执行效率：

c
int compare(Task* a, Task* b) {
    // 第一级：用户优先级（数值越小越优先）
    if (a->priority != b->priority) {
        return a->priority - b->priority;
    }
    // 第二级：页数（页数越少越优先，提高小任务响应速度）
    if (a->pageNum != b->pageNum) {
        return a->pageNum - b->pageNum;
    }
    // 第三级：提交时间（先到先服务）
    return (int)difftime(a->submitTime, b->submitTime);
}
算法时间复杂度：O(1)

3.2 堆插入算法（上浮调整）
原理：新元素插入堆尾，然后向上调整至合适位置，维持堆性质。

c
void insertTask(MinHeap* heap, Task* newTask) {
    // 1. 插入到堆尾
    int i = heap->size++;
    heap->heap[i] = newTask;
    
    // 2. 上浮调整
    while (i > 0 && compare(heap->heap[i], heap->heap[(i-1)/2]) < 0) {
        swap(&heap->heap[i], &heap->heap[(i-1)/2]);
        i = (i-1)/2;
    }
}
算法流程图
text
开始
    ↓
判断堆是否已满？ → 是 → 输出"队列已满" → 返回
    ↓ 否
创建新任务节点
    ↓
记录提交时间戳
    ↓
插入到堆末尾
    ↓
上浮调整
    ↓
输出"提交成功"
    ↓
结束
时间复杂度：O(log n)，n为堆中元素个数

3.3 堆删除算法（下沉调整）
原理：取出堆顶元素后，将堆尾元素移至堆顶，然后向下调整至合适位置。

c
void printTask(MinHeap* heap) {
    // 1. 取出堆顶任务
    Task* task = heap->heap[0];
    
    // 2. 堆尾元素移至堆顶
    heap->heap[0] = heap->heap[--heap->size];
    
    // 3. 下沉调整
    heapify(heap, 0);
    
    // 4. 释放任务内存
    free(task);
}

void heapify(MinHeap* heap, int i) {
    while (2*i+1 < heap->size) {
        int left = 2*i+1;
        int right = 2*i+2;
        int smaller = left;
        
        // 找出左右子节点中更优的
        if (right < heap->size && compare(heap->heap[right], heap->heap[left]) < 0) {
            smaller = right;
        }
        
        // 当前节点已是最优，停止调整
        if (compare(heap->heap[smaller], heap->heap[i]) >= 0) break;
        
        // 交换并继续下沉
        swap(&heap->heap[i], &heap->heap[smaller]);
        i = smaller;
    }
}
算法流程图
text
开始
    ↓
判断队列是否为空？ → 是 → 输出"队列为空" → 返回
    ↓ 否
取出堆顶任务
    ↓
输出打印信息
    ↓
堆尾元素移至堆顶
    ↓
堆大小减1
    ↓
下沉调整
    ↓
释放任务内存
    ↓
结束
时间复杂度：O(log n)

3.4 遍历与撤销算法
查看所有任务
c
void viewAllTasks(MinHeap* heap) {
    // 创建临时副本并排序输出
    // 不改变原堆结构
}
时间复杂度：O(n log n)

撤销指定任务
c
int cancelTask(MinHeap* heap, int taskId) {
    // 1. 遍历查找目标任务
    // 2. 用堆尾元素替换
    // 3. 根据情况向上或向下调整
}
时间复杂度：O(n)

四、业务流程设计
4.1 系统整体流程图
text
程序启动
    ↓
初始化最小堆
    ↓
显示调度规则
    ↓
←─────────────────────┐
    ↓                     │
显示主菜单                │
    ↓                     │
等待用户输入              │
    ↓                     │
├─ 1 → 提交任务 → 入堆调整 → ┐
├─ 2 → 执行打印 → 出堆调整 → │
├─ 3 → 查看任务 → 排序输出 → │
├─ 4 → 撤销任务 → 查找删除 → │
├─ 5 → 查看数量 → 显示个数 → │
├─ 6 → 清空队列 → 释放内存 → │
├─ 0 → 退出程序             │
└─ 其他 → 提示错误          │
    ↓                       │
继续循环 ───────────────────┘
4.2 任务调度规则
本系统采用三级优先级调度算法：

级别	比较字段	规则	设计理由
第一级	priority	数值小优先	满足VIP用户需求
第二级	pageNum	页数少优先	提高小任务响应速度
第三级	submitTime	先提交优先	保证公平性
4.3 模块调用关系
text
main.c（主控模块）
├── ui.c（界面模块）
│     ├── displayMenu()      - 显示菜单
│     ├── submitTaskUI()     - 提交界面
│     ├── cancelTaskUI()     - 撤销界面
│     └── showScheduleRules()- 显示规则
│
├── heap.c（堆核心模块）
│     ├── insertTask()       - 插入任务
│     ├── printTask()        - 打印任务
│     ├── heapify()          - 下沉调整
│     └── siftUp()           - 上浮调整
│
└── task_manager.c（任务管理模块）
      ├── viewAllTasks()     - 查看任务
      ├── cancelTask()       - 撤销任务
      └── clearAll()         - 清空队列
五、难点与解决方法
5.1 难点一：多级优先级比较的实现
问题描述：如何同时考虑优先级、页数和提交时间三个因素，且保持比较函数的高效性？

解决方案：设计三级比较器，逐级比较，提前返回结果。

c
int compare(Task* a, Task* b) {
    if (a->priority != b->priority) return a->priority - b->priority;
    if (a->pageNum != b->pageNum) return a->pageNum - b->pageNum;
    return (int)difftime(a->submitTime, b->submitTime);
}
5.2 难点二：查看任务时不破坏原堆结构
问题描述：遍历输出时需要按优先级顺序，但不能修改原堆。

解决方法：创建临时副本，在副本上执行堆排序。

c
void viewAllTasks(MinHeap* heap) {
    // 创建临时数组
    Task* temp[MAX_SIZE];
    memcpy(temp, heap->heap, sizeof(Task*) * heap->size);
    
    // 在临时数组上构建堆并逐个输出
    // 原堆结构保持不变
}
5.3 难点三：动态数组扩容
问题描述：固定数组存在容量上限，当任务数超过上限时无法继续提交；而设置过大的固定数组又会浪费内存。

解决方案：采用动态数组实现堆，支持自动扩容和可选的缩容机制。

（1）动态数组数据结构设计
c
// 动态数组实现的最小堆（替代固定数组）
typedef struct {
    Task** heap;     // 动态分配的数组指针
    int size;        // 当前元素个数
    int capacity;    // 当前容量
} MinHeap;
（2）扩容算法实现
c
static int expandHeap(MinHeap* heap) {
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
六、其他
6.1 系统特色
多级优先级调度：综合考虑优先级、页数和提交时间，比简单FCFS更灵活

模块化设计：代码分为界面层、业务层、数据结构层，便于维护和扩展

完整的错误处理：对队列空、队列满、无效输入等情况都有处理

6.2 运行示例
text
========== 打印机任务调度系统 ==========
版本: 2.0 (多级优先级调度)

========== 调度规则说明 ==========
任务执行顺序由以下规则决定（按优先级从高到低）：
1. 用户优先级（数值越小越优先，1为最高）
2. 页数（页数越少越优先，提高小任务响应速度）
3. 提交时间（先提交的先执行）
===================================

==================== 菜单 ====================
1. 提交打印任务
2. 执行打印（取出最高优先级任务）
3. 查看所有等待任务
4. 撤销指定任务（按ID）
5. 查看任务数量
6. 清空所有任务
0. 退出系统
==============================================
6.3 尚未实现的功能
多用户并发提交：支持多个用户同时提交
