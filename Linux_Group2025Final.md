# Linux_Group 2025 一面题目

题目出处：https://www.xiyoulinux.com/archive/interview-2025

- 本题目只作为西邮 Linux 兴趣小组 2025 纳新面试的有限参考。
- 为节省版面，本试题的程序源码省去了 `#include` 指令。
- 本试题中的程序源码仅用于考察 C 语言基础，不应当作为 C 语言“代码风格”的范例。
- 所有题目编译并运行于 `x86_64` `GNU/Linux` 环境。
- 文档的整理与排版借助于 Gemini 完成。
---

## NO.0. 拼命的企鹅

一只企鹅在爬山，每隔一段路都会遇到一块石头。第 1 块石头重量是 `a`，每往上走一段路，石头重量就会变成上一段的平方。企鹅可以选择把某些石头捡起来，最后把捡到的石头重量相乘。  
它怎样捡石头，才能得到重量乘积恰好是 `a` 的 `b` 次方的石头？（比如 `b = 173` 时，要捡哪些石头？）

**本质**：将 `b` 拆成二进制。  
`1, 2, 4, 8, 16...... 2^n` 理论可以组成任何数  
`173 = 128 + 32 + 8 + 4 + 1`  
所以利用贪心不断取最接近的 `2^n` 即可。

```cpp
int main()
{
    int a[10086];
    int n;
    scanf("%d", &n);
    int i,j,s=1;
    for(i=1;;i++)
    {
        a[i]=s;
        s*=2;
        if(s>=n)
            break;
    }
    printf("%d=",n);
    do
    {
        if(a[i]<=n)
        {
            n-=a[i];
            printf("%d",a[i]);
            if(n>0)
                printf("+");
        }
        else
            i--;
    }while(n>0);
    return 0;
}
```

---

## NO.1. 请解释以下代码的运行结果

```cpp
int main() {
    if (printf("Hi guys! ") || printf("Xiyou Linux ")) {
        printf("%d\n", printf("Welcome to Xiyou Linux 2%d", printf("")));
    }
    return 0;
}
```

`printf` 是一个函数，有返回值。  
第一个 `if` 判断两个 `printf` 返回的值是否为 `0`，`0` 为 `false`。  
但是，第一个 `printf` 返回了 `9`，此时整个表达式已经为真，触发短路机制。  
> **PS**: 在 `||` 里面，左边为真则表达式已经为真，不会再执行右边的；类似，在 `&&` 里面，左边为假亦不会继续执行右边的，因为表达式已经为假。

此时 `if` 被触发，执行下面的程序。

在 C 语言中，函数的参数如果也是一个函数调用，那么内层函数会先执行，执行完毕后的“返回值”会作为外层函数的参数。  
所以最里面的 `printf` 先算，不输出，这个 `0` 传给第二个 `printf`，就组成了 `20`（`2%d`，`%d` 输出 `0`）。  
全部混在一起一共 25 个字符，这个值再传回给最外面的 `printf`，它再输出 `25`。

于是结果为：
```text
Hi guys! Welcome to Xiyou Linux 2025    //结果
Hi guys!    //if（）内部条件
Welcome to Xiyou Linux 2    //中间的printf打印的
         0   //最里面的printf返回的值
         25  //最外面的printf打印的，中间返回的
```

---

## NO.2. 你能找到成功打印的语句吗？你能看出每个 if 中函数的返回值吗？这些函数各有什么特点？

```cpp
int main() {
    char p1[] = { 'W', 'e', 'l', 'c', 'o', 'm', 'e', '\0' };
    char p2[] = "Join us\0", p4[] = "xiyou_linux_group";
    const char* p3 = "Xiyou Linux Group\0\n2025\0";
    if (strcmp(p1, p2)) {
        printf("%s to %s!\n", p1, p2);
    }
    if (strlen(p3) > sizeof(p3)) {
        printf("%s", p3);
    }
    if (sizeof(p1) == sizeof(p2)) {
        printf("%s", p4);
    }
    return 0;
}
```

**结果**：
```text
Welcome to Join us!
Xiyou Linux Group
```

**Why?**  
首先：
```cpp
strcmp(s1, s2)
```
按字典序比较两个字符串。相等返回 `0`；`s1` 大于 `s2` 返回正数；`s1` 小于 `s2` 返回负数。

```cpp
strlen(s)
```
计算字符串的有效字符长度，遇到 `\0` 停止计数，不包含 `\0`。

```cpp
sizeof(s)
```
计算变量或数据类型在内存中占用的总字节数。对于数组，包含所有字符和末尾的 `\0`；对于指针，返回指针本身的大小（32 位系统是 4，64 位系统是 8）。

所以：
1. **第一个 `if`**
   ```cpp
   if (strcmp(p1, p2))
   ```
   `p1 = "Welcome"`，`p2 = "Join us"`  
   `strcmp` 比较的是 ASCII 码：`'W'` (87) 和 `'J'` (74) 比较。`'W' > 'J'`，所以 `strcmp` 返回一个正数（大于 0）。  
   在 C 语言中，非 0 即为真（`True`）。所以条件成立，执行内部 `printf`。  
   输出：`Welcome to Join us!`  
   （注意 `printf` 输出 `%s` 时遇到 `\0` 就会停止，所以不会把 `p2` 里的显式 `\0` 和后面的隐藏 `\0` 打印出来）。

2. **第二个 `if` 判断**
   ```cpp
   if (strlen(p3) > sizeof(p3))
   ```
   如上所述，`strlen(p3)` 是字符串长度 17。  
   `sizeof(p3)` 是指针大小，在 64 位环境下是 8。  
   判断变成了 `if (17 > 8)`，条件为真（`True`）。执行内部 `printf`。  
   输出：`printf("%s", p3);` 会打印 `p3` 指向的字符串，直到遇到第一个 `\0`。  
   结果：打印 `Xiyou Linux Group`。

3. **第三个 `if` 判断**
   ```cpp
   if (sizeof(p1) == sizeof(p2))
   ```
   前面我们已经计算过了：
   `sizeof(p1) = 8` （7 个字符 + 1 个显式 `\0`）  
   `sizeof(p2) = 9` （7 个可见字符 + 1 个显式 `\0` + 1 个自动添加的 `\0`）  
   判断变成了 `if (8 == 9)` （条件为假，`False`）。  
   不执行内部 `printf`。

---

## NO.3. 这个程序的输出是什么？解释为什么会这样？

```cpp
int main() {
    float a1 = 0.3, b1 = 6e-1, sum1 = 0.9;
    printf("a1 + b1 %s sum1\n", (a1 + b1 == sum1) ? "==" : "!=");
    float a2 = 0x0.3p0, b2 = 0x6p-4, sum2 = 0x0.9p0;
    printf("a2 + b2 %s sum2\n", (a2 + b2 == sum2) ? "==" : "!=");
    return 0;
}
```

