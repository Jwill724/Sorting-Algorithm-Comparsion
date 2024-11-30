#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <math.h>

// 2^n macro
#define POW2(n) (1 << (n))

// precise ms tracker using the current machine real time
// time.h wasn't getting values when sort was near instant
double GetElapsedTime(struct timeval start, struct timeval end) {
    double elapsedTime = (end.tv_sec - start.tv_sec) * 1000.0;    // sec to ms
    elapsedTime += (end.tv_usec - start.tv_usec) / 1000.0;        // us to ms
    return elapsedTime;
}

typedef struct Heap {
    unsigned long *arr;
    unsigned int heapSize, length;
} heap_t;

void MaxHeapify(heap_t *h, unsigned int i);
void BuildMaxHeap(heap_t *h);
void HeapSort(heap_t *h);
void InsertionSort(unsigned long *arr, unsigned int length);
void MergeSort(unsigned long *a, unsigned int left, unsigned int right);
void Merge(unsigned long *a, unsigned long left, unsigned long mid, unsigned long right);

void Swap(unsigned long *a, unsigned long *b) {
    unsigned long tmp = *a;
    *a = *b;
    *b = tmp;
}

void MaxHeapify(heap_t *h, unsigned int i) {
    unsigned int largest = i;
    unsigned int left = 2 * i;       // left and right setup for 1 based indexing
    unsigned int right = 2 * i + 1;

    if (left <= h->heapSize && h->arr[left] > h->arr[i]) {
        largest = left;
    }

    if (right <= h->heapSize && h->arr[right] > h->arr[largest]) {
        largest = right;
    }
    if (largest != i) {
        Swap(&h->arr[i], &h->arr[largest]);
        MaxHeapify(h, largest);
    }
}

void BuildMaxHeap(heap_t *h) {
    h->heapSize = h->length;

    for (int i = (h->length / 2); i >= 1; i--) {
        MaxHeapify(h, i);
    }
}

void HeapSort(heap_t *h) {
    BuildMaxHeap(h);

    for (int i = h->length; i >= 2; i--) {
        Swap(&h->arr[1], &h->arr[i]);
        h->heapSize--;
        MaxHeapify(h, 1);    // start at root
    }
}

void InsertionSort(unsigned long *arr, unsigned int length) {
    for (int j = 1; j < length; j++) {
        unsigned long key = arr[j];
        int i = j - 1;

        while (i >= 0 && arr[i] > key) {
            arr[i + 1] = arr[i];
            i--;
        }
        arr[i + 1] = key;
    }
}

void MergeSort(unsigned long *a, unsigned int left, unsigned int right) {
    if (left < right) {
        unsigned long mid = left + (right - left) / 2;
        MergeSort(a, left, mid);
        MergeSort(a, mid + 1, right);
        Merge(a, left, mid, right);
    }
}

void Merge(unsigned long *a, unsigned long left, unsigned long mid, unsigned long right) {
    unsigned long n1 = mid - left + 1;
    unsigned long n2 = right - mid;
    // on larger data sets a stack overflow occurs, so this allocates the appropriate size
    unsigned long *leftArr = (unsigned long *)malloc(n1 * sizeof(unsigned long));
    unsigned long *rightArr = (unsigned long *)malloc(n2 * sizeof(unsigned long));

	for (unsigned long i = 0; i < n1; i++)
		leftArr[i] = a[left + i];
    for (unsigned long i = 0; i < n2; i++)
        rightArr[i] = a[mid + 1 + i];

    unsigned long i = 0, j = 0;
    unsigned long k = left;

    while(i < n1 && j < n2) {
        if (leftArr[i] <= rightArr[j]) {
            a[k] = leftArr[i];
            i++;
        }
        else {
            a[k] = rightArr[j];
            j++;
        }
        k++;
    }
    while (i < n1) {
        a[k] = leftArr[i];
        i++;
        k++;
    }
    while (j < n2) {
        a[k] = rightArr[j];
        j++;
        k++;
    }
    free(leftArr);
    free(rightArr);
}

int main(int argc, char **argv) {

    FILE *csvFile = fopen("sorting_results.csv", "w");
    if (csvFile == NULL) {
        printf("Error: Could not open file.\n");
        return -1;
    }
    fprintf(csvFile, "Array_Size,log2(Array_Size),HeapSort_Time_ms,InsertionSort_Time_ms,MergeSort_Time_ms\n");

    // Testing array sizes from 2^7 to 2^22
    unsigned int testCount = 1;
    for (unsigned int power = 7; power <= 22; power++) {
        unsigned long int arraySize = POW2(power);
        double heapTime = 0.0, insertionTime = 0.0, mergeTime = 0.0;
        double logSize = log2(arraySize);

        // heap data structure
        heap_t *h = (heap_t*)malloc(sizeof(heap_t));
        h->length = arraySize;

        // + 1 for 1 based indexing
        h->arr = (unsigned long*)malloc((arraySize + 1) * sizeof(unsigned long));
        unsigned long *f = (unsigned long*)malloc((arraySize + 1) * sizeof(unsigned long));
        unsigned long *g = (unsigned long*)malloc((arraySize + 1) * sizeof(unsigned long));

        // fill the arrays with same values
        for (unsigned int i = 1; i <= arraySize; i++) {
            unsigned long tmp = rand() % INT16_MAX;
            f[i] = tmp;
            g[i] = tmp;
            h->arr[i] = tmp;
        }

        struct timeval start, end;
        printf("Test %d: Array size = 2^%d (%lu elements)\n", testCount, power, arraySize);

        // Heap sort
        printf("\tRunning HeapSort...\n");
        gettimeofday(&start, NULL);
        HeapSort(h);
        gettimeofday(&end, NULL);
        heapTime = GetElapsedTime(start, end);

        // Insertion sort
        // 19 and onward it takes multiple mins to possible hours
        if (power >= 19) {
            printf("\tSkipping InsertionSort for array size 2^%d: Too inefficient to process.\n", power);
            insertionTime = -1;
        }
        else {
            // reset array to original unsorted state, same for mergesort
            memcpy(g, f, (arraySize + 1) * sizeof(unsigned long));
            printf("\tRunning InsertionSort...\n");
            gettimeofday(&start, NULL);
            InsertionSort(g, arraySize);
            gettimeofday(&end, NULL);
            insertionTime = GetElapsedTime(start, end);
        }
        // Merge sort
        memcpy(f, g, (arraySize + 1) * sizeof(unsigned long));
        printf("\tRunning MergeSort...\n");
        gettimeofday(&start, NULL);
        MergeSort(f, 1, arraySize);
        gettimeofday(&end, NULL);
        mergeTime = GetElapsedTime(start, end);

        // #N/A won't show on scatter plot
        if (insertionTime == -1) {
            fprintf(csvFile, "%lu,%.0f,%.5f,#N/A,%.5f\n",
            arraySize, logSize, heapTime, mergeTime);
        }
        else {
            fprintf(csvFile, "%lu,%.0f,%.5f,%.5f,%.5f\n",
            arraySize, logSize, heapTime, insertionTime, mergeTime);
        }

        testCount++;

        free(h->arr);
        free(f);
        free(g);
        free(h);  
    }

    printf("Writing results completed. Closing file...\n");
    fclose(csvFile);

    return 0;
}