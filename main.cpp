#include <iostream>
#include <string>
#include <fstream>
#include <dirent.h>
#include <ctime>
#include <cstring>
#include <cstdlib>

#define field_size 10
#define ships 4 // количество кораблей

using namespace std; 

string letters1 = "АБВГДЕЖЗИКЛМНОПРСТ";
string letters2 = "абвгдежзиклмнопрст";
int Move_Number = 0;
int NGame = 0; // для сохранения 
int N = 0;  // для загрузки
int Player_Number = 0;

const char * menu[30] = {"Начать новую игру", "Загрузить старую игру", "Таблица рекордов", "Выход"};

struct Rating{
	string Name;
	int step;
	string datatime;
};

struct Move_Player{
	int x;
	int y;
	int Player_Number;
};

class Square{
	public:
		int field[field_size][field_size][2]; // поле для игры
		int field2[field_size][field_size][2]; // сохранённое поле
		
		Square();
		// очищение игрового поля
		void Empty_Square();
		// печать игрового поля для игры
		void Print_Square();
		// печать игрового поля для расстановки кораблей
		void Print_SquareForArrangement();
		// проверка на правльность расстановки 1 судна
		int CheckArrangementShips(char c, int x, int y, int deck);
		// расстановка 1 судна 
		void ArrangementDeckShip(char c, int x, int y, int deck); 
		// проверка на корректность аттаки на поле
		int CheckAttack(struct Move_Player Move); 
		// проверка на убитого корабля
		void CheckShip(struct Move_Player Move); 
		// функция для оконтовки потопленного корабля
		void function(int x, int y, int desk); 
		// проверка на наличие живых кораблей
		int CheckAllSquare();
		// копирование игворого поля
		void Copy();
		// вставка игрового поля
		void Paste();
};

class Player{
	public:
		struct Move_Player Move;
		int Steps;
		
		// проверка на правильность введённых данных для аттаки
		int EnteringAndCheckingCoordinatesForAttack(); 
		// проверка на правильность введённых данных для расстановки кораблей
		char EnteringAndCheckingCoordinatesForArrangement(); 
		int Kods(char c, int i); // проверка на коды
};

class Game{
	public:
		Player *P[2];
		Square *S[2];
		struct Move_Player History[field_size*field_size*2];
		struct Rating R[10];
		
		Game();
		void ArrangementShips(); 
		void ArrangementShipsYourself();
		void ArrangementShipsAuto();
		int Attack(); 
		void Print_Squares(); // вывод всего поля	
		void LetsPlay();
		void ContinuePlay();
		void Print_History();
		void Save_History();
		void Save_Game();
		int Load_Game();
		void Load_Start();
		void Save_Start();
		void Menu();
		int PrintFilesInFolder();
		void CountSteps();
		void ArrangementMove(); // расстановка ходов
		void Load_Rating();
		void Save_Rating();
		void Print_Rating();
		void Create_Rating();
		void New_Rating();
		~Game();
};

int main(int argc, char** argv) {
	
	system("chcp 1251>NUL");
	srand(time(NULL));
	Game G;
	G.Menu();
	
	return 0;
}

// Поле

Square::Square(){
	this->Empty_Square();
}

void Square::Empty_Square(){
	for(int i = 0; i < field_size; i++){
		for(int j = 0; j < field_size; j++){
			this->field[i][j][0] = 0;
			this->field[i][j][1] = -1;
		}
	}
}

void Square::Print_Square(){
	string line (field_size*2+1, '-');
	cout << "     ";
	for(int i = 0; i < field_size; i++) cout << letters1[i] << " "; 
	cout << "\n" << "    " << line << endl;
	for(int i = 0; i < field_size; i++){
		printf(" %-3d|", i+1);
		for(int j = 0; j < field_size-1; j++){
			if(this->field[i][j][0] == -1) cout << "о ";
			else if(this->field[i][j][0] == 2) cout << "x ";
			else cout << "  ";
		}
		if(this->field[i][field_size-1][0] == -1) cout << "о|" << endl;
		else if(this->field[i][field_size-1][0] == 2) cout << "x|" << endl;
		else cout << " |" << endl;
	}
	cout << "    " << line << endl;
}