我们看到的：
```text
a1 = 0.3
b1 = 6e-1 = 6 * 10^-1 = 0.6
sum1 = 0.9
// 应该相等
```

但是，计算机使用二进制（IEEE 754 标准）来存储浮点数，很多十进制的小数无法用二进制精确表示，从而产生精度误差。

三个数在二进制中都是无限循环小数：
```c
0.3 = 0.0100110011001100...₂    //（循环 1100）
0.6 = 0.1001100110011001...₂    //（循环 1001）
0.9 = 0.1110011001100110...₂    //（循环 0110）
```
存储时各自被舍入到最近的 53 位有效数字，加法时又发生一次舍入。  
误差累积后，`0.3 + 0.6` 的舍入结果与 `0.9` 的字面量舍入结果落在不同的浮点数上。  
导致：`0.3 + 0.6 = 0.899999999999999...... != 0.9`

而 `a2`, `b2`, `sum2` 都是十六进制的数：
```text
a2 = 0x0.3p0 = 0.3 * 2^0 = 3/16 = 0.1875 
b2 = 0x6p-4 = 6 * 2^-4 = 6/16 = 0.375
sum2 = 0x0.9p0 = 0.9 * 2^0 = 9/16 = 0.5625
```
这些通过十六进制表示的数字，换算成十进制后，都是形如 `x/16` 的分数。  
分母是 `2^n` 的分数，在二进制中是可以被精确表示的，因此没有精度损失。

---

## NO.4. 在一个数组中，所有数字都出现了偶数次，只有两个数字出现了奇数次，请聪明的你帮我看看以下的代码是如何找到这两个数字的呢？

```cpp
void findUndercoverIDs(int nums[], int size) {
    int xorAll = 0,id_a = 0,id_b = 0;
    for (int i = 0; i < size; i++) {
        xorAll ^= nums[i];
    }
    int diffBit = xorAll & -xorAll;
    for (int i = 0; i < size; i++) {
        if(nums[i] & diffBit){
            id_a ^= nums[i];
        } else {
            id_b ^= nums[i];
        }
    }
    printf("These nums are %d %d\n", id_a, id_b);
}
```

**核心**：异或 (`^`)

- 归零律：`a ^ a = 0` （两个相同的数异或，结果为 0）。
- 恒等律：`a ^ 0 = a` （任何数与 0 异或，结果还是它自己）。
- 交换律和结合律：`a ^ b ^ c = a ^ c ^ b` （异或的顺序不影响结果）。

另外：
`&` 是 C 语言中的按位与（Bitwise AND）运算符，只有两个对应的位都是 1 时，结果才是 1；否则（只要有一个是 0），结果就是 0。  
类似还有一个：`|` （按位或，规则：有 1 就是 1，全是 0 才是 0）。

三者总结：
- `&`（与）：见 0 则 0。
- `|`（或）：见 1 则 1。
- `^`（异或）：一样则 0，不一样则 1。

下面进行分析：
```cpp
int xorAll = 0,id_a = 0,id_b = 0;
for (int i = 0; i < size; i++) {
    xorAll ^= nums[i];
}
```
这段代码将数组中所有的数字挨个进行异或操作。  
因为其他成对的数字异或后都变成了 0（被抵消了），所以 `xorAll` 最终的结果就是那两个唯一数字：
```text
xorAll = 3 ^ 3 ^ 5 = 5
xorAll = 2 ^ 3 ^ 1 ^ 4 ^ 5 ^ 3 ^ 2 ^ 1 = 4 ^ 5
```
只要 `xorAll` 不为 0，就说明这两个数不是同一个数。

**第二步**：
```cpp
int diffBit = xorAll & -xorAll;
```
这行代码是提取一个二进制数中最右侧的 1（最低位的 1）。  
在 `xorAll = 0001` 的例子中，`diffBit` 算出来就是 `0001`。  
为什么这么做？既然 `xorAll` 的某一位是 1，说明那两个“内鬼”在这一位上必然是一个为 0，一个为 1（因为如果一样，异或就是 0 了）。  
所以，我们可以用这个 `diffBit` 作为标准，把数组分成两拨。

**第三步**：
```cpp
for (int i = 0; i < size; i++) {
    if(nums[i] & diffBit){
        id_a ^= nums[i];  // 第一拨
    } else {
        id_b ^= nums[i];  // 第二拨
    }
}
```
程序再次遍历数组，根据 `nums[i] & diffBit` 的结果（也就是看当前数字在 `diffBit` 这一位上是 0 还是 1）将数字分成两组。

**关键点**：
那两个“内鬼”因为在这个位上一个 0 一个 1，所以必定会被分到不同的组里。  
数组中其他成对的数字，因为数值完全一样，所以必定会被分到同一个组里。  
现在，问题就被降维了：在每一组里，除了一个“内鬼”，其他数字全是成对出现的。

在各自的组内执行 `id_a ^= nums[i]`，成对的数字互相抵消变成 0，最后剩下的那个数字就是该组的“内鬼”。

**举例**：  
数组：`{2, 3, 4, 3, 2, 5}`  
全部异或：`xorAll = 2 ^ 3 ^ 4 ^ 3 ^ 2 ^ 5 = 4 ^ 5 = 1`  
取最低位 1：`diffBit = 1 & -1 = 1` （二进制 `0001`）

按 `diffBit` 分组（看末位是 0 还是 1）：
- `2 (0010)` -> 末位 0 -> 组 B
- `3 (0011)` -> 末位 1 -> 组 A
- `4 (0100)` -> 末位 0 -> 组 B (内鬼 1)
- `3 (0011)` -> 末位 1 -> 组 A
- `2 (0010)` -> 末位 0 -> 组 B
- `5 (0101)` -> 末位 1 -> 组 A (内鬼 2)

各组内部异或：
- 组 A：`3 ^ 3 ^ 5 = 5` （找到了 `id_a`）
- 组 B：`2 ^ 4 ^ 2 = 4` （找到了 `id_b`）

---

## NO.5. 你了解 argc 和 argv 吗，程序的输出是什么？为什么会这样?

```cpp
int main(int argc, char* argv[]) {
    printf("argc = %d\n", argc);
    while (argc++ > 0) {
        if(argc < 0){
            printf("argv[0] %s\n", argv[0]);
            break;
        }
    }
    printf("argc = %d\n", argc);
    return 0;
}
```

**说明**：
在 C/C++ 中，`main` 函数的这两个参数用于接收命令行参数：
- `argc` (`Argument Count`)：是一个整数，代表命令行参数的总个数。它至少为 1，因为程序自己的名字（执行路径）算作第一个参数。
- `argv` (`Argument Vector`)：是一个字符指针数组（`char*[]`），里面存放着具体的参数字符串。`argv[0]` 是程序名，`argv[1]` 是第一个参数，以此类推，`argv[argc]` 必须是 `NULL`。

