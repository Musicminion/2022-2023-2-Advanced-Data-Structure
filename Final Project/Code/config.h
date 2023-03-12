#pragma once

// sstable的header的大小
#define sstable_headerSize 32

// sstable的大小(Byte)
#define sstable_bfSize 10240

// sstable的key的大小(Byte)
#define sstable_keySize 8

// sstable的key的大小(Byte)
#define sstable_keyOffsetSize 4

// sstable的文件偏移量(Byte)
#define sstable_fileOffset_header 0

// sstable的bf偏移量(Byte)
#define sstable_fileOffset_bf 32


#define sstable_fileOffset_key 10272




// sstable文件的最大限制(2MB = 2 * 1024 * 1024)
#define sstable_maxSize  2 * 1024 * 1024

#define sstable_outOfRange "~![ERROR] Exceed Limit!~"

#define memtable_not_exist "~![ERROR] MemTable No Exist!~"

#define memtable_already_deleted "~![ERROR] ALREADY DELETED!~"

#define sstvalue_readFile_file "~![ERROR] No File!~"

#define sstvalue_readFile_outOfRange "~![ERROR] Exceed Limit!~"

#define sstvalue_outOfRange "~![ERROR] Exceed Limit!~"

#define delete_tag "~DELETED~"

// class Config{
// public:
//     // sstable的大小(Byte)
//     const size_t sstable_bfSize = 10240;
    
//     
//     const size_t sstable_maxSize = 2 * 1024 * 1024;
// }config;