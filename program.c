/* Program to generate term-biased snippets for paragraphs of text.

   Skeleton program written by Alistair Moffat, ammoffat@unimelb.edu.au,
   August 2022, with the intention that it be modified by students
   to add functionality, as required by the assignment specification.

   Student Authorship Declaration:

   (1) I certify that except for the code provided in the initial skeleton
   file, the  program contained in this submission is completely my own
   individual work, except where explicitly noted by further comments that
   provide details otherwise.  I understand that work that has been developed
   by another student, or by me in collaboration with other students, or by
   non-students as a result of request, solicitation, or payment, may not be
   submitted for assessment in this subject.  I understand that submitting for
   assessment work developed by or in collaboration with other students or
   non-students constitutes Academic Misconduct, and may be penalized by mark
   deductions, or by other penalties determined via the University of
   Melbourne Academic Honesty Policy, as described at
   https://academicintegrity.unimelb.edu.au.

   (2) I also certify that I have not provided a copy of this work in either
   softcopy or hardcopy or any other form to any other student, and nor will I
   do so until after the marks are released. I understand that providing my
   work to other students, regardless of my intention or any undertakings made
   to me by that other student, is also Academic Misconduct.

   (3) I further understand that providing a copy of the assignment
   specification to any form of code authoring or assignment tutoring service,
   or drawing the attention of others to such services and code that may have
   been made available via such a service, may be regarded as Student General
   Misconduct (interfering with the teaching activities of the University
   and/or inciting others to commit Academic Misconduct).  I understand that
   an allegation of Student General Misconduct may arise regardless of whether
   or not I personally make use of such solutions or sought benefit from such
   actions.

   Signed by: [Itmam Khan Labib, 1319974]
   Dated:     [15/09/2022]

*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

    /* you may need to delete this next line on some platforms
       in order to get it to compile; if you end up doing that
       make sure you put it back before you submit to the
       Gradescope system, because it is required there */
#include <strings.h>

    /* maximum number of characters per word */
#define MAX_WORD_LEN 23
    /* maximum number of words per paragraph */
#define MAX_PARA_LEN 10000

    /* return code from get_word if end of paragraph found */
#define PARA_END 1
    /* return code from get_word if a word successfully found */
#define WORD_FND 2


    /* terminating punctuation that may follow a word */
#define TERM_PUNCT ".,;:!?"
    /* terminating punctuation that needs dots added */
#define NEEDS_DOTS ",;:"
    /* the string the separates paragraphs */
#define PARA_SEPARATOR "\n\n"
    /* insertion characters to indicate "bold" */
#define BBOLD "**"
    /* length of those characters when wrapped around a word */
#define BBOLD_LEN (2*strlen(BBOLD))
    /* insertion characters to indicate omitted text */
#define DDOTS "..."

    /* maximum words in any output snippet */
#define MAX_SNIPPET_LEN 30
    /* minimum words in any output snippet */
#define MIN_SNIPPET_LEN 20
    /* maximum length of output lines */
#define MAX_OUTPUT_LINE 72

    /* maximum terms allowed on command line */
#define MAX_TERMS 50

typedef char word_t[MAX_WORD_LEN+1];
int     get_word(word_t w, int limit, int newline);

/* If you wish to add further #defines, put them below this comment,
   then prototypes for the functions that you add

   The only thing you should alter above this line is to complete the
   Authorship Declaration 
*/

/****************************************************************/
int     stage2(word_t a[], int words[], int matches[], char** query, int argc);
int     get_char(word_t w);
void    print_paragraph(word_t b[], int word[], char** query, int argc);
double  get_score(word_t C[], int matches[], int word[], char** query, int argc, int start, int end, int flag);
void    print_snippet(word_t D[], int start, int end, int words[], char** query, int argc);

