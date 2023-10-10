#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct Pattern_struct {
    char* pat;  // string pattern - exs: "hello", "-ell-", "-----"
    int count;  // frequency of pattern in the current word list
    int changes; // number of occurrences of the new letter in this pattern
} Pattern;

// Function to print the word list
void printWordList(char** words, int numWords) {
    for (int i = 0; i < numWords; i++) {
        printf("  %s\n", words[i]);
    }
}

// Function to print the letter list
void printLetterList(char* letters) {
    printf("Letters Guessed: ");
    for (int i = 0; i < 26; i++) {
        if (letters[i]) {
            printf("%c ", 'a' + i);
        }
    }
    printf("\n");
}

// Function to print the pattern list
void printPatternList(Pattern *patterns, int numPatterns)
{
    for (int i = 0; i < numPatterns; i++)
	{
        printf("  %s \tcount = %d \tchanges = %d\n", patterns[i].pat, patterns[i].count, patterns[i].changes);
	}
}


void addWord(char ***words, int *numWords, int *maxWords, char *newWord)
{
    if (*numWords >= *maxWords)
	{
        *maxWords *= 2;
 
        char **newWords = (char **)malloc(*maxWords * sizeof(char *));
 
        for (int i = 0; i < *numWords; i++)
    	{
            newWords[i] = (*words)[i];
    	}
 
        free(*words);
 
        *words = newWords;
	}
 
	(*words)[*numWords] = (char *)malloc((strlen(newWord) + 1) * sizeof(char));
    strcpy((*words)[*numWords], newWord);
 
	(*numWords)++;
}


int strNumMods(char* str1, char* str2) {
    int diffCount = 0;
    while (*str1 != '\0' || *str2 != '\0') {
        if (*str1 != *str2) {
            diffCount++;
        }
        if (*str1 != '\0') {
            str1++;
        }
        if (*str2 != '\0') {
            str2++;
        }
    }
    return diffCount;
}

int strDiffInd(char* str1, char* str2) {
    int index = 0;
    while (*str1 != '\0' && *str2 != '\0') {
        if (*str1 != *str2) {
            return index;
        }
        str1++;
        str2++;
        index++;
    }
    return index;
}

