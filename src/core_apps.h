#pragma once
#include "defs.h"
#include "textmode.h"
#include "utils.h"
#include "heap.h"
#include "drivers/cpu.h"
#include "ext2.h"

void cls();
void info();

void load(int argc, char** argv);
void create(int argc, char** argv);
void write(int argc, char** argv);
void ls();
void cat(int argc, char** argv);
void cd(int argc, char** argv);

void sb(int argc, char** argv);
void memdmp(int argc, char** argv);
void memcpy_app(int argc, char** argv);
void memset_app(int argc, char** argv);
void exec(int argc, char** argv);
void inode_app(int argc, char** argv);
void remove(int argc, char** argv);
void mkdir(int argc, char** argv);
void rmdir(int argc, char** argv);

void heap();





