#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

// get file length
int filelen(FILE* file, long int* length) {
    if (file == NULL) return -1; // invalid file
    // get original position
    long int original = ftell(file);
    // move to EOF & get length
    fseek(file, 0, SEEK_END);
    *length = ftell(file);
    // restore to original position
    fseek(file, original, SEEK_SET);
    // return success
    return 0;
}

typedef struct {
    char* fname;
    long int lastavaidx;
    long int length;
    signed char stat;
} TH_ARGS;

// check file
int ckfile(TH_ARGS* ags) {
    // data chunk
    unsigned char chunk[128];
    for (unsigned char i = 0; i <= 127; i++) {
        chunk[i] = i;
    }
    // open file
    FILE* file = fopen(ags->fname, "r+");
    if (file == NULL) return -1; // open error
    // get length
    if (filelen(file, &(ags->length)) < 0) {
        fclose(file);
        return -1; // filelen error
    }
    // write data
    ags->stat = 1;
    fseek(file, 0, SEEK_SET);
    for (ags->lastavaidx = 0; ags->lastavaidx < ags->length; (ags->lastavaidx)+=128) {
        if (fwrite(chunk, 128, 1, file) < 1) {
            fclose(file);
            ags->stat = 3;
            return 1; // write error
        }
    }
    // read data
    ags->stat = 2;
    unsigned char rdff[128];
    fseek(file, 0, SEEK_SET);
    for (ags->lastavaidx = 0; ags->lastavaidx < ags->length; (ags->lastavaidx)+=128) {
        if (fread(rdff, 128, 1, file) < 1) {
            fclose(file);
            ags->stat = 3;
            return 1; // read error
        }
        if (rdff[ags->lastavaidx % 128] != chunk[ags->lastavaidx % 128]) {
            fclose(file);
            ags->stat = 3;
            return 1; // data error
        }
    }
    fclose(file); // close
    if (ags->lastavaidx >= ags->length) {
        ags->lastavaidx = ags->length - 1;
    }
    ags->stat = 3;
    return 0; // success
}

void* reporter(void* args) {
    TH_ARGS* ags = (TH_ARGS*) args;
    long int prev;
    while (ags->stat != 3 && ags->stat != -1) {
        if (ags->stat == 1) {
            printf("\033[A\033[2K\033[1G%s: %ld bytes writen, %.02f%%, %.2fKiB/s\n",
                   ags->fname, ags->lastavaidx, 
                   100.0 * ags->lastavaidx / ags->length, 
                   (ags->lastavaidx - prev) / 1024.0);
        } else if (ags->stat == 2) {
            printf("\033[A\033[2K\033[1G%s: %ld bytes read, %.02f%%, %.2fKiB/s\n",
                   ags->fname, ags->lastavaidx, 
                   100.0 * ags->lastavaidx / ags->length, 
                   (ags->lastavaidx - prev) / 1024.0);
        }
        sleep(1);
    }
    return NULL;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s [filename]\n", argv[0]);
        return 1;
    }
    // args & thread
    TH_ARGS thargs = {argv[1], 0, 0, 0};
    pthread_t th;
    pthread_create(&th, NULL, reporter, &thargs);
    // check file
    if (ckfile(&thargs) < 0) {
        thargs.stat = -1;
        perror("fatal");
        return -1;
    } else {
        printf("%s: last available index is %ld\n", argv[1], thargs.lastavaidx);
        return 0;
    }
}
