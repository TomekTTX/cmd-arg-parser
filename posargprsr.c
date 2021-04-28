#include "posargprsr.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#ifndef RETURN_FAIL
#define RETURN_FAIL -1;
#endif // !RETURN_FAIL


int splitArg(const char *arg, const char **resultBuf) {
	int argcnt = strlen(arg), i;

	for (i = 0; i < argcnt - 1; ++i) {
		resultBuf[2 * i] = arg + i;
		resultBuf[2 * i + 1] = (char *)1;
	}
	resultBuf[2 * i] = arg + i;

	return 2 * argcnt - 1;
}
 /*
args_t parseArgs(char *argstr, char initChar, char **resultBuf, int resultBufCnt, bool append) {
	static const char delim[] = " ";
	int resInd = 0;
	bool valueExpected = false;
	char *token, *next, *args = strchr(argstr, initChar);
	
	token = strtok_s(args, delim, &next);
	while (token != NULL && resInd < resultBufCnt - 1) {
		if (token[0] == initChar) {
			if (valueExpected)
				resultBuf[resInd++] = (char *)1;
			resultBuf[resInd++] = token + 1 + (token[1] == initChar);
			valueExpected = true;
		}
		else if (valueExpected) {
			resultBuf[resInd++] = token;
			valueExpected = false;
		}
		else if (append)
			resultBuf[resInd][-1] = ' ';
		token = strtok_s(NULL, delim, &next);
	}

	if (resInd & 1)
		resultBuf[resInd++] = (char *)1;

	args_t ret = { resultBuf, resInd / 2 };
	return ret;
}
*/

args_t parseArgv(char **argv, char initChar, char **resultBuf, int argc, bool append) {
	const char valForcedStr[] = { initChar, initChar, '\0' };
	int i = -1, resInd = 0;
	bool valueExpected = false, valueForced = false;

	while (++i < argc && argv[i][0] != initChar); // skip all uninteresting arguments in argv
	while (i < argc) {
		// two repeating initChars will force the next token to be interpreted as a value
		valueForced = (valueExpected && strcmp(argv[i], valForcedStr) == 0 && argv[i + 1] != NULL);

		// if a key token is encountered and it was
		// not forced to be interpreted as a value
		if (argv[i][0] == initChar && !valueForced) {	
			if (valueExpected)						
				resultBuf[resInd++] = (char *)1; // if no value was found, substitute it with 1	
			if (argv[i][1] != initChar)
				// one-char arguments; run splitArg in case there are multiple specified at once
				resInd += splitArg(argv[i] + 1, resultBuf + resInd);
			else {
				// multichar arguments, put a space at the beggining to identify them later
				argv[i][1] = ' ';
				resultBuf[resInd++] = argv[i] + 1;
			}
			valueExpected = true;
		}
		else if (valueExpected) { // if a value token is encountered and a value has not yet been provided
			i += valueForced;	// skip the valForcedStr to get the value after it
			resultBuf[resInd++] = argv[i];
			valueExpected = false;
		}
		else if (append) // if a value is encountered but has already been provided (append mode only)
			argv[i][-1] = ' ';

		++i;
	}

	if (resInd & 1)
		resultBuf[resInd++] = (char *)1;

	args_t ret = { resultBuf, resInd / 2 };
	return ret;
}

void dumpArgs(const args_t *pArgs) {
	char *curKey, *curVal;
	for (int i = 0; i < pArgs->count; ++i) {
		curKey = pArgs->content[2 * i];
		curVal = pArgs->content[2 * i + 1];
		if (curKey[0] == ' ')
			printf("(long)%s => %s\n", curKey, curVal == (char *)1 ? "true" : curVal);
		else {
			for (int j = 0, len = strlen(curKey); j < len; ++j) {
				curVal = pArgs->content[2 * i++ + 1];
				printf("%c => %s\n", curKey[j], curVal == (char *)1 ? "true" : curVal);
			}
			--i;
		}
	}
}

const char *getArg(const char *key, const args_t *pArgs) {
	bool longArg = (strlen(key) > 1);

	for (int i = 0; i < pArgs->count; ++i) {
		char *curKey = pArgs->content[2 * i];

		if (longArg && curKey[0] == ' ' && strcmp(key, curKey + 1) == 0)
			return pArgs->content[2 * i + 1];

		if (!longArg && curKey[0] != ' ') {
			for (int j = 0, curLen = strlen(curKey); j < curLen; ++j) {
				if (key[0] == curKey[j])
					return pArgs->content[2 * i + 1];
				++i;
			}
			--i;
		}
	}

	return NULL;
}

bool getArgBool(const char *key, const args_t *parsedArgs) {
	return getArg(key, parsedArgs) != NULL;
}

int getArgInt(const char *key, const args_t *parsedArgs) {
	const char *result = getArg(key, parsedArgs);

	if (result && result != (char *)(1))
		return atoi(result);
	return RETURN_FAIL;
}

long getArgLong(const char *key, const args_t *parsedArgs) {
	const char *result = getArg(key, parsedArgs);

	if (result && result != (char *)(1))
		return atol(result);
	return RETURN_FAIL;
}

long long getArgLLong(const char *key, const args_t *parsedArgs) {
	const char *result = getArg(key, parsedArgs);

	if (result && result != (char *)(1))
		return atoll(result);
	return RETURN_FAIL;
}

double getArgDouble(const char *key, const args_t *parsedArgs) {
	const char *result = getArg(key, parsedArgs);

	if (result && result != (char *)(1))
		return atof(result);
	return RETURN_FAIL;

}

/*
bool getArgFormat(void *dst, const char *format, const char *key, const args_t *parsedArgs) {
	const char *result = getArg(key, parsedArgs);

	if (result && sscanf_s(result, format, dst) > 0)
		return true;
	return false;
}
*/