void Square::Print_SquareForArrangement(){
	string line (field_size*2+1, '-');
	cout << "     ";
	for(int i = 0; i < field_size; i++) cout << letters1[i] << " "; 
	cout << "\n" << "    " << line << endl;
	for(int i = 0; i < field_size; i++){
		printf(" %-3d|", i+1);
		for(int j = 0; j < field_size-1; j++){ // для [2]: в - 0; г - 1
			if(this->field[i][j][0] == 1) {
				if(this->field[i][j][1] == 0) cout << "| ";
				else cout << "- ";
			}
			else cout << "  ";
		}
		if(this->field[i][field_size-1][0] == 1){
			if(this->field[i][field_size-1][1] == 0) cout << "||" << endl;
				else cout << "-|" << endl;
		}
		else cout << " |" << endl;
	}
	cout << "    " << line << endl;
}

int Square::CheckArrangementShips(char c, int x, int y, int deck){ 
	if((c == 'в'?y:x)+deck-1 >=field_size) return 0; // проверка на выход корабля за пределы поля
	for(int i = y-1; i <= y+(c == 'в' ? deck : 1); i++){  // ?: deck*(c == 'v')+1*(c != 'v')
		if(i < 0 || i >= field_size) continue; // проверка на выход за пределы массива
		for(int j = x-1; j <= x+(c == 'в' ? 1 : deck); j++){ // 1*(c == 'v')+deck*(c != 'v')
			if(j < 0 || j >= field_size) continue; // проверка на выход за пределы массива
			if (this->field[i][j][0] != 0) return 0; // проверка на наличие не пустой клетки
		}
	}	
	return 1;
}

void Square::ArrangementDeckShip(char c, int x, int y, int deck){ // завершена
	if (c == 'в'){												  // в - 0; h - 1
		for(int i = 0; i < deck; i++){
			this->field[y+i][x][0] = 1;
			this->field[y+i][x][1] = 0;
		}
	}else{
		for(int i = x; i < x+deck; i++){
			this->field[y][i][0] = 1;
			this->field[y][i][1] = 1;
		}
	}
}

int Square::CheckAttack(struct Move_Player Move){ // завершена
	switch(this->field[Move.y][Move.x][0]){
		case 0:
			this->field[Move.y][Move.x][0] = -1;
			return 0;
		case 1:
			this->field[Move.y][Move.x][0] = 2;
			return 1;
		default:
			return 2;
	}
}

int Square::CheckAllSquare(){
	for(int i = 0; i < field_size; i++){
		for(int j = 0; j < field_size; j++){
			if(this->field[i][j][0] == 1) return 1;
		}
	}
	return 0;
}

void Square::CheckShip(struct Move_Player Move){
	int x = Move.x;
	int y = Move.y;
	int desk = 1;
	if(this->field[y][x][1] == 0){ // vertical
		while(this->field[y+1][x][0] > 0){
			if(y+1 >= field_size) break;
			if(this->field[y+1][x][0] == 1) return ;
			y++;
			desk++;
		}
		y = Move.y;
		while(this->field[y-1][x][0] > 0){
			if(y-1 < 0) break;
			if(this->field[y-1][x][0] == 1) return ;
			y--;
			desk++;
		}
	}else{ // horizontal
		while(this->field[y][x+1][0] > 0){
			if(x+1 >= field_size) break;
			if(this->field[y][x+1][0] == 1) return ; 
			x++;
			desk++;
		}
		x = Move.x;
		while(this->field[y][x-1][0] > 0){
			if(x-1 < 0) break;
			if(this->field[y][x-1][0] == 1) return ;
			x--;
			desk++;
		}
	}
	this->function(x, y, desk);
}
	
void Square::function(int x, int y, int desk){
	int v = y+(this->field[y][x][1] == 0? desk:1);
	int h = x+(this->field[y][x][1] == 1? desk:1);
	for(int i = y-1; i <= v; i++){ // vert - 0; horz - 1
		if(i < 0 || i >= field_size) continue;
		for(int j = x-1; j <= h; j++){
			if(j < 0 || j >= field_size) continue;
			if(this->field[i][j][0] != 2) 
				this->field[i][j][0] = -1;
		}
	}
}

void Game::Print_Squares(){
	system("cls");
	cout << "Поле 1 игрока: " << endl;
	this->S[0]->Print_Square();
	cout << "Поле 2 игрока: " << endl;
	this->S[1]->Print_Square();
}

