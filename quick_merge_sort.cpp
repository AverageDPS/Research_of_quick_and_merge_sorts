#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <chrono>
#include <utility>
#include <algorithm>
#include <random>
#include <ctime>

using namespace std;

chrono::system_clock::duration elapsed =
    chrono::system_clock::now().time_since_epoch();
chrono::milliseconds ms =
    chrono::duration_cast<chrono::milliseconds>(elapsed);
mt19937::result_type seed =
    static_cast<mt19937::result_type>(ms.count());

mt19937 gen(seed);
uniform_int_distribution<int> distrib(1, 1000000);

void generate_random_data(vector<int>& arr){
    int n = arr.size();
    for(int i = 0; i < n; i++){
        arr[i] = distrib(gen);
    }
}

//Рандомизированная быстрая сортировка
void Quicksort(vector<int>& A, int a, int b){
    if(a < b){
        uniform_int_distribution<int> i_distr(a, b);
        int i = i_distr(gen);
        int splitter = A[i];
        swap(A[i], A[b]);
        int left = a - 1, mid = a - 1;
        for(int right = a; right <= b; right++){
            if(A[right] < splitter){
                left++;
                mid++;
                swap(A[left], A[right]);
                if(left != mid){
                    swap(A[mid], A[right]);
                }
            }
            else if(A[right] == splitter){
                mid++;
                if(mid < right){
                    swap(A[mid], A[right]);
                }
            }
        }
        Quicksort(A, a, left);
        Quicksort(A, mid + 1, b);
    }
}
double QuickSort_measure(vector<int>& A) {
    double total_duration = 0.0;
    chrono::steady_clock::time_point start_time, end_time;
    int b = A.size() - 1;
    start_time = chrono::high_resolution_clock::now();
    Quicksort(A, 0, b);
    end_time = chrono::high_resolution_clock::now();
    total_duration += static_cast<double>(
        chrono::duration_cast<chrono::microseconds>(
            end_time - start_time).count());
    return total_duration;
}


//Слияние двух соседних интервалов
void merge(vector<int>& A, int a, int median, int b){
    vector<int> L(A.begin() + a, A.begin() + (median + 1));
    vector<int> R(A.begin() + (median + 1), A.begin() + (b + 1));

    int L_size = (median - a) + 1;
    int R_size = b - median;
    int cur = a;
    int i = 0, j = 0;
    while(i < L_size && j < R_size){
        if(L[i] < R[j]){
            A[cur] = L[i];
            i++;
        }
        else{
            A[cur] = R[j];
            j++;
        }
        cur++;
    }
    if(i == L_size){
        while(j < R_size){
            A[cur] = R[j];
            j++;
            cur++;
        }
    }
    else{
        while(i < L_size){
            A[cur] = L[i];
            i++;
            cur++;
        }
    }
}
//Сортировка слиянием:
void merge_sort(vector<int>& A, int a, int b){
    if(a < b){
        int median = a + ((b - a) >> 1);
        merge_sort(A, a, median);
        merge_sort(A, median + 1, b);
        merge(A, a, median, b);
    }
}
double MergeSort_measure(vector<int>& A) {
    double total_duration = 0.0;
    chrono::steady_clock::time_point start_time, end_time;
    int b = A.size() - 1;
    start_time = chrono::high_resolution_clock::now();
    merge_sort(A, 0, b);
    end_time = chrono::high_resolution_clock::now();
    total_duration += static_cast<double>(
        chrono::duration_cast<chrono::microseconds>(
            end_time - start_time).count());
    return total_duration;
}


bool vector_is_sorted(vector<int>& A) {
    int n = A.size();
    int nminus = n - 1;
    if (n < 2) return true;
    for (int i = 0; i < nminus; i++) {
        if (A[i] > A[i + 1]) {
            cout << "Element A[" << i << "] = " << A[i] << " and "
                << "A[" << i + 1 << "] = " << A[i + 1] << " are unsorted\n";
            return false;
        }
    }
    return true;
}


// Функция записи результатов в файл
void add_to_file(int n, const vector<double>& res, ofstream& file){
    file << n << ";";
    size_t res_size = res.size();
    for (size_t k = 0; k < res_size; k++){
        file << int(res[k]) << ";"; // Запись в микросекундах
    }
    file << "\n";
}


void build_tests()
{
    cout << seed << endl;
    int test_vectors_amount = 500;
    int test_runs = 10;
    int test_vector, test_run;
    vector<int> sizes = { 100, 200, 400, 800, 1600, 3200, 6400, 12800, 25600, 51200, 102400};

    ofstream file_sorts("quick_and_merge_sorts.csv");
    file_sorts << "size;quick_sort_1_percentile;quick_sort_median;quick_sort_99_percentile;"
        << "merge_sort_1_percentile;merge_sort_median;merge_sort_99_percentile\n";
    int test;
    for(int size : sizes){
        cout << "size = " << size << endl;
        vector<int> data(size);
        vector<double> quick_sort_times(test_vectors_amount, 0.0);
        vector<double> merge_sort_times(test_vectors_amount, 0.0);
        for(test_vector = 0; test_vector < test_vectors_amount; test_vector++) {
            generate_random_data(data);
            for (test_run = 0; test_run < test_runs; test_run++) {
                vector<int> quick_sort_vector = data;
                vector<int> merge_sort_vector = data;
                if (test_run % 2 == 0) {
                    quick_sort_times[test_vector] += QuickSort_measure(quick_sort_vector);
                    merge_sort_times[test_vector] += MergeSort_measure(merge_sort_vector);
                }
                else {
                    merge_sort_times[test_vector] += MergeSort_measure(merge_sort_vector);
                    quick_sort_times[test_vector] += QuickSort_measure(quick_sort_vector);
                }

                if (!vector_is_sorted(quick_sort_vector)) {
                    cout << "Quick sort couldn't sort vector:\n";
                    cout << "  Unsorted: {";
                    for (int elem : data) cout << elem << " ";
                    cout << "}\n  After sorting: {";
                    for (int elem : quick_sort_vector) cout << elem << " ";
                    cout << "}\n";
                    return;
                }
                if (!vector_is_sorted(merge_sort_vector)) {
                    cout << "Merge sort couldn't sort vector:\n";
                    cout << "  Unsorted: {";
                    for (int elem : data) cout << elem << " ";
                    cout << "}\n  After sorting: {";
                    for (int elem : merge_sort_vector) cout << elem << " ";
                    cout << "}\n";
                    return;
                }
            }
            quick_sort_times[test_vector] /= static_cast<double>(test_runs);
            merge_sort_times[test_vector] /= static_cast<double>(test_runs);
        }
        std::sort(quick_sort_times.begin(), quick_sort_times.end());
        std::sort(merge_sort_times.begin(), merge_sort_times.end());
        int ten_percentile = test_vectors_amount / 100;
        int median = test_vectors_amount >> 1;
        int ninty_percentile = (test_vectors_amount * 99) / 100;
        add_to_file(size, { quick_sort_times[ten_percentile], quick_sort_times[median],
            quick_sort_times[ninty_percentile], merge_sort_times[ten_percentile],
            merge_sort_times[median], merge_sort_times[ninty_percentile]},
                    file_sorts);
    }
    file_sorts.close();
}

int main(){
    build_tests();

    return 0;
}
