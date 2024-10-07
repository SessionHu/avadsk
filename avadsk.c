#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* get file length */
int filelen(FILE* file, long int* length) {
    if (file == NULL) {
        return -1; /* invalid file */
    }
    /* get original position */
    long int original = ftell(file);
    /* move to EOF & get length */
    fseek(file, 0, SEEK_END);
    *length = ftell(file);
    /* restore to original position */
    fseek(file, original, SEEK_SET);
    /* return success */
    return 0;
}

/* check file */
int ckfile(char* filename, long int* lastavaidx) {
    /* open file */
    FILE* file = fopen(filename, "r+");
    if (file == NULL) {
        return -1; /* open error */
    }
    /* get length */
    long int length;
    if (filelen(file, &length) < 0) {
        fclose(file);
        return -1; /* filelen error */
    }
    /* start clock */
    clock_t lastclock = clock();
    /* write data */
    printf("\n");
    unsigned char c = 0;
    fseek(file, 0, SEEK_SET);
    for (*lastavaidx = 0; *lastavaidx < length; (*lastavaidx)++) {
        if (fwrite(&c, 1, 1, file) < 1) {
            fclose(file);
            return 1; /* write error */
        }
        if (!(c++) && (*lastavaidx % 16384 == 0)) {
            clock_t thisclock = clock();
            double taken = ((double)(thisclock - lastclock)) / CLOCKS_PER_SEC;
            double speed = ((double)16384 / 1024) / taken;
            printf("\033[A\033[2K\033[1G%s: %ld bytes written, %.02f%%, %.2fKiB/s\n",
                   filename, *lastavaidx, 
                   100.0 * (*lastavaidx) / length, 
                   speed);
            lastclock = thisclock;
        }
    }
    /* read data */
    printf("\n");
    unsigned char expected = 0;
    fseek(file, 0, SEEK_SET);
    for (*lastavaidx = 0; *lastavaidx < length; (*lastavaidx)++) {
        if (fread(&c, 1, 1, file) < 1) {
            fclose(file);
            return 1; /* read error */
        }
        if (c != expected) {
            fclose(file);
            return 1; /* data error */
        }
        if (!(expected++) && (*lastavaidx % 16384 == 0)) {
            clock_t thisclock = clock();
            double taken = ((double)(thisclock - lastclock)) / CLOCKS_PER_SEC;
            double speed = ((double)16384 / 1024) / taken;
            printf("\033[A\033[2K\033[1G%s: %ld bytes read, %.02f%%, %.2fKiB/s\n",
                   filename, *lastavaidx, 
                   100.0 * (*lastavaidx) / length, 
                   speed);
            lastclock = thisclock;
        }
    }
    fclose(file); /* close */
    if (*lastavaidx >= length) {
        *lastavaidx = length - 1;
    }
    return 0; /* success */
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s [filename]\n", argv[0]);
        return 1;
    }
    /* prepare variables */
    long int lastavaidx = 0;
    /* check file */
    if (ckfile(argv[1], &lastavaidx) < 0) {
        perror("fatal");
        return -1;
    } else {
        printf("%s: last available index is %ld\n", argv[1], lastavaidx);
        return 0;
    }
}
