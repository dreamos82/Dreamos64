# Dreamos VFS

This is a draft document on how the VFS will be implemented, lot of things will change.

Every filesystem that will be accessible by the kernel will be represented by a `mountpoint_t` item. The item will be orgranized using a _Tree_ structure, where the root is always "/"

This data structure is defined as follows: 

```c
typedef struct {
    char name[FILESYSTEM_NAME_LEN];  // The filesystem name

    char mountpoint[MAX_MOUNTPOINT_LEN];

    fs_file_operations_t file_operations;

} mountpoint_t;
```

The fields: 

* _name_ is the name of the file system
* _mountpoint_ is the directory entry where we want to place the filesystem (like linux). There always be a root node, with "/" as _mountpoint_
* _file_operations_ contains pointers to the functions that will access the file system, usually they are provided by the driver. File operations will be detailed later.

In this design the `file_operations` for a `vnode` will always be stored in the `mountpoint_t` struct, sonce a file is found to belong to a specific FS, the new vnode, just need to copy these 

Everytime we want to mount a new file system it has to be added to the tree. 

Searching for a file is first searching for the _longest_  `mountpoint` inside the tree, that is contained in the file path.

Inside the `vfs` mountpoint (or superblock) there will be a `void *` pointer that will contain a pointer to the fs specific struct.  

## VFS Workflow

The steps to add a new file system: 

* For every fs type supported a driver needs to be implemented (initial support: _ustar_). 
* Each vnode represent a file, a single file is loeaded once, and then shared across various processes. 
* They will need some locking. 


## FS Driver

__This part is a work in progress so the design can change at any moment__

### FS Drivers

The kernel needs to know what are the supported FS types.

So every suppoted FS needs to be regesitered with the kernel

There will be a list that will contain all the supported fs, every item will contain the basic info and all the structures needed. Especially a pointer to the fs init function. 

There will be a fs_register function. 

This is a Layer separated from the VFS

### FS Implementation

It will have a `fs_type_t` structure. 

This will contains the basic initialization structures to let the fs be mounted by the VFS: 

```c
struct fs_type_t {
    char *name;
    superblock_operations_t operations;
}
```

operations will contain the function pointer for the `mount` function.

When a new fs needs to be mounted, the vfs will search in the list of virtual file systems. 

The list of supported virtual file systems will be part of a kernel list, every fs that will be supported needs to be registered by the kernel. 

So when we want to mount a new FS, the parameters will be: 

* FS Type, pointer to the device block.
* DevFS is basically a pointer to all devices available on the system that support open/read/write operations?

### Operations

There are two types of operations:

* `fs_file_operations`
* `vnode_operations`

#### Vnode Operations

Contains the following functions:

* `lookup` this is the fs function that search for a file inside  a given folder of the given filesystem.

### Mountpoint

The mountpoint is defined by the struct: `mountpoint_t` in `fs/vfs.h`. 

When the kernel is running at least one mountpoint will always be present: the _root_ (`/`) mountpoint.

The kernel keep track of all the mounted filesystem in an array (**This is temporary, in future will be either a list or a tree**)

#### File Operations

The struct `file_operations_t` contains pointer to fs specific file functions like read, write, etc.

```c
struct fs_file_operations_t{     
    int (*open)(const char *, int, ... );
    int (*close)(int);
    ssize_t (*read)(vnode_t *, int, char*, size_t);
    ssize_t (*write)(int,const void*, size_t);
};
```

