#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>

#define DEBUG   0
#define STR_LENG    255
char notExistWordList[10000][STR_LENG] = {0};
char existWordList[10000][STR_LENG] = {0};
int cntNotExistWord = 0;
int cntExistWord = 0;
static int temp = 0;

bool checkFirstMeaninglessData(char *pstr)
{
    if (pstr[0] == '\r' || pstr[0] == '\n' || pstr[0] == '\v' || pstr[0] == '\t')
        return true;
    else
        return false;
}

bool checkExistingWord(char *argv, char *word)
{
    FILE *fpBnf = NULL;
    char prefixWord[STR_LENG] = "!pronounce ";
    char pstr[STR_LENG] = {0};
    char onlyWord[STR_LENG] = {0};
    int i = 0;

    fpBnf = fopen(argv, "r");
    if (fpBnf == NULL)
    {
        printf("bnf file open error\n");
        return false;
    }

    strncpy(onlyWord, word, strlen(word)+1);

    //pre condition for result array
    //1. check already searched existing word
    for (i=0; i<cntExistWord; i++)
    {
        if (strcmp(existWordList[i], onlyWord) == 0)
            goto exitCheckFunc;
    }

    //2. check already searched not existing word
    for (i=0; i<cntNotExistWord; i++)
    {
        if (strcmp(notExistWordList[i], onlyWord) == 0)
            goto exitCheckFunc;
    }

    //make prefix word as like !pronounce xxx for comparing word
    strcat(prefixWord, onlyWord);

    while (1)
    {
        if (feof(fpBnf)!=0)
        {
            strcpy(notExistWordList[cntNotExistWord++], onlyWord);
#if DEBUG
            printf("--> not exist: %s\n", onlyWord);
#endif
            break;
        }

        //get one sentece from xxx.bnf file
        fgets(pstr,sizeof(pstr),fpBnf);

        //skip carriage return or line feed
        if (pstr[0] == '\r' || pstr[0] == '\n')
        {
            memset(pstr, STR_LENG, 0);
            continue;
        }

        //check existing word
        if (strstr(pstr, prefixWord))
        {
            strcpy(existWordList[cntExistWord++], onlyWord);
#if DEBUG
            printf("--> exist: %s\n", onlyWord);
#endif
            break;
        }
        memset(pstr, STR_LENG, 0);
    }

exitCheckFunc:
    fclose(fpBnf);
    return true;
}

int saveNotExistWordList()
{
    FILE *fp = NULL;
    int i = 0;

    fp = fopen("notExistWordList.txt", "wt");
    if (fp == NULL)
    {
        printf("notExistWordList.txt open error\n");
        return 0;
    }

    for (i=0; i<cntNotExistWord; i++)
        fprintf(fp, "%s\n", notExistWordList[i]);

    fclose(fp);
}

int saveExistWordList()
{
    FILE *fp = NULL;
    int i = 0;

    fp = fopen("existWordList.txt", "wt");
    if (fp == NULL)
    {
        printf("existWordList.txt open error\n");
        return 0;
    }

    for (i=0; i<cntExistWord; i++)
        fprintf(fp, "%s\n", existWordList[i]);

    fclose(fp);
}

int main( int argc, char *argv[] )
{
    FILE *fpTxt = NULL;
	char pstr[STR_LENG] = {0};
    char *deleteSepeator = " <>[],.?|\t\n\v\r";

    if (strstr(argv[1], ".bnf") && strstr(argv[2], ".txt"))
    {
        fpTxt = fopen(argv[2], "r");

        if (fpTxt == NULL)
        {
            printf("txt file open error\n");
			return 0;
        }

        while (1)
		{
			if (feof(fpTxt)!=0)
			{
				printf("success!\ncheck as the below files\nexistWordList.txt\nnotExistWordList.txt\n");
				break;
			}

            //get one sentence from 2.txt file
			fgets(pstr,sizeof(pstr),fpTxt);
#if DEBUG
            printf("------------------------------------------------\n");
            printf("pattern: %s", pstr);
#endif
            //skip carriage return or line feed and so on..
            if (checkFirstMeaninglessData(pstr) == true)
            {
                memset(pstr, STR_LENG, 0);
                continue;
            }

            //get one word in the sentence
            char *word = NULL;
            word = strtok(pstr, deleteSepeator);
            while (word != NULL)
            {
                if (checkExistingWord(argv[1], word) == false)
                {
                    printf("Terminated due to an error.\n");
                    goto exitMain;
                }
                word = strtok(NULL, deleteSepeator);
            }
            memset(pstr, STR_LENG, 0);
        }
    }
    else
    {
        printf("%s, %d input param error! \n", __func__, __LINE__);
    }

    saveNotExistWordList();
    saveExistWordList();

exitMain:
    fclose(fpTxt);
    return 0;
}