Game::Game(){
	S[0] = new Square;
	S[1] = new Square;
	P[0] = new Player;
	P[1] = new Player;
}

Game::~Game(){
	delete [] S[0];
	delete [] S[1];
	delete [] P[0];
	delete [] P[1];
}

void Game::ArrangementShips(){
	fflush(stdin);
	system("cls");
	printf("Введите 'я', если хотите расставить корабли самостоятельно\n\t'а', если хотите расставить корабли автоматически\n: ");
	char o;
	o = getchar();
	if(o == 'я') this->ArrangementShipsYourself();
	else this->ArrangementShipsAuto();
	this->S[Player_Number]->Print_SquareForArrangement();
	fflush(stdin);
	printf("Введите 'о', чтобы продолжить, или 'х', чтобы расставить корабли заново: ");
	char c = getchar();
	if(c == 'х'){
		this->S[Player_Number]->Empty_Square();
		this->ArrangementShips();
	}
	this->S[Player_Number]->Copy();
}

void Game::ArrangementShipsAuto(){
	int m = 1;
	char orintation;
	char var[] = {'в', 'г'};
	int x;
	int y;
	for(int i = ships; i > 0; i--){
		for(int j = m; j > 0; j--){
			orintation = var[rand()%2];
			x = rand()%10;
			y = rand()%10;
			if(this->S[Player_Number]->CheckArrangementShips(orintation, x, y, i)){
				this->S[Player_Number]->ArrangementDeckShip(orintation, x, y, i);
			}
			else{
				j++;
			}
		}
		m++;
	}
}

void Game::ArrangementShipsYourself(){
	int m = 1;
	char orintation;
	for(int i = ships; i > 0; i--){
		for(int j = m; j > 0; j--){
			this->S[Player_Number]->Print_SquareForArrangement();
			orintation = this->P[Player_Number]->EnteringAndCheckingCoordinatesForArrangement();
			if(this->S[Player_Number]->CheckArrangementShips(orintation, this->P[Player_Number]->Move.x, this->P[Player_Number]->Move.y, i)){
				this->S[Player_Number]->ArrangementDeckShip(orintation, this->P[Player_Number]->Move.x, this->P[Player_Number]->Move.y, i);
				printf("Корабль успешно поставлен!\n");
			}
			else{
				printf("Вы не можете поставить здесь корабль!\n");
				j++;
			}
		}
		m++;
	}
}

int Game::Attack(){
	int check;
	int kod;
	for(int i = 0; i < 5;){
		cout << "Ходит " << Player_Number+1 << " игрок" << endl; 
		kod = this->P[Player_Number]->EnteringAndCheckingCoordinatesForAttack();
		if(kod == 2) {
			this->Save_Game();
			printf("Игра сохранена.\n");
			return -1;
		}
		if(kod == 3) return 2;
		if(kod == 4){
			char q;
			printf("Игрок %d редложил ничью. Игрок %d вы согласны?(y/n)\n", Player_Number+1, (Player_Number^1)+1);
			cin >> q;
			if(q == 'y') return 3;
			else continue;
		}
		if(kod == 5){
			if(Move_Number > 0) Move_Number--;
			this->ArrangementMove();
			this->Print_Squares();
			if(this->S[this->History[Move_Number].Player_Number^1]->field[this->History[Move_Number].y][this->History[Move_Number].x][0]==2) Player_Number = this->History[Move_Number].Player_Number;
			else Player_Number = this->History[Move_Number].Player_Number^1;
		}
		else{
			check = this->S[Player_Number^1]->CheckAttack(this->P[Player_Number]->Move);
			if(check == 0){ // 0 - мимо
				this->History[Move_Number].x = this->P[Player_Number]->Move.x;
				this->History[Move_Number].y = this->P[Player_Number]->Move.y;
				this->History[Move_Number].Player_Number = Player_Number;
				Move_Number++;
				printf("Мимо!\n");
				return 0;
			} 
			else if(check == 1){ // 1 - попал
				this->History[Move_Number].x = this->P[Player_Number]->Move.x;
				this->History[Move_Number].y = this->P[Player_Number]->Move.y;
				this->History[Move_Number].Player_Number = Player_Number;
				this->S[Player_Number^1]->CheckShip(this->History[Move_Number]);
				Move_Number++;
				this->Print_Squares();
				if(this->S[Player_Number^1]->CheckAllSquare() == 0){ // все корабли убиты
					
					return 7;
				}
				else printf("Попал!\n");
				i = 0;
			}
			else { // 2 - удар по той же координате
				printf("Вы уже били по этой координате!\n");
				i++;
			}
		}
	}
	return 1; // техническое поражение
}

