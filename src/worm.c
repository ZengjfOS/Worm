#include <stdio.h>
#include <error.h>
#include <stdlib.h>
#include <signal.h>
/**
 * 声明数组的长度
 */
#define arraySize   100
/**
 * 贪吃蛇尺寸宽度
 */
#define maxRows     20
#define maxCols     40
#define minRows     3
#define minCols     2
/**
 * 随机到这么多次数，还是无法找到下一个无冲突的点，就重新开始
 */
#define timesReset  20

/**
 * 设置初始时刻蠕虫运行方向
 */
int direction = 0;
/**
 * 当前蠕虫的长度
 */
int currentLength = 20;
/**
 * 用于保存蠕虫列坐标
 */
int x[arraySize] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
/**
 * 用于保存蠕虫行坐标
 */
int y[arraySize] = {10, 10, 10, 10, 10, 10, 10, 10, 10, 10};
/**
 * 绘制方框
 */
void drawBox(){
    int i;
    int j;
    for (i = (minRows - 1); i < (maxRows+1+1); i++){
        for (j = (minCols - 1); j < (maxCols+1+1); j++){
            if((i == (minRows - 1)) || (i == (maxRows+1)) || (j == (minCols - 1)) || (j == (maxCols+1))){
                printf("\033[%d;%dH*", i, j);
            }
        }
    }
}

/**
 * 获取一个随机数
 */
int randomNum(){
	FILE *randomFile = fopen("/dev/urandom", "r");
	if(randomFile == NULL){
		perror("/dev/urandom file can't open");
	}
	char ch;
	fread(&ch, 1, sizeof(char), randomFile);
	fclose(randomFile);
	srand(ch);
	return rand();
}
/**
 * 将数组中的数据向右移动一个，最后一个丢失，第一个和第二个相同
 */
int arrayMoveRight(int* array){
	int i;
	int num = array[ currentLength-1 ];
	for(i = (currentLength - 1); i > 0; i--){
		array[i] = array[ i-1 ];
	}
	return num;
}

/**
 * 清楚蠕虫头和尾
 */
void cleanArray(int* rows, int* cols){
	printf("\033[%d;%dH ", rows[ currentLength-1 ], cols[ currentLength-1 ]);
	printf("\033[%d;%dH ", rows[0], cols[0]);
	printf("\n");
}

/**
 * 输出出蠕虫的坐标
 */
void printfArray(int* rows, int* cols){
	int i;
	for(i = 1; i < currentLength; i++){
		printf("\033[%d;%dH*", rows[i], cols[i]);
	}
	printf("\033[%d;%dH@", rows[0], cols[0]);
	printf("\n");
}

/**
 * 判断是否有咬到蠕虫身体，或者越界
 */
int hasPosition(void){
	int i;
	for(i = 1; i < currentLength; i++){
		if(x[0] == x[i] && y[0] == y[i]){
			return 1;
		}
	}
	if((x[0] < minCols) || (y[0] < minRows) || (x[0] > maxCols) || (y[0] > maxRows)){
		return 1;
	}
	return 0;
}

/**
 * 初始化清屏
 */
void initScreen(void){
	printf("\033[2J");
	printf("\033[?25l");
	printf("\033[1;1H");
}

/**
 * 信号量回调函数
 */
void outt(int signo){
	printf("\033[?25h");
	printf("\033[0m");
	printf("\033[%d;1H", maxRows + 1 + 1);
	exit(0);
}

int main(int argc, char** argv){
	initScreen();
    drawBox();
	signal(SIGINT, outt);
	int times = 0;
	while(1){
		cleanArray(y, x);
		arrayMoveRight(y);
		arrayMoveRight(x);
		do{
			direction= randomNum()%4;
			times++;
			printf("\033[1;1Htimes:%02d", times);
			if(times >= timesReset){
				printf("\033[%d;%dH ", y[0], x[0]);
				times = 0;
                do{
                    x[0] = randomNum()%maxCols;
                    y[0] = randomNum()%maxRows;
                }while((y[0] > (minRows+5)) && (y[0] < (maxRows-5)) && (x[0] < (minCols+5)) && (x[0] > (maxCols-5)));
                x[1] = x[0];
                y[1] = y[0];
			}
			switch(direction){
				case 0:
					x[0] = x[1] + 1;
					y[0] = y[1];
					break;
				case 1:
					x[0] = x[1] - 1;
					y[0] = y[1];
					break;
				case 2:
					x[0] = x[1];
					y[0] = y[1] + 1;
					break;
				case 3:
					x[0] = x[1];
					y[0] = y[1] - 1;
					break;
			}
			usleep(5000);
		}while(hasPosition());
		times = 0;
		printfArray(y, x);
        fflush(0);
		usleep(100000);
	}
}
