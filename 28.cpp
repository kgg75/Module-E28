#include <future>
#include <chrono>
#include <iostream>
#include <random>


using namespace std;


const int arr_size =    30'000'000;
const int threads =     400;

bool make_thread = true;


void merge(int arr[], uint64_t l, uint64_t m, uint64_t r) {
    uint64_t nl = m - l + 1;
    uint64_t nr = r - m;

    // создаем временные массивы
    uint32_t* left = new uint32_t[nl];
    uint32_t* right = new uint32_t[nr];

    // копируем данные во временные массивы
    for (uint64_t i = 0; i < nl; i++)
        left[i] = arr[l + i];

    for (uint64_t j = 0; j < nr; j++)
        right[j] = arr[m + 1 + j];

    uint64_t i = 0, j = 0;
    uint64_t k = l;  // начало левой части

    while (i < nl && j < nr) {  // записываем минимальные элементы обратно во входной массив
        if (left[i] <= right[j])
            arr[k] = left[i++];
        else
            arr[k] = right[j++];
        k++;
    }

    while (i < nl)    // записываем оставшиеся элементы левой части
        arr[k++] = left[i++];

    while (j < nr)    // записываем оставшиеся элементы правой части
        arr[k++] = right[j++];

    delete[] left;
    delete[] right;
}


void mergeSort(int arr[], uint64_t l, uint64_t r) {
    if (l >= r)
        return;     // выход из рекурсии

    uint64_t m = (l + r) / 2;   // середина

    if (make_thread && (m - l > arr_size / threads)) {  // если элементов в левой части больше чем 10'000 вызываем асинхронно рекурсию для правой части
        future <void> f = async(launch::async, 
            [&]()
            {
                mergeSort(arr, l, m);
            }
        );
        mergeSort(arr, m + 1, r);
    }
    else {  // запускаем обе части синхронно
        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);
    }

    merge(arr, l, m, r);
}


int main() {
    system("chcp 1251");
    srand(0);

    uint64_t ms_start, ms_end;

    int* array = new int[arr_size];

    for (auto i = 0; i < arr_size; i++) {   // заполнение массива случайными числами
        array[i] = rand() % 500'000;
    }

    cout << "Многопоточный запуск: ";
    ms_start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    mergeSort(array, 0, arr_size - 1);
    ms_end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    cout << (ms_end - ms_start)/1'000.0 << " seconds\n";

    for (auto i = 0; i < arr_size - 1; i++) {   // проверка сортированности
        if (array[i] > array[i + 1]) {
            cout << "Unsorted" << endl;
            break;
        }
    }

    for (auto i = 0; i < arr_size; i++) {   // заполнение массива случайными числами
        array[i] = rand() % 500'000;
    }
 
    make_thread = false;

    cout << "Однопоточный запуск: ";
    ms_start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    mergeSort(array, 0, arr_size - 1);
    ms_end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    cout << (ms_end - ms_start) / 1'000.0 << " seconds\n";

    delete[] array;

    return 0;
}
