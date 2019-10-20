#ifndef IO_HPP
#define IO_HPP

typedef char* va_list;

#define __va_argsiz(t)	\
		(((sizeof(t) + sizeof(int) - 1) / sizeof(int)) * sizeof(int))

#define va_start(ap, pN) ((ap) = ((va_list) __builtin_next_arg(pN)))

#define va_end(ap)	((void)0)

#define va_arg(ap, t)	\
		 (((ap) = (ap) + __va_argsiz(t)), *((t*) (void*) ((ap) - __va_argsiz(t))))

int putx( int tid, int uart, char c );

int putstr( int tid, int uart, char *str );

int putr( int tid, int uart, unsigned int reg );

void putw( int tid, int uart, int n, char fc, char *bf );

void printf( int tid, int uart, char *format, ... );

#endif