**注意**！  
`argc` 和 `argv` 不是固定的关键字，它们只是程序员约定俗成的命名习惯。你完全可以把它们换成 `a` 和 `b`，甚至换成 `apple` 和 `banana`。  
在 C 语言中，`main` 函数的标准定义是：`int main(int 变量1, char *变量2[])`  
当然你可以这么写：`int main(void)` （在较新的编译器里面没有 `void` 有可能报错）。  
因为绝大多数我们在写作业、做算法题、或者开发普通软件时，根本不需要从终端给程序传递额外的参数。程序运行所需要的数据，通常是通过 `scanf`、文件读取或者网络请求获取的。

以作者环境为例：（在 Linux 终端上面依次输入：）
```bash
gcc 261019.3.c -o 261019.3  # 编译程序
./261019.3                  # 运行程序
```

**结果**：
```text
argc = 1
argv[0] ./261019.3
argc = -2147483648
```

**解释**：
我运行的执行命令只有一个参数：`./261019.3`  
所以：`argc = 1`

`argc = -2147483648` 怎么来的？  
我们知道：`argc = 1`，在下面的循环里面：
```c
while (argc++ > 0) {
    if(argc < 0){
        printf("argv[0] %s\n", argv[0]);
        break;
    }
}
```
看起来，`while` 不会停下来。  
但是！`int` 有上限！  
当 `argc` 超过 `int` 上限之后，发生“整数溢出” (`Integer Overflow`)！  
`argc` 是一个有符号的 `int` 类型（通常是 32 位）。当它一直加下去，超过了 `int` 能表示的最大正数 `2147483647` 时，它会再增加 1，直接变成负数 `-2147483648`（最小的负数）。  
此时 `argc < 0` 条件成立，执行：
```c
printf("argv[0] %s\n", argv[0]);
printf("argc = %d\n", argc);
```
输出结果：
```text
argv[0] ./261019.3      // 依个人实际情况而定
argc = -2147483648      // 固定
```

---

## NO.6. 说说下列操作是否合法，并解释原因

```cpp
struct P {
    int x;
    const int y;
};

int main() {
    struct P p1 = { 10, 20 }, p2 = { 30, 40 };
    const struct P p3 = { 50, 60 };
    struct P* const ptr1 = &p1;
    const struct P* ptr2 = &p2;
    const struct P* const ptr3 = &p3;
    return 0;
}
```

**操作如下**：
```c
ptr1->x = 100,      ptr2->x = 300,      ptr3->x = 500

ptr1->y = 200,      ptr1 = &p2,         ptr2->y = 400

ptr2 = &p1,         ptr3->y = 600,      ptr3 = &p1
```

**前置知识**：
看 `const` 在 `*` 的哪一边：
- `const` 在 `*` 左边：`const int *p`（不能改值，不能通过指针修改它指向的内容）。
- `const` 在 `*` 右边：`int * const p`（不能改址，指针本身的指向不能变，不能指向别人）。
- 两边都有：`const int * const p;`（既不能改值，也不能改址）。

**解释**：
在 C 语言中，一个指针 `p` 其实包含两个独立的东西：
1. 指针本身（地址）：即 `p` 这个变量里存的数字（比如 `0x7ffd...`），它指向哪块内存。
2. 指针指向的内容（值）：即顺着地址找过去，那个内存里存的真正数据（比如 `*p`）。

`const` 的作用就是给其中一个“上锁”。这个口诀的本质是看 `const` 紧挨着谁，它就去锁谁。

1. **`const` 在 `*` 左边：锁住“内容”（不能改值）**
   `const int *p;`  
   - 读法：`p` 是一个指针，指向一个 `const int`（常量整数）。  
   - 谁被锁了：值（`*p`）被锁了。  
   - 怎么理解：你顺着 `p` 的地址找过去，看到的数据是被 `const` 保护起来的。所以，你不能通过 `p` 去修改这个数据。  
   - 但是：`p` 本身没有加 `const`，所以 `p` 只是一个普通的指针变量。你可以让它指向别的地址（`p = &别的变量;` 是合法的）。

   示例：
   ```c
   int a = 10, b = 20;
   const int *p = &a;      
   *p = 100;               // 报错！家具不能砸（不能改值）
   p = &b;                 // 合法！可以换个房间（可以改地址）
   ```
   题目：`ptr2` 是 `const struct P* ptr2` -> `const` 在 `*` 左边 -> 锁死了值。

2. **`const` 在 `*` 右边：锁住“指针”（不能改址）**
   `int * const p;`  
   - 读法：`p` 是一个 `const` 指针，指向一个 `int`。  
   - 谁被锁了：指针本身（`p`）被锁了。  
   - 怎么理解：`p` 这个变量被 `const` 锁死了。它在初始化（指向某个地址）之后，就再也不能指向别人了。  
   - 但是：它指向的房间里的东西并没有被 `const` 保护。你可以随便修改 `*p` 的值。

   示例：
   ```c
   int a = 10, b = 20;
   int * const p = &a;
   *p = 100;    // 合法！可以改值
   p = &b;      // 报错！你被锁死在 a 身上了（不能改地址）
   ```
   题目：`ptr1` 是 `struct P* const ptr1` -> `const` 在 `*` 右边 -> 锁死了地址。

3. **两边都有 `const`：双重上锁（既不能改值，也不能改址）**
   `const int * const p;`  
   - 读法：`p` 是一个 `const` 指针，指向一个 `const int`。  
   - 谁被锁了：全锁了。`p` 不能指向别人，也不能通过 `p` 修改房间里的东西。

   题目：`ptr3` 是 `const struct P* const ptr3 = &p3;`（值与地址都不能变）。

**题目代码**：
```c
struct P {
    int x;          // x 是普通的 int
    const int y;    // 但 y 是被 const 修饰的只读常量
};
```
所以 `p1`, `p2` 当中的 `x` 可以变，`y` 不行。  
`const struct P p3 = { 50, 60 };` // `p3` 为只读，`x`, `y` 都变不了。

