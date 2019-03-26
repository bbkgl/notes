# std::bind和std::function以及回调函数

---

> 这篇文章主要讲如何结合std::bind以及std::function实现需要继承才能实现的功能。
>
> 参考：[现代C++的回调技术--使用std::bind和std::function](https://blog.csdn.net/will_free/article/details/61622122)

在不继承的情况下，在类A中运行类B的成员函数。

## 类A

类A中可以运行B的成员函数

**声明仿函数类型**

```cpp
using CallBack1 = std::function<int(int, int)>;    // 有参数
using CallBack2 = std::function<void()>;           // 无参数
using CallBack3 = std::function<void()>;
```

**定义仿函数变量，类似于函数指针**

```cpp
CallBack1 cb1_;
CallBack2 cb2_;
CallBack3 cb3_;
```

**给仿函数赋值**

```cpp
void SetCall1(CallBack1 cb1) {cb1_ = cb1; };
void SetCall2(CallBack2 cb2) {cb2_ = cb2; };
void SetCall3(CallBack3 cb3) {cb3_ = cb3; };
```

**运行B的成员函数**

```cpp
void run()
{
    printf("%d\n", cb1_(1, 2));

    cb2_();

    cb3_();
}
```

## 类B

类B定义要在A中运行的回调函数，再通过bind绑定，传给A的对象运行。

```cpp
class B
{
public:
    static void Prints()
    {
        printf("I am B\n");
    }

    void Print()
    {
        printf("I am B\n");
    }

    int Add(int x, int y)
    {
        return x + y;
    }

    void run()
    {
        std::shared_ptr<A> a = std::make_shared<A>();
        a->SetCall1(std::bind(&B::Add, this, 1, 2));
        a->SetCall2(std::bind(&B::Print, this));
        a->SetCall3(std::bind(&B::Prints));
        a->run();
    }
};
```

要注意的是，绑定静态函数不需要将对象传入，非静态函数需要将当前对象指针传入。

```cpp
a->SetCall2(std::bind(&B::Print, this));    // 非静态函数绑定
a->SetCall3(std::bind(&B::Prints));         // 静态函数绑定
```

**测试用例：**

```cpp
#include <iostream>
#include <functional>
#include <memory>

class A
{
public:

    using CallBack1 = std::function<int(int, int)>;
    using CallBack2 = std::function<void()>;
    using CallBack3 = std::function<void()>;

    void SetCall1(CallBack1 cb1) {cb1_ = cb1; };
    void SetCall2(CallBack2 cb2) {cb2_ = cb2; };
    void SetCall3(CallBack3 cb3) {cb3_ = cb3; };

    void run()
    {
        printf("%d\n", cb1_(1, 2));

        cb2_();

        cb3_();
    }

    CallBack1 cb1_;
    CallBack2 cb2_;
    CallBack3 cb3_;
};

class B
{
public:
    static void Prints()
    {
        printf("I am B\n");
    }

    void Print()
    {
        printf("I am B\n");
    }

    int Add(int x, int y)
    {
        return x + y;
    }

    void run()
    {
        std::shared_ptr<A> a = std::make_shared<A>();
        a->SetCall1(std::bind(&B::Add, this, 1, 2));
        a->SetCall2(std::bind(&B::Print, this));
        a->SetCall3(std::bind(&B::Prints));
        a->run();
    }
};

int main()
{
    using CallBack1 = std::function<int(int, int)>;
    using CallBack2 = std::function<void()>;


    (new B())->run();

    return 0;
}
```

