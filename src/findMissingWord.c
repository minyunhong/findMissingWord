#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>
#include <ctype.h>

#define DEBUG   0
#define STR_LENG    255
char notExistWordList[10000][STR_LENG] = {0};
char existWordList[10000][STR_LENG] = {0};
char existDiffCaseWordList[10000][STR_LENG] = {0};
char existOnlyOneWordList[10000][STR_LENG] = {0};
int cntNotExistWord = 0;
int cntExistWord = 0;
int cntExistDiffCaseWord = 0;
int cntExistOnlyOneWord = 0;
static int temp = 0;

//It is implemented in compare_list.cpp file.
//you can compare two wordlist-files.
extern int makeFileCompareResult(char*, char*);

int compareFunction(const void *a, const void *b)
{
    return (strcmp((char*)a, (char*)b));
}

void changeToLowerWord(char* word)
{
    int length = strlen(word);
    int i = 0;

    for (i=0; i<length; i++)
        word[i] = (char)tolower(word[i]);
}

bool isAllCapitalWord(char* word)
{
    int length = strlen(word);
    int i = 0;

    if (length == 1)
        return false;

    for (i=0; i<length; i++)
    {
        if (islower(word[i]))
            return false;
    }
#if DEBUG
    printf("All capital word %s\n", word);
#endif

    return true;
}

void preProcessingForCaseSensitiveWord(char *word)
{
    if (!isAllCapitalWord(word))
        changeToLowerWord(word);
}

bool checkFirstMeaninglessData(char *pstr)
{
    if (pstr[0] == '\r' || pstr[0] == '\n' || pstr[0] == '\v' || pstr[0] == '\t')
        return true;
    else
        return false;
}

bool checkAlreadySearchedWord(char *word)
{
    int i = 0;

    //1. check already searched existing word
    for (i=0; i<cntExistWord; i++)
    {
        if (strcmp(existWordList[i], word) == 0)
            return true;
    }

    //2. check already searched not existing word
    for (i=0; i<cntNotExistWord; i++)
    {
        if (strcmp(notExistWordList[i], word) == 0)
            return true;
    }

    //3. check already searched existing different case word
    for (i=0; i<cntExistDiffCaseWord; i++)
    {
        if (strcmp(existDiffCaseWordList[i], word) == 0)
            return true;
    }

    //4. check already searched existing onlyone word
    for (i=0; i<cntExistOnlyOneWord; i++)
    {
        if (strcmp(existOnlyOneWordList[i], word) == 0)
            return true;
    }

    return false;
}

bool checkExistingWord(char *argv, char *word)
{
    FILE *fpBnf = NULL;
    char *ptr = NULL;
    char *delimiter = " ";
    unsigned char *pSafe;
    char prefixWord[STR_LENG] = "!pronounce ";
    char pstr[STR_LENG] = {0};
    char onlyWord[STR_LENG] = {0};
    bool existDiffCase = false;

    fpBnf = fopen(argv, "r");
    if (fpBnf == NULL)
    {
        printf("bnf file open error\n");
        return false;
    }

    strncpy(onlyWord, word, strlen(word)+1);

    //pre condition for result array
    if (checkAlreadySearchedWord(onlyWord) == true)
        goto exitCheckFunc;

    while (1)
    {
        if (feof(fpBnf)!=0)
        {
            if (existDiffCase == true)
            {
                preProcessingForCaseSensitiveWord(onlyWord);
                strcpy(existDiffCaseWordList[cntExistDiffCaseWord++], onlyWord);
#if DEBUG
                printf("--> exist diff case: %s\n", onlyWord);
#endif
            }
            else
            {
                preProcessingForCaseSensitiveWord(onlyWord);
                strcpy(notExistWordList[cntNotExistWord++], onlyWord);
#if DEBUG
                printf("--> not exist: %s\n", onlyWord);
#endif
            }
            break;
        }

        //get one sentece from xxx.bnf file
        fgets(pstr,sizeof(pstr),fpBnf);

        //skip carriage return or line feed
        if (pstr[0] == '\r' || pstr[0] == '\n' || strstr(pstr, prefixWord) == NULL)
        {
            memset(pstr, 0, STR_LENG);
            continue;
        }

        //check existing word
        ptr = NULL;
        if (strtok_r(pstr, delimiter, (char**)&pSafe) != NULL)
        {
            ptr = strtok_r(NULL, delimiter, (char**)&pSafe);

            if (!strcasecmp(ptr, onlyWord))
            {
                if (!strcmp(ptr, onlyWord)) // perfectly the same word exists in .bnf
                {
                    if (strchr(strtok_r(NULL, delimiter, (char**)&pSafe), ';'))  // exists only one pronunciation
                    {
                        preProcessingForCaseSensitiveWord(onlyWord);
                        strcpy(existOnlyOneWordList[cntExistOnlyOneWord++], onlyWord);
#if DEBUG
                        printf("--> exist only one word: %s\n", onlyWord);
#endif
                    }
                    else    // exists two or more pronunciations
                    {
                        preProcessingForCaseSensitiveWord(onlyWord);
                        strcpy(existWordList[cntExistWord++], onlyWord);
#if DEBUG
                        printf("--> exist: %s\n", onlyWord);
#endif
                    }
                    break;
                }
                else                       // the word with different case exists in .bnf
                    existDiffCase = true;
            }
        }
        memset(pstr, 0, STR_LENG);
    }
    existDiffCase = false;

exitCheckFunc:
    fclose(fpBnf);
    return true;
}

