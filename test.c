int main() {
    int i = 0, j = 0, arr[2][3];
    while (i < 2) {
        j = 0;
        while (j < 3) {
            arr[i][j] = read();
            write(arr[i][j]);
            j++;
        }
        i++;
    }
    return 0;
}