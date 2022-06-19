#include <bits/stdc++.h>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <array>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <climits>

struct Point {
    int x, y;
    Point() : Point(0, 0) {}
    Point(float x, float y) : x(x), y(y) {}
	bool operator==(const Point& rhs) const {
		return x == rhs.x && y == rhs.y;
	}
	bool operator!=(const Point& rhs) const {
		return !operator==(rhs);
	}
	Point operator+(const Point& rhs) const {
		return Point(x + rhs.x, y + rhs.y);
	}
	Point operator-(const Point& rhs) const {
		return Point(x - rhs.x, y - rhs.y);
	}
};

int root_player;  //player
const int size = 15;
std::array<std::array<int, size>, size> root_board;  //SIZE, size root_board, board
//std::vector<Point> root_next_valid_spots;
int minimax_depth=2;  //!!

class node {  //GomokuBoard
public:
    enum SPOT_STATE {
        EMPTY = 0,
        BLACK = 1,
        WHITE = 2
    };
    static const int SIZE = 15;
    //new
    const std::array<Point, 8> directions{{  //maybe only need last 4
        Point(-1, -1), Point(-1, 0), Point(-1, 1),
        Point(0, -1), /*{0, 0}, */Point(0, 1),
        Point(1, -1), Point(1, 0), Point(1, 1)
    }};
    //
    std::array<std::array<int, SIZE>, SIZE> board;
    int empty_count;
    std::vector<Point> next_valid_spots;  //new
    int cur_player;
    bool done;
    int winner;
    //new
    node(std::array<std::array<int, SIZE>, SIZE> board){
        int empty=0;
        for(int i=0;i<15;i++){
            for(int j=0;j<15;j++){
                this->board[i][j]=board[i][j];
                if(board[i][j]==EMPTY){
                	empty++;
                	//next_valid_spots.push_back(Point(i,j));  //
				}
            }
        }
        cur_player=root_player;
        done=false;
        winner=-1;
        next_valid_spots=get_valid_spots();  //!!new
        empty_count=empty;
    }