int saveNotExistWordList()
{
    FILE *fp = NULL;
    int i = 0;
    char previous_word[STR_LENG] = "";

    fp = fopen("notExistWordList.txt", "wt");
    if (fp == NULL)
    {
        printf("notExistWordList.txt open error\n");
        return 0;
    }

    //1. sort the word list
    qsort(notExistWordList, cntNotExistWord, sizeof(*notExistWordList), compareFunction);

    //2. remove duplicated word and write the list to file
    for (i=0; i<cntNotExistWord; i++){
        if(strcmp(previous_word, notExistWordList[i]))
            fprintf(fp, "%s\n", notExistWordList[i]);
        strcpy(previous_word, notExistWordList[i]);
    }

    fprintf(fp, "\nonly one word exists..\n");
    qsort(existOnlyOneWordList, cntExistOnlyOneWord, sizeof(*existOnlyOneWordList), compareFunction);
    for (i=0, strcpy(previous_word,""); i<cntExistOnlyOneWord; i++)
    {
        if (strcmp(previous_word, existOnlyOneWordList[i]))
            fprintf(fp, "%s\n", existOnlyOneWordList[i]);
        strcpy(previous_word, existOnlyOneWordList[i]);
    }

    fclose(fp);
}

int saveExistWordList()
{
    FILE *fp = NULL;
    int i = 0;
    char previous_word[STR_LENG] = "";

    fp = fopen("existWordList.txt", "wt");
    if (fp == NULL)
    {
        printf("existWordList.txt open error\n");
        return 0;
    }

    qsort(existWordList, cntExistWord, sizeof(*existWordList), compareFunction);
    for (i=0; i<cntExistWord; i++)
    {
        if (strcmp(previous_word, existWordList[i]))
            fprintf(fp, "%s\n", existWordList[i]);
        strcpy(previous_word, existWordList[i]);
    }

    fprintf(fp, "\ndiffrent case word exists..\n");
    qsort(existDiffCaseWordList, cntExistDiffCaseWord, sizeof(*existDiffCaseWordList), compareFunction);
    for (i=0, strcpy(previous_word,""); i<cntExistDiffCaseWord; i++)
    {
        if (strcmp(previous_word, existDiffCaseWordList[i]))
            fprintf(fp, "%s\n", existDiffCaseWordList[i]);
        strcpy(previous_word, existDiffCaseWordList[i]);
    }

    fclose(fp);
}

int main( int argc, char *argv[] )
{
    FILE *fpTxt = NULL;
    char pstr[STR_LENG] = {0};
    char *deleteSepeator = " [],./?\t\n\v\r";
    unsigned char *pSafe = NULL;

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
                memset(pstr, 0, STR_LENG);
                continue;
            }

            //get one word in the sentence
            char *word = NULL;
            word = strtok_r(pstr, deleteSepeator, (char**)&pSafe);
            while (word != NULL)
            {
                if (strchr(word, '<') != NULL || strchr(word, '>') != NULL)
                {
                    // '<', '>' skip case
                    word = strtok_r(NULL, deleteSepeator, (char**)&pSafe);
                }
                else
                {
                    if (checkExistingWord(argv[1], word) == false)
                    {
                        printf("Terminated due to an error.\n");
                        goto exitMain;
                    }
                    word = strtok_r(NULL, deleteSepeator, (char**)&pSafe);
                }
            }
            memset(pstr, 0, STR_LENG);
        }
        saveNotExistWordList();
        saveExistWordList();
    }
    else if(strstr(argv[1], ".txt") && strstr(argv[2], ".txt"))
    {
        //Compare two txt file and find different word lists.
        //This function generates CompareResult.txt file as a result.
    	makeFileCompareResult(argv[1], argv[2]);
        return 0;
    }
    else
    {
        printf("%s, %d input param error! \n", __func__, __LINE__);
    }

exitMain:
    fclose(fpTxt);
    return 0;
}