**分析这九个语句**：
1. `ptr1->x = 100;` **[合法]**：`ptr1` 是 `struct P* const`（指针是常量，不能变地址）。指针不能指向别人，但它指向的 `p1` 里的 `x` 是普通的 `int`，所以可以随便改。
2. `ptr2->x = 300;` **[不合法]**：`ptr2` 是 `const struct P*`（指向常量的指针，不能变值）。虽然 `p2.x` 本身不是常量，但通过 `ptr2` 这个指针看过去，整个结构体都是只读的。你不能通过它修改任何成员。
3. `ptr3->x = 500;` **[不合法]**：`ptr3` 是 `const struct P* const`（值与地址都不能改）。既指向常量，指针本身也是常量。绝不能改。
4. `ptr1->y = 200;` **[不合法]**：结构体内部的 `y` 是用 `const int y;` 声明的。它是一个只读常量，在初始化（`{10, 20}`）之后，无论在什么情况下都不能被修改。编译器会直接拦截。
5. `ptr1 = &p2;` **[不合法]**：`ptr1` 是 `struct P* const`（常量指针）。它的初始指向是 `&p1`，一辈子都不能变，不能让它去指 `p2`。
6. `ptr2->y = 400;` **[不合法]**：理由和 `ptr1->y = 200` 一样，`y` 本身就是不可变的常量。更何况 `ptr2` 还是一个指向常量的指针（双重禁止）。
7. `ptr2 = &p1;` **[合法]**：`ptr2` 是 `const struct P*`（指向常量的指针）。虽然不能通过它改值，但指针本身是可以移动的（改地址是合法的）。你可以让它重新指向 `p1`。
8. `ptr3->y = 600;` **[不合法]**：还是老规矩，`y` 是常量。再加上 `ptr3` 是双重 `const`，绝对碰不得。
9. `ptr3 = &p1;` **[不合法]**：`ptr3` 是 `const struct P* const`。不仅指向的内容是只读的，指针自己的地址也是锁死的。不能重新赋值。

**总结**：

| 操作语句 | 是否合法 | 核心原因（违反了什么规则？） |
| :--- | :--- | :--- |
| `ptr1->x = 100` | **合法** | `ptr1` 指向的 `x` 是普通变量，可改。 |
| `ptr2->x = 300` | 非法 | `ptr2` 有 `const` 限制（左定值），不能通过它改内容。 |
| `ptr3->x = 500` | 非法 | `ptr3` 双重 `const`，不能改内容。 |
| `ptr1->y = 200` | 非法 | 成员 `y` 本身是 `const`，终生不可改。 |
| `ptr1 = &p2` | 非法 | `ptr1` 是常量指针（右定址），不能改指向。 |
| `ptr2->y = 400` | 非法 | 成员 `y` 本身是 `const`。 |
| `ptr2 = &p1` | **合法** | `ptr2` 允许改变指向（仅仅是不能通过它改值）。 |
| `ptr3->y = 600` | 非法 | 成员 `y` 是 `const`，且 `ptr3` 双重 `const`。 |
| `ptr3 = &p1` | 非法 | `ptr3` 是常量指针，不能改指向。 |

---

## NO.7. 在主函数中定义如下变量

```c
int main() {
    int a[3] = { 2, 4, 8 };
    int(*b)[3] = &a;
    int* c[3] = { a, a + 1, a + 2 };
    int (*f1(int))(int*, int);
    return 0;
}
```

先来看代码：
- `int a[3] = {2, 4, 8};` // 一个包含 3 个 `int` 的数组。
- `int (*b)[3] = &a;` // 数组指针。`b` 是一个指针，指向“包含 3 个 `int` 的数组”。目前它指向整个数组 `a`。
- `int* c[3] = {a, a+1, a+2};` // 指针数组。`c` 是一个数组，里面有 3 个元素，每个元素都是一个 `int*` 指针。
- `int (*f1(int))(int*, int);` // 函数声明。`f1` 是一个函数，它接收一个 `int` 参数，返回值是一个函数指针。被返回的那个函数接收 `(int*, int)`，返回 `int`。

**逐个表达式解析**：

1. `a`  
   是什么：数组名。在大多数表达式中，数组名会退化（`Decay`）为指向首元素的指针。  
   值/类型：它等价于 `&a[0]`，类型是 `int*`，指向数组的第一个元素（即数字 2 的地址，不妨假设为 `0x1000`）。

2. `*b`  
   是什么：`b` 是数组指针，`*b` 就是把 `&a` 解引用，拿回了整个数组 `a`。  
   值/类型：它代表整个数组。在表达式中，它会退化指向首元素的指针。所以，`*b` 的地址值和 `a` 是一模一样的，类型也是 `int*`。

3. `*b + 1`  
   是什么：由于 `*b` 退化为 `a`（`int*` 类型），`+1` 就是指针向后移动 1 个 `int`。  
   值/类型：它指向第二个元素（数字 4）。如果 `a` 是 `0x1000`，那么 `*b + 1` 就是 `0x1004`。

4. `b`  
   是什么：数组指针本身，指向整个数组 `a`。  
   值/类型：地址值等于 `0x1000`，但类型是 `int (*)[3]`。注意：`b` 和 `a` 的地址值虽然一样，但类型不同（步长不同）。

5. `b + 1`  
   是什么：指针算术运算。`b` 指向的是“大小为 3 个 `int` 的数组”。所以 `b + 1` 会跨过一整个数组（`3 * 4 = 12` 个字节）。  
   值/类型：如果 `b` 是 `0x1000`，那么 `b + 1` 就是 `0x1000 + 12 = 0x100C`（这个地址已经越界了，在数组 `a` 的屁股后面）。

6. `*(*b + 1)`  
   是什么：由内向外剥洋葱。`*b` 等价于 `a`。所以表达式变成 `*(a + 1)`。  
   值/类型：`a + 1` 是指向第二个元素的指针，再解引用 `*`，就是取出了第二个元素的值。结果就是整数 4。

7. `c`  
   是什么：指针数组 `c` 的数组名。  
   值/类型：和 `a` 一样，数组名退化为指向首元素的指针。`c` 的第一个元素是 `a`（是一个 `int*`）。所以 `c` 退化为指向 `c[0]` 的指针，类型是 `int**`（二级指针）。

8. `sizeof(a)`  
   是什么：`sizeof` 对数组名不会发生退化，它计算的是整个数组占用的内存大小。  
   值：3 个 `int`，每个 4 字节，所以结果是 12。

9. `sizeof(b)`  
   是什么：`b` 是一个指针变量。在 64 位系统中，指针永远是 8 个字节。  
   值：8。

10. `sizeof(&a)`  
    是什么：`&a` 取的是整个数组的地址。它也是一个指针。  
    值：只要是 64 位系统下的指针，就是 8。（哪怕它的类型是 `int (*)[3]`）。

11. `sizeof(f1)`  
    是什么：`f1` 是一个函数名，虽然它只是一个声明。在 C 语言中，对函数名使用 `sizeof` 是不合法的（会编译报错：`invalid application of 'sizeof' to a function type`）。  
    值：编译错误。

### 汇总表（64 位系统）

