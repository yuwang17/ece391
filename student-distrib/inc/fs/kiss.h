#ifndef _FS_KISS_H_
#define _FS_KISS_H_

#include <stddef.h>
#include <stdint.h>
#include <inc/multiboot.h>
#include <inc/klibs/lphashtable.h>
#include <inc/klibs/lib.h>
#include <inc/fs/filesystem.h>

struct dentry_t {
    char filename[33] = {};
    uint32_t filetype;
    uint32_t inode;
};

struct inode_t {
    uint32_t size;
    uint32_t datablocks[4096 / sizeof(uint32_t) - 1];
    uint32_t numDataBlocks;
};

#ifdef __cplusplus
namespace filesystem {

    struct KissFileDescriptorData {
        uint8_t filetype;
        union {
            uint32_t inode;
            struct {
                uint8_t *base;
                uint32_t idx;
                uint32_t max;
            } dentryData;
        };
    };

    static const uint32_t MaxNumFiles = 64;
    static const uint32_t BlockSize = 4096;

    template<size_t num>
    struct __attribute__ ((__packed__)) SkipStruct {
        uint8_t members[num];
        // Assignment does nothing!
        const SkipStruct& operator= (const SkipStruct &source) const
        {
            return *this;
        }
    };

    class Reader {
        private:
            uint8_t *startingAddr;
            size_t read;

        public:

            Reader(uint8_t *startingAddr)
            {
                this->startingAddr = startingAddr;
                this->read = 0;
            }

            template<typename T>
            Reader& operator >> (T& val)
            {
                T local = T();
                const size_t size = sizeof(T);
                uint8_t *a = (uint8_t *) &local;
                for (size_t i = 0; i < size; i++)
                {
                    a[i] = startingAddr[read];
                    read++;
                }
                val = local;
                return *this;
            }

            template<size_t num>
            static const SkipStruct<num> skip()
            {
                return SkipStruct<num>();
            }

            void reposition(size_t offset)
            {
                read = offset;
            }
    };

    class KissFS : public AbstractFS {
    private:
        /* The hash table size must be not smaller than the number of dentries */
        util::LinearProbingHashTable<133, Filename, uint32_t, HashFunc> dentryIndexOfFilename;
        dentry_t *dentries;
        inode_t *inodes;
        uint32_t numDentries;
        uint32_t numInodes;
        uint32_t numTotalDataBlocks;
        uint8_t *imageStartingAddress;
        uint8_t imageLength;
        uint32_t numBlocks;

        bool readBlock(uint32_t datablockId, uint32_t offset, uint8_t *buf, uint32_t len);
        int32_t readDir(FsSpecificData *data, uint32_t offset, uint8_t *buf, uint32_t len);
        void initFromMemoryAddress(uint8_t *mem, uint8_t *end);

    public:
        virtual void init();
        virtual bool open(const char* filename, FsSpecificData *&fdData);
        virtual int32_t read(FsSpecificData *data, uint32_t offset, uint8_t *buf, uint32_t len);
        virtual int32_t write(FsSpecificData *data, uint32_t offset, const uint8_t *buf, uint32_t len);
        virtual bool close(FsSpecificData *fdData);
        virtual int32_t fstat(FsSpecificData *data, stat *st);
        virtual bool canSeek(FsSpecificData *fdData);
        virtual Maybe<uint32_t> getFileSize(FsSpecificData *fdData);

        int32_t readDentry(const uint8_t* fname, dentry_t* dentry);
        int32_t readDentry(uint32_t index, dentry_t* dentry);
        int32_t readData(uint32_t inode, uint32_t offset, uint8_t *buf, uint32_t length);
    };

}

#endif

#endif
