// Lock-free queue (multiple consumer-multiple producer).
// License: WTFPL. https://github.com/darkautism/lfqueue
// Use -O0 flag to compile.

#pragma once

// api

struct lfq_node{
    void * data;
    struct lfq_node * next;
};

struct lfq_ctx{
    struct lfq_node * head;
    struct lfq_node * tail;
    int count;
};

int lfq_init(struct lfq_ctx *ctx);
int lfq_clean(struct lfq_ctx *ctx);
int lfq_enqueue(struct lfq_ctx *ctx, void * data);
void * lfq_dequeue(struct lfq_ctx *ctx );

// impl

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _MSC_VER
#   include <windows.h>
#   define __sync_add_and_fetch(ptr,x) InterlockedIncrement(ptr)
#   define __sync_sub_and_fetch(ptr,x) InterlockedDecrement(ptr)
#   define __sync_bool_compare_and_swap(ptr, old, val) \
    (InterlockedCompareExchange64((LONGLONG volatile *)ptr, (LONGLONG)val, \
    (LONGLONG)old) == (LONGLONG)old)
#endif

int lfq_init(struct lfq_ctx *ctx) {
    struct lfq_node * tmpnode = malloc(sizeof(struct lfq_node));
    if (!tmpnode)
        return -errno;

    memset(ctx,0,sizeof(struct lfq_ctx));
    ctx->head=ctx->tail=tmpnode;
    return 0;
}

int lfq_clean(struct lfq_ctx *ctx){
    if ( !ctx->count )
        free(ctx->head);
    return ctx->count;
}

int lfq_enqueue(struct lfq_ctx *ctx, void * data) {
    struct lfq_node * p;
    struct lfq_node * tmpnode = malloc(sizeof(struct lfq_node));
    if (!tmpnode)
        return -errno;

    memset(tmpnode,0,sizeof(struct lfq_node));
    tmpnode->data=data;
    do {
        p = ctx->tail;
        if ( __sync_bool_compare_and_swap(&ctx->tail,p,tmpnode)) {
            p->next=tmpnode;
            break;
        }
    } while(1);
    __sync_add_and_fetch( &ctx->count, 1);
    return 0;
}

void * lfq_dequeue(struct lfq_ctx *ctx ) {
    void * ret=0;
    struct lfq_node * p;
    do {
        p = ctx->head;
    } while(p==0 || !__sync_bool_compare_and_swap(&ctx->head,p,0));

    if( p->next==0) {
        ctx->head=p;
        return 0;
    }
    ret=p->next->data;
    ctx->head=p->next;
    __sync_sub_and_fetch( &ctx->count, 1);
    free(p);
    return ret;
}

// demo

#ifdef DEMO

#include <stdio.h>
#include <stdlib.h>

int main() {
    long ret;
    struct lfq_ctx ctx;
    lfq_init(&ctx);
    lfq_enqueue(&ctx,(void *)1);
    lfq_enqueue(&ctx,(void *)3);
    lfq_enqueue(&ctx,(void *)5);
    lfq_enqueue(&ctx,(void *)8);
    lfq_enqueue(&ctx,(void *)4);
    lfq_enqueue(&ctx,(void *)6);

    while ( (ret = (long)lfq_dequeue(&ctx)) != 0 )
        printf("lfq_dequeue %ld\n", ret);

    lfq_clean(&ctx);
    return 0;
}

#endif DEMO
