/*
 * WengoPhone, a voice over Internet phone
 * Copyright (C) 2004-2005  Wengo
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <phapi-util/util.h>

#include <phapi-util/mystring.h>

#include <stdio.h>

/**
*
* WARNING : needs improvements
*
* Makes the same as sscanf but allocates memory for the variables.
* There need to be as much variables as specified parsing tokens.
* For the moment, the only token supported is '%s' for strings.
*
* @param	[in]	text : the text to parse
* @param	[in]	format : the format for parsing (same syntax as printf, scanf, ...)
* @param	[out]	... : variables being allocated and receiving the extracted values
* @return	TRUE if succeeds; FALSE else
*/
PHAPI_UTIL_EXPORTS unsigned int msscanf(char ** text, char * format, ...){
	int i;
	va_list ap;
	char * buf_text;
	char * buf_format;
	int inter_format_length;
	int inter_text_length;
	char ** s_arg;

	if(text == NULL || *text == NULL || strlen(*text) <= 0 || format == NULL || strlen(format)<= 0){
		return FALSE;
	}

	va_start(ap, format);

	while(*format != '\0'){
		if ( *format == '%' ) {
			switch (*++format) {
				case '%' : 
					break;
				case 'c' :
					break;
				case 'd' :
					break;
				case 'f' :
					break;
				case 's' :
					buf_text = *text;
					buf_format = ++format;
					inter_format_length = 0;
					// go to the next format token
					while(*buf_format != '\0' && (*buf_format != '%' && *(buf_format+1) != 's')){
						inter_format_length++;
						buf_format++;
					}
					// rewinds
					buf_format -= inter_format_length;
					// compare format and text until it matches, by shifting the text
					inter_text_length = 0;
					while(*buf_text != '\0'){												
						if(strncmp(buf_format, buf_text, inter_format_length) != 0){
							for(i=1; i<=inter_format_length; i++){ // check that we can shift the text buffer enough
								if(*(buf_text+i) == '\0'){
									break;
								}
							}
							buf_text++;
							inter_text_length++;
						}else{
							break;
						}
					}
					// copies the found string into a given parameter
					if(inter_text_length > 0){
						s_arg = va_arg(ap, char **);
						*s_arg = (char *)malloc((inter_text_length+1) * sizeof(char));
						strncpy(*s_arg, *text, inter_text_length);
						(*s_arg)[inter_text_length] = '\0';
						*text += inter_text_length;
					}
					break;
			}
		}else{
			if(*format != **text){
				return FALSE; // TODO errorcode
			}
			format++;
			(*text)++;
		}		
	}   

	va_end(ap);

	return TRUE;
}

/**
*
* WARNING : needs improvements
*
* Does exactly the same as sscanf but if we give it "%s/%s" as the format and "a/b" as buffer
* and finally two char s[2] as variables, we will get the first s="a" and the second="b" which
* does not work with sscanf!
* For the moment, the only token supported is '%s' for strings.
* NOTE: after a %s, the size of the string variable must be given in argument
* for example
* <pre>
*	int i;
*	char[SIZE] string;
*	sscanf2("25 bazaaar", "%d %s", i, string, sizeof(string));
* </pre>
*
* @param	[in]	buffer : the text to parse
* @param	[in]	format : the format for parsing (same syntax as printf, scanf, ...)
* @param	[out]	... : variables already allocated and receiving the extracted values
* @return	TRUE if succeeds; FALSE else
*/
PHAPI_UTIL_EXPORTS unsigned int sscanf2(const char * buffer, char * format, ...){
	int i;
	int res;
	va_list ap;
	char * buf_text;
	char * buf_format;
	int inter_format_length;
	int inter_text_length;
	char * s_arg;
	int s_arg_size;
	char ** text;

	if(buffer == NULL || strlen(buffer) <= 0 || format == NULL || strlen(format)<= 0){
		return FALSE;
	}

	text = (char **)&buffer;

	va_start(ap, format);

	while(*format != '\0'){
		if ( *format == '%' ) {
			switch (*++format) {
				case '%' : 
					break;
				case 'c' :
					break;
				case 'd' :
					break;
				case 'f' :
					break;
				case 's' :
					buf_text = *text;
					buf_format = ++format;
					inter_format_length = 0;
					// go to the next format token
					while(*buf_format != '\0' && (*buf_format != '%' && *(buf_format+1) != 's')){
						inter_format_length++;
						buf_format++;
					}
					// rewinds
					buf_format -= inter_format_length;
					// compare format and text until it matches, by shifting the text
					inter_text_length = 0;
					if(strlen(buf_format) > 0){ // if not at the end of format
						while(*buf_text != '\0'){												
							if((res = strncmp(buf_format, buf_text, inter_format_length)) != 0){
								for(i=1; i<=inter_format_length; i++){ // check that we can shift the text buffer enough
									if(*(buf_text+i) == '\0'){ // we arrived at the end of the text : put it all in the variable
										break;
									}
								}
								buf_text++;
								inter_text_length++;
							}else{
								break;
							}
						}
					}else{ // if at the end of format, the rest of text goes in the last variable
						inter_text_length = (int)strlen(buf_text);
					}
					// copies the found string into a given parameter
					if(inter_text_length >= 0){
						s_arg = va_arg(ap, char *);
						s_arg_size = va_arg(ap, int);
						//
						// CHECK THAT THE SIZE DOES NOT EXEEDS DESTINATION BUFFER SIZE
						//
						if(s_arg_size < inter_text_length+1){
							return FALSE; // TODO errorcode
						}
						strncpy(s_arg, *text, inter_text_length);
						(s_arg)[inter_text_length] = '\0';
						*text += inter_text_length;
					}
					break;
			}
		}else{
			if(*format != **text){
				return FALSE; // TODO errorcode
			}
			format++;
			(*text)++;
		}		
	}   

	va_end(ap);

	return TRUE;
}