| 表达式 | 类型 | 代表的内容 / 实际值 (假设 a 在 0x1000) |
| :--- | :--- | :--- |
| `a` | `int*` | 数组首元素地址（`0x1000`） |
| `*b` | `int*` (退化) | 数组首元素地址（`0x1000`） |
| `*b + 1` | `int*` | 第二个元素地址（`0x1004`） |
| `b` | `int (*)[3]` | 整个数组的地址（`0x1000`） |
| `b + 1` | `int (*)[3]` | 数组末尾后方的地址（`0x100C`） |
| `*(*b + 1)` | `int` | 第二个元素的值：`4` |
| `c` | `int**` (退化) | 指针数组首元素 `c[0]` 的地址 |
| `sizeof(a)` | `size_t` | `12`（3 * 4 字节） |
| `sizeof(b)` | `size_t` | `8`（指针大小） |
| `sizeof(&a)` | `size_t` | `8`（指针大小） |
| `sizeof(f1)` | 无 | **编译报错**（不能对函数使用 `sizeof`） |



# NO.8. 观察程序输出，思考为什么？

```c
int g;
int func() {
    static int j = 98;
    j += g;
    return j;
}
int main() {
    g += 3;
    char arr[6] = {};
    arr[1] = func();
    arr[0] = func();
    arr[2] = arr[3] = func() + 1;
    arr[4] = func() + 1;
    printf("%s linux\n", arr);
    return 0;
}
```

### 前置知识：
`static` // 特殊定义

如果我们这样写：
```c
int func() {
    int j = 98;
    j += g;
    return j;
}
```
`j` 只能在 `func` 函数内部使用，但是它每一次用都会重复定义、销毁。

如果这样写：
```c
int j = 98;
int func() {
    j += g;
    return j;
}
```
`j` 不会重复定义了，但是它不止能在 `func` 内部使用了，它变成了普通全局变量。  
*PS: 对于本题来说，这样写结果相同，但是如果程序变得复杂，这样写会有些危险。*

而这种写法：
```c
int func() {
    static int j = 98;
    j += g;
    return j;
}
```
- **作用域不变**：它依然只能在 `func` 函数内部使用。
- **生命周期改变（变成全局）**：普通的局部变量在函数执行完毕后，内存就销毁了（在栈上）。但加了 `static` 后，变量 `j` 会被移到全局区（数据段），它会一直存活，直到整个程序结束才被销毁。
- **只初始化一次**：`static int j = 98;` 这句代码在程序运行期间只会执行一次。下次再调用 `func` 时，这句代码会被直接跳过，`j` 保留着上次计算后的值。

---

### 回到本题分析：

1. **第一次调用**：
   ```c
   arr[1] = func();
   ```
   进入函数：`j += g`，即 `98 + 3 = 101`。此时全局的 `j` 被改写为 `101`。  
   返回 `101`（ASCII 码对应的字符为 `'e'`）。赋值给 `arr[1]`。

2. **第二次调用**：
   ```c
   arr[0] = func();
   ```
   进入函数：`j` 不是 `98`，它是上次存的 `101`，不会二次定义。  
   `j += g` 即 `101 + 3 = 104`。  
   返回 `104`（ASCII 码对应的字符为 `'h'`）。赋值给 `arr[0]`。

3. **第三次调用**：
   ```c
   arr[2] = arr[3] = func() + 1;
   ```
   进入函数：`j += g` 即 `104 + 3 = 107`。  
   函数返回 `107`。  
   注意 `+ 1`：`func() + 1` 结果是 `108`（ASCII 码对应的字符为 `'l'`）。  
   通过连续赋值（从右往左），`arr[3]` 和 `arr[2]` 都变成了 `108`。

4. **第四次调用**：
   ```c
   arr[4] = func() + 1;
   ```
   进入函数：`j += g` 即 `107 + 3 = 110`。  
   函数返回 `110`。  
   加上外面的 `+ 1`，结果是 `111`（ASCII 码对应的字符为 `'o'`）。  
   赋值给 `arr[4]`。

### 结果：
组成字符串：
```text
"hello"
```
输出：
```text
hello linux
```

---

# NO.9. 观察程序结果，说说程序运行的过程

```c
#define CALL_MAIN(main, x) (*(int (*)(int))*main)(x);
#define DOUBLE(x) 2 * x
int (*registry[1])(int);
int main(int argc) {
    if (argc > 2e3) return 0;
    printf("%d ", argc + 1);
    *registry = (int(*)(int))main;
    CALL_MAIN(registry, DOUBLE(argc + 1));
    return 0;
}
```

### 分析：
1. **关于 `int (*registry[1])(int);`**  
   这是一个“函数指针数组”。  
   *验证*：由于 `[]` 的优先级高于 `*`，所以 `registry` 首先是个包含 1 个元素的数组。数组里的元素是 `int (*)(int)` 类型的函数指针。  
   *关于 `*registry`*：在 C 语言中，数组名在表达式中会退化为指向首元素的指针。所以 `registry` 等价于 `&registry[0]`，而 `*registry` 就是解引用，等价于 `registry[0]`。

2. **关于 `#define DOUBLE(x) 2 * x`**  
   经典的宏替换陷阱。  
   *验证*：宏定义只是无脑的文本替换。当你传入 `argc + 1` 时，预处理器直接把 `x` 替换掉，变成了 `2 * argc + 1`。  
   *结果*：当 `argc = 1` 时，`2 * 1 + 1 = 3`。

3. **关于 `#define CALL_MAIN(main, x) (*(int (*)(int))*main)(x);`**  
   极其复杂的类型强转与调用。  
   `main` 是形参名（实际调用时传入了 `registry` 数组名）。  
   `*main`：因为 `registry` 本质是指针，`*registry` 就是取出了数组里的那个函数指针。  
   `(int (*)(int))*main`：把取出来的指针，强制转换成一个“接收 `int` 返回 `int`”的函数指针类型。  
   最外层还有一个 `*`，意思是“解引用这个函数指针，拿到函数本身”。  
   最后加上 `(x)`，意思是“传入参数 `x` 并执行它”。

---

### 运行过程推演：

#### 核心计算公式
- **屏幕打印值** = `当前 argc + 1`
- **下一层递归传入值** = `2 * 当前 argc + 1`

#### 执行过程推演表

