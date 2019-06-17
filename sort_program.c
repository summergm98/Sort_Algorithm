#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct timeval tv1, tv2;
double sec, usec;

int tot_b = -1;
int tot_w = 0;

double get_time(struct timeval tv1){
	gettimeofday(&tv2, NULL);

	sec = difftime(tv2.tv_sec, tv1.tv_sec);
	usec = tv2.tv_usec - tv1.tv_usec;
	
	if(usec < 0){
		sec--;
		usec = usec + 1000000;
	}

	usec = usec + sec * 1000000;	
	
	return usec;
}

void gen(int *b, int *w, int num_items){
	int i = 0;
	srand(time(NULL));
	
	for(i = 0; i < num_items; i++){
		b[i] = (rand() % 300) + 1;
		w[i] = (rand() % 100) + 1;
	}

	return;
}

void sort_temp(int a[][2], int n){
	int i,j;
	int temp[2];
	
	for(i = 0; i < n; i++){
		temp[0] = a[i][0];
		temp[1] = a[i][1];
		j = i - 1;
		while(j >= 0 && a[j][1] < temp[1]){
			a[j + 1][0] = a[j][0];
			a[j + 1][1] = a[j][1];
			j = j - 1;
		}
	a[j + 1][0] = temp[0];
	a[j + 1][1] = temp[1];
	}
}

void sort_bw(int *b, int *w, int num_items){
	int t, t1, t2;
	int i, j = 0;
	int *temp = (int *) malloc (sizeof(int) * (num_items));

	for(i = 0; i < num_items; i++)
		temp[i] = (int)b[i] / (int)w[i];
	for(i = 0; i < num_items - 1; i++){
		for(j = i + 1; j < num_items; j++){
			if(temp[i] < temp[j]){
				t2 = temp[i];
				temp[i] = temp[j];
				temp[j] = t2;
				t = b[i];
				b[i] = b[j];
				b[j] = t;
				t1 = w[i];
				w[i] = w[j];
				w[j] = t1;
			}
		}
	}
}
			
long long _max(long long a, long long b){
	return (a > b)? a : b;
}

void knap_greedy(int num_items, int max_w, int *b, int *w, FILE *fp){
	
	gettimeofday(&tv1, NULL);

	int i = 0;
	int temp[num_items + 1][2];
	long long t_w = 0;
	int count = 0;
	long long t_b = 0;

	for(i = 0; i < num_items; i++){
		temp[i][0] = i;
		temp[i][1] = (int)(b[i] / w[i]);
	}
		
	sort_temp(temp, num_items);	

	for(count = 0; count < num_items; count ++){
		if(get_time(tv1) > 900000000){
			printf(">Greedy Result\n\tTotal benefit: ERROR_TIMEOUT\n");
			fprintf(fp, "TIMEOUT\t");
			return;
		}
		if(t_w + w[count] > max_w){
			t_b = t_b + ((temp[count][1]) * (max_w - t_w));
			break;
		}else{
			t_b = t_b + b[temp[count][0]];
			t_w = t_w + w[temp[count][0]];
		}
	}	

	usec = get_time(tv1);
		
	printf(">Greedy Result\n\tTotal benefit: %lld\n", t_b);
	printf("\tComputing time: %.fms\n\n", usec);	
	fprintf(fp, "%d\t%.fms/%lld\t", num_items,usec, t_b);

}


void knap_dp(int num_items, int max_w, int *b, int *w, FILE *fp){
	gettimeofday(&tv1, NULL);
	int i, j;
	long long **k;
	
	k = (long long **) malloc (sizeof(long long *) * (num_items + 1));
	
	for(i = 0; i < num_items + 1; i++)
		k[i] = (long long *) malloc (sizeof(long long) * (max_w + 1));
	
	for(i = 0; i <= num_items; i++){
		if(get_time(tv1) > 900000000){
			printf(">DP Result\n\tTotal benefit: ERROR_TIMEOUT\n");
			fprintf(fp, "TIMEOUT\t");
			return;
		}
		for(j = 0; j <= max_w; j++){
			if(i == 0 || j == 0)
				k[i][j] == 0;
			else if(w[i - 1] <= j){
				k[i][j] = _max(b[i - 1] + k[i - 1][j - w[i - 1]], k[i - 1][j]);
			}else{
				k[i][j] = k[i - 1][j];
			}
		}
	}
	
	gettimeofday(&tv2, NULL);
	
	usec = get_time(tv1);

	printf(">DP Result\n\tTotal benefit: %lld\n", k[num_items][max_w]);
	printf("\tComputing time: %.fms\n\n", usec);	
	fprintf(fp, "%.fms/%lld\t\t", usec, k[num_items][max_w]);
	
	for(i = 0; i < num_items + 1; ++i)
		free(k[i]);
	free(k);
}	

