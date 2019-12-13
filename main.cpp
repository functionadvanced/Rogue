#include <curses.h>
#define EMPTY 0
#define ME 1
	#define ME_ATK 10
	#define ME_DEF 15
	#define ME_HP 100
	#define LEVEL_EXP 300 
	#define ATK_UP 5 
	#define DEF_UP 2
#define WALL 2
#define IN 4
#define OUT 5
#define MEDICINE 8
	#define MED_HP 20
#define MONSTER_A 3
	#define A_ATK 20 
	#define A_DEF 2
	#define A_HP 20
	#define A_EXP 20
#define MONSTER_B 6
	#define B_ATK 20
	#define B_DEF 5
	#define B_HP 60
	#define B_EXP 25
#define MONSTER_C 7
	#define C_ATK 30
	#define C_DEF 5
	#define C_HP 70
	#define C_EXP 30 
#define LEGEND "\
 <.>--<floor>\n\
 <#>--<wall>\n\
 <@>--<yourself>\n\
 <+>--<medicine>\n\
 <(>--<upstairs>\n\
 <)>--<downstairs>\n\
 <A><B><C>--<monster>\
"
#define INSTRUCTION mvprintw(1, 1, "My Rogue\n a/<left> w/<up> s/<down> d/<right> q/<exit> r/<restart>");\
					mvprintw(offset_y+m_h+1, 0, LEGEND);\
					box(stdscr, '|', '-');
