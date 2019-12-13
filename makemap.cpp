#include <stdlib.h>
#include <time.h>
#define ROOM_MIN_W 4
#define ROOM_MIN_H 4
#define ROOM_MAX_W 8 
#define ROOM_MAX_H 7
#define ROOM_NUM 8 
#define MAX_TRY 300
#define EMPTY 0
#define WALL 2
#define ME 1
#define IN 4
#define OUT 5
#define MONSTER_A 3
#define MONSTER_B 6
#define MONSTER_C 7
#define MEDICINE 8
class room
{
public:
	int x;
	int y;
	int w;
	int h;
	bool TryCreateRoom(int _w, int _h, int **map)
	{
		do
			w = rand() % (ROOM_MAX_W + 1);
		while(w < ROOM_MIN_W);
		do
			h = rand() % (ROOM_MAX_H + 1);
		while(h < ROOM_MIN_H);
		x = rand() % (_w + 1);
		y = rand() % (_h + 1);
		if (x + w > _w || y + h > _h)
			return false;
		for (int i = x;i < x + w;++i)
			for (int j = y;j < y + h;++j)
			{
				if (map[j][i] != WALL || i == 0 || i == _w-1 || j == 0 || j == _h-1)
					return false;
			}
		return true;
	}
	bool CreateRoom(int _w, int _h, int **map)
	{
		for (int i = 0;i < MAX_TRY;++i)
			if (TryCreateRoom(_w, _h, map))
			{
				for (int ii = x;ii < x + w;++ii)
					for (int jj = y;jj < y + h;++jj)
						map[jj][ii] = 0;
				return true;
			}
		return false;
	}
};
void MakePassageWay(int fx, int fy, int tx, int ty, int **map)
{
	for (int i = 0;i <= tx || i <= fx;++i)
		if (map[fy][i] == WALL 
			&& ((i <= tx && i >= fx)||(i >= tx && i <= fx)))
			map[fy][i] = EMPTY;
	for (int i = 0;i <= ty || i <= fy;++i)
		if (map[i][tx] == WALL 
			&& ((i <= ty && i >= fy)||(i >= ty && i <= fy)))
			map[i][tx] = EMPTY;
}
void LinkRoom(room *_r1, room *_r2, int **map)
{
	MakePassageWay(_r1->x+_r1->w/2, _r1->y+_r1->h/2, _r2->x+_r2->w/2, _r2->y+_r2->h/2, map);
}
void MakeSth(int _kind, int _num, int **map, int _w, int _h)
{
	for (int i = 0;i < _num;++i)
	{
		int x, y;
		for (int ii = 0;ii < MAX_TRY;++ii) 
		{
			x = rand() % _w;
			y = rand() % _h;
			if (map[y][x] == EMPTY)
			{
				map[y][x] = _kind;
				break;
			}
		}
	}
}
void MakeMap(int _w, int _h, int **map, bool **visited)
{
	srand((unsigned)time(NULL));
	for (int i = 0;i < _w;++i)
		for (int j = 0;j < _h;++j)
		{
			visited[j][i] = false;
			map[j][i] = WALL;
		}
	room r[ROOM_NUM];
	r[0].CreateRoom(_w, _h, map);//generate room
	for (int i = 1;i < ROOM_NUM;++i)
		if (r[i].CreateRoom(_w, _h, map))
			LinkRoom(&r[i-1], &r[i], map);
	MakeSth(MONSTER_A, 10, map, _w, _h);
	MakeSth(MONSTER_B, 20, map, _w, _h);
	MakeSth(MONSTER_C, 10, map, _w, _h);
	MakeSth(MEDICINE, 5, map, _w, _h);
	while(1)//generate the entrance
	{
		int x = rand() % _w;
		int y = rand() % _h;
		if (map[y][x] == EMPTY)
		{
			map[y][x] = IN;
			break;
		}
	}
	while(1)//generate the exit
	{
		int x = rand() % _w;
		int y = rand() % _h;
		if (map[y][x] == EMPTY)
		{
			map[y][x] = OUT;
			break;
		}
	}
	return;
}
