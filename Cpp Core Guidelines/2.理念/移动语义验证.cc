#include <iostream>
#include <string>
#include <utility>
using std::string;
using std::cout;
using std::endl;
using std::cin;

struct S {
  std::string s_;
  S(std::string s):s_(s){}
  S(const S& rhs):s_(rhs.s_) {
    std::cerr << "调用拷贝构造\n";
  }
  // S(S&& rhs):s_(rhs.s_) {
  //   std::cerr << "调用移动构造\n";
  // }
  S& operator=(const S& rhs) {
    s_ = rhs.s_;
    std::cerr << "调用赋值运算符\n";
    return *this;
  }
};

int main()
{
    S s1("this is a test string");
    // S s2("test");
    // s2 = std::move(s1);  // 此处是拷贝语义，不会调用移动赋值运算。
    S s2 = std::move(s1);  // 此处是拷贝语义，不会调用移动构造。
}
