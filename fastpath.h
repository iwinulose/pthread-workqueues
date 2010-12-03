//Charles Duyk
//charles.duyk@gmail.com
//John Stumpo
//stump@jstump.com 

#ifndef FASTPATH_H
#define FASTPATH_H
#define fastpath(x)			(__builtin_expect((long)(x), 1l))
#define slowpath(x)			(__builtin_expect((long)(x), 0l))
#define exists(x)			((x) != NULL)
#define nexists(x)			((x) == NULL)
#define exists_fast(x)		(fastpath(exists((x))))
#define nexists_fast(x)		(fastpath(nexists((x))))
#define exists_slow(x)		(slowpath(exists((x))))
#define nexists_slow(x)		(slowpath(nexists((x))))
#endif /* FASTPATH_H */