void Game::Print_History(){
	cout << "История ходов:" << endl;
	for(int i = 0; i < Move_Number; i++){
		cout << this->History[i].x << ' ' << this->History[i].y << ' '  << this->History[i].Player_Number << endl;
	}
}

void Game::Save_Start(){
	NGame++;
	ofstream fout;
	fout.open("For_Game/start.txt");
	if(fout.is_open()) fout << NGame;
	else cout << "Не получилось открыть файл" << endl;
	fout.close();
}

void Game::Load_Start(){
	ifstream fin;
	fin.open("For_Game/start.txt");
	if(fin.is_open()) fin >> NGame;
	else cout << "Не удалось загрузить файл" << endl;
	fin.close();
}

void Game::Save_Game(){
	this->Load_Start();
	ofstream fout;
	string fname("For_Game/Games/Game");
	fname.append(to_string(NGame));
	fname.append(".txt");
	fout.open(fname);
	if(fout.is_open()){
		for(int i = 0; i < field_size; i++){
			for(int j = 0; j < field_size; j++){
				fout<<this->S[0]->field2[i][j][0]<< ' '<< this->S[0]->field[i][j][1]<< ' ';
			}
			fout << '\n';
		}
		for(int i = 0; i < field_size; i++){
			for(int j = 0; j < field_size; j++){
				fout<<this->S[1]->field2[i][j][0]<< ' '<< this->S[1]->field[i][j][1] << ' ';
			}
			fout << '\n';
		}
		fout << Move_Number << endl;
		for(int i = 0; i < Move_Number; i++){
			fout << this->History[i].x << ' ' << this->History[i].y << ' '  << this->History[i].Player_Number << endl;
		}
		this->Save_Start();
	}else{
		cout << "Не получилось открыть файл" << endl;
	}
	fout.close();
}

int Game::Load_Game(){
	ifstream fin;
	string fname("For_Game/Games/Game");
	fname.append(to_string(N));
	fname.append(".txt");
	fin.open(fname);
	if(fin.is_open()){
		for(int i = 0; i < field_size; i++){
			for(int j = 0; j < field_size; j++){
				fin >> this->S[0]->field[i][j][0];
				fin >> this->S[0]->field[i][j][1];
			}
		}
		for(int i = 0; i < field_size; i++){
			for(int j = 0; j < field_size; j++){
				fin >> this->S[1]->field[i][j][0];
				fin >> this->S[1]->field[i][j][1];
			}
		}
		fin >> Move_Number;
		for(int i = 0; i < Move_Number; i++){
			fin >> this->History[i].x >> this->History[i].y >> this->History[i].Player_Number;
		}
		this->S[0]->Copy();
		this->S[1]->Copy();
		fin.close();
		return 1;
	}else{
		cout << "Не удалось загрузить файл" << endl;
		fin.close();
		return 0;
	}
}

int Game::PrintFilesInFolder(){
	string folderPath("For_Game/Games");
	int i = 0;
	DIR *it;
	dirent *pdir;
	it = opendir("For_Game/Games");
	while ((pdir = readdir(it))) {
		string name = pdir->d_name;
		if (name.substr(name.find_last_of(".") + 1) == "txt"){
			cout << name << std::endl;
			i++;
		}	
    }
	closedir(it);
	return i;
}

void Game::CountSteps(){
	this->P[0]->Steps = 0;
	this->P[1]->Steps = 0;
	for(int i = 0; i < Move_Number; i++){
		this->P[this->History[i].Player_Number]->Steps += 1;
	}
}

int Player::Kods(char c, int i){
	// коды:
	// сохранение s1
	// признать поражение d1
	// предложить ничью t1
	// ход назад z1
	if(c == 's' && i == 1) return 2;
	if(c == 'd' && i == 1) return 3;
	if(c == 't' && i == 1) return 4;
	if(c == 'z' && i == 1) return 5;
	return 0;
}