| 轮次 (递归深度) | 刚进入函数时的 `argc` | 判定 `argc > 2000` | 屏幕打印内容 (`argc + 1`) | 计算下一层参数 (`2 * argc + 1`) | 动作状态 |
| :---: | :---: | :---: | :---: | :---: | :--- |
| **第 1 轮** | `1` | 假 (1 不大于 2000) | **`2 `** | `2 * 1 + 1 = 3` | 带着 `3` 调用下一层 |
| **第 2 轮** | `3` | 假 | **`4 `** | `2 * 3 + 1 = 7` | 带着 `7` 调用下一层 |
| **第 3 轮** | `7` | 假 | **`8 `** | `2 * 7 + 1 = 15` | 带着 `15` 调用下一层 |
| **第 4 轮** | `15` | 假 | **`16 `** | `2 * 15 + 1 = 31` | 带着 `31` 调用下一层 |
| **第 5 轮** | `31` | 假 | **`32 `** | `2 * 31 + 1 = 63` | 带着 `63` 调用下一层 |
| **第 6 轮** | `63` | 假 | **`64 `** | `2 * 63 + 1 = 127` | 带着 `127` 调用下一层 |
| **第 7 轮** | `127` | 假 | **`128 `** | `2 * 127 + 1 = 255` | 带着 `255` 调用下一层 |
| **第 8 轮** | `255` | 假 | **`256 `** | `2 * 255 + 1 = 511` | 带着 `511` 调用下一层 |
| **第 9 轮** | `511` | 假 | **`512 `** | `2 * 511 + 1 = 1023` | 带着 `1023` 调用下一层 |
| **第 10 轮** | `1023` | 假 | **`1024 `** | `2 * 1023 + 1 = 2047` | 带着 `2047` 调用下一层 |
| **第 11 轮** | **`2047`** | **真 (2047 > 2000)** | *(无输出)* | *(不计算)* | **触发 `return 0;` 开始退栈** |

---

# NO.10. 本题要求你编写以下函数，不能改动 `main` 函数里的代码。实现对 `arr1` 和 `arr2` 的拼接、排序和去重。你需要自行定义 `result` 结构体并使用 `malloc` 手动开辟内存。

```c
int main() {
    int arr1[] = { 6, 1, 2, 1, 9, 1, 3, 2, 6, 2 };
    int arr2[] = { 4, 2, 2, 1, 6, 2 };
    int len1 = sizeof(arr1) / sizeof(arr1[0]);
    int len2 = sizeof(arr2) / sizeof(arr2[0]);

    struct result result;
    your_concat(arr1, len1, arr2, len2, result);
    print_result(result);
    your_sort(result);
    print_result(result);
    your_dedup(result);
    print_result(result);
    free(result.arr);
    return 0;
}
```

### 解答：
```c
#include <stdio.h>
#include <stdlib.h>

// 1. 定义 result 结构体
struct result {
    int *arr;
    int len;
};

// 2. 实际执行拼接的内部函数（接收结构体指针以修改原数据）
void _concat(const int *arr1, int len1, const int *arr2, int len2, struct result *res) {
    res->len = len1 + len2;
    res->arr = (int *)malloc(res->len * sizeof(int));
    for (int i = 0; i < len1; i++) {
        res->arr[i] = arr1[i];
    }
    for (int i = 0; i < len2; i++) {
        res->arr[len1 + i] = arr2[i];
    }
}

// 3. 实际执行排序的内部函数（冒泡排序示例）
void _sort(struct result *res) {
    for (int i = 0; i < res->len - 1; i++) {
        for (int j = 0; j < res->len - i - 1; j++) {
            if (res->arr[j] > res->arr[j + 1]) {
                int temp = res->arr[j];
                res->arr[j] = res->arr[j + 1];
                res->arr[j + 1] = temp;
            }
        }
    }
}

// 4. 实际执行去重的内部函数（前提是数组已经有序）
void _dedup(struct result *res) {
    if (res->len <= 1) return;
    
    int write_index = 0;
    for (int i = 1; i < res->len; i++) {
        if (res->arr[i] != res->arr[write_index]) {
            write_index++;
            res->arr[write_index] = res->arr[i];
        }
    }
    res->len = write_index + 1;
    res->arr = (int *)realloc(res->arr, res->len * sizeof(int));        // 缩减内存空间
}

// 5. 打印结果函数
void print_result(struct result res) {
    printf("{ ");
    for (int i = 0; i < res.len; i++) {
        printf("%d ", res.arr[i]);
    }
    printf("}\n");
}

// 6. 通过宏定义拦截 main 中的函数调用，自动取地址传入
#define your_concat(a1, l1, a2, l2, res) _concat(a1, l1, a2, l2, &(res))
#define your_sort(res)                   _sort(&(res))
#define your_dedup(res)                  _dedup(&(res))

// --- 以下为原封不动的 main 函数 ---
int main() {
    int arr1[] = { 6, 1, 2, 1, 9, 1, 3, 2, 6, 2 };
    int arr2[] = { 4, 2, 2, 1, 6, 2 };
    int len1 = sizeof(arr1) / sizeof(arr1[0]);
    int len2 = sizeof(arr2) / sizeof(arr2[0]);

    struct result result;
    your_concat(arr1, len1, arr2, len2, result);
    print_result(result);
    your_sort(result);
    print_result(result);
    your_dedup(result);
    print_result(result);
    free(result.arr);
    return 0;
}
```

### 解析：
这道题的核心难点在于 `main` 函数的这一句：
```c
your_concat(arr1, len1, arr2, len2, result);
```
这里传入的是 `result`，不是 `&result`。  
这意味着如果你直接写个同名函数，函数里拿到的只是个副本。  
你在里面 `malloc` 分配了内存，函数一结束，副本销毁，`main` 里的 `result` 依然是个未初始化的野指针。

**怎么解决？**
```c
// 利用宏定义拦截
#define your_concat(a1, l1, a2, l2, res) _concat(a1, l1, a2, l2, &(res))
#define your_sort(res)                   _sort(&(res))
#define your_dedup(res)                  _dedup(&(res))
```
当编译器在 `main` 函数里看到：
```c
your_concat(arr1, len1, arr2, len2, result)
```
时，因为 `"your_concat"` 是个宏，预处理器（Preprocessor）在编译前直接进行了暴力文本替换。  
替换后，`main` 里的代码实际上变成了：
```c
_concat(arr1, len1, arr2, len2, &result); // 注意这里加了 &
```
虽然不能修改 `main` 函数的代码，但宏定义在编译前强行塞了一个 `&` 进去。  
内部函数拿到了 `result` 的真实内存地址，所有的操作都能落到实处了。

**补充结构体定义**：
```c
struct result {     // 补充 main 函数缺失的结构体
    int *arr; 
    int len;  
};                  // 按照题目要求，我们需要一个指针来指向 malloc 出来的数组，以及一个变量记录长度。

// 补充定义内部真实的函数，注意最后一个参数是结构体指针 struct result *res
void _concat(..., struct result *res) { ... }
void _sort(struct result *res) { ... }
void _dedup(struct result *res) { ... }
```

