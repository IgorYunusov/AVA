// [ref] https://blog.forrestthewoods.com/reverse-engineering-sublime-text-s-fuzzy-match-4cffeed33fdb

#pragma once
#include <string.h>

int fuzzy_score( const char *text, const char *pattern ) {
    int score = 0, consecutive = 0, maxerrors = 0;
    while( *text && *pattern ) {
        int is_leading = (*text & 64) && !(text[1] & 64);
        if( (*text & ~32) == (*pattern & ~32) ) {
            int had_separator = (text[-1] <= 32);
            int x = had_separator || is_leading ? 10 : consecutive * 5;
            //printf("+%d,",x);
            score += x;
            ++pattern;
            consecutive = 1;
        } else {
            int x = -1, y = is_leading * -3;
            maxerrors += y;
            score += x;
            //printf("%d,",x+y);
            consecutive = 0;
        }
        ++text;
    }
    return score + (maxerrors < -9 ? -9 : maxerrors);
}

const char *fuzzy( int N, const char *texts[], const char *pattern ) {
    const char *best = 0;
    int scoremax = 0;
    for( int i = 0; i < N /*texts[i]*/; ++i ) {
        int score = fuzzy_score( texts[i], pattern );
        int record = ( score >= scoremax );
        int draw = ( score == scoremax );
        //printf("=> %s : %d/%d (%s)\n", texts[i], score, scoremax, record ? "record!" : "");
        if( record ) {
            scoremax = score;
            if( !draw ) best = texts[i];
            else best = best && strlen(best) < strlen(texts[i]) ? best : texts[i];
        }
    }
    return best ? best : "";
}

#ifdef DEMO

#include <stdio.h>

void test( const char *text, const char *pattern ) {
    printf("fuzzy(%s,%s) : %d\n", text, pattern, fuzzy_score(text, pattern));
}

int main(int argc, const char **argv) {
    test("yeah", "nope");
    test("the black knight", "tk");
    test("The Black Knight", "tk");
    test("The Black Knight", "tblk");
    test("SVisualLoggerLogsList.h", "LLL");
    test("AnimGraphNode_CopyBone", "agn");
    test("client_unit.cpp", "clu");
    test("calas university", "clu");

    {
        const char *texts[] = {
            "ce skipsplash",
            "ce skipsplashscreen",
            "ce skipscreen",
            "ce skipaa",
            "r.maxfps",
            "r.maxsteadyfps",
            "slomo 10",
        };
        if( argc > 1 ) printf("%s\n", fuzzy( sizeof(texts)/sizeof(texts[0]), texts, argv[1] ) );
    }

    {
        const char *texts[] = {
            "MockAI.h",
            "MacroCallback.cpp",
            "MockGameplayTasks.h",
            "MovieSceneColorTrack.cpp",
        };
        char const *pattern = "Mock";
        printf("%s\n", fuzzy( sizeof(texts)/sizeof(texts[0]), texts, pattern ) );
    }
}

#endif