long long bound(int cb, int cw, int k, int num_items, int max_w, int *b, int *w){
	int i = 0;
	int tb = cb;
	int tw = cw;
	for(i = k; i <= num_items; i++){
		tw = tw + w[i];
		if(tw < max_w)
			tb = tb + b[i];
		else
			return (tb + (1 - (tw - max_w) / w[i]) * b[i]);
	}
	return tb;
}

void result_bb(int num_items, int *b, int *x, struct timeval tv1, FILE *fp){
	long long s = 0;
	int i = 0;
	double usec;
	for(i = 0; i < num_items; i++){
		if(x[i] == 1){
			s += b[i] * x[i];
		}
	}	
	usec = get_time(tv1);
	
	printf(">BB Result\n\tTotal benefit: %lld\n", s);
	printf("\tComputing time: %.fms\n\n", usec);
	fprintf(fp, "%.fms/%lld\n", usec,s);
}

void knap_bb(int k, long long cb, int cw, int *b, int *w, int num_items, int max_w, int *x ,int *y){
	int i, j;

	if(cw + w[k] <= max_w){
		y[k] = 1;
		if(k <= num_items){
			knap_bb(k + 1, cb + b[k], cw + w[k], b, w, num_items, max_w, x, y);
		}
		if(((cb + b[k]) > tot_b) && (k == num_items)){
			tot_b = cb + b[k];	
			tot_w = cw + w[k];
			for(j = 0; j <= k; j++)
				x[j] = y[j];
		}
	}
	if(bound(cb, cw, k, num_items, max_w, b, w) >= tot_b){
		y[k] = 0;
		if(k <= num_items)
			knap_bb(k + 1, cb ,cw, b ,w, num_items, max_w, x ,y);
		if((cb > tot_b) && (k == num_items)){
			tot_b = cb;
			tot_w = cw;
			for(j = 0; j <= k; j++)
				x[j] = y[j];
		}
	}
	
}
int main(){
	int num_items = 0;
	int max_w = 0;
	int i = 0;

	FILE *fp = fopen("output.txt", "w");
	fputs("size of\t\talgorithm\n", fp);
	fputs("items\tgreedy\t\tdp\t\t\tbb\n", fp);
	
	//# of items = 10
	num_items = 10;
	max_w = num_items * 40;	
	int b[num_items + 1], w[num_items + 1];
	gen(b, w, num_items);	
	
	printf("----------------------------------------\n");
	printf("For %d items...\n", num_items);
	knap_greedy(num_items, max_w, b, w, fp);
	knap_dp(num_items, max_w, b, w, fp);

	tot_b = -1;
	tot_w = 0;
	int *x = (int *) malloc (sizeof(int) * (num_items + 1));
	int *y = (int *) malloc (sizeof(int) * (num_items + 1));
	sort_bw(b, w, num_items);
	gettimeofday(&tv1, NULL);
	knap_bb(0, 0, 0,b, w, num_items, max_w, x, y);	
	result_bb(num_items, b, x, tv1, fp);
	free(x);
	free(y);

	//# of items = 100;	
	num_items = 100;
	max_w = num_items * 40;	
	int b1[num_items + 1], w1[num_items + 1];
	gen(b1, w1, num_items);	
	
	printf("----------------------------------------\n");
	printf("For %d items...\n", num_items);
	knap_greedy(num_items, max_w, b1, w1, fp);
	knap_dp(num_items, max_w, b1, w1, fp);
	
	tot_b = -1;
	tot_w = 0;
	int *x1 = (int *) malloc (sizeof(int) * (num_items + 1));
	int *y1 = (int *) malloc (sizeof(int) * (num_items + 1));
	sort_bw(b1, w1, num_items);
	gettimeofday(&tv1, NULL);
	knap_bb(0, 0, 0,b1, w1, num_items, max_w, x1, y1);	
	result_bb(num_items, b1, x1, tv1, fp);
	free(x1);
	free(y1);
	
	//# of items = 500;
	num_items = 500;
	max_w = num_items * 40;	
	int b2[num_items + 1], w2[num_items + 1];
	gen(b2, w2, num_items);	
	
	printf("----------------------------------------\n");
	printf("For %d items...\n", num_items);
	knap_greedy(num_items, max_w, b2, w2, fp);
	knap_dp(num_items, max_w, b2, w2, fp);

	tot_b = -1;
	tot_w = 0;
	int *x2 = (int *) malloc (sizeof(int) * (num_items + 1));
	int *y2 = (int *) malloc (sizeof(int) * (num_items + 1));
	sort_bw(b2, w2, num_items);
	gettimeofday(&tv1, NULL);
	knap_bb(0, 0, 0,b2, w2, num_items, max_w, x2, y2);	
	result_bb(num_items, b2, x2, tv1, fp);
	free(x2);
	free(y2);
	
	//# of items = 1000;
	num_items = 1000;
	max_w = num_items * 40;	
	int b3[num_items + 1], w3[num_items + 1];
	gen(b3, w3, num_items);	
	
	printf("----------------------------------------\n");
	printf("For %d items...\n", num_items);
	knap_greedy(num_items, max_w, b3, w3, fp);
	knap_dp(num_items, max_w, b3, w3, fp);

	tot_b = -1;
	tot_w = 0;
	int *x3 = (int *) malloc (sizeof(int) * (num_items + 1));
	int *y3 = (int *) malloc (sizeof(int) * (num_items + 1));
	sort_bw(b3, w3, num_items);
	gettimeofday(&tv1, NULL);
	knap_bb(0, 0, 0,b3, w3, num_items, max_w, x3, y3);	
	result_bb(num_items, b3, x3, tv1, fp);
	free(x3);
	free(y3);
	
	//# of items = 3000;
	num_items = 3000;
	max_w = num_items * 40;	
	int b4[num_items + 1], w4[num_items + 1];
	gen(b4, w4, num_items);	

	printf("----------------------------------------\n");
	printf("For %d items...\n", num_items);
	knap_greedy(num_items, max_w, b4, w4, fp);
	knap_dp(num_items, max_w, b4, w4, fp);

	tot_b = -1;
	tot_w = 0;
	int *x4 = (int *) malloc (sizeof(int) * (num_items + 1));
	int *y4 = (int *) malloc (sizeof(int) * (num_items + 1));
	sort_bw(b4, w4, num_items);
	gettimeofday(&tv1, NULL);
	knap_bb(0, 0, 0,b4, w4, num_items, max_w, x4, y4);	
	result_bb(num_items, b4, x4, tv1, fp);
	free(x4);
	free(y4);


	//# of items = 5000
	num_items = 5000;
	max_w = num_items * 40;
	int b5[num_items + 1], w5[num_items + 1];
	gen(b5, w5, num_items);
	
	printf("----------------------------------------\n");
	printf("For %d items...\n", num_items);
	knap_greedy(num_items, max_w, b5, w5, fp);
	knap_dp(num_items, max_w, b5, w5, fp);

	tot_b = -1;
	tot_w = 0;
	int *x5 = (int *) malloc (sizeof(int) * (num_items + 1));
	int *y5 = (int *) malloc (sizeof(int) * (num_items + 1));
	sort_bw(b5, w5, num_items);
	gettimeofday(&tv1, NULL);
	knap_bb(0, 0, 0,b5, w5, num_items, max_w, x5, y5);	
	result_bb(num_items, b5, x5, tv1, fp);
	free(x5);
	free(y5);


	//# of items = 7000
	num_items = 7000;
        max_w = num_items * 40;
        int b6[num_items + 1], w6[num_items + 1];
        gen(b6, w6, num_items);

	printf("----------------------------------------\n");
        printf("For %d items...\n", num_items);
        knap_greedy(num_items, max_w, b6, w6, fp);
        knap_dp(num_items, max_w, b6, w6, fp);
	
	tot_b = -1;
	tot_w = 0;
	int *x6 = (int *) malloc (sizeof(int) * (num_items + 1));
	int *y6 = (int *) malloc (sizeof(int) * (num_items + 1));
	sort_bw(b6, w6, num_items);
	gettimeofday(&tv1, NULL);
	knap_bb(0, 0, 0,b6, w6, num_items, max_w, x6, y6);	
	result_bb(num_items, b6, x6, tv1, fp);
	free(x6);
	free(y6);
	

	//# of items = 9000
	num_items = 9000;
	max_w = num_items * 40;
	int b7[num_items + 1], w7[num_items + 1];
	gen(b7, w7, num_items);
	
	printf("----------------------------------------\n");
	printf("For %d items...\n", num_items);
	knap_greedy(num_items, max_w, b7, w7, fp);
	knap_dp(num_items, max_w, b7, w7, fp);

	tot_b = -1;
	tot_w = 0;
	int *x7 = (int *) malloc (sizeof(int) * (num_items + 1));
	int *y7 = (int *) malloc (sizeof(int) * (num_items + 1));
	sort_bw(b7, w7, num_items);
	gettimeofday(&tv1, NULL);
	knap_bb(0, 0, 0,b7, w7, num_items, max_w, x7, y7);	
	result_bb(num_items, b7, x7, tv1, fp);
	free(x7);
	free(y7);

	//# of items = 10000
	num_items = 10000;
	max_w = num_items * 40;
	int b8[num_items + 1], w8[num_items + 1];
	gen(b8, w8, num_items);

	printf("----------------------------------------\n");
	printf("For %d items...\n", num_items);
	knap_greedy(num_items, max_w, b8, w8, fp);
	knap_dp(num_items, max_w, b8, w8, fp);

	tot_b = -1;
	tot_w = 0;
	int *x8 = (int *) malloc (sizeof(int) * (num_items + 1));
	int *y8 = (int *) malloc (sizeof(int) * (num_items + 1));
	sort_bw(b8, w8, num_items);
	gettimeofday(&tv1, NULL);
	knap_bb(0, 0, 0,b8, w8, num_items, max_w, x8, y8);	
	result_bb(num_items, b8, x8, tv1, fp);
	free(x8);
	free(y8);
	fclose(fp);
	return 0;
}

	




