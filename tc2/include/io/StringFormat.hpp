#ifndef STRING_FORMAT_HPP
#define STRING_FORMAT_HPP

int charToIntegerValue(char ch);
void unsignedIntegerToCharArray(unsigned int number, unsigned int base, char *buffer);
void integerToCharArray(int number, char *buffer);
char* formatWord(char* formattedString, const char *word);
int format(char* formattedString, const char *fmt, ...);

#endif
