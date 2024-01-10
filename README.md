Notes about 《C++ Core Guidelines》

> 改进：对ls输出排序，对目录和文件分颜色显示
# ls
`./ls {pathname}`
对目录流的操作
```cpp
opendir()
readdir()
closedir()
// use man 9 inode can see;
struct dirent
DIR
```

# tree
`./tree {pathname}`
对目录流的操作+DFS遍历

# chmod
`./chmod {new mode} {pathname}`
对文件权限的操作