/* main program controls all the action
*/
// algorithms are fun
int
main(int argc, char *argv[]) {
    word_t all_words[MAX_PARA_LEN];
    int totwords[1], match[1];
    int para =0;
    int variable =0;
    int paragraph =0;
    int space =0;
    //Looping through all the functions.
    while (variable != 1){
        para =stage2(all_words, totwords,match, argv, argc);
        paragraph++;
        printf("\n");
        printf("======= Stage 1 [para %d; %d words]\n\n", paragraph, totwords[0]);
        printf("======= Stage 2 [para %d; %d words; %d matches]\n", paragraph, totwords[0], match[0]);
        print_paragraph(all_words, totwords, argv, argc);
        double score;
        double max_score = 0.0;
        int start_ind = 0;
        int end_ind = 0;
        // Attempt to find the maximum snippet score.
        for (int start=0; start<totwords[0]; start++){
            for(int end=start+MIN_SNIPPET_LEN-1; end<= start+MAX_SNIPPET_LEN; end++){
                score = get_score(all_words, match, totwords,argv, argc, start, end, space);
                /* Record the highest snippet score while recording
                   the index of start and end of the snipet */
                if (score>max_score){
                    max_score = score;
                    start_ind = start;
                    end_ind = end;
                }
            }
        }
        printf("\n");
        printf("======= Stage 3 [para %d; start %d; length %d; score %.2f]\n",paragraph, start_ind, end_ind+1, max_score);
        print_snippet(all_words, start_ind, end_ind, totwords, argv, argc);
        //If EOF is reached, terminate the loop.
        if (para == 1){
            variable =1;
            break;
        }
    }
    printf("\nta daa!\n");
    return 0;
}

/****************************************************************/


/* extract a single word out of the standard input, but not
   more than "limit" characters in total. One character of
   sensible trailing punctuation is retained.  
   argument array W must be limit+1 characters or bigger
*/
int
get_word(word_t W, int limit, int newline) {
    int c;
    /* first, skip over any non alphanumerics */
    while ((c=getchar())!=EOF && !isalnum(c)) {
        if (c == '\n'){
            newline++;
            if (newline == 2){
                newline = 0;
                return PARA_END;
            }
        }
    }
    if (c==EOF) {
        return EOF;
    }
    /* ok, first character of next word has been found */
    *W = c;
    W += 1;
    limit -= 1;
    while (limit>0 && (c=getchar())!=EOF && isalnum(c)) {
        /* another character to be stored */
        *W = c;
        W += 1;
        limit -= 1;
    }
    /* take a look at that next character, is it a sensible trailing
       punctuation? */
    if (strchr(TERM_PUNCT, c) && (limit>0)) {
        /* yes, it is */
        *W = c;
        W += 1;
        limit -= 1;
    }
    /* now close off the string */
    *W = '\0';
    return WORD_FND;
}

int stage2(word_t A[], int words[], int matches[], char** query, int argc) {
    word_t w;
    int nline =0;
    int ans;
    int word =0, word_match=0;
    // Looping through the get_word function until the end of the paragraph or EOF is found
    while ((ans = get_word(w, MAX_WORD_LEN, nline)) != PARA_END) {
        if (ans == PARA_END) {
            break;
        }
        if (ans == EOF){
            words[0] = word;
            matches[0] = word_match;
            return 1;
        }
        /* Copying the given word in a new array in order to make changes to the
           word if necessary */
        strcpy(A[word], w);
        word++;
        int index = 0;
        word_t str;
        strcpy(str, w);
        /* If the word has retained its punctuation at the end, then the
           punctuation is removed, so that it can be compared to the
           query term */
        for (; index < MAX_WORD_LEN; index++) {
            if (!str[index]) {
                if (str[index-1] == ';' || str[index-1] == ',' || str[index-1] == '.' || str[index-1] ==':' || str[index-1]=='!' || str[index-1] =='?') {
                    str[index-1] = '\0';
                }
                break;
            }
            str[index] = tolower(str[index]);
        }
        /* A particular word from the paragraph is compared to
           the query term in a case insensitive manner */
        for (int i=0; i < argc; i++) {
            query[i][index] = tolower(query[i][index]);
            if (!strcmp(str, query[i])) {
                word_match++;
                continue;
            }
        }
    }
    /*The total words and the number of matching words in the paragraph
      are recorded*/
    words[0] = word;
    matches[0] = word_match;
    return 0;
}

void print_paragraph(word_t B[], int word[], char** query, int argc){
    int totchars =0, chars =0;
    word_t new_word;
    int total_words = word[0];
    // Loop through the array of words
    for (int i=0; i<total_words;i++){
        strcpy(new_word, B[i]);
        chars = strlen(new_word);
        totchars += chars;
        /* If the number of characters in a line exceeds 72, then place
           the word in a new line */
        if (totchars>72 ){
            printf("\n");
            totchars =strlen(new_word);
        }
        totchars++;
        int found = 0;
        int index = 0;
        word_t str;
        strcpy(str, new_word);
        /* Print the paragraph while highlighting the query term in an
           case insensitive manner, while considering the punctuations
           with the words */
        for (; index < MAX_WORD_LEN; index++) {
            if (!str[index]) {
                if (str[index-1] == ';' || str[index-1] == ',' || str[index-1] == '.' || str[index-1] ==':' || str[index-1]=='!' || str[index-1] =='?') {
                    str[index-1] = '\0';
                }
                break;
            }
            str[index] = tolower(str[index]);
        }
        for (int k=0; k < argc; k++) {
            query[k][index] = tolower(query[k][index]);
            if (!strcmp(str, query[k])) {
                if (new_word[index-1] == ';' || new_word[index-1] == ',' || new_word[index-1] == '.' || new_word[index-1] ==':' || new_word[index-1]=='!' || new_word[index-1] =='?') {
                    printf("**%.*s**", index-1, new_word);
                    printf("%c ", new_word[index-1]);
                }
                else printf("**%s** ", new_word);
                totchars += 4;
                found = 1;
                continue;
            }
        }
        if (found) continue;
        printf("%s ", new_word);
    }
    printf("\n");
}

