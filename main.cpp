//
//  main.c
//  LangRec
//
//  Created by Varun RamaRaju Dandu on 07/03/16.
// Copyright © 2016 Varun Rama Raju. All rights reserved
//

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "tinydir.h"  // source GitHub
#include <time.h>

double norm(long *vec, int n) {
    double result = 0;
    int i;
    for(i=0; i<n; i++) {
        result += (double)vec[i]*vec[i];
    }
    result = sqrt(result);

    return result;
}

double cosAngle(long *u, long *v, int n) {
    double dot = 0;
    int i;
    for(i=0; i<n; i++) dot += (double)u[i]*v[i];
    double temp = (norm(u,n)*norm(v,n));
    double result = dot/temp;

    return result;
}

int* randPerm(int SZ) {
    int *perm;
    perm = (int *) malloc(SZ * sizeof(int));

    for (int i = 0; i < SZ; i++) perm[i] = i;

    // Random perm in the order
    for (int i = 0; i < SZ; i++) {
        int j, t;
        j = rand() % (SZ-i) + i;
        t = perm[j];
        perm[j] = perm[i];
        perm[i] = t; // Swap i and j with t
    }
    return perm;

}

void genRandomHV(int *randomHV, int D)
{
    int i;
    if(D % 2 == 1) {
        printf("Dimenison D is odd");
        exit(0);
    }
    else {
        int* perm = randPerm(D);
        for(i =0; i < D/2; i++) randomHV[perm[i]] = 1;
        for(i=D/2; i < D; i++) randomHV[perm[i]] = -1;

        free(perm);
    }
}


int* lookupItemMemory(int** im, char key, int D){
    if(im[key] == NULL) {
        im[key]  = (int *) malloc(D * sizeof(int));
        genRandomHV(im[key], D);
    }
    return im[key];
}

void circularshift(int **in, int xdim, int ydim) {
    int i;
    int j;
    int *temp = in[xdim-1];
    for (i=xdim-1; i>0; i--) in[i] = in[i-1];
    in[0] = temp;

    for(i=0; i<xdim; i++) {
        int tempj = in[i][ydim-1];
        for(j=ydim-1; j>0; j--) in[i][j] = in[i][j-1];
        in[i][0] = tempj;
    }

}

int computeSumHV(char *buffer, long length, int ** im, int N, int D, long *sumHV) {
    int **block = (int **) malloc(N * sizeof(int *));
    int *nGrams;
    int isIMChanged = 0;
    int i;
    int j;
    for(i=0; i<N; i++) {
        block[i] = (int *) malloc(D * sizeof(int));
        for(j = 0; j<D; j++) block[i][j] = 0;
    }
    //long *sumHV = malloc(D * sizeof(long));
    for(i=0; i<D; i++) sumHV[i] = 0;

    int numItems;
    char key;
    nGrams = (int *) malloc(D * sizeof(int));

    for (numItems = 0; numItems < length; numItems++) {
        key = buffer[numItems];
        circularshift(block,N,D);
        if(im[key] == NULL) isIMChanged = 1;

        int * temp = lookupItemMemory(im, key, D);
        memcpy(block[0], temp, D * sizeof(int));

        if (numItems >= N) {
            memcpy(nGrams, block[0], D * sizeof(int));
            for(i = 1; i<N; i++) {
                for(j=0; j<D; j++) nGrams[j] *= block[i][j];
            }

            for(i=0; i<D; i++) sumHV[i] += nGrams[i];
        }
    }
    free(nGrams);
    for(i=0; i<N; i++) free(block[i]);
    free(block);
    //return sumHV;
    return isIMChanged;
}

void binarizeHV(long *v, int size){
    int i;
    int threshold = 0;
    for(i =0; i<size; i++) {
        if (v[i]>threshold) v[i] = 1;
        else v[i] = -1;
    }
}

const int numOfLang = 22;
const char *langLabels[22] = {"afr", "bul", "ces", "dan", "nld", "deu", "eng", "est", "fin", "fra", "ell", "hun", "ita", "lav", "lit", "pol", "por", "ron", "slk", "slv", "spa", "swe"};
const char *langMap[22] = {"af", "bg", "cs", "da", "nl", "de", "en", "et", "fi", "fr", "el", "hu", "it", "lv", "lt", "pl", "pt", "ro", "sk", "sl", "es", "sv"};