int Player::EnteringAndCheckingCoordinatesForAttack(){ // Ввод и проверка координат
	int y;
	char c;
	printf("Правило ввода:  1 символ буква,\n\t\t2 символ цифра\n");
	printf("Введите координаты: ");
	scanf(" %c%d", &c, &y); 
	while((letters1.find(c) == string::npos && letters2.find(c) == string::npos)
		   || !(1 <= y && y <= field_size)){
		if(this->Kods(c, y)) return this->Kods(c, y);
		printf("Не правильно введённые координаты!\n");
		printf("Введите координаты заново: ");
		scanf(" %c%d", &c, &y);		
	}
	fflush(stdin);
	this->Move.y=y-1;
	if(letters1.find(c) != string::npos){
		this->Move.x = letters1.find(c);
	}else{
		this->Move.x = letters2.find(c);
	}
	return 1;
}

char Player::EnteringAndCheckingCoordinatesForArrangement(){
	int y;
	char c;
	char orintation;
	printf("Правило ввода:  1 символ - буква ориентации (г-горизонтально, в-вертикально)\n\t\t2 символ - координата поля (буква)\n\t\t3 символ - координата поля (цифра)\n");
	printf("Введите координаты: ");
	scanf(" %c%c%d", &orintation, &c, &y);
	while((letters1.find(c) == string::npos && letters2.find(c) == string::npos)
		   || !(1 <= y && y <= field_size) || !(orintation == 'в' || orintation == 'г')){
		printf("Не правильно введённые координаты!\n");
		printf("Введите координаты заново: ");
		scanf(" %c%c%d", &orintation, &c, &y);		
	}
	fflush(stdin);
	this->Move.y=y-1;
	if(letters1.find(c) != string::npos){
		this->Move.x = letters1.find(c);
	}else{
		this->Move.x = letters2.find(c);
	}
	return orintation;
}

void Game::ArrangementMove(){
	this->S[0]->Paste();
	this->S[1]->Paste();
	for(int i = 0; i < Move_Number; i++){
		this->S[this->History[i].Player_Number^1]->CheckAttack(this->History[i]);
	}
	struct Move_Player M;
	for(int i = 0; i < field_size; i++){
		for(int j = 0; j < field_size; j++){
			M.y=i;
			M.x=j;
			if(this->S[0]->field[i][j][0] == 2){
				this->S[0]->CheckShip(M);
			}
			if(this->S[1]->field[i][j][0] == 2){				
				this->S[1]->CheckShip(M);
			}
		}
	}
}

void Square::Copy(){
	for(int i = 0; i < field_size; i++){
		for(int j = 0; j < field_size; j++){
			this->field2[i][j][0] = this->field[i][j][0];
			this->field2[i][j][1] = this->field[i][j][1];
		}
	}
}

void Square::Paste(){
	for(int i = 0; i < field_size; i++){
		for(int j = 0; j < field_size; j++){
			this->field[i][j][0] = this->field2[i][j][0];
			this->field[i][j][1] = this->field2[i][j][1];
		}
	}
}

void Game::Save_Rating(){
	ofstream fout;
	string fname("For_Game/Rating.txt");
	fout.open(fname);
	if(fout.is_open()){
		for(int i = 0; i < 10; i++){
			fout << this->R[i].Name<<endl; //for(int j=0; j < 30-this->R[i].Name.length()+1;j++) fout<<' '; 
			fout << this->R[i].step<<endl; //for(int j=0; j < 3-to_string(this->R[i].step).length()+1;j++) fout<<' ';
			fout << this->R[i].datatime << endl;	
		}
	}else{
		cout << "Не получилось открыть файл" << endl;
	}
	fout.close();
}

void Game::Load_Rating(){
	ifstream fin;
	string fname("For_Game/Rating.txt");
	fin.open(fname);
	string buff;
	if(fin.is_open()){
		for(int i = 0; i < 10; i++){
			getline(fin, this->R[i].Name);
			getline(fin, buff);
			this->R[i].step = atoi(buff.c_str());
			getline(fin, this->R[i].datatime);
		}
	}else{
		cout << "Не удалось загрузить файл" << endl;
	}
	fin.close();
}