double
get_score(word_t C[], int matches[], int word[], char** query, int argc, int start, int end, int flag){
    int found[]={0};
    double score = 0.0;
    double s = (double) start;
    double e = (double) end;
    if (end >= word[0]){
        end = word[0]-1;
    }
    /* Looping through the particular paragraphs word by word
       and calculating the snippet score */
    int index;
    for (int j=start; j<=end; j++){
        for (int i=1; i<argc; i++){
            word_t str;
            strcpy(str, C[j]);
            index =strlen(str);
            if (str[index-1] == ';' || str[index-1] == ',' || str[index-1] == '.' || str[index-1] ==':' || str[index-1]=='!' || str[index-1] =='?') {
                str[index-1] = '\0';
            }
            for (int l=0; l < MAX_WORD_LEN; l++) {
                str[l] = tolower(str[l]);
                query[i][l] = tolower(query[i][l]);
            }
            if (!strcmp(query[i], str)){
                flag++;
                found[i-1] = flag;
                // If a different query term appears in the snippet, add l/2.
                if (found[i-1]==1){
                    double len = (double) strlen(str);
                    score += len/2;
                }
                // If the same query term appeared, add 1.0
                else if (found[i-1]>1){
                    score += 1;
                }
            }
        }
    }
    /* Checking whether the word comes with a punctuation,
       then incrementing the snippet score by the given 
       rules */
    index=0;
    if (start != 0){
        index = strlen(C[start-1]);
    }
    if (start==0 || (start>0 && !isalnum(C[start-1][index-1]))){
        score += 0.6;
        
    }
    int nindex = strlen(C[end]);
    if (C[end][nindex-1] == ';' || C[end][nindex-1] == ',' || C[end][nindex-1] == '.' || C[end][nindex-1] ==':' || C[end][nindex-1]=='!' || C[end][nindex-1] =='?') {
        score += 0.3;
    }
    // Subtracting 0.1 for every word exceeding the MIN_SNIPPET_LEN
    if (end-start>19){
        score -= 0.1*(e-s-19);
    }
    score += (15/(s+10));
    return score;
}

void  print_snippet(word_t D[], int start, int end, int words[], char** query, int argc){
    int totchars =0, chars =0;
    word_t new_word;
    int total_words = words[0];
    int j=-1;
    // Printing out the snippet
    for (int i=start; i<=end;i++){
        strcpy(new_word, D[i]);
        chars = strlen(new_word);
        totchars += chars;
        if (totchars>72 ){
            printf("\n");
            totchars =strlen(new_word);
        }
        totchars++;
        int found = 0;
        int index = 0;
        word_t str;
        strcpy(str, new_word);
        for (; index < MAX_WORD_LEN; index++) {
            if (!str[index]) {
                if (str[index-1] == ';' || str[index-1] == ',' || str[index-1] == '.' || str[index-1] ==':' || str[index-1]=='!' || str[index-1] =='?') {
                    str[index-1] = '\0';
                }
                break;
            }
            str[index] = tolower(str[index]);
            
        }
        for (int k=0; k < argc; k++) {
            query[k][index] = tolower(query[k][index]);
            if (!strcmp(str, query[k])) {
                if (new_word[index-1] == ';' || new_word[index-1] == ',' || new_word[index-1] == '.' || new_word[index-1] ==':' || new_word[index-1]=='!' || new_word[index-1] =='?') {
                    printf("**%.*s**", index-1, new_word);
                    printf("%c", new_word[index-1]);
                    
                }
                else printf("**%s**", new_word);
                totchars += 4;
                found = 1;
                j++;
                if (end != j )
                    printf(" ");
                continue;
            }
        }
        if (found) continue;
        printf("%s", new_word);
        j++;
        if (end != j )
            printf(" ");
    }
    if (end+1 < total_words){
        printf(DDOTS);
    }
    printf("\n");
}
