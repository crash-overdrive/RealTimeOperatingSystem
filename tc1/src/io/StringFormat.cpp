#include <cstdarg>

#include "io/StringFormat.hpp"

int charToIntegerValue(char ch) {
	if(ch >= '0' && ch <= '9') return ch - '0';
	if(ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
	if(ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
	return -1;
}

void unsignedIntegerToCharArray(unsigned int number, unsigned int base, char *buffer) {
	int n = 0;
	int digit;
	unsigned int d = 1;

	while((number / d) >= base) d *= base;
	while(d != 0) {
		digit = number / d;
		number %= d;
		d /= base;
		if(n || digit > 0 || d == 0) {
			*buffer++ = digit + ( digit < 10 ? '0' : 'a' - 10 );
			++n;
		}
	}
	*buffer = 0;
}

void integerToCharArray(int number, char *buffer) {
	if(number < 0) {
		number = -number;
		*buffer++ = '-';
	}
	unsignedIntegerToCharArray(number, 10, buffer);
}


char* formatWord(char* formattedString, const char *word) {
	char ch;
	while((ch = *word++)) {
		*formattedString++ = ch;
	}
	return formattedString;
}

int format(char* formattedString, const char *fmt, ...) {
	va_list args;
	va_start(args,fmt);

	char* temp = formattedString;
	char buffer[12];
	char ch;

	while ( ( ch = *(fmt++) ) ) {
		if ( ch != '%' ) {
			*(formattedString++) = ch;
		}
		else {
			ch = *(fmt++);
			switch(ch) {
				case '\0':
					va_end(args);
					return formattedString-temp;
				case 'c':
					*(formattedString++) = va_arg(args, int);
					break;
				case 's':
					formattedString = formatWord(formattedString, va_arg(args, const char*));
					break;
				case 'u':
					unsignedIntegerToCharArray(va_arg(args, unsigned int), 10, buffer);
					formattedString = formatWord(formattedString, buffer);
					break;
				case 'd':
					integerToCharArray(va_arg(args, int), buffer);
					formattedString = formatWord(formattedString, buffer);
					break;
				case 'x':
					unsignedIntegerToCharArray(va_arg(args, unsigned int), 16, buffer);
					formattedString = formatWord(formattedString, buffer);
					break;
				case '%':
					*(formattedString++) = ch;
					break;
			}
		}
	}
	va_end(args);
	return formattedString-temp;
}
