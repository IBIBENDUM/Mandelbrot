#ifndef UTILS_H_
#define UTILS_H_

#define RET_IF_ERR(STATEMENT, ERR_CODE)                \
if (!STATEMENT)                                        \
do {                                                   \
    fprintf(stderr, "Error in %s in %s in line: %d\n", \
           __FILE__, __PRETTY_FUNCTION__, __LINE__);   \
    return ERR_CODE;                                   \
} while(0)

#endif // UTILS_H_
