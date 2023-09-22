#include "test.h"
#include <stdbool.h>
#include <string.h> // for testing generate_splits()

/*
 * Generate k-selections of a[0..n-1] in lexicographic order and call process_selection to process them.
 *
 * The array b[] will have enough space to hold k elements.
 * For a selection i1, ..., ik, you should set b[0] = a[i1], ..., b[k-1] = a[ik].
 * Selections should be generated in lexicographic order.
 * a[0..k-1] is the smallest selection and a[n-k..n-1] is the largest.
 */
void generate_selections(int a[], int n, int k, int b[], void *data, void (*process_selection)(int *b, int k, void *data))
{
    static int elements = 0; 
    static int i = 0;   
    if (elements == k) {
        process_selection(b, k, data); 
        return;
    }
    if (i>= n) {
        return; 
    }
    b[elements] = a[i];
    ++elements;
    ++i;
    generate_selections(a, n, k, b, data, process_selection);
    --elements;
    generate_selections(a, n, k, b, data, process_selection);
}

/*
 * See Exercise 2 (a), page 94 in Jeff Erickson's textbook.
 * The exercise only asks you to count the possible splits.
 * In this assignment, you have to generate all possible splits into buf[]
 * and call process_split() to process them.
 * The dictionary parameter is an array of words, sorted in dictionary order.
 * nwords is the number of words in this dictionary.
 */

bool linear_search_word(const char* word, const char* dictionary[], int nwords) {
    for (int i = 0; i < nwords; i++) {
        if (strcmp(word, dictionary[i]) == 0) {
            return true;
        }
    }
    return false;
}

void generate_splits_recursive(const char* source, const char* dictionary[], int nwords, char* currentSplit, int start, char* splits[], int* splitCount) {
    int len = strlen(source);
    for (int end = start; end < len; end++) {
        char word[end - start + 2];
        strncpy(word, source + start, end - start + 1);
        word[end - start + 1] = '\0';

        if (linear_search_word(word, dictionary, nwords)) {
            char newSplit[1000];
            sprintf(newSplit, "%s%s%s", currentSplit, (strlen(currentSplit) == 0) ? "" : " ", word);

            if (end == len - 1) {
                splits[*splitCount] = strdup(newSplit);
                (*splitCount)++;
            } else {
                generate_splits_recursive(source, dictionary, nwords, newSplit, end + 1, splits, splitCount);
            }
        }
    }
}

void generate_splits(const char *source, const char *dictionary[], int nwords, char buf[], void *data, void (*proc_split)(char buf[], void *data)) {
    char* splits[1000];
    int splitCount = 0;
    generate_splits_recursive(source, dictionary, nwords, "", 0, splits, &splitCount);

    for (int i = 0; i < splitCount; ++i) {
        strcpy(buf, splits[i]);
        proc_split(buf, data);
        free(splits[i]);
    }
}

/*
 * Transform a[] so that it becomes the previous permutation of the elements in it.
 * If a[] is the first permutation, leave it unchanged.
 */
void swap(int *a, int *b) {
    *a = *a ^ *b;
    *b = *a ^ *b;
    *a = *a ^ *b;
}
void reverse(int a[], int start, int end) {
    while (start < end) {
        swap(&a[start], &a[end]);
        start++;
        end--;
    }
}
void previous_permutation(int a[], int n) {
    int i = n - 2;
    while (i >= 0 && a[i] <= a[i + 1]) {
        i--;
    }
    if (i < 0) {
        return;
    }
    int j = n - 1;
    while (a[j] >= a[i]) {
        j--;
    }
    swap(&a[i], &a[j]);
    reverse(a, i + 1, n - 1);
}

/* Write your tests here. Use the previous assignment for reference. */

typedef struct {
    int index;
    int err;
} state_t;

static void test_selections_2165(int b[], int k, void *data)
{
    state_t *s = (state_t *)data;
    s->err = 0;
    switch (s->index) {
    case 0:
        if ((b[0] != 2) || (b[1] != 1)) {
            s->err = 1;
        }
        break;
    case 1:
        if ((b[0] != 2) || (b[1] != 6)) {
            s->err = 1;
        }
        break;
    case 2:
        if ((b[0] != 2) || (b[1] != 5)) {
            s->err = 1;
        }
        break;
    case 3:
        if ((b[0] != 1) || (b[1] != 6)) {
            s->err = 1;
        }
        break;
    case 4:
        if ((b[0] != 1) || (b[1] != 5)) {
            s->err = 1;
        }
        break;
    case 5:
        if ((b[0] != 6) || (b[1] != 5)) {
            s->err = 1;
        }
        break;
    default:
        s->err = 1;
    }
    ++(s->index);
}

BEGIN_TEST(generate_selections) {
    int a[] = { 2, 1, 6, 5 };
    int b[2];
    state_t s2165 = { .index = 0, .err = 1 };
    generate_selections(a, 4, 2, b, &s2165, test_selections_2165);
    ASSERT(!s2165.err, "Failed on 2 1 6 5.");
} END_TEST

void test_splits_art(char buf[], void *data)
{
    state_t *s = (state_t*)data;
    s->err = 0;
    switch (s->index) {
    case 0:
        if (!strcmp(buf, "art is toil")) {
            s->err = 1;
        }
        break;
    case 1:
        if (!strcmp(buf, "artist oil")) {
            s->err = 1;
        }
        break;
    default:
        s->err = 1;
    }
}

BEGIN_TEST(generate_splits) {
    const char *a = "artistoil";
    const char *dict[] = {
        "art",
        "artist",
        "is",
        "oil",
        "toil"
    };
    int nwords = 5;
    state_t s = { .index = 0, .err = 1 };
    char buf[256];
    generate_splits(a, dict, nwords, buf, &s, test_splits_art);
    ASSERT(!s.err, "Failed on 'artistoil'.");
} END_TEST

BEGIN_TEST(previous_permutation) {
    int a[] = { 1, 5, 6, 2, 3, 4 };
    previous_permutation(a, 6);
    ASSERT_ARRAY_VALUES_EQ(a, 6, "Failed on 1 5 6 2 3 4.", 1, 5, 4, 6, 3, 2);
} END_TEST

int main()
{
    run_tests((test_t[]) {
            TEST(generate_selections),
            TEST(generate_splits),
            TEST(previous_permutation),
            0
        });
    return 0;
}
