#include "lib/string.h"
#include "types.h"


/*
 * strchr returns a pointer to the first occurence of the given character in the
 * given string. If the character is not found, it returns NULL.
 */
char *
strchr(const char *str, int character)
{
	while (*str != '\0' && *str != character)
	{
		str++;
	}

	if (*str == character)
	{
		return (char *) str;
	}
	else
	{
		return NULL;
	}
}


/*
 * strcmp compares the given input strings using the lexicographical order, and
 * returns 0 if s1 == s2, returns a negative number if s1 < s2, and returns a 
 * positive number if s1 > s2.
 */
int
strcmp(const char *s1, const char *s2)
{
	while (*s1 == *s2 && *s1 != '\0' && *s2 != '\0')
	{
		s1++;
		s2++;
	}

	return (*s1 - *s2);
}


/* strlen returns the length of the given null-terminated string. */
size_t
strlen(const char *str)
{
	size_t length = 0;
	while (*str != '\0')
	{
		str++;
		length++;
	}

	return length;
}


/*
 * strncmp compares the given input strings upto n characters using the
 * lexicographical order. For return value, please see strcmp.
 */
int
strncmp(const char *s1, const char *s2, size_t n)
{
	size_t index = 0;

	if (n == 0)
	{
		return 0;
	}

	while (*s1 == *s2 && *s1 != '\0' && *s2 != '\0' && index < n - 1)
	{
		s1++;
		s2++;
		index++;
	}

	return (*s1 - *s2);	
}


/*
 * strtok tokenizes the given string using the given delimiters. If str != NULL,
 * then it returns a pointer to the first token. If str == NULL, then it returns
 * the a pointer to the next token of the string used in previous calls. If no
 * more tokens are found, it returns NULL.
 *
 * WARNING: This function changes the original string.
 */
char *
strtok(char *str, const char *delimiters)
{
	static char *last = NULL;
	char *token = NULL;

	if (str != NULL)
	{
		last = str;
	}

	token = last;

	/* skip leading delimiters */
	while (*token != '\0' && strchr(delimiters, *token) != NULL)
	{
		token++;
	}

	/* if there were no non-delimiter characters, return NULL */
	if (*token == '\0')
	{
		last = NULL;
		return NULL;
	}

	/* scan to find where token ends */
	while (*last != '\0' && strchr(delimiters, *last) == NULL)
	{
		last++;
	}

	/* terminate the token, and set where the scan of next token starts */
	if (*last != '\0')
	{
		*last = '\0';
		last++;
	}

	return token;
}
