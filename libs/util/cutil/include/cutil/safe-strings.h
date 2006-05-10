/*
 * safe-strings.h:
 *
 * Functions to manipulate strings a safe way
 * strlcpy and strlcat taken from OpenBSD project, see their respective source code for license and copyright
 *
 * Written by Sebastien Tricaud 
 */

size_t strlcpy(char *dst, const char *src, size_t siz);
size_t strlcat(char *dst, const char *src, size_t siz);