void Game::Print_Rating(){
	cout << "   Имя пользователя              балл    дата и время" << endl;
	for(int i = 0; i < 10; i++){
		printf("%2d.%-30s %2d %20s\n", i+1, this->R[i].Name.c_str(), this->R[i].step, this->R[i].datatime.c_str());
	}
}

void Game::Create_Rating(){
	for(int i = 0; i < 10; i++){
		this->R[i].Name = "-";
		this->R[i].step=100;
		this->R[i].datatime = '0';
	}
}

void Game::Menu(){
	int k;
	int h;
	this->Load_Rating();
	do{
		for(int i = 1; i < 5; i++){
			cout<<"   " << i<<'.'<<menu[i-1]<<endl;
		}
		scanf("%d", &k);
		fflush(stdin);
		switch(k){
			case 1:
				this->LetsPlay();
				break;
			case 2:
				h = this->PrintFilesInFolder();
				if(h){
			    	cout << "Выберите номер игры (цифру после Game):"<< endl;
					cin >> N;
					if(this->Load_Game()) this->ContinuePlay();					
				}else{
					cout << "Нет сохранённых игр."<< endl;
				}
				break;
			case 3:
				this->Print_Rating();
				break;
			case 4:
				break;
			default:
				k = 5;
				break;
		}
	}while(k != 4);
}

void Game::ContinuePlay(){
	int k = 10000;
	this->ArrangementMove();
	Move_Number--;
	if(this->S[this->History[Move_Number].Player_Number^1]->field[this->History[Move_Number].y][this->History[Move_Number].x][0]==2) Player_Number = this->History[Move_Number].Player_Number;
	else Player_Number = this->History[Move_Number].Player_Number^1;
	Move_Number++;
	while(k != 7 && (k > 3 || k == 0)){
		this->Print_Squares();
		k = this->Attack();
		Player_Number = Player_Number^1;
	}
	if(k == 1) printf("Игрок %d получает техническое поражение. Победил %d игрок.\n", (Player_Number^1)+1, Player_Number+1);
	if(k == 2) printf("Игрок %d признал поражение. Победил %d игрок.\n", (Player_Number^1)+1, Player_Number+1);
	if(k == 3) printf("Ничья\n");
	if(k == 7){
		cout << "Выйграл " << (Player_Number^1)+1 << " Игрок" << endl;
		this->CountSteps();
		if(this->P[Player_Number^1]->Steps < this->R[9].step) this->New_Rating();
	}
}

void Game::New_Rating(){
	time_t t = time(nullptr);
    tm* now = localtime(&t);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%m-%d-%Y %X", now);
    
	int j = 0;
	for(int i = 8; (i >= 0) && (this->P[Player_Number^1]->Steps < this->R[i].step); i--){
		this->R[i+1].step = this->R[i].step;
		this->R[i+1].datatime=this->R[i].datatime;
		this->R[i+1].Name=this->R[i].Name;
		j = i;
	}
	this->R[j].step = this->P[Player_Number^1]->Steps;
	this->R[j].datatime=buffer;
	cout << "Введите ваше имя: ";
	cin >> this->R[j].Name;
	this->Save_Rating();
}

void Game::LetsPlay(){
	int k = 10000;
	this->S[0]->Empty_Square(); // очистка поля 1 игрока
	this->S[1]->Empty_Square(); // очистка поля 2 игрока
	Move_Number = 0;
	this->ArrangementShips();
	Player_Number = Player_Number^1;
	this->ArrangementShips();
	Player_Number = Player_Number^1;	
	while(k != 7 && (k > 3 || k == 0)){
		this->Print_Squares();
		k = this->Attack();
		Player_Number = Player_Number^1;
	}
	if(k == 1) printf("Игрок %d получает техническое поражение. Победил %d игрок.\n", (Player_Number^1)+1, Player_Number+1);
	if(k == 2) printf("Игрок %d признал поражение. Победил %d игрок.\n", (Player_Number^1)+1, Player_Number+1);
	if(k == 3) printf("Ничья\n");
	if(k == 7){
		cout << "Выйграл " << (Player_Number^1)+1 << " Игрок" << endl;
		this->CountSteps();
		if(this->P[Player_Number^1]->Steps < this->R[9].step) this->New_Rating();
	}
} 










