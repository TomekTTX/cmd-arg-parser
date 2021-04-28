#pragma once
#include <stdbool.h>

typedef struct {
	char **content;
	int count;
} args_t;

args_t parseArgv(char **argv, char initChar, char **resultBuf, int argc, bool append);

const char *getArg(const char *key, const args_t *parsedArgs);

bool getArgBool(const char *key, const args_t *parsedArgs);
int getArgInt(const char *key, const args_t *parsedArgs);
long getArgLong(const char *key, const args_t *parsedArgs);
long long getArgLLong(const char *key, const args_t *parsedArgs);
double getArgDouble(const char *key, const args_t *parsedArgs);

void dumpArgs(const args_t *parsedArgs);

//args_t parseArgs(char *argstr, char initChar, char **resultBuf, int resultBufCnt, bool append);
//bool getArgFormat(void *dst, const char *format, const char *key, const args_t *parsedArgs);
