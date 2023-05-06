#include "myshell_parser.h"
#include "stddef.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    struct pipeline* pipeline1;

    pipeline1 = pipeline_build("ls | cat <lines\n");

    pipeline_free(pipeline1);

    return 0;
}