**拼接部分**：
```c
void _concat(const int *arr1, int len1, const int *arr2, int len2, struct result *res) // const 保证只读，确保内容不被修改
{
    res->len = len1 + len2; // 1. 算出总长度 10 + 6 = 16
    res->arr = (int *)malloc(res->len * sizeof(int)); // 2. 在堆区申请 16 个 int 的空间
    
    for (int i = 0; i < len1; i++) {
        res->arr[i] = arr1[i]; // 3. 把 arr1 的数据拷进去
    }
    for (int i = 0; i < len2; i++) {
        res->arr[len1 + i] = arr2[i]; // 4. 把 arr2 的数据拷进去
    }
}
```

**排序用冒泡即可（数据少，冒泡简单速度快）**：
```c
void _sort(struct result *res) {
    // 经典冒泡排序
    for (int i = 0; i < res->len - 1; i++) {
        for (int j = 0; j < res->len - i - 1; j++) {
            if (res->arr[j] > res->arr[j + 1]) {
                // 交换相邻元素
                int temp = res->arr[j];
                res->arr[j] = res->arr[j + 1];
                res->arr[j + 1] = temp;
            }
        }
    }
}
```

**去重**：
```c
void _dedup(struct result *res) 
{
    if (res->len <= 1) return;
    int write_index = 0;
    for (int i = 1; i < res->len; i++) {
        // 如果发现新数，就把它写到前面
        if (res->arr[i] != res->arr[write_index]) {
            write_index++;
            res->arr[write_index] = res->arr[i];
        }
    }
    res->len = write_index + 1; // 更新长度
    res->arr = (int *)realloc(res->arr, res->len * sizeof(int));    // 释放多余的堆内存
}
```
*去重逻辑*：利用排序后的特性（相同的数挨在一起）。`write_index` 指向有效数组的末尾。用 `i` 往后扫描，遇到不一样的数，就把它挪到 `write_index + 1` 的位置。  
执行完后，数组前面的内容变成了：`{1, 2, 3, 4, 6, 9}`，长度 `len` 变为了 6。

`realloc`：原本 `malloc` 申请了 16 个 `int` 的空间，现在只剩 6 个有效数据。`realloc` 会把原本的内存块截断（或者重新找一块 6 个大小的内存），把多余的 10 个 `int` 空间还给操作系统。

**最后输出即可**：
```c
void print_result(struct result res)    // 这里传值，因为不用返回，只读
```

---

# NO.11. 用你智慧的眼睛，透过这指针魔法的表象，看清其本质：

```c
void magic(int(*pa)[6], int** pp) {
    **pp += (*pa)[2];
    *pp = (*pa) + 5;
    **pp -= (*pa)[0];
    *pp = (*pa) + ((*(*pa + 3) & 1) ? 3 : 1);
    *(*pp) += *(*pp - 1);
    *pp = (*pa) + 2;
}
int main() {
    int a[6] = { 2, 4, 6, 8, 10, 12 };
    int* p = a + 1, **pp = &p;
    magic(&a, pp);
    printf("%d %d\n%d %d %d\n%d %d\n", *p, **pp, a[1], a[2], a[3], a[5], p - a);
    return 0;
}
```

### 一步一步分析：

#### 初始状态
`a[0]=2, a[1]=4, a[2]=6, a[3]=8, a[4]=10, a[5]=12` // `a` 数组  
`p = a + 1` // `p` 指向 `a[1]`（即数字 4 的地址）。  
`pp = &p` // `pp` 是二级指针，指向 `p` 的地址。

#### 调用时：
在这个 `magic` 函数的调用中，传递的全是指针（地址）。
```c
magic(&a, pp);
```
- **第一个参数：`&a`**  
  传递的是整个数组 `a` 的首地址。  
  接收端：函数定义是 `void magic(int(*pa)[6], int** pp)`，这是一个数组指针。它告诉函数：“我接收一个地址，这个地址指向一块内存，这块内存里装的是 6 个 `int` 类型的数据。”
- **第二个参数：`pp`**  
  `pp` 在 `main` 函数里被定义为 `int** pp = &p;`（二级指针）。它传递的是指针变量 `p` 的内存地址。  
  接收端：`int** pp` 接收了这个二级指针。

---

#### `magic` 函数内部执行过程：

1. `**pp += (*pa)[2];`
   - **拆解**：
     - `**pp` 即 `*(*pp)`。因为 `pp = &p`，所以 `*pp` 就是 `p`，`**pp` 就是 `*p`，即 `a[1]` 的值（当前为 4）。
     - `(*pa)[2]`：`pa` 是指向整个数组的指针（`&a`）。`*pa` 就是数组首地址 `a`。`(*pa)[2]` 等价于 `a[2]`，其值为 6。
   - **操作**：`4 += 6`，即 `a[1]` 的值变为 **10**。
   - **状态**：数组变为 `{2, 10, 6, 8, 10, 12}`，`p` 依然指向 `a[1]`。

2. `*pp = (*pa) + 5;`
   - **拆解**：
     - `*pp` 也就是 `p`。
     - `(*pa) + 5`：`*pa` 是首地址 `a`，`+5` 就是地址向后偏移 5 个 `int`，即指向 `a[5]`。
   - **操作**：修改指针 `p` 的指向，使其指向 `a[5]`。
   - **状态**：`p` 指向 `a[5]`（数字 12 的地址）。

3. `**pp -= (*pa)[0];`
   - **拆解**：
     - `**pp` 就是现在的 `*p`，即 `a[5]` 的值（当前为 12）。
     - `(*pa)[0]` 等价于 `a[0]`，值为 2。
   - **操作**：`12 -= 2`，即 `a[5]` 的值变为 **10**。
   - **状态**：数组变为 `{2, 10, 6, 8, 10, 10}`。

4. `*pp = (*pa) + ((*(*pa + 3) & 1) ? 3 : 1);`
   - **拆解（从内向外）**：
     - `*pa + 3`：`*pa` 是首地址，`+3` 后指向 `a[3]`。
     - `*(*pa + 3)`：取出 `a[3]` 的值，也就是 8。
     - `8 & 1`：二进制 `1000 & 0001`，结果为 0。
     - `0 ? 3 : 1`：三目运算符，条件为假，结果取 1。
     - `(*pa) + 1`：首地址 `a` 偏移 1，指向 `a[1]`。
   - **操作**：修改指针 `p`，使其重新指向 `a[1]`。
   - **状态**：`p` 指向 `a[1]`。

5. `*(*pp) += *(*pp - 1);`
   - **拆解**：
     - `*(*pp)` 即 `*p`，也就是 `a[1]` 的值（当前为 10）。
     - `*(*pp - 1)`：`*pp` 就是 `p`，`p - 1` 就是向前移动一个 `int`，也就是指向 `a[0]`。再解引用 `*` 取值，也就是 `a[0]` 的值（当前为 2）。
   - **操作**：`a[1] += a[0]` 即 `10 += 2`，`a[1]` 的值变为 **12**。
   - **状态**：数组变为 `{2, 12, 6, 8, 10, 10}`。

