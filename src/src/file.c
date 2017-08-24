// AVA file format
//
// - [x] streamable.
// - [x] updateable. append-only.
// - [x] random access.
// - [x] compact.
// - [x] both text and binary.
// - [x] both compressed and uncompressed.
// - [x] unlimited key/value pairs.
// - [x] unlimited data size.
// - [x] can append, modify or delete keys and values.
// - [x] hierarchical data (as long as keys are in ...parent3.parent2.parent1.key format)
//
// TAG4 + BUNDLE[...]

// BUNDLE = [text-data] \0 [binary-data] \0 [compressed-data] \0
// Datas are list of key/value pairs.
// For all found keys:
// - If key does not exist, key/value is created; else key/value is continued
// - If value is nil, key is deleted

//         /          /  \nkey=value   << store data if new key
//         |          |  \nkey=value   << continue data if existing key
//         |   TXT   <|  \nkey=        << delete key
//         |          |  \nkey=value   << new recreated key
//         |          \  [...]         << etc.
//         |   \0
//         |          /  {\byte-len key} {\short-len value }   << store data if new key
//         |          |  {\byte-len key} {\short-len value }   << continue data if existing key
// BUNDLE <|   RAW   <|  {\byte-len key} {\short-len-0     }   << delete key
//         |          |  {\byte-len key} {\short-len value }   << new recreated key
//         |          \  [...]                                 << etc.
//         |   \0
//         |          /  {\byte-len key} {\short-len \short-ulen \byte-flags compressed_value }   << store data if new key
//         |          |  {\byte-len key} {\short-len \short-ulen \byte-flags compressed_value }   << continue data if existing key
//         |   ZIP   <|  {\byte-len key} {\short-len-0  }                                         << delete key
//         |          |  {\byte-len key} {\short-len \short-ulen \byte-flags compressed_value }   << new recreated key
//         |          \  [...]                                                                    << etc.
//         \   \0

#include <stdio.h>

int fileget( const char *fname, const char *key, void *val );
int fileput( const char *fname, const char *key, const void *val );
