// api

int trp_add( int trophyid, const char *json );
int trp_unlock( int trophyid );
int trp_update();

// impl

//#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct trophy {
    struct { // sortable id: unlocked state first, then transition state
        uint8_t displayed : 1;
        uint8_t unlocked : 1;
    } sort;
    uint32_t name;
    const char *json; // icon, title, description,
} trophy;

static trophy trophies[256] = {0};

int trp_add( int trophyid, const char *json ) {
    trophy *t = &trophies[ trophyid ];
    t->sort.displayed = 0; // @todo: sync w/ local/cloud user data here
    t->sort.unlocked = 0;  // @todo: sync w/ local/cloud user data here
    t->name = trophyid;
    t->json = strdup(json);
    return 1;
}

int trp_unlock( int trophyid ) {
    trophy *t = &trophies[trophyid];
    t->sort.unlocked = 1;
    return 1;
}

int trp_update() {
    trophy *t = &trophies[0];
    for( int i = 0; i < 256; ++i, ++t ) {
        if( t->sort.unlocked && !t->sort.displayed ) {
            puts( trophies[i].json );
            t->sort.displayed = 1;
        }
    }
    return 1;
}

#ifdef DEMO

enum {
    THE_UNLOCKER = 0x01,
};

int main() {

    trp_add( THE_UNLOCKER, "icon=hello.jpg\ntitle=hello world\ndescription=modify & compile this demo" );

    //trp_unlock( THE_UNLOCKER );

    trp_update();
}

#endif