#include <stdlib.h>
static char symbol[] = {'.', '@', '#', 'A', '(', ')', 'B', 'C', '+'};
static const int m_w = 50;
static const int m_h = 22;
static const int offset_x = 5;
static const int offset_y = 3;
static int **map;
int ** all_maps[100];
bool ** visited_maps[100];
int floor;
int reached_floor;
void MakeMap(int _w, int _h, int **_map, bool **_visited);
void drawMap()
{
	for (int i = 0;i < m_w;++i)
		for (int j = 0;j < m_h;++j)
			if (visited_maps[floor-1][j][i] || i == 0 || j == 0 || i == m_w-1 || j == m_h-1)
				mvaddch(j + offset_y, i + offset_x, symbol[map[j][i]]);
}
void MakeFloor()
{
	all_maps[floor] = new int*[m_h];
	for (int i = 0;i < m_h;++i)
		all_maps[floor][i] = new int[m_w];
	visited_maps[floor] = new bool*[m_h];
	for (int i = 0;i < m_h;++i)
		visited_maps[floor][i] = new bool[m_w];
	MakeMap(m_w, m_h, all_maps[floor], visited_maps[floor]);
	map = all_maps[floor];
	++floor;
	++reached_floor;
	clear();
}
void DelFloor()
{
	for (int ii = 0;ii < reached_floor;++ii)
	{
		for (int i = 0;i < m_h;++i)
			delete []all_maps[ii][i];
		delete []all_maps[ii];
		for (int i = 0;i < m_h;++i)
			delete []visited_maps[ii][i];
		delete []visited_maps[ii];
	}	
}
class spirit
{
protected:
	int x;
	int y;
	int kind;
	int hp;
	int atk;
	int def;
public:
	spirit(){}
	spirit(int _x, int _y, int _kind)
	{
		x = _x;
		y = _y;
		kind = _kind;
		switch(kind)
		{
			case MONSTER_A:
				atk = A_ATK;
				def = A_DEF;
				hp = A_HP;
				break;
			case MONSTER_B:
				atk = B_ATK;
				def = B_DEF;
				hp = B_HP;
				break;
			case MONSTER_C:
				atk = C_ATK;
				def = C_DEF;
				hp = C_HP;
				break;
		}
	}
	virtual void ShowStatus(){}
	bool isDead()
	{
		if (hp <= 0)
			return true;
		return false;
	}
	void attack(spirit *aim)
	{
		aim->defend(this, atk);
	}
	void defend(spirit *aim, int _atk)
	{
		if (def >= _atk)
			return;
		hp -= (_atk - def);
		if (!isDead())
			attack(aim);
	}
	void FightToDeath(spirit *aim)
	{
		while(1)
		{
			attack(aim);
			if (aim->isDead() || isDead())
				break;
		}
	}
};
class human:public spirit
{
	int level;
	int exp;
public:
	human(int _kind);
	void Move(int _dir);
	void InFloor();
	void OutFloor();
	void ShowMove(int fx, int fy, int tx, int ty);
	virtual void ShowStatus();
	void ShowView();
	void GainExp(int _kind);
	void LevelUp();
	void GainHp(int _kind); 
}*me;
human::human(int _kind)
{
	kind = _kind;
	level = 1;
	exp = 0;
	atk = ME_ATK;
	def = ME_DEF;
	hp = ME_HP;
	mvaddch(y + offset_y, x + offset_x, symbol[kind]);
	refresh();
}
void human::Move(int _dir)
{
	if (isDead())
		return;
	int dir_x[4] = {-1, 0, 1, 0};
	int dir_y[4] = {0, 1, 0, -1};
	int old_x = x;
	int old_y = y;
	x += dir_x[_dir];
	y += dir_y[_dir];
	if (map[y][x] == WALL)
	{
		x = old_x;
		y = old_y;
		return;
	}
	if (map[y][x] == MEDICINE)
	{
		GainHp(map[y][x]);
		map[y][x] = EMPTY;
	}
	if (map[y][x] == MONSTER_A
		||map[y][x] ==  MONSTER_B 
		||map[y][x] ==  MONSTER_C)//fight with monster
	{
		spirit temp_m(x, y, map[y][x]);
		FightToDeath(&temp_m);
		if (isDead())
		{
			clear();
			INSTRUCTION
			mvprintw(offset_y+m_h/2, offset_x+m_w/2, "You are dead!");
			mvprintw(offset_y+m_h/2+1, offset_x+m_w/2, "You have reached floor %d.", reached_floor);
			refresh();
			return;	
		}
		GainExp(map[y][x]);
		map[y][x] = EMPTY;
	}
	if (map[y][x] == IN)
	{
		InFloor();
		return;
	}
	if (map[y][x] == OUT)
	{
		OutFloor();
		return;
	}
	ShowMove(old_x, old_y, x, y);
	ShowStatus();
	ShowView();
	refresh();
}
void human::InFloor()
{
	if (reached_floor == floor)
		MakeFloor();
	else
	{
		map = all_maps[floor];
		++floor;
	}
	for (int i = 0;i < m_w;++i)
		for (int j = 0;j < m_h;++j)
			if (map[j][i] == OUT)
			{
				x = i;
				y = j;
				clear();
				drawMap();
				ShowStatus();
				ShowView();
				INSTRUCTION
				refresh();
				return;
			}
}
void human::OutFloor()
{
	if (floor == 1)
	{
		ShowView();
		refresh();
		return;
	}
	--floor;
	map = all_maps[floor-1];
	for (int i = 0;i < m_w;++i)
		for (int j = 0;j < m_h;++j)
			if (map[j][i] == IN)
			{
				x = i;
				y = j;
				clear();
				drawMap();
				ShowStatus();
				ShowView();
				INSTRUCTION
				refresh();
				return;
			}
}
void human::ShowMove(int fx, int fy, int tx, int ty)
{
	mvaddch(fy + offset_y, fx + offset_x, symbol[EMPTY]); //EMPTY
	mvaddch(ty + offset_y, tx + offset_x, symbol[this->kind]);
}
void human::ShowStatus()
{
	mvprintw(offset_y, offset_x + m_w +1,   "floor:%d    ", floor);
	mvprintw(offset_y+1, offset_x + m_w +1, "status of %c", symbol[kind]);
	mvprintw(offset_y+2, offset_x + m_w +1, "HP:%d/%d    ", hp, ME_HP);
	mvprintw(offset_y+3, offset_x + m_w +1, "ATK:%d    ", atk);	
	mvprintw(offset_y+4, offset_x + m_w +1, "DEF:%d    ", def);	
	mvprintw(offset_y+5, offset_x + m_w +1, "LEV:%d    ", level);	
	mvprintw(offset_y+6, offset_x + m_w +1, "EXP:%d/%d   ", exp, LEVEL_EXP);	
}
void human::ShowView()
{
	visited_maps[floor-1][y][x] = true;
	mvaddch(offset_y+y, offset_x+x, symbol[ME]);
	int dx[8] = {0, 0, 1, 1, 1, -1, -1, -1};
	int dy[8] = {1, -1, 1, 0, -1, 1, 0, -1};
	for (int i = 0;i < 8;++i)
	{
		visited_maps[floor-1][y+dy[i]][x+dx[i]] = true;
		mvaddch(offset_y+y+dy[i], offset_x+x+dx[i], symbol[map[y+dy[i]][x+dx[i]]]);
	}
}
void human::GainExp(int _kind)
{
	switch(_kind)
	{
		case MONSTER_A: exp += A_EXP;break;
		case MONSTER_B: exp += B_EXP;break;
		case MONSTER_C: exp += C_EXP;break;
	}
	while (exp >= LEVEL_EXP)
	{
		exp -= LEVEL_EXP;
		LevelUp();
	}
}
void human::LevelUp()
{
	++level;
	hp = ME_HP;
	atk += ATK_UP;
	def += DEF_UP;
}
void human::GainHp(int _kind)
{
	switch(_kind)
	{
		case MEDICINE:
			hp += MED_HP;
			if (hp > ME_HP)
				hp = ME_HP;
			break;
	}
}
void InitGame()
{
	clear();
	if (all_maps != NULL)
		DelFloor();
	floor = 0;
	reached_floor = 0;
	if (me != NULL)
		delete me;
	me = new human(ME);
	me->InFloor();
	me->ShowStatus();
	me->ShowView();
	INSTRUCTION
	refresh();
}
int main()
{
	initscr();
	noecho();
	curs_set(0);
//	start_color();
//	use_default_colors();
//	init_pair(10, COLOR_RED, COLOR_YELLOW);
//	void * color_point;
//	color_set(10, color_point);
	InitGame();
	while(1)
	{
		char order = getch();
		if (order == 'q') break;
		switch(order)
		{
			case 'a': me->Move(0);break;
			case 's': me->Move(1);break;
			case 'd': me->Move(2);break;
			case 'w': me->Move(3);break;
			case 'r': InitGame();break;
		}
	}
	delete me;
	DelFloor();
	clear();
	refresh();
	endwin();
	return 0;
}
