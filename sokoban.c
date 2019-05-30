#include<stdio.h>
#include<stdlib.h>
#include<termio.h>


	/*
	 *  Sokoban / 19.05.30
	 *
	 *	Who made? Mj Cho, Dn Seo, Gw Lee, Yg Kim.
	 *
	 *	@:Player,  #:Wall,  $:Gold,  O:Safe,  .:Blank
	 *
	*/


int x = 0, y = 0, lvl = 0, isOnGame = 0;
int allMap[5][31][31] = {0};
int curMap[31][31] = {0};

int getch(void);							// 엔터없이 입력
int getMap(void);							// 맵불러오기
void setMap(char n[12]);					// 맵 출력
int onGame(char name[12]);					// 게임 
void mv(int ch, char name[12]);				// 움직이기

// 해야할것
void undo(void);
void save();
void load();
void display(void);
void top(int);
//


int main(void){

	char player_name[12] = {0};

	system("clear");

	printf("Loading...\n");
	if(getMap() == 1)			// 맵 불러올시 에러가 있으면
		goto end;

	printf("Start...\n\n");
	
	// Get a name
	printf("input name : ");
	scanf("%10s", player_name);

//	do{
		onGame(player_name);	
//	}while(level < 6);

	end :

	return 0;
}


int getch(void){
	int ch;

	struct termios buf;
	struct termios save;

	tcgetattr(0, &save);
	buf = save;

	buf.c_lflag&=~(ICANON|ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	tcsetattr(0, TCSAFLUSH, &buf);
	
	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);

	return ch;
}

int getMap(void){
	
	int c, lvls = 0, i = 0, j = 0, cmp = 0;

	FILE *ifp;
	
	// Load maps
    if ((ifp = fopen("map","r")) == NULL){
		printf("Error : Cannot open the map file!\n");
		system("cat >> eLog <<EOF\nError : Cannot open the map file!\nEOF");
		fclose(ifp);
		return 1;
	}
	else{
		while((c = getc(ifp)) != EOF){				// '1' = 49, '\n' = 10
			switch(c){
				case '\n' :
					if(j != 0){                     // 스테이지가 바뀌지 않았을 때 
						allMap[lvls][i++][j] = c;
						j = 0;
					}
			 		break;
				case 'e' :
					break;
				case '2' :
				case '3' :
				case '4' :
				case '5' :
					lvls++; i=0; j=0;
					if(cmp != 0){                   // 금과 금고개수 확인
						printf("Error : [Stage-%d] The number of $ and O is different!\n", lvls+1);
						system("cat >> eLog <<EOF\nError : The number of $ and O is different!\nEOF");
						fclose(ifp);
						return 1;
					}
					break;
				case '$' :					// $,O,@,#,.
				case 'O' :
				case '@' :
				case '#' :
				case '.' :
					if(c == '$')
						cmp++;
					else if(c == 'O')
						cmp--;
					allMap[lvls][i][j++] = c;
					break;
			}
		}
	}
	fclose(ifp);

	printf("\nDone\n\n");
 		
	return 0;
}


void setMap(char n[12]){
	
	int i,j;
	
	system("clear");

	printf("Hello %s\n\nStage %d\n\n\n", n, lvl);	

	for(i = 0; i < 31; i++){
		for(j = 0; j < 31; j++){
			switch(curMap[i][j]){
				case '@' : putchar('@'); x=j; y=i; break;			// 플레이어 위치
				case '#' : putchar('#'); break;
				case '$' : putchar('$'); break;
				case 'O' : putchar('O'); break;
				case '.' : putchar(' '); break;
				case '\n' : putchar('\n');i++; j = -1; break;
				default : break;
			}
			
		}
	}
	
	printf("\n\n\n(Command) ");
	
	return;

}

void mv(int ch, char name[12]){
	
	int tmpx = x, tmpy = y;
	
	char isS[12] = {0};

	t=0;
	if(name[10] == '$'){
		switch(ch){
			case 'h' : tmpx--; break;
			case 'l' : tmpx++; break;
			case 'j' : tmpy++; break;
			case 'k' : tmpy--; break;
			default : break;
		}
	}	
	switch(ch){
		case 'h' : tmpx--; break;
		case 'l' : tmpx++; break;
		case 'j' : tmpy++; break;
		case 'k' : tmpy--; break;
		default : break;
	}
	
	switch(curMap[tmpy][tmpx]){
		case '.' : 
		case 'O' : 
			if(name[10] != '$'){
				curMap[tmpy][tmpx]='@';
				if(allMap[lvl][y][x]=='O')
					curMap[y][x]=allMap[lvl][y][x];
				else
					curMap[y][x]='.';
			}
			else{
				curMap[tmpy][tmpx]='$';
				t++;
			}
			break;
		case '$' :
			if(name[10] != '$'){
				isS[10] = '$';

				mv(ch, isS);
				if(t==1){
					curMap[tmpy][tmpx]='@';
					if(allMap[lvl][y][x]=='O')
						curMap[y][x]=allMap[lvl][y][x];
					else
						curMap[y][x]='.';
				}
			}
			break;
		case '#' :
		default : break;
	}
	if(name[10] != '$')	
		setMap(name);

	return;
}
int onGame(char name[12]){
	
	int steps;
	int undoArr[2][5];


	//게임 값 초기화
	for(int i=0; i<2; i++)
		for(int j=0; j<5; j++)
			undoArr[i][j] = {0};
	steps = 0;	
	for(int i = 0; i < 31; i++){
		for(int j = 0; j < 31; j++){
			curMap[i][j]=allMap[lvl][i][j];
		}
	}

	//게임 시작
	isOnGame = 1;

	setMap(name);


	do{
		int key;
		
		key = getch();

		switch(key){
			case 'h' :
			case 'j' :
			case 'k' :
			case 'l' : steps++; mv(key, name); break; 	//이동
			case 'u' : break;
			case 'r' : break;
			case 'n' : break;
			case 'e' : break;
			case 's' : break;
			case 'f' : break;
			case 'd' : display(); setMap(name); break;	//명령어
			case 't' : break;
			default : break;
		}

	}while(isOnGame == 1);
	
	return steps;
}

void display(void){
	int ch = '0';
	
	system("clear");
	
	printf("* * * * * * * * * * * * * * * * * * *\n");
	printf("*              Command              *\n");
	printf("*                                   *\n");
	printf("* h(up), j(left), k(right), l(down) *\n");
	printf("* u(undo)                           *\n");
	printf("* r(retry current stage)            *\n");
	printf("* n(new game)                       *\n");
	printf("* e(exit game)                      *\n");
	printf("* s(save current)                   *\n");
	printf("* f(load saved file)                *\n");
	printf("* d(display Command)                *\n");
	printf("* t(top ranking)                    *\n");
	printf("*                                   *\n");
	printf("* * * * * * * * * * * * * * * * * * *\n\n\n");
	printf("press q to quit");

	do{
		ch = getch();
	}while(ch != 'q' || ch != 'Q');
	
	return;
}












