void print_matrix(double *a, int l, int n, int m, int rank, int size, double *buff);
int mod(int a);
int max(int a, int b);
int enter_matrix(int n, int k, FILE *in, double *a, int rank, int size);
int LU(double *a, double *buff, int n, int rank, int size);
int obratnaya(double *a, double *e, int n);
double nevyazka(double *a, double *b, double *buff, int n, int rank, int size);
double f(int k, int n, int i, int j);
double scalar_product(double *a, double *buff, int n, int k, int raw1, int col2, int rank, int size);
int Gaus(int n, double *a, double *b, double *x, int my_rank, int p);
void trans(double *a, double *buff, double *x, int n, int rank, int p);
void enter_i_col(double *a, double *buff, int i, int n, int rank, int p);
void enter_i_row(double *a, double *buff, int i, int n, int rank, int p);
void copy_i_row(double *a, double *buff, int i, int n, int rank, int p);
void copy_i_col(double *a, double *buff, double *buff2, int i, int n, int rank, int p);
int inv(int n, double *b, double *a, double *x, double *buff, int my_rank, int p);
double scalar_product2(double *a, double *buff, int n, int k, int raw1, int col2, int rank, int size);
double scalar_product3(double *a, double *buff, int n, int k, int raw1, int col2, int rank, int size);
double scalar_product4(double *a, double *b, double *buff, int n, int raw1, int col2, int rank, int size);