/**
*
* WARNING : needs improvements
*
* Does exactly the same as sscanf but if we give it "%s/%s" as the format and "a/b" as buffer
* and finally two char s[2] as variables, we will get the first s="a" and the second="b" which
* does not work with sscanf!
* For the moment, the only token supported is '%s' for strings.
* NOTE: after a %s, the size of the string variable must be given in argument
* for example
* <pre>
*	int i;
*	char[SIZE] string;
*	sscanf2("25 bazaaar", "%d %s", i, string, sizeof(string));
* </pre>
*
* @param	[in][out]	text : the text to parse
* @param	[in]	format : the format for parsing (same syntax as printf, scanf, ...)
* @param	[out]	... : variables already allocated and receiving the extracted values
* @return	TRUE if succeeds; FALSE else
*/
PHAPI_UTIL_EXPORTS unsigned int sscanf3(char ** text, char * format, ...){
	int i;
	int res;
	va_list ap;
	char * buf_text;
	char * buf_format;
	int inter_format_length;
	int inter_text_length;
	char * s_arg;
	int s_arg_size;

	if(text == NULL || *text == NULL || strlen(*text) <= 0 || format == NULL || strlen(format)<= 0){
		return FALSE;
	}

	va_start(ap, format);

	while(*format != '\0'){
		if ( *format == '%' ) {
			switch (*++format) {
				case '%' : 
					break;
				case 'c' :
					break;
				case 'd' :
					break;
				case 'f' :
					break;
				case 's' :
					buf_text = *text;
					buf_format = ++format;
					inter_format_length = 0;
					// go to the next format token
					while(*buf_format != '\0' && (*buf_format != '%' && *(buf_format+1) != 's')){
						inter_format_length++;
						buf_format++;
					}
					// rewinds
					buf_format -= inter_format_length;
					// compare format and text until it matches, by shifting the text
					inter_text_length = 0;
					if(strlen(buf_format) > 0){ // if not at the end of format
						while(*buf_text != '\0'){												
							if((res = strncmp(buf_format, buf_text, inter_format_length)) != 0){
								for(i=1; i<=inter_format_length; i++){ // check that we can shift the text buffer enough
									if(*(buf_text+i) == '\0'){
										break;
									}
								}
								buf_text++;
								inter_text_length++;
							}else{
								break;
							}
						}
					}else{ // if at the end of format, the rest of text goes in the last variable
						inter_text_length = (int)strlen(buf_text);
					}
					// copies the found string into a given parameter
					if(inter_text_length >= 0){
						s_arg = va_arg(ap, char *);
						s_arg_size = va_arg(ap, int);
						//
						// CHECK THAT THE SIZE DOES NOT EXEEDS DESTINATION BUFFER SIZE
						//
						if(s_arg_size < inter_text_length+1){
							return FALSE; // TODO errorcode
						}
						strncpy(s_arg, *text, inter_text_length);
						(s_arg)[inter_text_length] = '\0';
						*text += inter_text_length;
					}
					break;
			}
		}else{
			if(*format != **text){
				return FALSE; // TODO errorcode
			}
			format++;
			(*text)++;
		}		
	}   

	va_end(ap);

	return TRUE;
}

/**
* Checks wether a string contains something or not
*
* @param	[in]	string : the string to check
* @return	TRUE if the string is not empty; FALSE else
*/
PHAPI_UTIL_EXPORTS unsigned int strfilled(const char * string){
	if(string != NULL && strlen(string) > 0){
		return TRUE;
	}
	return FALSE;
}

/**
* Indicates wether the strings are the same or not.
*
* @param	[in]	s1 : a string
* @param	[in]	s2 : another string
* @return	TRUE if the strings are the same; FALSE else.
*/
PHAPI_UTIL_EXPORTS unsigned int strequals(const char * s1, const char * s2){
	if(!strcmp(s1, s2)){
		return TRUE;
	}
	return FALSE;
}

