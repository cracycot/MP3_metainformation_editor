#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <wchar.h>
#include "string.h"

#pragma pack(push, 1)
union Metadata{
    struct {
        char  naim[3];
        char  version[2];
        unsigned int unsynchronisationFlag: 1;
        unsigned int extendedHeaderFlag: 1;
        unsigned int experimentalFlag: 1;
        unsigned int footerFlag: 1;
        unsigned int garbageFlags: 4;
        unsigned int size: 32;
    } fields;
    unsigned char bytes[10];
};

union Frame {
    struct  {
        unsigned char  naim [4];
        unsigned int size: 32;
        unsigned char  flags[2];
    };
    unsigned  char bytes[10];
};

#pragma pack(pop)


uint32_t big_to_little_endian(uint32_t num) {
    return ((num>>24)&0xFF) | ((num>>8)&0xFF00) | ((num<<8)&0xFF0000) | ((num<<24)&0xFF000000);
}

int str_check(char * x, char * x2) {
    int flag = 1;
    for (int i =0; i < 4; i ++) {
        if (x[i] != x2[i]) {
            flag = 0;
        }
    }
    return flag;
}


void write_in_file() {
    FILE *file = fopen("your_file_path", "r+");
    union Metadata a;
    int s = fread(a.bytes, sizeof(char), 10, file);
    if (s  == 0) {
        printf("EROOR");
    }

}
int main(int argc, char **argv) {
    FILE *file = fopen("your_file_path", "r+");
    if (file == NULL) {
        perror("Ошибка создания файла!");
        return 1;
    }
    union Metadata a;
    int s = fread(a.bytes, sizeof(char), 10, file);
    if (s  == 0) {
        printf("EROOR");
    }
    char * current_teg = (char *)(malloc(sizeof(char ) * strlen(argv[1])));
    int len = 0;
    for (int i = 0; i < strlen(argv[2]); i ++) {
        if(argv[2][i] == '=') {
            len = i;
            break;
        }
    }
    char * tag_name = (char *) (malloc(sizeof (char) * (strlen(argv[2]) - len - 1) ));
    char * command = (char * ) (malloc(sizeof (char ) *(len )));
    char * data;
    for (int i = 0; i < len; i ++ ) {
        command[i] = argv[2][i];
    }

    for (int i = len + 1; i < strlen(argv[2]); i ++) {
        tag_name[ i - len - 1] = argv[2][i];
    }
    if (argc == 4) {
        int len = 0;
        for (int i = 0; i < strlen(argv[3]); i ++) {
            if(argv[3][i] == '=') {
                len = i;
                break;
            }
        }
        data = (char * ) (malloc(sizeof (char ) *(len )));
        for (int i = len + 1; i < strlen(argv[3]); i ++) {
            data[ i - len - 1] = argv[3][i];
        }

    }
    int k = 0;
    int flag = 0;
    if (str_check(command, "--set")) {
        flag = 1;
    }
    else if (str_check(command, "--get")) {
        flag = 2;
    }
    //printf("%s\n", command);
    while (1) {
        union Frame m;
        fread(m.bytes, sizeof(char), 10, file);
        m.size = big_to_little_endian(m.size);
        if (m.size == 0) {
            fseek(file, -10, SEEK_CUR);
            break;
        }
        if (flag == 1 &&  str_check(m.naim, tag_name) == 1){
            char * z = (char *)malloc(sizeof(char )* m.size);
            for (int i = 0; i < m.size; i ++) {
                if (i < strlen(data)) {
                    z[i] = data[i];
                }
                else {
                    z[i] = 0;
                }
            }
            int count = 0;
            fseek(file, 0, SEEK_CUR);
            count += fwrite(z, sizeof(char), sizeof(char ) * m.size, file);
            fclose(file);
            printf("%d\n", count);
            free(z);
            break;
        }
        else if (flag == 2 && str_check(m.naim, tag_name) == 1) {
            char *value = malloc(m.size * sizeof(char));

            fread(value, sizeof(char), m.size, file);


            if (m.naim != NULL && m.size != 0) {
                for (int i = 0; i < 4; i++) {
                    if (m.naim[i] > 0) {
                        printf("%c", m.naim[i]);
                    }
                }
                printf(" ");
                for (int i = 0; i < m.size; i++) {
                    if (value[i] > 0) {
                        printf("%c", value[i]);
                    }
                }
                printf("\n");
            }
            break;

        }
        else {
            char *value = malloc(m.size * sizeof(char));

            fread(value, sizeof(char), m.size, file);

            if(flag == 0) {
                if (m.naim != NULL && m.size != 0) {
                    for (int i = 0; i < 4; i++) {
                        if (m.naim[i] > 0) {
                            printf("%c", m.naim[i]);
                        }
                    }
                    printf(" ");
                    for (int i = 0; i < m.size; i++) {
                        if (value[i] > 0) {
                            printf("%c", value[i]);
                        }
                    }
                    printf("\n");
                }
            }
            free(value);
            k++;
        }

    }
    fclose(file);
    return 0;
}
