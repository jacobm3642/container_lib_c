This is just a basic implementation of container datastructs so I can use it in other projects
The main reason to make this is to allow a custom allocator and deallocator

Every datastruct with have these operations
Insert, Remove, Sequential access, Random access, Find, Sort

When you init a datastruct you will be able to passed a function ptr to a allocator and a sorting function
when NULL is pass it will use malloc/free and quicksort

TODO:
- [x] Dynamic Array
- [x] Linked List
- [x] Stack
- [x] Queue
- [ ] Deque
- [ ] Priority Queue
- [ ] Tree
- [ ] Binary Search Tree
- [ ] Binary Heap
- [x] Dictionary (hash table)
- [ ] Set
- [ ] Graph
- [ ] Directed Acyclic Graph 
