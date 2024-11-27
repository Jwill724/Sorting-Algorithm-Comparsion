#include <stdio.h>
#include <stdlib.h>
#include <time.h>

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
    unsigned long leftArr[n1], rightArr[n2];

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
}

int main(int argc, char **argv) {
    srand(time(NULL));

    unsigned int arraySize = 128;
    // heap data structure
    heap_t *h = (heap_t*)malloc(sizeof(heap_t));
    h->length = arraySize;

    // + 1 for 1 based indexing
    h->arr = (unsigned long*)malloc((arraySize + 1) * sizeof(unsigned long));
    unsigned long *f = (unsigned long*)malloc((arraySize + 1) * sizeof(unsigned long));
    unsigned long *g = (unsigned long*)malloc((arraySize + 1) * sizeof(unsigned long));

    for (unsigned int i = 1; i <= arraySize; i++) {
        unsigned long tmp = rand() % INT16_MAX;
        f[i] = tmp;
        g[i] = tmp;
        h->arr[i] = tmp;
    }

    printf("\narray h before heapsort\n");
    for (unsigned int i = 1; i <= arraySize; i++) {
        printf("%lu ", h->arr[i]);
    }

    printf("\narray g before insertionsort\n");
    for (unsigned int i = 1; i <= arraySize; i++) {
        printf("%lu ", g[i]);
    }

    printf("\narray f before mergesort\n");
    for (unsigned int i = 1; i <= arraySize; i++) {
        printf("%lu ", f[i]);
    }

    HeapSort(h);
    InsertionSort(g, arraySize);
    MergeSort(f, 1, arraySize);

    printf("\nHeapSort...\n");
    for (unsigned int i = 1; i <= arraySize; i++) {
        printf("%lu ", h->arr[i]);
    }

    printf("\nInsertionSort...\n");
    for (unsigned int i = 1; i <= arraySize; i++) {
        printf("%lu ", g[i]);
    }

    printf("\nMergeSort...\n");
    for (unsigned int i = 1; i <= arraySize; i++) {
        printf("%lu ", f[i]);
    }

    free(h->arr);
    free(f);
    free(g);
    free(h);

    return 0;
}