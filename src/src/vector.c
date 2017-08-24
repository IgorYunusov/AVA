// # vector
// api

#pragma once
#include <stdlib.h>
#include <string.h>

typedef void *any;

typedef struct vector {
    any *data;
    int len;
    int cap;
} vector;

void  vec_new( vector *v );
void  vec_del( vector *v );
 int  vec_size( vector *v );
void  vec_push( vector *v, void *value );
void* vec_at( vector *v, unsigned pos );
void  vec_pop ( vector *v );
void  vec_each( vector *v, int (*yield)( void * ) );
void* vec_back( vector *v );
void  vec_copy( vector *dst, const vector *src );

void  vec_sort( vector *v, int (*cmp)(const void *,const void *) );

// impl

#if 0
// # vector
#include <vector>
static std::vector< void * > vecs[32+32];
void vec_clr( int id ) { vecs[id].clear(); }
 int vec_len( int id ) { return (int)vecs[id].size(); }
void vec_push( int id, void *value ) { vecs[id].push_back( value ); }
void*vec_back( int id ) { return vecs[id].back(); }
void vec_pop( int id ) { vecs[id].pop_back(); }
void vec_each( int id, int (*yield)( void * ) ) { for( auto &v : vecs[id] ) if( !yield( v ) ) break; }
#endif

const double vec__grow_kfactor = 2.33;
void  vec__grow( vector *v ) { v->data = (any *)realloc( v->data, sizeof(any) * (v->cap = (int)((v->cap + 1) * vec__grow_kfactor)) ); }

void  vec_new( vector *v ) { vector w = {0}; *v = w; }
void  vec_del( vector *v ) { vector w = {0}; v->data = (any *)realloc(v->data, 0); *v = w; }
 int  vec_size( vector *v ) { return v->len; }
void  vec_push( vector *v, void *value ) { if( v->len >= v->cap ) vec__grow(v); v->data[v->len++]/*[0]*/ = value; }
void* vec_at( vector *v, unsigned pos ) { return v->data[v->len++]/*[0]*/; }
void  vec_pop ( vector *v ) { --v->len; }
void  vec_each( vector *v, int (*yield)( void * ) ) { for( int i = 0; i < v->len; ++i) yield( v->data[i]/*[0]*/ ); }
void* vec_back( vector *v ) { return v->data[v->len - 1]/*[0]*/; }
void  vec_copy( vector *dst, const vector *src ) {
  vec_del(dst);
  dst->len = src->len;
  dst->cap = src->cap;
  dst->data = (any *)realloc( dst->data, sizeof(any) * dst->cap );
  memcpy(dst->data, src->data, sizeof(any) * dst->cap );
}

void  vec_sort( vector *v, int (*cmp)(const void *,const void *) ) { qsort( v->data, v->len, sizeof(any), cmp ); }

#if defined(__cplusplus) && defined(DEMO)
#include <stdio.h>
#include <omp.h>
#include <vector>
#include "cmp.c"
int main() {
  vector v = {0};
  printf("%d\n", vec_size(&v));
  vec_push(&v, "hello"); vec_push(&v, "cruel"); vec_push(&v, "world");
  printf("%d\n", vec_size(&v));
  vec_each(&v, (int(*)(void*))puts);
  vec_pop(&v);
  printf("%d\n", vec_size(&v));
  printf("%s\n", (char*)vec_back(&v));
  vec_pop(&v);
  printf("%d\n", vec_size(&v));
  printf("%s\n", (char*)vec_back(&v));
  vec_del(&v);

  enum { J = 100 };
  enum { N = 10000000 };

  {
  double t = -omp_get_wtime();
  for( int j = 0; j < J; ++j ) {
      vector v = {0};
      for( int i = 0; i < N; ++i ) {
        vec_push(&v, (void*)i);
      }
      vec_del(&v);
  }
  t += omp_get_wtime();
  printf("C vector %fs\n", t);
  }

  {
  double t = -omp_get_wtime();
  for( int j = 0; j < J; ++j ) {
    std::vector<int> v;
    for( int i = 0; i < N; ++i ) {
      v.push_back(i);
    }
  }
  t += omp_get_wtime();
  printf("std::vector %fs\n", t);
  }
}
#endif
