#include <iostream>
#include <utility>
#include <random>
#include <ctime>

template <typename T>
class NDArray {
private:
    T * array;
    int slice_start,
        slice_stop;

    /*
    // здесь будут храниться указатели на ряды для получения элемента по индексу
    T * temp_row = new T[this->shape.second];

    void change_temp_row(T* new_row) {
        for (int i = 0; i < this->shape.second; i++) {
            this->temp_row[i] = new_row[i];
        }
    }
    */

    T * get_row_start(int index) {
        return &(array[this->shape.second * index]); 
    }

    NDArray<T> operation(NDArray<T>& other, char oper) {
        if (this->shape != other.shape) {
            std::cout << "Matrixes should have same shape!\n";
            std::cout << this->shape.first << ' ' << this->shape.second << '\n';
            std::cout << other.shape.first << ' ' << other.shape.second << '\n';
            return *this;
        }
        NDArray result(this->shape);
        switch (oper) {
            case '+':
                for (int i = 0; i < this->size; i++) {
                    result.get_by_true_index(i) = this->get_by_true_index(i) + other.get_by_true_index(i);
                }
            break;
            case '-':
                for (int i = 0; i < this->size; i++) {
                    result.get_by_true_index(i) = this->get_by_true_index(i) - other.get_by_true_index(i);
                }
            break;
            case '*':
                for (int i = 0; i < this->size; i++) {
                    result.get_by_true_index(i) = this->get_by_true_index(i) * other.get_by_true_index(i);
                }
            break;
            case '/':
                for (int i = 0; i < this->size; i++) {
                    result.get_by_true_index(i) = this->get_by_true_index(i) / other.get_by_true_index(i);
                }
            break;
        }

        return result;
    }

    void fill(T fill_num) {
        for (int i = 0; i < this->size; i++) {
            this->array[i] = fill_num;
        }
    }

    int flatten_indices(int x, int y) {
        return shape.second * y + x;
    }

    std::pair<int, int> bloat_index(int index) {
        return std::make_pair(index / (this->shape.second + 1), index % (this->shape.second + 1));
    }

    NDArray<T> reduce_axises(int operation, char axis) {
        int first_dim, 
            second_dim,
            i = 0,
            j = 0;
        int* x_dim;
        int* y_dim;
        if (axis == 'y') {
            first_dim = shape.second;
            second_dim = shape.first;
            x_dim = &j;
            y_dim = &i;
        } 
        else if (axis == 'x') {
            first_dim = shape.first;
            second_dim = shape.second;
            x_dim = &i;
            y_dim = &j;
        }
        else {
            std::cout << "Incorrect axis\n";
            return (*this);
        }
        std::pair<int, int> pair = std::make_pair(1, first_dim);
        NDArray<float> result(pair);
        switch (operation) {
            case 1:
            for (i = 0; i < first_dim; i++) {
                T value = 0;
                for (j = 0; j < second_dim; j++) {
                    value += array[flatten_indices(*y_dim, *x_dim)];
                }
                result.get_by_true_index(i) = value;
            }
            break;
            case 2:
            for (i = 0; i < first_dim; i++) {
                T value = array[i * second_dim];
                for (j = 0; j < second_dim; j++) {
                    if (value > array[flatten_indices(*y_dim, *x_dim)]) {
                        value = array[flatten_indices(*y_dim, *x_dim)];
                    }
                }
                result.get_by_true_index(i) = value;
            }
            break;
            case 3:
            for (i = 0; i < first_dim; i++) {
                T value = array[i * first_dim];
                for (j = 0; j < second_dim; j++) {
                    if (value < array[flatten_indices(*y_dim, *x_dim)]) {
                        value = array[flatten_indices(*y_dim, *x_dim)];
                    }
                }
                result.get_by_true_index(i) = value;
            }
            break;
            case 4:
            for (i = 0; i < first_dim; i++) {
                T value = 0;
                for (j = 0; j < second_dim; j++) {
                    value += array[flatten_indices(*y_dim, *x_dim)];
                }
                result.get_by_true_index(i) = value / shape.first;
            }
            break;
        }
        return result;
    }

    T reduce_axises(int operation) {
        T result; 
        switch (operation) {
            case 1:
            result = 0;
            for (int i = 0; i < size; i++) {
                result += array[i];
            }
            break;
            case 2:
            result = array[0];
            for (int i = 0; i < size; i++) {
                if (result > array[i]) {
                    result = array[i];
                }
            }
            break;
            case 3:
            result = array[0];
            for (int i = 0; i < size; i++) {
                if (result < array[i]) {
                    result = array[i];
                }
            }
            break;
            case 4:
            result = 0;
            for (int i = 0; i < size; i++) {
                result += array[i];
            }
            result /= size;
            break;
        }
        return result;
    }

public:
    int size;
    std::pair<int, int> shape;
    
    NDArray<T>(std::pair<int, int>& shape) {
        this->shape = shape;
        this->size = shape.first * shape.second;
        this->array = (T*) malloc(this->size * sizeof(T));
        this->slice_start = 0;
        this->slice_stop = this->size;
    }

    NDArray<T>(std::pair<int, int>& shape, char arg) {
        this->shape = shape;
        this->size = shape.first * shape.second;
        this->array = (T*) malloc(this->size * sizeof(T));
        this->slice_start = 0;
        this->slice_stop = this->size;
        switch (arg) {
            case '0':
                this->fill(0);
            break;
            case '1':
                this->fill(1);
            break;
            case 'r':
                srand(time(NULL));
                for (int i = 0; i < this->size; i++)
                {
                    array[i] = rand() % (50 + 1);
                }
            break;
        }
    }