6. `*pp = (*pa) + 2;`
   - **操作**：修改指针 `p`，使其指向 `a[2]`。
   - **状态**：`p` 指向 `a[2]`。

---

#### 最终内存与指针状态
- `a[6] = {2, 12, 6, 8, 10, 10}`
- 指针 `p`：指向 `a[2]`
- 二级指针 `pp`： statically 指向 `p`

#### 输出推导汇总表：

| 序号 | 打印表达式 | 解析与推导过程 | 最终值 |
| :---: | :--- | :--- | :---: |
| 1 | `*p` | `p` 经过魔法函数内部的多次修改，最终指向了 `a[2]`，解引用取得其值。 | **6** |
| 2 | `**pp` | `pp` 指向 `p`，`*pp` 就是 `p` 本身，再解引用 `*p`，与第 1 项完全等价。 | **6** |
| 3 | `a[1]` | 在魔法第 1 步和第 5 步被累加修改，`4 + 6 = 10`，然后 `10 + 2 = 12`。 | **12** |
| 4 | `a[2]` | 虽被指针多次指向，但魔法函数内没有对其进行赋值修改，保留原始值。 | **6** |
| 5 | `a[3]` | 魔法第 4 步中仅读取了它（用于三目运算），并未修改其值，保留原始值。 | **8** |
| 6 | `a[5]` | 在魔法第 2 步被指向，在第 3 步被修改：原值为 12，减去 `a[0]` (2) 得 10。 | **10** |
| 7 | `p - a` | 指针相减，计算相隔的元素个数。`p` 指向 `a[2]`，`a` 指向 `a[0]`，距离为 2。 | **2** |

#### 控制台最终输出：
```text
6 6
12 6 8
10 2
```

---

# NO.12. 你能看明白这个程序怎样运行吗？试着理解这个程序吧！

```c
union data {
    void**** p;
    char arr[20];
};
typedef struct node {
    int a;
    union data b;
    void (*use)(struct node* n);
    char string[0];
} Node;
void func2(Node* node);

void func1(Node* node) {
    node->use = func2;
    printf("%s\n", node->string);
}
void func2(Node* node) {
    node->use = func1;
    printf("%d\n", ++(node->a));
}
int main() {
    const char* s = "Your journey begins here!";
    Node* P = (Node*)malloc(sizeof(Node) + (strlen(s) + 1) * sizeof(char));
    strcpy(P->string, s);
    P->use = func1;
    P->a = sizeof(Node) * 50 + sizeof(union data);
    while (P->a < 2028) {
        P->use(P);
    }
    free(P);
    return 0;
}
```

### 运行结果：
```text
Your journey begins here!
2025
Your journey begins here!
2026
Your journey begins here!
2027
Your journey begins here!
2028
```

### 怎么做到的？
本程序综合运用了 **结构体（struct）**、**联合体（union）**、**函数指针（Function Pointer）**、**柔性数组（Flexible Array Member）** 以及 **动态内存分配（malloc）**。通过这些特性的结合，程序在底层实现了一个简单的 **有限状态机（Finite State Machine）**。

---

## 一、 结构体与联合体的内存布局 (Memory Layout)

在常见的 64 位系统下，数据结构的内存对算与空间占用如下：

```c
union data {
    void**** p;       // 四级指针，占用 8 字节
    char arr[20];     // 字符数组，占用 20 字节
};
```
- **`union data` 的大小**：由最大成员决定。`char arr[20]` 占 20 字节，但在 64 位系统按 8 字节对齐的要求下，实际占用空间向上补充至 **24 字节**。

```c
typedef struct node {
    int a;                       // 占 4 字节 (+ 4 字节填充对齐)
    union data b;                // 占 24 字节
    void (*use)(struct node* n); // 函数指针，占 8 字节
    char string[0];              // 柔性数组，不占结构体本身空间
} Node;
```
- **`Node` 的总大小**：
  - `int a` (4 字节) + 填充 (4 字节) = **8 字节**
  - `union data b` = **24 字节**
  - `void (*use)(...)` = **8 字节**
  - **结构体大小为**：`8 + 24 + 8 = 40 字节`。

---

## 二、 动态内存与柔性数组 (Flexible Array Member)

在 `main` 函数中：
```c
const char* s = "Your journey begins here!";
Node* P = (Node*)malloc(sizeof(Node) + (strlen(s) + 1) * sizeof(char));
```
1. **内存分配逻辑**：
   - `sizeof(Node)` 分配了结构体主体的 40 字节。
   - `(strlen(s) + 1)` 额外分配了 26 字节（25 个字符 + 1 个结尾符 `\0`）。
2. **柔性数组的作用**：
   - `char string[0];` 位于结构体末尾，在 `malloc` 连同额外空间一起申请后，`string` 自动指向紧跟在结构体后面的这块动态区域。`strcpy(P->string, s);` 将字符串顺利写入其中。

---

## 三、 初始值计算与循环条件

```c
P->a = sizeof(Node) * 50 + sizeof(union data);
```
- **初始值推导**：$40 \times 50 + 24 = 2000 + 24 = 2024$。
- **循环判断**：`while (P->a < 2028)`
  - 当 `P->a` 依次增至 2025、2026、2027、2028 时，循环判定并在达到 2028 时终止。

---

## 四、 状态机切换与执行轨迹

程序的逻辑核心在于两个交替修改函数指针的函数：

- **`func1`**：把 `node->use` 设置为 `func2`，并打印字符串。
- **`func2`**：把 `node->use` 设置为 `func1`，将 `node->a` 自增 1，并打印当前数字。

### 详细执行流程：

1. **初始状态**：`P->a = 2024`，`P->use` 指向 `func1`。
2. **第 1 次循环**：
   - 调用 `func1` $\rightarrow$ 输出 `Your journey begins here!`
   - `P->use` 被切换为 `func2`。
3. **第 2 次循环**：
   - 调用 `func2` $\rightarrow$ `P->a` 自增为 **2025**，输出 `2025`
   - `P->use` 被切换为 `func1`。
4. **第 3 次循环**：
   - 调用 `func1` $\rightarrow$ 输出 `Your journey begins here!`
   - `P->use` 被切换为 `func2`。
5. **第 4 次循环**：
   - 调用 `func2` $\rightarrow$ `P->a` 自增为 **2026**，输出 `2026`
   - `P->use` 被切换为 `func1`。
6. **后续循环**：按此规律交替，直到 `P->a` 在 `func2` 中自增到 **2028**。
7. **退出循环**：`P->a < 2028` 不再满足，退出 `while` 循环并通过 `free(P)` 释放内存。

---


至此，Xiyou_Linux_Group 2025 年的招新题目就已经完成了，谢谢你的阅读。  
如有错误，请指出，谢谢！

---

