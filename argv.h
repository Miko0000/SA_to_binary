#ifndef ARGV_H
#define ARGV_H
#ifdef __cplusplus
extern "C" { // allows C specific stuff to be used in C++
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

struct argv {
	char *data;
	int size;
};



struct argv valid_ = { .data = (char[512*30]){'\0'}, .size = 512 };
struct argv *valid = &valid_;
char *argv_set(struct argv *valid, int pos, const char *flag, int argc, char **argv){
	char *current;
	char *next;
	int flen = strlen(flag);
	for(int i = 0; i < argc; i++){
		current = argv[i];

		if((i + 1) >= argc){
			next = (char *) "-\0";
		} else {
			next = argv[i + 1];
		}

		if(strlen(current) >= flen && strncmp(current, flag, flen) == 0){
			memcpy(&valid->data[pos * valid->size], next, strlen(next) + 1);
			i++;

			continue ;
		}
	}

	return next;
}

char *argv_get(struct argv *valid, int pos){
	return &valid->data[pos * valid->size];
}

#ifdef __cplusplus
}
#endif
#endif