/**
* Indicates wether the strings is of one of the given values.
*
* @param	[in]	s1 : a string
* @param	[in]	nb_values : the number of given values to check
* @param	[in]	... : the values to check (must all be char *)
* @return	TRUE if the strings are the same; FALSE else.
*/
PHAPI_UTIL_EXPORTS unsigned int strequalsamong(const char * s1, unsigned int nb_values, ...){
	va_list ap;

	if(nb_values == 0){
		return FALSE;
	}

	va_start(ap, nb_values);

	while(nb_values-- > 0){
		if(strcmp(s1, va_arg(ap, char *)) == 0){
			return TRUE;
		}
	}

	return FALSE;
}

/**
* Find the string that represent the max int value between two strings
* that represent integer values. Equivalent to a max between two int.
*
* @param	[in]	s1 : a string that represent an integer value
* @param	[in]	s2 : another string that represent an integer value
* @return	the string that represents the max integer value between s1 and s2;
*			NULL if an error occurs
*/
PHAPI_UTIL_EXPORTS const char * str_int_max(const char * int1, const char * int2){
	int i1, i2;

	if(!strfilled(int1) || !strfilled(int2)){
		return NULL;
	}

	i1 = atoi(int1);
	i2 = atoi(int2);

	if(i1 == 0 && !strequals(int1, "0")){ // an error occured
		return NULL;
	}

	if(i2 == 0 && !strequals(int2, "0")){ // an error occured
		return NULL;
	}

	if(i1 > i2){
		return int1;
	}
	return int2;
}

/**
* Find the string that represent the min int value between two strings
* that represent integer values. Equivalent to a min between two int
*
* @param	[in]	s1 : a string that represent an integer value
* @param	[in]	s2 : another string that represent an integer value
* @return	the string that represents the min integer value between s1 and s2;
*			NULL if an error occurs
*/
PHAPI_UTIL_EXPORTS const char * str_int_min(const char * int1, const char * int2){
	int i1, i2;

	if(!strfilled(int1) || !strfilled(int2)){
		return NULL;
	}

	i1 = atoi(int1);
	i2 = atoi(int2);

	if(i1 == 0 && !strequals(int1, "0")){ // an error occured
		return NULL;
	}

	if(i2 == 0 && !strequals(int2, "0")){ // an error occured
		return NULL;
	}

	if(i1 < i2){
		return int1;
	}
	return int2;
}

/**
* Calculates the length of a 'printf-style' format whithout considering
* the special tokens (%c, %d, %f, %s)
*
* @param	[in]	format : the 'printf-style' format
* @return	the length of the string format whithout considering the special tokens
*/
PHAPI_UTIL_EXPORTS unsigned int format_length_without_tokens(char * format){
	char * tmp = format;
	unsigned int length = 0;

	if(!strfilled(format)){
		return 0;
	}

	while(*tmp != '\0'){
		if(!(*tmp == '%' && *(tmp+1) != '\0' && (*(tmp+1) != 'c' || *(tmp+1) != 'd' || *(tmp+1) != 'f' || *(tmp+1) != 's'))){
			length++;
		}else{
			tmp++;
		}
		tmp++;
	}

	return length;
}

/**
* Simple log error. Simply print on stdout for the moment...
*
* @param	[in]	message, the error message to be displayed
* @param	[in]	function, the function name in which the error happened
* @param	[in]	filename, the filename from which originates the error (should be passed __FILE__)
* @param	[in]	line_number, the line of the file where the error happened (should be passed __LINE__)
*/
PHAPI_UTIL_EXPORTS void phapi_log(const char * type, const char * message, const char * function, const char * filename, unsigned int line_number){
	if(strfilled(type) && strfilled(message) && strfilled(function) && strfilled(filename))
		printf("%s : %s (in %s(...) of file %s at line %d)\n", type, message, function, filename, line_number);
	//else ??
}

PHAPI_UTIL_EXPORTS void itostr(int value, char * buffer, const size_t sizeof_buffer, int radix){
	char HEX[] = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	unsigned int cpt=0;
	unsigned int cpt2;
	int y;
	int reste;
	char * temp = malloc(sizeof_buffer * sizeof(char));
	int k;
	int l;

	while(value > 0){
		y = value;
		value = (int) y / radix;
		reste = y - value * radix;
		if(cpt >= sizeof_buffer){
			break;
		}
		temp[cpt] = HEX[reste];
		cpt++;
	}
	buffer[cpt]='\0';

	cpt2 = cpt;
	//inverse le sens de chaine
	while((k = cpt+(cpt2-cpt-1)) >= 0){
		l = cpt-cpt2;
		buffer[l] = temp[k];
		cpt2--;
	}

	free(temp);
}
