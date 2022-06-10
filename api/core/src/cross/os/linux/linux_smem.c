#include <cross/cross_os_api.h>

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sys/mman.h>

#define SMEM_EN_DBG                 1

#if SMEM_EN_DBG
    #define _smemd                  _cos_log
#else
    #define _smemd(...)
#endif

typedef struct _linux_smem
{
    char name[256];
    uint8_t* ptr;
    int32_t size;
    #if _CROSS_OS_EN_LINUX_SMEM_
        int32_t hsmem;
        int32_t created;
    #else
        struct _linux_smem* next;
        int32_t ref;
    #endif
}linux_smem,*Plinux_smem;

#define LINUX_SMEM_BASE_TAG     "vacmem"

extern vatek_result linux_smem_init(const char* tag, hcross_smem* hsmem,int32_t size,int32_t iscreate);
extern vatek_result linux_smem_free(Plinux_smem psmem);

vatek_result cross_os_create_smem(const char* tag, hcross_smem* hsmem,int32_t size)
{
    return linux_smem_init(tag,hsmem,size,1);
}

vatek_result cross_os_open_smem(const char* tag, hcross_smem* hsmem,int32_t size)
{
    return linux_smem_init(tag,hsmem,size,0);
}

uint8_t* cross_os_get_smem(hcross_smem hsmem)
{
    Plinux_smem psmem = (Plinux_smem)hsmem;
    return psmem->ptr;
}

vatek_result cross_os_close_smem(hcross_smem hsmem)
{
    Plinux_smem psmem = (Plinux_smem)hsmem;
    return linux_smem_free(psmem);
}

#if _CROSS_OS_EN_LINUX_SMEM

vatek_result linux_smem_init(const char* tag, hcross_smem* hsmem,int32_t size,int32_t iscreate)
{
    Plinux_smem news = (Plinux_smem)malloc(sizeof(linux_smem));
    vatek_result nres = vatek_memfail;
    if(news)
    {
        memset(news,0,sizeof(linux_smem));
        sprintf(&news->name[0],"/%s_%s",LINUX_SMEM_BASE_TAG,tag);
        news->size = size;

        if(iscreate)news->hsmem = shm_open(&news->name[0],O_CREAT|O_RDWR,0660);
        else news->hsmem = shm_open(&news->name[0],O_RDWR, 0660);
        news->created = iscreate;

        _smemd("shm_open - %s : %d - %08x",&news->name[0],news->size,news->hsmem);

        if(news->hsmem > 0)nres = vatek_success;
        else nres = vatek_hwfail;

        if(is_vatek_success(nres))
        {
            nres = (vatek_result)ftruncate(news->hsmem,size);
            if(is_vatek_success(nres))
            {
                news->ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, news->hsmem, SEEK_SET);
                if(!news->ptr)nres = vatek_memfail;
                else if(news->created)memset(news->ptr,0,size);
            }

            _smemd("shm truncate and mmap : %d",nres);

            if(!is_vatek_success(nres))
            {
                close(news->hsmem);
                shm_unlink(&news->name[0]);
            }
        }

        if(!is_vatek_success(nres))free(news);
        else *hsmem = news;
    }
    return nres;
}

vatek_result linux_smem_free(Plinux_smem psmem)
{
    vatek_result nres = (vatek_result)munmap(psmem->ptr,psmem->size);
    close(psmem->hsmem);
    if(psmem->created)
        nres = (vatek_result)shm_unlink(&psmem->name[0]);
    free(psmem);
    return nres;
}

#else

extern void push_linux_smem(Plinux_smem psmem);
extern Plinux_smem find_linux_smem(const char* tag);
extern void del_linux_smem(Plinux_smem psmem);

vatek_result linux_smem_init(const char* tag, hcross_smem* hsmem,int32_t size,int32_t iscreate)
{
    vatek_result nres = vatek_badparam;
    Plinux_smem psmem = find_linux_smem(tag);
    
    if(psmem)
    {
        psmem->ref++;
        *hsmem = psmem;
        nres = vatek_success;
    }else if(iscreate)
    {
        uint8_t* praw = (uint8_t*)malloc(sizeof(linux_smem) + size);
        if(praw)
        {
            psmem = (Plinux_smem)&praw[0];
            memset(praw,0,sizeof(linux_smem) + size);
            psmem->ptr = &praw[sizeof(linux_smem)];
            strncpy(&psmem->name[0],tag,256);
            psmem->size = size;
            psmem->ref++;

            push_linux_smem(psmem);
            *hsmem = psmem;
            nres = vatek_success;
        }
    }
    return nres;
}

vatek_result linux_smem_free(Plinux_smem psmem)
{
    del_linux_smem(psmem);
    return vatek_success;
}

static Plinux_smem asmem_root = NULL;
static Plinux_smem asmem_last = NULL;

void push_linux_smem(Plinux_smem psmem)
{
    if(!asmem_root)asmem_root = psmem;
    else asmem_last->next = psmem;
    asmem_last = psmem;
}

Plinux_smem find_linux_smem(const char* tag)
{
    Plinux_smem ptr = asmem_root;
    while(ptr)
    {
        if(strcmp(&ptr->name[0],tag) == 0)
            return ptr;
        ptr = ptr->next;
    }
    return NULL;
}

void del_linux_smem(Plinux_smem psmem)
{
    psmem->ref--;
    if(!psmem->ref)
    {
        if(psmem == asmem_root)asmem_root = psmem->next;
        else
        {
            Plinux_smem ptr = asmem_root;
            while(ptr)
            {
                if(ptr->next == psmem)
                {
                    ptr->next = psmem->next;
                    if(ptr->next == NULL)asmem_last = ptr;
                    break;
                }
                ptr = ptr->next;
            }
        }
        free(psmem);
    }
}

#endif