    node(const node& cmp){
        for(int i=0;i<SIZE;i++){  //SIZE, 15
            for(int j=0;j<SIZE;j++){  //
                board[i][j]=cmp.board[i][j];
            }
        }
        for(int i=0;i<cmp.next_valid_spots.size();i++){  //auto
            next_valid_spots.push_back(cmp.next_valid_spots[i]);
        }
        empty_count=cmp.empty_count;
        cur_player=cmp.cur_player;
        done=cmp.done;
        winner=cmp.winner;
    }
    //new_end
//private:
    int get_next_player(int player) const {
        return 3 - player;
    }
    bool is_spot_on_board(Point p) const {
        return 0 <= p.x && p.x < SIZE && 0 <= p.y && p.y < SIZE;
    }
    int get_disc(Point p) const {
        return board[p.x][p.y];
    }
    void set_disc(Point p, int disc) {
        board[p.x][p.y] = disc;
    }
    bool is_disc_at(Point p, int disc) const {
        if (!is_spot_on_board(p))
            return false;
        if (get_disc(p) != disc)
            return false;
        return true;
    }
    bool is_spot_valid(Point center) const {  //
        if (get_disc(center) != EMPTY)
            return false;
        return true;
    }
    
//public:
    //new
    std::vector<Point> get_valid_spots() const {
        std::vector<Point> valid_spots;
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                Point p = Point(i, j);
                if (board[i][j] != EMPTY)
                    continue;
                if (is_spot_valid(p))  //can be deleted
            		valid_spots.push_back(p);
            }
        }
        return valid_spots;
    }
    //
    bool put_disc(Point p) {
        if(!is_spot_valid(p)) {  //!!
            winner = get_next_player(cur_player);
            done = true;
            return false;
        }
        set_disc(p, cur_player);
        empty_count--;
        // Check Win
        if (checkwin(cur_player)) {
            done = true;
            winner = cur_player;
        }
        if (empty_count == 0) {
            done = true;
            winner = EMPTY;
        }
        // Give control to the other player.
        //cur_player = get_next_player(cur_player);     //!!!!!
        next_valid_spots = get_valid_spots();  //new
        return true;
    }
    bool checkwin(int disc){
        for (int i = 0; i < SIZE; i++) {
            for (int j = 0; j < SIZE; j++) {
                if (is_disc_at(Point(i, j), disc)){
                    bool iswin = true;
                    if (i + 4 < SIZE) {
                        for(int k = 0; k < 5; k++)
                            if (!is_disc_at(Point(i+k, j), disc)) {
                                iswin = false;
                                break;
                            }
                        if (iswin) return true;
                    }
                    iswin = true;
                    if (j + 4 < SIZE) {
                        for(int k = 0; k < 5; k++)
                            if (!is_disc_at(Point(i, j+k), disc)) {
                                iswin = false;
                                break;
                            }
                        if (iswin) return true;
                    }
                    iswin = true;
                    if (i + 4 < SIZE && j + 4 < SIZE) {
                        for(int k = 0; k < 5; k++)
                            if (!is_disc_at(Point(i+k, j+k), disc)) {
                                iswin = false;
                                break;
                            }
                        if (iswin) return true;
                    }
                    iswin = true;
                    if (i - 4 >= 0 && j + 4 < SIZE) {
                        for(int k = 0; k < 5; k++)
                            if (!is_disc_at(Point(i-k, j+k), disc)) {
                                iswin = false;
                                break;
                            }
                        if (iswin) return true;
                    }
                }
            }
        }
        return false;
    }
    double evaluate(){
    	double state_value=0;
    	int next_player=get_next_player(cur_player);  //opponent
        for(int i=0;i<SIZE;i++)
        {
        	for(int j=0;j<SIZE;j++)
        	{
        		if (is_disc_at(Point(i, j), cur_player))
        		{
        			//0
        			if(!is_disc_at(Point(i-1, j), cur_player))
        			{
        				int len=0;
        				while(is_disc_at(Point(i+len, j), cur_player))
        				{
        					len++;
						}
						if(is_disc_at(Point(i-1, j), EMPTY) && is_disc_at(Point(i+len, j), EMPTY))  //double empty
						{
							if(len==1)
								state_value+=5;
							else if(len==2)
								state_value+=100;
							else if(len==3)
								state_value+=50000;
							else if(len==4)
								state_value+=1000000;
							else if(len==5)
								state_value+=5000000;
						}
						else //if(is_disc_at(Point(i-1, j), next_player) || is_disc_at(Point(i+len, j), next_player) || (!is_spot_on_board(Point(i-1, j))) || (!is_spot_on_board(Point(i+len, j))))
						//one dead
						{
							int cnt=0;
							if(is_disc_at(Point(i-1, j), next_player)) cnt++;
							if(is_disc_at(Point(i+len, j), next_player)) cnt++;
							if(!is_spot_on_board(Point(i-1, j))) cnt++;
							if(!is_spot_on_board(Point(i+len, j))) cnt++;
							if(cnt<=1)
							{
								if(len==1)
									state_value+=0;  //1
								else if(len==2)
									state_value+=10;
								else if(len==3)
									state_value+=100;
								else if(len==4)
									state_value+=100;
								else if(len==5)
									state_value+=0;  //100
							}
						}
					}
					//1
					if(!is_disc_at(Point(i, j-1), cur_player))
					{
        				int len=0;
        				while(is_disc_at(Point(i, j+len), cur_player))
        				{
        					len++;
						}
						if(is_disc_at(Point(i, j-1), EMPTY) && is_disc_at(Point(i, j+len), EMPTY))  //double empty
						{
							if(len==1)
								state_value+=5;
							else if(len==2)
								state_value+=100;
							else if(len==3)
								state_value+=50000;
							else if(len==4)
								state_value+=1000000;
							else if(len==5)
								state_value+=5000000;
						}
						else //if(is_disc_at(Point(i, j-1), next_player) || is_disc_at(Point(i, j+len), next_player) || (!is_spot_on_board(Point(i, j-1))) || (!is_spot_on_board(Point(i, j+len))))
						//one dead
						{
							int cnt=0;
							if(is_disc_at(Point(i, j-1), next_player)) cnt++;
							if(is_disc_at(Point(i, j+len), next_player)) cnt++;
							if(!is_spot_on_board(Point(i, j-1))) cnt++;
							if(!is_spot_on_board(Point(i, j+len))) cnt++;
							if(cnt<=1)
							{
								if(len==1)
									state_value+=0;  //1
								else if(len==2)
									state_value+=10;
								else if(len==3)
									state_value+=100;
								else if(len==4)
									state_value+=100;
								else if(len==5)
									state_value+=0;  //100
							}
						}
					}
					//2
					if(!is_disc_at(Point(i-1, j-1), cur_player))
					{
        				int len=0;
        				while(is_disc_at(Point(i+len, j+len), cur_player))
        				{
        					len++;
						}
						if(is_disc_at(Point(i-1, j-1), EMPTY) && is_disc_at(Point(i+len, j+len), EMPTY))  //double empty
						{
							if(len==1)
								state_value+=5;
							else if(len==2)
								state_value+=100;
							else if(len==3)
								state_value+=50000;
							else if(len==4)
								state_value+=1000000;
							else if(len==5)
								state_value+=5000000;
						}
						else //if(is_disc_at(Point(i-1, j-1), next_player) || is_disc_at(Point(i+len, j+len), next_player) || (!is_spot_on_board(Point(i-1, j-1))) || (!is_spot_on_board(Point(i+len, j+len))))
						//one dead
						{
							int cnt=0;
							if(is_disc_at(Point(i-1, j-1), next_player)) cnt++;
							if(is_disc_at(Point(i+len, j+len), next_player)) cnt++;
							if(!is_spot_on_board(Point(i-1, j-1))) cnt++;
							if(!is_spot_on_board(Point(i+len, j+len))) cnt++;
							if(cnt<=1)
							{
								if(len==1)
									state_value+=0;  //1
								else if(len==2)
									state_value+=10;
								else if(len==3)
									state_value+=100;
								else if(len==4)
									state_value+=100;
								else if(len==5)
									state_value+=0;  //100
							}
						}
					}
					//3
					if(!is_disc_at(Point(i-1, j+1), cur_player))
					{
        				int len=0;
        				while(is_disc_at(Point(i+len, j-len), cur_player))
        				{
        					len++;
						}
						if(is_disc_at(Point(i-1, j+1), EMPTY) && is_disc_at(Point(i+len, j-len), EMPTY))  //double empty
						{
							if(len==1)
								state_value+=5;
							else if(len==2)
								state_value+=100;
							else if(len==3)
								state_value+=50000;
							else if(len==4)
								state_value+=1000000;
							else if(len==5)
								state_value+=5000000;
						}
						else //if(is_disc_at(Point(i-1, j+1), next_player) || is_disc_at(Point(i+len, j-len), next_player) || (!is_spot_on_board(Point(i-1, j+1))) || (!is_spot_on_board(Point(i+len, j-len))))
						//one dead
						{
							int cnt=0;
							if(is_disc_at(Point(i-1, j+1), next_player)) cnt++;
							if(is_disc_at(Point(i+len, j-len), next_player)) cnt++;
							if(!is_spot_on_board(Point(i-1, j+1))) cnt++;
							if(!is_spot_on_board(Point(i+len, j-len))) cnt++;
							if(cnt<=1)
							{
								if(len==1)
									state_value+=0;  //1
								else if(len==2)
									state_value+=10;
								else if(len==3)
									state_value+=100;
								else if(len==4)
									state_value+=100;
								else if(len==5)
									state_value+=0;  //100
							}
						}
					}
				}
				else if(is_disc_at(Point(i, j), next_player))
				{
					//00
        			if(!is_disc_at(Point(i-1, j), next_player))
        			{
        				int len=0;
        				while(is_disc_at(Point(i+len, j), next_player))
        				{
        					len++;
						}
						if(is_disc_at(Point(i-1, j), EMPTY) && is_disc_at(Point(i+len, j), EMPTY))  //double empty
						{
							if(len==1)
								state_value-=0;
							else if(len==2)
								state_value-=120;
							else if(len==3)
								state_value-=100000;
							else if(len==4)
								state_value-=5000000;
							else if(len==5)
								state_value-=5000000;
						}
						else //if(is_disc_at(Point(i-1, j), cur_player) || is_disc_at(Point(i+len, j), cur_player) || (!is_spot_on_board(Point(i-1, j))) || (!is_spot_on_board(Point(i+len, j))))
						//one dead
						{
							int cnt=0;
							if(is_disc_at(Point(i-1, j), cur_player)) cnt++;
							if(is_disc_at(Point(i+len, j), cur_player)) cnt++;
							if(!is_spot_on_board(Point(i-1, j))) cnt++;
							if(!is_spot_on_board(Point(i+len, j))) cnt++;
							if(cnt<=1)
							{
								if(len==1)
									state_value-=0;
								else if(len==2)
									state_value-=50;
								else if(len==3)
									state_value-=1000;
								else if(len==4)
									state_value-=5000000;
								else if(len==5)
									state_value-=0;
							}
						}
					}
					//11
					if(!is_disc_at(Point(i, j-1), next_player))
					{
        				int len=0;
        				while(is_disc_at(Point(i, j+len), next_player))
        				{
        					len++;
						}
						if(is_disc_at(Point(i, j-1), EMPTY) && is_disc_at(Point(i, j+len), EMPTY))  //double empty
						{
							if(len==1)
								state_value-=0;
							else if(len==2)
								state_value-=120;
							else if(len==3)
								state_value-=100000;
							else if(len==4)
								state_value-=5000000;
							else if(len==5)
								state_value-=5000000;
						}
						else //if(is_disc_at(Point(i, j-1), cur_player) || is_disc_at(Point(i, j+len), cur_player) || (!is_spot_on_board(Point(i, j-1))) || (!is_spot_on_board(Point(i, j+len))))
						//one dead
						{
							int cnt=0;
							if(is_disc_at(Point(i, j-1), cur_player)) cnt++;
							if(is_disc_at(Point(i, j+len), cur_player)) cnt++;
							if(!is_spot_on_board(Point(i, j-1))) cnt++;
							if(!is_spot_on_board(Point(i, j+len))) cnt++;
							if(cnt<=1)
							{
								if(len==1)
									state_value-=0;
								else if(len==2)
									state_value-=50;
								else if(len==3)
									state_value-=1000;
								else if(len==4)
									state_value-=5000000;
								else if(len==5)
									state_value-=0;
							}
						}
					}
					//22
					if(!is_disc_at(Point(i-1, j-1), next_player))
					{
        				int len=0;
        				while(is_disc_at(Point(i+len, j+len), next_player))
        				{
        					len++;
						}
						if(is_disc_at(Point(i-1, j-1), EMPTY) && is_disc_at(Point(i+len, j+len), EMPTY))  //double empty
						{
							if(len==1)
								state_value-=0;
							else if(len==2)
								state_value-=120;
							else if(len==3)
								state_value-=100000;
							else if(len==4)
								state_value-=5000000;
							else if(len==5)
								state_value-=5000000;
						}
						else //if(is_disc_at(Point(i-1, j-1), cur_player) || is_disc_at(Point(i+len, j+len), cur_player) || (!is_spot_on_board(Point(i-1, j-1))) || (!is_spot_on_board(Point(i+len, j+len))))
						//one dead
						{
							int cnt=0;
							if(is_disc_at(Point(i-1, j-1), cur_player)) cnt++;
							if(is_disc_at(Point(i+len, j+len), cur_player)) cnt++;
							if(!is_spot_on_board(Point(i-1, j-1))) cnt++;
							if(!is_spot_on_board(Point(i+len, j+len))) cnt++;
							if(cnt<=1)
							{
								if(len==1)
									state_value-=0;
								else if(len==2)
									state_value-=50;
								else if(len==3)
									state_value-=1000;
								else if(len==4)
									state_value-=5000000;
								else if(len==5)
									state_value-=0;
							}
						}
					}
					//33
					if(!is_disc_at(Point(i-1, j+1), next_player))
					{
        				int len=0;
        				while(is_disc_at(Point(i+len, j-len), next_player))
        				{
        					len++;
						}
						if(is_disc_at(Point(i-1, j+1), EMPTY) && is_disc_at(Point(i+len, j-len), EMPTY))  //double empty
						{
							if(len==1)
								state_value-=0;
							else if(len==2)
								state_value-=120;
							else if(len==3)
								state_value-=100000;
							else if(len==4)
								state_value-=5000000;
							else if(len==5)
								state_value-=5000000;
						}
						else //if(is_disc_at(Point(i-1, j+1), cur_player) || is_disc_at(Point(i+len, j-len), cur_player) || (!is_spot_on_board(Point(i-1, j+1))) || (!is_spot_on_board(Point(i+len, j-len))))
						//one dead
						{
							int cnt=0;
							if(is_disc_at(Point(i-1, j+1), cur_player)) cnt++;
							if(is_disc_at(Point(i+len, j-len), cur_player)) cnt++;
							if(!is_spot_on_board(Point(i-1, j+1))) cnt++;
							if(!is_spot_on_board(Point(i+len, j-len))) cnt++;
							if(cnt<=1)
							{
								if(len==1)
									state_value-=0;
								else if(len==2)
									state_value-=50;
								else if(len==3)
									state_value-=1000;
								else if(len==4)
									state_value-=5000000;
								else if(len==5)
									state_value-=0;
							}
						}
					}
				}
			}
		}
        return state_value;
	}
};

