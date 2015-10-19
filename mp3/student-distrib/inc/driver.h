#ifndef _INC_DRIVER_H
#define _INC_DRIVER_H

typedef struct {
	const char* name;
	void (*init)();
	void (*remove)();
} driver_t;

#define DEFINE_DRIVER_INIT(ID)	\
	void _##ID##_init()

#define DEFINE_DRIVER_REMOVE(ID)	\
	void _##ID##_remove()

#endif
