#pragma once
#include "Typedefs.h"
#include "TextMode.h"
#include "kconvert.h"
#include "Filesystem.h"

void cls();
void mem();
void ls();
void load(int argc, char** argv);
void exec(int argc, char** argv);
void setbytes(int argc, char** argv);
void memdmp(int argc, char** argv);
void create(int argc, char** argv);
void save(int argc, char** argv);
void del(int argc, char** argv);