    NDArray<T>(const NDArray<T>& other) {
        this->size = other.size;
        this->shape = other.shape;
        this->array = (T*) malloc(this->size * sizeof(T));
        this->slice_start = 0;
        this->slice_stop = this->size;
        for (int i = 0; i < this->size; i++) {
            this->array[i] = other.get_by_true_index(i);
        }
    }

    T* operator[](int index) {
        if (index > shape.first || index < 0) {
            std::cout << "Index out of range\n";
            return nullptr;
        } 
        return get_row_start(index);
    }

    T& get_by_true_index(int index) {
        return array[index];
    }

    T& get_by_true_index(int index) const {
        return array[index];
    }

    void display() {
        for (int i = 0; i < this->shape.first; i++) {
            for (int j = 0; j < this->shape.second; j++) {
                std::cout << (*this)[i][j] <<' ';
            }
            std::cout << '\n';    
        }
        std::cout << '\n';
    }

    NDArray<T> operator+(NDArray<T>& other) {
        return operation(other, '+');
    }

    NDArray<T> operator-(NDArray<T>& other) {
        return operation(other, '-');
    }

    NDArray<T> operator*(NDArray<T>& other) {
        return operation(other, '*');
    }

    NDArray<T> operator/(NDArray<T>& other) {
        return operation(other, '/');
    }

    NDArray<T>& slice(int y_start, int x_start, int y_stop, int x_stop) {
        int start_idx = flatten_indices(x_start, y_start);
        int stop_idx = flatten_indices(x_stop, y_stop);
        if ( (stop_idx - start_idx <= 0) || (start_idx < 0) || (stop_idx > size) )  {
            std::cout << "Incorrect indices\n";
            return *this;
        }
        slice_start = start_idx;
        slice_stop = stop_idx;
        return *this;
    }

    template <int N>
    NDArray<T>& operator=(T (&other_arr)[N]) {
        if (N == slice_stop - slice_start) {
            std::cout << "Incorrect array size\n";
        } 
        else { 
            for (int i = slice_start; i < slice_stop; i++) {
                array[i] = other_arr[i - slice_start];
            }
        }
        slice_start = 0;
        slice_stop = size;
        return *this;
    }

    NDArray<T> transpose(){
        NDArray<T> result(this->shape);
        for (int i = 0; i < this->shape.first; i++) {
            for (int j = 0; j < this->shape.second; j++) {
                result[i][j] = (*this)[j][i];
            }
        }
        return result;
    }

    NDArray<T> mathmul(NDArray<T>& other) {
        if (this->shape.second != other.shape.first) {
            std::cout << "Matrixes should have other shape\n";
            return *this;
        }
        std::pair<int, int> pair = std::make_pair(shape.first, other.shape.second);
        NDArray<T> result(shape);
        T value;
        for (int i = 0; i < shape.first; i++)
        {
            for (int j = 0; j < other.shape.second; j++)
            {
                value = 0;
                for (int k = 0; k < shape.second; k++)
                {
                    value += (*this)[i][k] * other[k][j];
                }
                result[i][j] = value;
            }
        }
        return result;
    }

    NDArray<T> sum(char axis) {
        return reduce_axises(1, axis);
    }
    
    NDArray<T> min(char axis) {
        return reduce_axises(2, axis);
    }
    
    NDArray<T> max(char axis) {
        return reduce_axises(3, axis);
    }

    NDArray<T> avg(char axis) {
        return reduce_axises(4, axis);
    }

    T sum() {
        return reduce_axises(1);
    }
    
    T min() {
        return reduce_axises(2);
    }
    
    T max() {
        return reduce_axises(3);
    }

    T avg() {
        return reduce_axises(4);
    }
    
};

int main() {
    std::pair<int, int> pair = std::make_pair(2, 4);
    pair = std::make_pair(3, 3);
    NDArray<float> mrx3(pair, '0');
    std::cout << "Заполнение нулями:\n";
    mrx3.display();

    std::cout << "Заполнение единицами:\n";
    NDArray<float> mrx4(pair, '1');
    mrx4.display();

    std::cout << "Заполнение случайными числами (х2):\n";
    NDArray<float> mrx1(pair, 'r');
    NDArray<float> mrx2(pair, 'r');
    mrx1.display();
    mrx2.display();
    
    std::cout << "Сумма:\n";
    (mrx1 + mrx2).display();

    std::cout << "Разность:\n";
    (mrx1 - mrx2).display();

    std::cout << "Посимвольное умножение:\n";
    (mrx1 * mrx2).display();

    std::cout << "Посимвольное деление:\n"; 
    (mrx1 / mrx2).display();   

    std::cout << "Матричное умножение:\n";
    mrx1.mathmul(mrx2).display(); 
    
    std::cout << "Сумма рядов/столбцов:\n";
    mrx1.sum('x').display();
    mrx1.sum('y').display();
    std::cout << mrx1.sum() << "\n\n";  

    std::cout << "Минимум рядов/столбцов/всего:\n";
    mrx1.min('x').display();
    mrx1.min('y').display(); 
    std::cout << mrx1.min() << "\n\n";

    std::cout << "Максимум рядов/столбцов/всего:\n";
    mrx1.max('x').display();
    mrx1.max('y').display(); 
    std::cout << mrx1.max() << "\n\n";

    std::cout << "Среднее рядов/столбцов/всего:\n";
    mrx1.avg('x').display();
    mrx1.avg('y').display(); 
    std::cout << mrx1.avg() << "\n\n";

    std::cout << "Присвоение по слайсу (от (1,2) до (2,2) не включительно значений 0, 1, 2):\n";
    float arr[3] = {0, 1, 2};
    mrx1.slice(1, 2, 2, 2) = arr;
    mrx1.display();
    return 0;
}