long load_file(char const* path, char* buffer) {
    long length = -1;
    FILE * f = fopen (path, "rb"); //was "rb"
    if (f){
        fseek (f, 0, SEEK_END);
        length = ftell (f);
        fseek (f, 0, SEEK_SET);
        fread (buffer, sizeof(char), length, f);
        fclose (f);
    }
    return length;
}

void buildLanguage(int N, int D, long **langAM, int **itemMemory) {
    int i;
    char *path= (char*)calloc(150, sizeof(char));
    char *buffer = (char*)calloc(10*1024*1024, sizeof(char));
    long length;
    //long* langHV;

    for(i=0; i<numOfLang; i++) {
        path[0] = '\0';
        strcat(path, "/Users/Varun/Desktop/LangRec12/training_texts/");
        strcat(path, langLabels[i]);
        strcat(path, ".txt");
        length = load_file(path, buffer);
        printf("Loaded traning language file %s \n", path);
        langAM[i] = (long *) malloc(D * sizeof(long));
        computeSumHV(buffer,length,itemMemory, N, D, langAM[i]);
    }
    free(path);
    free(buffer);
}


void test(int **itemMemory, long **langAM, int N, int D) {
    int total = 0;
    int correct = 0;
    float accuracy = 0;

    char actualLabel[3];
    char predicLang[3];
    int i;
    char *path = (char*)calloc(150, sizeof(char));
    char *buffer = (char*)calloc(10*1024*1024, sizeof(char));
    long length = -1;
    long* textHV = (long *) malloc(D * sizeof(long));

    tinydir_dir dir;
    tinydir_open_sorted(&dir, "/Users/Varun/Desktop/LangRec12/testing_texts");
    double angle;
    double maxAngle;
    int k;
    actualLabel[2] = '\0';
    predicLang[2] = '\0';
    printf("dir.n_files = %d \n", dir.n_files);

    for (k = 0; k<dir.n_files; k++) {
        tinydir_file file;
        tinydir_readfile_n(&dir, &file, k);
        if (strstr(file.name, ".txt") == NULL) continue;
        for(i=0; i<2; i++)
            actualLabel[i] = file.name[i];
        path[0] = '\0';
        strcpy(path, "/Users/Varun/Desktop/LangRec12/testing_texts/");
        strcat(path, file.name);
        length = load_file(path, buffer);
        printf("Loaded testing text file %s \n", path);

        int isChanged = computeSumHV(buffer, length, itemMemory, N, D, textHV);
        binarizeHV(textHV, D);
        if(isChanged) {
            printf("NEW UNSEEN ITEM IN TEST FILE \n");
            exit(0);
        }

        maxAngle = -1;
        for(i=0; i<numOfLang; i++){
            angle = cosAngle(langAM[i], textHV, D);
            if(angle > maxAngle) {
                maxAngle = angle;
                strcpy(predicLang, langMap[i]);
            }
        }
        if(strcmp(predicLang, actualLabel) == 0) correct += 1;
        else printf("%s --> %s \n", actualLabel, predicLang);
        total = total + 1;
    }
    free(path);//printf("Total 1= %f \n", total);
    free(textHV);//printf("Total 2 = %f \n", total);
    free(buffer);//printf("Total 3= %f \n", total);
    tinydir_close(&dir);
    //printf("Total 4= %f \n", total);
    //printf("correct = %f \n", correct);
    accuracy = correct/(float)total * 100;
    printf("accuracy = %f% \n", accuracy);
}


int main(int argc, const char * argv[]) {
    int N = 3;
    int D = 10000;
    int i = 0;
    long **langAM = (long **) malloc(numOfLang * sizeof(long *));
    int **itemMemory= (int **) malloc(256 * sizeof(int *)); //256 characters possible
    for (i=0; i<256; i++) itemMemory[i] = NULL;

    buildLanguage(N, D, langAM, itemMemory);
    test(itemMemory, langAM, N, D);
    free(itemMemory);
    free(langAM);
}