int main(int argc, char* argv[]) {
    printf("Welcome to the (Evil) Word Guessing Game!\n\n");

    bool solved = false;

    int wordSize = 5;
    int numGuesses = 26;
    bool statsMode = false;
    bool wordListMode = false;
    bool letterListMode = false;
    bool patternListMode = false;
    bool verboseMode = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-n") == 0) {
            i++;
            if (i < argc) {
                wordSize = atoi(argv[i]);
            } else {
                printf("Invalid usage of -n. Specify a word size.\n");
                printf("Terminating program...\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-g") == 0) {
            i++;
            if (i < argc) {
                numGuesses = atoi(argv[i]);
                if (numGuesses <= 0) {
                    printf("Invalid number of guesses.\n");
                    printf("Terminating program...\n");
                    return 1;
                }
            } else {
                printf("Invalid usage of -g. Specify the number of guesses.\n");
                printf("Terminating program...\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-s") == 0) {
            statsMode = true;
        } else if (strcmp(argv[i], "-w") == 0) {
            wordListMode = true;
        } else if (strcmp(argv[i], "-l") == 0) {
            letterListMode = true;
        } else if (strcmp(argv[i], "-p") == 0) {
            patternListMode = true;
        } else if (strcmp(argv[i], "-v") == 0) {
            verboseMode = true;
            statsMode = true;
            wordListMode = true;
            letterListMode = true;
            patternListMode = true;
        } else {
            printf("Invalid command-line argument.");
            printf("Terminating program...\n");
            return 1;
        }
    
    }
    printf("Game Settings:\n");
    printf("  Word Size = %d\n", wordSize);
    printf("  Number of Guesses = %d\n", numGuesses);
    printf("  View Stats Mode = %s\n", statsMode ? "ON" : "OFF");
    printf("  View Word List Mode = %s\n", wordListMode ? "ON" : "OFF");
    printf("  View Letter List Mode = %s\n", letterListMode ? "ON" : "OFF");
    printf("  View Pattern List Mode = %s\n", patternListMode ? "ON" : "OFF");

    if (wordSize < 2) {
        printf("Invalid word size. \n");
        printf("Terminating program...\n");
        return 1;
    }
    
    int capacity = 4;
    char** wordList = (char**)malloc(capacity * sizeof(char*));

    if (wordList == NULL) {
        fprintf(stderr, "Memory allocation for wordList failed.\n");
        return 1;
    }

    int numWords = 0;

    FILE* file = fopen("dictionary.txt", "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open dictionary.txt.\n");
        free(wordList);
        return 1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        size_t len = strlen(line);
        if (len > 0 && (line[len - 1] == '\n' || line[len - 1] == '\r')) {
            line[len - 1] = '\0';
        }

        if (strlen(line) == wordSize) {
            addWord(&wordList, &numWords, &capacity, line);
        }
    }

    fclose(file);
    if (statsMode) {
        printf("The dictionary contains 127142 words total.\n");
        printf("The longest word floccinaucinihilipilification has 29 chars.\n");
        printf("The dictionary contains %d words of length %d.\n", numWords, wordSize);
        printf("Max size of the dynamic words array is %d.\n", capacity);
    }
     
    if (wordListMode)
	{
        printf("Words of length %d:\n", wordSize);
   	 for (int i = 0; i < numWords; i++)
    	{
            printf("  %s\n", wordList[i]);
    	}
	}

    if (wordSize > 24) {
        printf("Dictionary has no words of length %d.\n", wordSize);
        printf("Terminating program...\n");
        return 1;
    }


    char guessedLetters[26]; 
    memset(guessedLetters, 0, 26);
 
    char finalWordPattern[wordSize + 1];
    for (int i = 0; i < wordSize; i++)
	{
        finalWordPattern[i] = '-';
	}
    finalWordPattern[wordSize] = '\0';
    printf("The word pattern is: %s\n", finalWordPattern);

 
    char currentPattern[wordSize + 1];
    while (numGuesses > 0 && !solved)
	{
        char currentGuess;
        do
    	{
            printf("\nNumber of guesses remaining: %d\n", numGuesses);
            if (letterListMode)
        	{
        	    printf("Previously guessed letters: ");
                for (int i = 0; i < 26; i++)
            	{
                    if (guessedLetters[i])
                	{
                        printf("%c ", 'a' + i);
                	}
         	   }
                printf("\n");
        	}
    

    printf("Guess a letter (# to end game): ");
    scanf(" %c", &currentGuess);
    printf("\n");

    if (currentGuess == '#') {
        printf("Terminating game...\n");
        for (int i = 0; i < numWords; ++i) {
            free(wordList[i]);
        }
        free(wordList);
        return 0;
    }

    if (currentGuess < 'a' || currentGuess > 'z') {
        //printf("\nNumber of guesses remaining: %d\n", numGuesses);
        printf("Invalid letter...\n");
        //printf("\nNumber of guesses remaining: %d\n", numGuesses);
        //break;
    }

    if (guessedLetters[currentGuess - 'a']) {
        printf("Letter previously guessed...\n");
                    printf("Oops, there are no %c's. You used a guess.\n", currentGuess);

        continue;
    }
} while (currentGuess < 'a' || currentGuess > 'z' || guessedLetters[currentGuess - 'a']);
        guessedLetters[currentGuess - 'a'] = true;
 
        int numPatterns = 0;
        int maxPatterns = 4;
        Pattern *patterns = (Pattern *)malloc(maxPatterns * sizeof(Pattern));
 
        for (int i = 0; i < numWords; i++)
    	{
            strcpy(currentPattern, finalWordPattern); // Start with the current pattern
            for (int j = 0; j < wordSize; j++)
        	{
                if (wordList[i][j] == currentGuess)
            	{
                    currentPattern[j] = currentGuess;
            	}
        	}
	        currentPattern[wordSize] = '\0';
 
            int foundPatternIndex = -1;
            for (int j = 0; j < numPatterns; j++)
        	{
                if (strcmp(patterns[j].pat, currentPattern) == 0)
            	{
                    foundPatternIndex = j;
                    break;
            	}
        	}
 
            if (foundPatternIndex != -1)
        	{
                patterns[foundPatternIndex].count++;
        	}
            else
        	{
                patterns[numPatterns].pat = (char *)malloc(sizeof(char) * (wordSize + 1));
                strcpy(patterns[numPatterns].pat, currentPattern);
                patterns[numPatterns].count = 1;
                patterns[numPatterns].changes = strNumMods(finalWordPattern, currentPattern);
                numPatterns++;
                if (numPatterns == maxPatterns)
            	{
                    maxPatterns *= 2;
                    Pattern *tempPattern = (Pattern *)malloc(sizeof(Pattern) * maxPatterns);
                    for (int i = 0; i < numPatterns; ++i)
                	{
                        tempPattern[i] = patterns[i];
                	}
                    free(patterns);
                    patterns = tempPattern;
            	}
        	}
    	}
        if (patternListMode)
    	{
            printf("All patterns for letter %c: \n", currentGuess);
            printPatternList(patterns, numPatterns);
    	}
        int commonPatternIndex = 0;
        int highestCount = 0;
        for (int i = 1; i < numPatterns; i++)
    	{
            if (patterns[i].count > patterns[commonPatternIndex].count)
        	{
                commonPatternIndex = i;
                highestCount = patterns[commonPatternIndex].count;
        	}
            else if (patterns[i].count == patterns[commonPatternIndex].count)
        	{
                if (patterns[i].changes < patterns[commonPatternIndex].changes)
            	{
                   commonPatternIndex = i;
            	}
                else if (patterns[i].changes == patterns[commonPatternIndex].changes)
            	{
                    int k;
                    for (k = 0; k < wordSize; ++k)
        	        {
                        if (patterns[i].pat[k] != patterns[commonPatternIndex].pat[k])
                    	{
                            break;
                    	}
                	}
 

                    if (k < wordSize && patterns[i].pat[k] != finalWordPattern[k] && patterns[commonPatternIndex].pat[k] == finalWordPattern[k])
                	{
                        commonPatternIndex = i;
                	}
            	}
        	}
    	}
        char *initalWord = (char *)malloc(sizeof(char) * (wordSize + 1));
        strcpy(initalWord, finalWordPattern);
        strcpy(finalWordPattern, patterns[commonPatternIndex].pat);
 
        int tempWordCount = 0;
        char **tempWordList = (char **)malloc(numWords * sizeof(char *));
 
        for (int i = 0; i < numWords; i++)
	    {
            strcpy(currentPattern, initalWord); 
            for (int j = 0; j < wordSize; j++)
        	{
                if (wordList[i][j] == currentGuess)
            	{
                    currentPattern[j] = currentGuess;
            	}
        	}
            if (strcmp(finalWordPattern, currentPattern) == 0)
        	{
                tempWordList[tempWordCount] = wordList[i];
                tempWordCount++;
        	}
  	      else
        	{
                free(wordList[i]);
        	}
    	}
        // printf("The word pattern is: %s\n\n", patterns[commonPatternIndex].pat);
 
        numWords = tempWordCount;
        free(wordList);
        wordList = tempWordList;
 
        if (strcmp(finalWordPattern, initalWord) == 0)
    	{
            numGuesses--;
            printf("Oops, there are no %c's. You used a guess.\n", currentGuess);
                    
            //printf("Letter previously guessed...\n");

    	}
        else
    	{
            printf("Good guess! The word has at least one %c.\n", currentGuess);
    	}
        if (statsMode)
    	{
            printf("Number of possible words remaining: %d\n", numWords);
    	}
        free(initalWord);
        if (wordListMode)
   	 {
            printf("Possible words are now:\n");
            for (int i = 0; i < numWords; ++i)
        	{
                printf("  %s\n", wordList[i]);
        	}
    	}
        printf("The word pattern is: %s\n", finalWordPattern);
        if (!strchr(finalWordPattern, '-'))
    	{
            solved = true;
    	}
 
        for (int i = 0; i < numPatterns; i++)
    	{
            free(patterns[i].pat);
    	}
        free(patterns);
	}
    if (solved)
	{
        printf("\nYou solved the word!\n");
	}
    else
	{
        printf("\nYou ran out of guesses and did not solve the word.\n");
	}
 
    printf("The word is: %s\nGame over.", wordList[0]);
    for (int i = 0; i < numWords; ++i)
	{
        free(wordList[i]);
	}
    free(wordList);
 
    return 0;
}
