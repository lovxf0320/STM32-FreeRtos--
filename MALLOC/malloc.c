#include "malloc.h"

//内存池(32字节对齐)
// __align(32)
// 这是一个内存对齐指令，表示将变量按32字节边界对齐。
// __attribute__((at(0X68000000)))
// 这是GCC编译器的属性语法，用于指定变量在内存中的绝对地址。
__align(32) u8 mem1base[MEM1_MAX_SIZE];                                     //内部SRAM内存池
__align(32) u8 mem2base[MEM2_MAX_SIZE]  __attribute__((at(0x68000000)));    //外部SRAM内存池
//内存管理表
u16 mem1mapbase[MEM1_ALLOC_TABLE_SIZE];                                     //内部SRAM内存池MAP
U16 mem2mapbase[MEM1_ALLOC_TABLE_SIZE]  __attribute__((at(0x68000000+MEM2_MAX_SIZE)));//外部SRAM内存池MAP
//内存管理参数
const u32 memtblsize[SRAMBANK]={MEM1_ALLOC_TABLE_SIZE,MEM2_ALLOC_TABLE_SIZE};//内存表大小
const u32 memblksize[SRAMBANK]={MEM1_BLOCK_SIZE,MEM2_BLOCK_SIZE};			 //内存分块大小
const u32 memsize[SRAMBANK]={MEM1_MAX_SIZE,MEM2_MAX_SIZE};					 //内存总大小


//内存管理控制器
struct _m_mallco_dev mallco_dev={
    my_mem_init,                //内存初始化
    my_mem_perused,             //内存使用率
    {mem1base,mem2base},        //内存池
    {mem1mapbase,mem2mapbase},  //内存管理状态表
    {0,0}                       //内存管理未就绪
};

//复制内存
//*des：目的地址
//*src：源地址
//n：需要复制的内存长度
void mymemcpy(void *des,void *src,u32 n){
    u8* xdes=des;
    u8* xsrc=src;

    while(n--){
        xdes++=xsrc++;
    }
}

//设置内存
//*s:内存首地址
// c:要设置的值
//count:需要设置的内存大小(字节为单位)
void mymemset(void *s,u8 c,u32 count){
    u8 *xs=s;
    while(count--){
        *xs++=c;
    }
}

// 初始化内存管理
// memx: 内存块索引（0表示内部SRAM，1表示外部SRAM）
void my_mem_init(u8 memx){
    mymemset(mallco_dev.memmap[memx],0,memtblsize[memx]*2);         //清零内存状态表
    //*2：因为状态表中的每个表项是u16类型（16位，占2个字节），所以需要乘以2来计算总字节数。
    mymemset(mallco_dev.membase[memx],0,memsize[memx]);             //清零内存池数据
    mallco_dev.memrdy[memx]=1;                                      //标记内存管理已初始化
}

// 计算内存使用率
// memx: 内存块索引
// 返回值: 使用率（0~100）
u8 my_mem_perused(u8 memx){
    u32 used=0;
    u32 i;
    for(i=0;i<memtblsize[memx],i++){
        if(mallco_dev.memmap[memx][i]){
            used++;         //统计已使用的内存块
        }
    }

    return (used*100)/memtblsize[memx]; //计算并返回使用率百分比
}

// 分配内存（内部函数）
// memx: 内存块索引
// size: 要分配的内存大小（字节）
// 返回值: 0xFFFFFFFF表示错误；否则返回内存偏移地址
u32 my_mem_malloc(u8 memx,u32 size){
    signed long offset=0;
    u32 nmemb;              //需要的内存块数
    u32 cmemb = 0;          //连续空闲内存块计数
    u32 i;

    if(!mallco_dev.memrdy[memx]){
        mallco_dev.init(memx);  //未初始化则先初始化
    }
    if(size==0){
        return 0xFFFFFFFF;      //不需要分配
    }
    nmemb=size/memblksize[memx];//计算需要的内存块数
    if(size%memblksize[memx]){
        nmemb++;                //如果大小不能整除块大小，则多分配一块
    }

    for(offset=memtblsize[memx]-1;offset>=0;offset--){
        if(!mallco_dev.memmap[memx][offset]){
            cmemb++;                            //增加连续空闲块计数
        }else{
            cmemb=0;                            //遇到非空块，清零计数
        }

        if(cmemb==nmemb){                       //找到足够的连续空闲块
            for(i=0;i<nmemb;i++){               //标记内存块为已分配
                mallco_dev.memmap[memx][offset+i]=nmemb;
            }
            return (offset*memblksize[memx]);   //返回偏移地址
        }
    }

    return 0xFFFFFFFF;
}


// 释放内存（内部函数）
// memx: 内存块索引
// offset: 内存偏移地址
// 返回值: 0表示成功；1表示未初始化；2表示偏移超范围
u8 my_mem_free(u8 memx,u32 offset){
    int i;
    if(mallco_dev.memrdy[memx]){    //未初始化则先初始化
        mallco_dev.init(memx);
        return 1;
    }

    if(offset<memsize[memx]){                       //检查偏移是否在内存池范围内
        int index=offset/memblksize[memx];          //计算内存块索引
        int nmemb=mallco_dev.memmap[memx][index];   //获取要释放的块数
        for(i=0;i<nmemb;i++){        //清零内存块
            mallco_dev.memmap[memx][index+i]=0;
        }
        return 0;
    }else{
        return 2;                   //偏移超出范围
    }
}

// 释放内存（外部函数）
// memx: 内存块索引
// ptr: 内存起始地址
void myfree(u8 memx,void *ptr){
    u32 offset;
    if(ptr==NULL){
        return;
    }

    offset=(u32)ptr-(u32)mallco_dev.membase[memx];  //计算偏移地址
    my_mem_free(memx,offset);
}

// 分配内存（外部函数）
// memx: 内存块索引
// size: 要分配的内存大小（字节）
// 返回值: 分配到的内存起始地址，失败返回NULL
void *mymalloc(u8 memx,void *ptr,u32 size){
    u32 offset;

    offset=my_mem_malloc(memx,size);    //分配新内存
    if(offset==oxFFFFFFFF){
        return NULL;
    }else{
        mymemcpy((void*)((u32)mallco_dev.membase[memx]+offset),ptr,size);  //复制旧内存内容到新内存
        myfree(memx,ptr);
        return (void*)((u32)mallco_dev.membase[memx] + offset);     //返回新内存地址
    }
}