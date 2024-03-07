# 0.1.1

## virtual int parseArgs(int& argc, char **argv)

Changed cause of program crash in Linux:
virtual int parseArgs(int argc, char **argv); >> virtual int parseArgs(int& argc, char **argv);

## std::atomic<int> m_refCount;

Changed cause of program crash in Linux:
g++ incorrectly initialize default constuctor for std::atomic<int> for some reason.
`std::atomic<int> m_refCount;` was replaced by

```cpp 
typedef QAtomicInt RefCount_t;
...
    mb::RefCount_t m_refCount;                 
```

# 0.1.2

Fix server-side bug when saving/exporting device