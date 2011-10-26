#ifndef MCC_CONFIG_H
#define MCC_CONFIG_H

/** Only one of the following three should be defined at any one time **/
#define MCC_USE_HASH_TABLE_FOR_MACROS (1)
#define MCC_USE_B_TREE_FOR_MACROS (0)
#define MCC_USE_LIST_FOR_MACROS (0)


#ifndef MCC_DEBUG
#define MCC_DEBUG (1)
#endif

#define MCC_USE_DYNAMIC_STRING_BUFFER_TOKENISER (1)

#define MCC_C99_COMPATIBLE (1)

#endif
