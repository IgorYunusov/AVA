#include <assert.h>
#include <stddef.h>
#include <stdio.h>

#ifdef _MSC_VER
#define inline __inline
#define __auto_type auto
#endif

#define var __auto_type
#define let __auto_type const

static inline void * variant_cast(void * variant_ptr, ptrdiff_t desired_tag) {
  ptrdiff_t * variant_tag = (ptrdiff_t *)variant_ptr;
  assert(*variant_tag == desired_tag);
  return (void *)((char *)variant_ptr + sizeof(ptrdiff_t));
}

#define tag(x) ((ptrdiff_t)&Tag##x)
#define is(x, T) ((x)->tag == tag(T))
#define as(x, T) ((struct T *)variant_cast((void *)(x), tag(T)))

#ifdef DEMO

struct SomeOne {
  int x;
  int y;
} TagSomeOne;

struct SomeTwo {
  float z;
  float w;
} TagSomeTwo;

struct Some {
  ptrdiff_t tag;
  union {
    struct SomeOne _;
    struct SomeTwo __;
  };
};

int main() {
  struct Some a = {0};
  a.tag = tag(SomeTwo);

  printf("Is `a` tagged as SomeOne: %d\n", is(&a, SomeOne));
  printf("Is `a` tagged as SomeTwo: %d\n", is(&a, SomeTwo));

  struct SomeTwo* /*let*/ b = as(&a, SomeTwo);
  b->w = 5.0;

  printf("b->w: %f\n", b->w);
}

#endif
