# 🧠 Custom Memory Allocator in C (Windows)

This project implements a **custom memory allocator** in C for Windows using `VirtualAlloc` and `VirtualFree`, mimicking the behavior of `malloc` and `free`. It includes block splitting, coalescing of free blocks, and memory alignment using a **doubly linked list**.

## 🔧 Features

- ✅ Custom `my_malloc()` and `my_free()` functions
- ✅ Memory block splitting to reduce fragmentation
- ✅ Coalescing adjacent free blocks for efficiency
- ✅ 16-byte alignment for performance and compatibility
- ✅ Works on Windows using `VirtualAlloc`/`VirtualFree`
- ✅ No use of standard `malloc`/`free`

---

## 📁 File Structure