void read_board(std::ifstream& fin) {
    fin >> root_player;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            fin >> root_board[i][j];
        }
    }
}

int minimax(node cur,int depth,int alpha,int beta,bool maximize,std::ofstream& fout){     
    if(depth==0){
        return cur.evaluate();
    }
    if(cur.done){
        if(cur.winner==root_player)
        return 50000000;
        else
        return -50000000;
    }
    if(maximize){
        int max_num=INT_MIN;
        for(int i=0;i<cur.next_valid_spots.size();i++){   
            node next_state=cur;
            next_state.put_disc(cur.next_valid_spots[i]);
            int value=minimax(next_state,depth-1,alpha,beta,false,fout);
            alpha=std::max(alpha,value);
            if(value>max_num){
                max_num=value;
                //if(alpha>=beta)
                //break;
                if(depth==minimax_depth){
                    fout << cur.next_valid_spots[i].x << " " << cur.next_valid_spots[i].y << std::endl;  //final decision
                    fout.flush();
                }
            }
            if(alpha>=beta)
                break;
        }
        return max_num;
    }
    else{
        int min_num=INT_MAX;
        for(int i=0;i<cur.next_valid_spots.size();i++){   
            node next_state=cur;
            next_state.put_disc(cur.next_valid_spots[i]);
            int value=minimax(next_state,depth-1,alpha,beta,true,fout);
            beta=std::min(beta,value);
            min_num=std::min(min_num,value);
            if(alpha>=beta)
                break;
        }
        return min_num;
    }
}

int main(int, char** argv) {
    std::ifstream fin(argv[1]);
    std::ofstream fout(argv[2]);
    read_board(fin);

    //write_valid_spot(fout);
    node cur_state(root_board);
    //if(cur_state.board[7][7]!=0)
    if(cur_state.empty_count<=223)
    	minimax(cur_state,minimax_depth,INT_MIN,INT_MAX,true,fout);
    else if(cur_state.empty_count==225)
    {
    	cur_state.put_disc(Point(7,7));
    	fout << 7 << " " << 7 << std::endl;  //final decision
        fout.flush();
	}
	else if(cur_state.board[7][7]!=0)
	{
		cur_state.put_disc(Point(6,6));
    	fout << 6 << " " << 6 << std::endl;  //final decision
        fout.flush();
	}
	else
	{
		cur_state.put_disc(Point(7,7));
    	fout << 7 << " " << 7 << std::endl;  //final decision
        fout.flush();
	}
    
    fin.close();
    fout.close();
    return 0;
}
