#include <iostream>
#include <vector>
#include <ctime>
#include <map>
#include <cassert>
#include <utility>
#include <fstream>
#include <windows.h>
#include <unistd.h>
#include <string>
#include <algorithm>
#include <bits/stdc++.h>

using namespace std;
/*
    *- CHORES -*
        Mateo:
            //- Map Gen  - Mostly done just requires it to gen spicy tiles
            - sprite design for tiles / characters
            - Tiles - Basic class is done just need interaction functions
            - Python visuals
            - Entity tracking

        Noah:
            //- Python
            - sprite design for tiles / characters
            - Game Mechanics
            - Shop
            - Menu
            //- enable comms between python and c++ program(s)
*/
/*
    INFO
        MOB INFO
            DETECTION
                1st range detection:
                    visible detection range
                    mobs will run at player
                2nd range detection
                    audio detection
                    double agitation value 
                        starts at rand()%50+50
                        decreases by player value when in 2nd range
                            when hits 0, mob walks towards player
                            runs when in 1st range
            ATTACK
                WEAPON (not special)
                    fist
                        base weapon
                        10 hits to death
                    stick
                        lvl 1
                        8 hits to death
    
        PLAYER INFO
            STATS
                base
                    speed=1
                    vis=5
                    (double)noise=10 (aka agi (agitation))
                    inv. cap=5
                    health=30
                upgrading
                    speed++
                    vis++
                    noise-=(noise*.1)
                    inv. cap++
                maxes/mins
                    speed=5
                    vis=15
                    noise=no limit
                    inv. cap=10
            
                
*/

/*  IDEAS
player var "luck"
mob "levels"

species spesific mob attacks
    mobs can only use EITHER normal or special
    species spesific is not guarenteed / only some of each species have it

throwing weapon causes greater stun, but mob has 50% chance to pick it up IF damage>.
1 currency ~= 1 cent

all "idols" are actually keys. all keys are needed to get out of the "dungeon". once "exit door" is opened, cutscene at nice scenery, similar to portal 2 ending
*/

//send keypresses through keyboard-stream
void press(std::string in){
  INPUT letter[in.length()*2]={};
  ZeroMemory(letter,sizeof(letter));
  for(int a=0;a<in.length();a++){
    letter[a].type=INPUT_KEYBOARD;
    letter[a].ki.wVk=VkKeyScanEx(in[a],GetKeyboardLayout(0));
  } 
  for(int a=in.length();a<in.length()*2;a++){
    letter[a].type=INPUT_KEYBOARD;
    letter[a].ki.wVk=VkKeyScanEx(in[a],GetKeyboardLayout(0));
    letter[a].ki.dwFlags=KEYEVENTF_KEYUP;
  }
  UINT sendLetterUp=SendInput(ARRAYSIZE(letter),letter,sizeof(INPUT));
}

class Tile {
  private:
    bool collision;
    /*
    0 = non interactable
    1 = interactable
    2 = bouncy
    3 = trap
    4 = slow
    5 = safety transit
    6 = palyer
    7 = monster
    8 = display
    */
    int type;
    string name;
  public:
    Tile () {
      collision = false;
      type = 0;
      name = "Not defined defualt construction";
    }
    Tile (bool c, int t, string n) {
      collision = c;
      type = t;
      name = n;
    }
    bool col () {
      return collision;
    }
    int typ () {
      return type;
    }
    string print () {
      return name;
    }
    void interact (Tile replace) {
      collision = replace.col();
      type = replace.typ();
      name = replace.print();
    }
};

map<char,Tile> tilemap () {
  //don't it worked just fine and that breaks maps - thanks - I'm working on mob path finding
  map<char, Tile> tileMap;
  tileMap['w'] = Tile(true, 0, "wall");
  tileMap['d'] = Tile(false, 1, "open_door");
  tileMap['D'] = Tile(true, 1, "closed_door");
  tileMap['f'] = Tile(false, 0, "floor");
  tileMap['l'] = Tile(true, 1, "switchable_light");
  tileMap['g'] = Tile(false, 4, "slow_tile");
  tileMap['T'] = Tile(true, 3, "wall_trap");
  tileMap['@'] = Tile(false, 3, "floor_trap");
  tileMap['D'] = Tile(true, 1, "closed_door");
  tileMap['t'] = Tile(true, 1, "treasure");
  tileMap['S'] = Tile(false, 5, "safety");
  tileMap['P'] = Tile(true, 6, "player_character");
  tileMap['M'] = Tile(true, 7, "mob_entity");
  return tileMap;
}

class Item{
  private:
    string itemName;
    double spawnRate;
    int value;

  public:
    Item(){}

    Item(double rate,string name,int itemValue){
      spawnRate=rate;
      itemName=name;
      value=itemValue;
    }
    
    string getItemName(){
      return itemName;
    }
    double getSpawnRate(){
      return spawnRate;
    }
    int getValue(){
      return value;
    }

    //for chest::open
    void eraseData(){
      itemName="";
      spawnRate=0;
      value=0;
    }
};

const Item rockItem=Item(.2,"Rock",5);
const Item smallGold=Item(.1,"Small Gold Bag",25);
const Item medGold=Item(.075,"Medium Gold Bag",50);
const Item largeGold=Item(.05,"Large Gold Bag",100);
//create all items, put in vector
const vector<Item> allItems{rockItem,smallGold,medGold,largeGold};

Item randomItem(){
    srand(time(0));
    Item ret=allItems[rand()%allItems.size()];
//choose random item, return to chest::randomitem
    return ret;
}

class Weapon{
  private:
    string name="";
    int damage=0;
    int level=0;

    string modifiers="";

  public:
    //create spesific weapon
    Weapon(string inName,int inDamage,int inLevel,string inMods="None"){
      name=inName;
      damage=inDamage;
      level=inLevel;
      modifiers=inMods;
    }

    string getName(){
      return name;
    }
    int getDamage(){
      return damage;
    }
    int getLevel(){
      return level;
    }
};

//type  name        name, dph, level, modifier
Weapon Fists=Weapon("Unarmed",1,1);
Weapon Stick=Weapon("Stick",2,2);
Weapon SharpStick=Weapon("Sharp Stick",3,3);
Weapon DullBroadsword=Weapon("Dull Broadsword",5,4);

vector<Weapon> weapons{Fists,Stick,SharpStick,DullBroadsword};

class Chest{
  private:
    Item contents;
    bool open=0;
  
  public:
    Chest(){
      contents=randomItem();
    }

    //user can decline item if inv full or on request
    Item openChest(){
      return contents;
    }
    void confirmOpen(){
      contents.eraseData();
      open=1;
    }
};

class Player{
  private:
    //stats
    int x,y;
    double agi; // noise for 2nd range detection
    double evasion;
    //inventory
    int invCap=5;
    vector<Item> inv;
    int currency;
    int numIdols=0;
  public:
    //constructs default player
    Player(){

      agi=10;
      evasion=30;
    }

    //run after currencyconvert or a purchace (or before, whatever is cool with you :face_with_raised_eyebrow: )
    void calcCurrency(){
      currency=0;
      for(int a=0;a<inv.size();a++)
        currency+=inv[a].getValue();
    }
    int currentCurrency(){
      calcCurrency();
      return currency;
    }

    //when stats mods are purchased
    void upgradeAgi(){
      if(currency>/*required*/0){
          agi-=(agi*.1);
          cout<<"Agitation decreased. Agitation is now "<<to_string(agi)<<endl;
      }else cout<<"Not enough currency"<<endl;
    }

    void pickUpItem(Item item){
      if(inv.size()<invCap){
          inv.push_back(item);
          cout<<"Item picked up. You have "<<inv.size()<<" items."<<endl;
          cout<<"Maximum is "<<invCap<<endl;
      } else{
          cout<<"ur out of space u greedy bitch"<<endl;
      }
    }
    //return position
    int xPos () {
      return x;
    }
    int yPos () {
      return y;
    }
    //position changing
    void cPos (int X, int Y) {
      x+=X;
      y+=Y;
    }
};
//needed for path finding it uses a DFS search
//gonna change it a bit
/*class Graph {
public:
    map<int, bool> visited;
    map<int, list<int> > adj;
 
    // function to add an edge to graph
    void addEdge(int v, int w);
 
    // DFS traversal of the vertices
    // reachable from v
    void DFS(int v);
};
void Graph::addEdge(int v, int w) {
    adj[v].push_back(w); // Add w to v’s list.
}
 
void Graph::DFS(int v) {
    // Mark the current node as visited and
    // print it
    visited[v] = true;
    cout << v << " ";
 
    // Recur for all the vertices adjacent
    // to this vertex
    list<int>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i)
        if (!visited[*i])
            DFS(*i);
}
*/
class Path {
  private:
    //the first one is the y cordinate and the second one is the x cordinate
    /*
    *Directions*
    0 is up
    1 is left
    2 is down
    3 is right
    */
    vector<pair<int, int>> path;
    //first one is y second one is x
    pair<int, int> dest;
    int range;
  public:
    Path (int range) {
      path.push_back({0, 0});
      dest = {rand()%range, rand()%range};
    }
    //sets the coordinates
    void set (int one, int two) {
      dest = {one, two};
    }
    //roams randomly
    void roam () {
      set(rand()%range,rand()%range);
    }
    //finds where an object is and then sets the coordinates to that
    void find (char object, vector<vector<char>> map) {
      for (int y = 0; y<map.size(); y++) {
        for (int x = 0; x<map[y].size(); x++) {
          if (map[y][x] == object) {
            set(y,x);
            break;
          }
        }
      }
    }
    /*bool used (pair<int, int> d) {
      for (int i = 0; i<path.size(); i++) {
        if (path[i] == d) {
          cout<<"\ntrue\n";
          return true;
          }
      }
      return false;
    }*/
    //determines if it is possible to get to the object and if possible creates a path to get there;
    bool go (char object, vector<vector<char>> map, int X, int Y) {
      find (object, map);
      //setting the traversable map for the graph
      std::map<char,Tile> tileMap = tilemap();
      vector<vector<bool>> solidMap;
      for (int y = 0; y<map.size(); y++) {
        vector<bool> temp;
        for (int x = 0; x<map[y].size(); x++) {
          temp.clear();
          temp.push_back(tileMap[map[y][x]].col());
        }
        solidMap.push_back(temp);
      }
      //sets up a  temporary variable
     /* vector<vector<bool>> plaus;
      //defines the variable
      for (int y = 0; y<map.size(); y++) {
        vector<bool> temp;
        for (int x = 0; x<map[y].size(); x++) {
          temp.clear();
          temp.push_back(false);
        }
        plaus.push_back(temp);
      }
      plaus[Y][X] = true;*/
      //checks if it is possible with infectious finding
      /*for (int i = 0; i<map.size()/2; i++){
        for (int y = 0; y<map.size(); y++) {
        for (int x = 0; x<map[y].size(); x++) {
         if (solidMap[y][x] == true) {
            if (y - 1 != -1 && !tileMap[map[y-1][x]].col())
              plaus[y - 1][x] = true;
            if (x - 1 != -1 && !tileMap[map[y][x-1]].col()) 
              plaus[y][x - 1] = true;
            if (y + 1 != map.size() && !tileMap[map[y+1][x]].col())
              plaus[y + 1][x] = true;
            if (x + 1 != map.size() && !tileMap[map[y][x+1]].col())
              plaus[y][x + 1] = true;
          }
        }
      }
        }*/
     // if (plaus[dest.first][dest.second]) {
        //pathfinding code goes here
        bool possible = true;
        bool plause[100][100] = {0};
        plause[Y][X] = true;
        //starts shirnking the search zone
        int z = map.size()/2; //Minimun bounds for pathfinding
        for (int p = map.size()/2; possible == true && z>=1; z--) {
          cout<<z<<"  ";
        //clears the variable
        //fill(begin(plause), end(plause), false);
        for (int I = 0; I<100; I++) {
          for (int J = 0; J<100; J++) {
            plause[I][J] = false;
          }
        }
        plause[Y][X] = true;
        //redefines the variable
        for (int i = 0; i<z; i++){
        for (int y = 0; y<map.size(); y++) {
        for (int x = 0; x<map[y].size(); x++) {
          if (plause[y][x] == true) {
            if (y - 1 != -1 && !tileMap[map[y-1][x]].col())
              plause[y - 1][x] = true;
            if (x - 1 != -1 && !tileMap[map[y][x-1]].col()) 
              plause[y][x - 1] = true;
            if (y + 1 != map.size() && !tileMap[map[y+1][x]].col())
              plause[y + 1][x] = true;
            if (x + 1 != map.size() && !tileMap[map[y][x+1]].col())
              plause[y][x + 1] = true;
          }
        }
      }
      }
          cout<<plause[dest.first][dest.second]<<endl;
          possible = plause[dest.first][dest.second];
          }
      if (!(z>=(map.size()/2)-1)) {
      //boundries variable
      //fill(begin(plause), end(plause), false);
      for (int I = 0; I<100; I++) {
          for (int J = 0; J<100; J++) {
            plause[I][J] = false;
          }
        }
      bool place[100][100] = {0};
      plause[Y][X] = true;
      place[Y][X] = true;
      //using the logic from this video - https://www.youtube.com/watch?v=-L-WgKMFuhE
      //first 2 are 2 and y and the last three go as follows 0 = Gcost (dsitance from start) 1 = Hcost (distance from end node) 2 = Fcost (H+G)
      double costs[100][100][3];
      costs[Y][X][0] = 0;
      costs[Y][X][1] = 0;
      //generates boundries for the pathfinding based on the previous z
      for (int i = 0; i<z+1; i++){
        for (int y = 0; y<map.size(); y++) {
        for (int x = 0; x<map[y].size(); x++) {
          if (plause[y][x] == true) {
            if (y - 1 != -1 && !tileMap[map[y-1][x]].col())
              plause[y - 1][x] = true;
            if (x - 1 != -1 && !tileMap[map[y][x-1]].col()) 
              plause[y][x - 1] = true;
            if (y + 1 != map.size() && !tileMap[map[y+1][x]].col())
              plause[y + 1][x] = true;
            if (x + 1 != map.size() && !tileMap[map[y][x+1]].col())
              plause[y][x + 1] = true;
          }
        }
      }
      }
      //gives everything a cost of possition
      for (int y = 0; y<map.size(); y++) {
        for (int x = 0; x<map[y].size(); x++) {
          if (plause[y][x]) {
            if (!(y == Y && x==X)) {
              costs[y][x][0] = sqrt(((x-X)*(x-X))+((y-Y)*(y-Y))); //straight line it's just easier for me
              costs[y][x][1] = sqrt(((x-dest.second)*(x-dest.second))+((y-dest.first)*(y-dest.first))); 
              costs[y][x][2] = costs[y][x][0]+costs[y][x][1];
              cout<<(int)costs[y][x][2]<<" ";
            }
            else {
              costs[y][x][0] = 0;
              costs[y][x][1] = 0;
              costs[y][x][2] = 0;
              cout<<costs[y][x][2]<<" ";
            }
          }
          else cout<<"--";
          /*costs[y][x][0] = 2000;
          costs[y][x][1] = 2000;
          costs[y][x][2] = 2000;*/
          //plause[y][x] = false;
        }
        cout<<endl;
      }
      //reusing a variable for better storage
      plause[Y][X] = true;
      //documents the path with every shortest node
      path.push_back({Y, X});
      bool kill = false;
      bool traversed[100][100] = {0};
      traversed[Y][X] = true;
      place[Y][X] = true;
      pair<int, int> current = {Y,X};
      cout<<"finding route\n"<<endl;
      //path.push_back({10, 1});
      //path.push_back({9, 8});
      for (int i = 0; (i<(z+1)&&!kill)/*||path[path.size()-1]!=dest*/; i++) {
        for (int y = 0; y<map.size()&&!kill; y++) {
          for (int x = 0; x<map[y].size()&&!kill; x++) {
            //if (plause[y][x]&&place[y][x]) {
              //oh fuck I broke it
              /*for (int j = 0; j<path.size(); j++) traversed[path[j].first][path[j].second] = true; 
              //cout<<y<<"  "<<x<<endl;
              int temp = 100;
              bool found = false;
              vector<pair<int, int>> poss;
              //first checks if it can get to the end because if so then it will
              if (y-1 == dest.first && x == dest.second) {
                path.push_back({y-1,x});
                cout<<"found"<<endl;
                kill = true;
              }
              if (y+1 == dest.first && x == dest.second) {
                cout<<"found"<<endl;
                path.push_back({y+1,x});
                kill = true;
              }
              if (y == dest.first && x+1 == dest.second) {
                cout<<"found"<<endl;
                path.push_back({y,x+1});
                kill = true;
              }
              if (y == dest.first && x-1 == dest.second) {
                cout<<"found"<<endl;
                path.push_back({y,x-1});
                kill = true;
              }
              //if it can't reach the end then keep trying
              if (costs[y-1][x][2] < temp && y!=0 && plause[y-1][x] && !traversed[y-1][x])
              {
                traversed[y-1][x] = true;
                temp = costs[y-1][x][2];
                place[y-1][x] = true;
                poss.push_back({y-1, x});
                found = true;
                }
              if (costs[y+1][x][2] < temp && y+1!=map.size() && plause[y+1][x] && !traversed[y+1][x]) 
              {
                traversed[y+1][x] = true;
                temp = costs[y+1][x][2];
                place[y+1][x] = true;
                poss.push_back({y+1, x});
                found = true;
                }
              if (costs[y][x-1][2] < temp && x!=0&& plause[y][x-1] && !traversed[y][x-1]) 
              {
                traversed[y][x-1] = true;
                place[y][x-1] = true;
                temp = costs[y][x-1][2];
                poss.push_back({y, x-1});
                found = true;
                  }
              if (costs[y][x+1][2] < temp && x+1 !=map[y].size() && plause[y][x+1] && !traversed[y][x+1]) 
              {
                traversed[y][x+1] = true;
                temp = costs[y][x+1][2];
                place[y][x+1] = true;
                poss.push_back({y, x+1});
                found = true;
                }
              //all of them have an increase in size
              if (!found) {
                if (y!=0) poss.push_back({y-1,x});
                if (y+1 != map.size()) poss.push_back({y+1, x});
                if (x!=0) poss.push_back({y, x-1});
                if (x!=map[y].size()) poss.push_back({y, x+1});
                for (int I = poss.size()-1; i>=0; i--) {
                  int TEMP = costs[poss[0].first][poss[0].second][1];
                  if (costs[poss[I].first][poss[I].second][1]<TEMP && !traversed[poss[I].first][poss[I].second] && plause[poss[I].first][poss[I].second]) TEMP = costs[poss[I].first][poss[I].second][1];
                  else poss.erase(poss.begin()+I);
                }
                }
              //if all of them have the same value then go with the one with the least Hcost
              if (poss.size() > 1 && poss.size() > 0) {
                for (int I = poss.size()-1; i>=0; i--) {
                  int TEMP = costs[poss[0].first][poss[0].second][1];
                  if (costs[poss[I].first][poss[I].second][1]<TEMP && !traversed[poss[I].first][poss[I].second] && plause[poss[I].first][poss[I].second]) TEMP = costs[poss[I].first][poss[I].second][1];
                  else poss.erase(poss.begin()+I);
                //if all of them have a greater or equal value then the current node then go with the lowest Hcost
                }
              }
              //pushes back the best one and has that possition equal to true
              path.push_back(poss[0]);
              for (int j = 0; j<path.size(); j++) traversed[path[j].first][path[j].second] = true; 
              place[path[path.size()-1].first][path[path.size()-1].second] = true;
              if (poss[0] == dest) kill = true;
              place[y][x] = false;*/
              if ((y == current.first && x == current.second)&&plause[y][x]) {
                vector<pair<int, int>> pos;
                //bool triggered = false;
                //if it can imediatly get to the end then do so
                if (y!=0 && y-1 == dest.first && x == dest.second && plause[y-1][x]&&!traversed[y-1][x]) {
                  cout<<"\npath end found\n";
                  path.push_back({y-1,x});
                  //pos[0] = {y-1,x};
                  kill = true;
                }
                if (y!=map.size()-1 && y+1 == dest.first && x == dest.second && plause[y+1][x] &&!traversed[y+1][x]) {
                  cout<<"\npath end found\n";
                  path.push_back({y+1,x});
                  //pos[0] = {y+1,x};
                  kill = true;
                }
                if (x!=0 && y == dest.first && x-1 == dest.second && plause[y][x-1] &&!traversed[y][x+1]) {
                  cout<<"\npath end found\n";
                  path.push_back({y,x-1});
                  kill = true;
                  //pos[0] = {y,x-1};
                }
                if (x!=map.size()-1 && y == dest.first && x+1 == dest.second && plause[y][x+1]&&!traversed[y][x-1]) {
                  cout<<"\npath end found\n";
                  path.push_back({y,x+1});
                  kill = true;
                  //pos[0] = {y,x+1};
                }
                //if solution isn't imediate then continue solving
                if (y!=0) if (plause[y-1][x]&&!traversed[y-1][x]) pos.push_back({y-1,x});
                if (y!=map.size()-1) if (plause[y+1][x]&&!traversed[y+1][x]) pos.push_back({y+1,x});
                if (x!=0) if (plause[y][x-1]&&!traversed[y][x-1]) pos.push_back({y,x-1});
                if (x!=map[y].size()-1)if (plause[y][x+1]&&!traversed[y][x+1]) pos.push_back({y,x+1});
                /*if (pos.size()==0) {
                  triggered  = true;
                  if (y!=0) if (!tileMap[map[y-1][x]].col()&&plause[y-1][x]) pos.push_back({y-1,x});
                  if (y!=map.size()-1) if (!tileMap[map[y+1][x]].col()&&plause[y+1][x]) pos.push_back({y+1,x});
                  if (x!=0) if (!tileMap[map[y][x-1]].col()&&plause[y][x-1]) pos.push_back({y,x-1});
                  if (x!=map[y].size()-1)if (!tileMap[map[y][x+1]].col()&&plause[y][x+1]) pos.push_back({y,x+1});
                } */
                double max = 200;
                cout<<"current cost list"<<endl;
                for (int S = 0; S<pos.size(); S++) {
                    cout<<"y: "<<pos[S].first<<" x: "<<pos[S].second<<" cost: "<<costs[pos[S].first][pos[S].second][2]<<endl;
                  }
                for (int i = 0; i<2; i++) {
                  for (int o = 01; o<pos.size(); o++) {
                  if (costs[pos[o].first][pos[o].second][2] <= max) {
                    max = costs[pos[o].first][pos[o].second][2];
                    //cout<<costs[pos[o].first][pos[o].second][2]<<endl;
                  }
                  else pos.erase(pos.begin()+o);
                }
                }
                while (pos.size() >= 2) {
                  cout<<"current cost list"<<endl;
                  for (int S = 0; S<pos.size(); S++) {
                    cout<<"y: "<<pos[S].first<<" x: "<<pos[S].second<<" cost: "<<costs[pos[S].first][pos[S].second][1]<<endl;
                  }
                  if (costs[pos[0].first][pos[0].second][1] < costs[pos[1].first][pos[1].second][1]) {
                    pos.erase(pos.begin()+1);
                  } else pos.erase(pos.begin()+0);
                  }
                place[pos[0].first][pos[0].second] = true;
                traversed[pos[0].first][pos[0].second] = true;
                //if (triggered) plause[pos[0].first][pos[0].second] = false;
                cout<<"\nfinding next\n";
                if (pos.size()>0) path.push_back(pos[0]);
                current = pos[0];
              }
            }
          }
        //}
        
      }
      //removes all of the duplicates
      vector<pair<int, int>> temp;
      for (int i = 0; i<path.size(); i++) {
        bool found = false;
        for (int j = 0; j<temp.size(); j++) {
          if (temp[j]==path[i]) found = true;
        }
        if (!found) temp.push_back(path[i]);
      }
      path = temp;
      //prints out the whole path
      for (int i = 0; i<path.size(); i++) {
        cout<<"y: "<<path[i].first<<" x: "<<path[i].second<<endl;
      }
     // }
        }
      return z<(map.size()/2)-1;
      //return !(z>=(map.size()/2)-1);
}
};

class Mob{
  private:
    string species;
    int level;
    int firstRange;
    int secondRange;
    double agitation;

    Weapon specialAtk=Weapon("None",0,0);
    Weapon weapon=Weapon("None",0,0);

  public:
    Mob(){
      species="Lvl1 basic boi";
      int level=1;
      weapon=Fists;
    }
    Mob(int level,string inSpecies,bool special=0){
      species=inSpecies;
      if(!special){
        //pick weapoon from level, level-1, or level +1
      } else{
        //specialAtk=
      }
    }
};

//tell me what u think of this - gonna rewrite it

//moved tiles to the top needed them for paths
///use replit chat - ok

//map generation - complete for now don't touch
//change it to a do/while loop https://www.w3schools.com/cpp/cpp_do_while_loop.asp - no
vector<vector<char>> mapInt (int cycles, int size){
  //variables
  srand(time(0));
  bool solidMap[100][100]; //collision mapping
  char map[100][100]; //tile type mapping
  bool possible = false;
  int coords[2];
  int paths[100][100];
  /*
    w = wall (is solid)
    f = floor (no collision)
    l = light (is solid)
    g = grass (slow but no collision)
    t = treasure (collision, lootable)
    d = door (no collison)
    D = open door (is solid)
    S = next safety
    s = current safety
*/
  string rands = "wfflffllffgfftfdfdfff";
  //functions
  auto PTrack = [&]() {
    //variable clear
    for(int y = 0; y<size; y++)
      for (int x = 0; x<size; x++)
        paths[y][x] = 0;
    //variable redefinition
    for (int y = 0; y<size; y++)
      for (int x = 0; x<size; x++)
        paths[y][x] = 0;
    for (int y = 0; y<size; y++) {
      for (int x = 0; x<size; x++) {
        //p = y w = x
         if (map[y][x] == 'w' || map[y][x] == 't') {
            if (y - 1 != -1) {
              paths[y - 1][x]++;
            }
            if (y - 1 != -1 && x - 1 != -1) {
              paths[y - 1][x - 1]++;
            }
            if (x - 1 != -1) {
              paths[y][x - 1]++;
            }
            if (y + 1 != size) {
              paths[y + 1][x]++;
            }
            if (y + 1 != size && x + 1 != size) {
              paths[y + 1][x + 1]++;
            }
            if (x + 1 != size) {
              paths[y][x + 1]++;
            }
            if (y + 1 != size && x - 1 != -1) {
              paths[y + 1][x - 1]++;
            }
            if (y - 1 != -1 && x + 1 != size) {
              paths[y - 1][x + 1]++;
            }
          }
      }
    }
    };
  //end setter
  auto End = [&]() {
    coords[0] = size/2;
    coords[1] = size/2;
    map[coords[0]][coords[1]] = 'S';
    solidMap[coords[0]][coords[1]] = true;
  };
  //plausiblility checker
  auto Checker = [&] () {
    solidMap[coords[0]][coords[1]] = true;
    for (int i = 0; i<size; i++)
      for (int y = 0; y<size; y++) {
        for (int x = 0; x<size; x++) {
         if (solidMap[y][x] == true) {
            if (y - 1 != -1 && (map[y+1][x] != 'w' && map[y+1][x] != 'D' &&map[y+1][x] != 't'))
              solidMap[y - 1][x] = true;
            if (x - 1 != -1 && (map[y+1][x] != 'w' && map[y+1][x] != 'D' &&map[y+1][x] != 't')) 
              solidMap[y][x - 1] = true;
            if (y + 1 != size && (map[y+1][x] != 'w' && map[y+1][x] != 'D' &&map[y+1][x] != 't'))
              solidMap[y + 1][x] = true;
            if (x + 1 != size && (map[y+1][x] != 'w' && map[y+1][x] != 'D' &&map[y+1][x] != 't'))
              solidMap[y][x + 1] = true;
          }
        }
      }
  };
  //initial generation
  for (int y = 0; y<size; y++) {
    for (int x = 0; x<size; x++) {
      map[y][x] = rands[rand()%rands.length()];
      PTrack();
    }
    
  }
  End();
  for (int O = 0; O<cycles-1; O++)
    for (int p = 0; p < size; p++) {
      for (int w = 0; w < size; w++) {
        // paths setter
        PTrack();
        // first rule
        if (paths[p][w] < 2) {
          map[p][w] = 'f';
        }
        // second rule
        if (paths[p][w] == 2 || paths[p][w] == 3) {
          map[p][w] = 'w';
        }
        // third rule
        if (paths[p][w] > 3) {
          map[p][w] = 'f';
        }
        // fourth rule
        if (paths[p][w] == 3) {
          map[p][w] = 'w';
        }
      }
    }
  End();
  //checks if possible
  while (possible == false) {
    cout<<"extending generation"<<endl;
        for (int i = 0; i<size; i++) {
          if (solidMap[size][i] == true || solidMap[i][size] == true || solidMap[0][i] == true || solidMap[i][0] == true)
            possible = true;
    }
  for (int y = 0; y<size; y++) 
    for (int x = 0; x<size; x++) 
      solidMap[y][x] = false;
      for (int p = 0; p <size; p++) {
        for (int w = 0; w <size; w++) {
          // paths setter
          PTrack();
          // first rule
          if (paths[p][w] < 2) {
            map[p][w] = 'f';
          }
          // second rule
          if (paths[p][w] == 2 || paths[p][w] == 3) {
            map[p][w] = 'w';
          }
          // third rule
          if (paths[p][w] > 3) {
            map[p][w] = 'f';
          }
          // fourth rule
          if (paths[p][w] == 3) {
            map[p][w] = 'w';
          }
        }
      }
  End();
  Checker();
  }
  //final generation touch up
    for (int y = 0; y<size; y++) {
      for (int x = 0; x<size; x++) {
        int t = rand()%20;
        string rand = "ggffffffffffffffffff";
        string rand2 = "ltDltDwwwwwwwwwwwwww";
        if (map[y][x] == 'w') {
          map[y][x]=rand2[t];
        } else if (map[y][x] == 'f') map[y][x] = rand[t];
      }
    }
  //final conversion to vector
  vector<vector<char>> a;
  for (int y = 0; y<size; y++){
    vector<char> s;
    for (int x = 0; x<size; x++) {
      s.push_back(map[y][x]);
    }
    a.push_back(s);
      }
  return a;
}
//safe room

  /*
  w = wall, collide
  b = button, collide, interact
  e = exchange, collide, interact (item to money exchange)
  L=locked door, collide
  l=lock, collide, interact, interact (input and extract idol to unlock and lock door)
  U=unlocked door, no collide,

  */
  char room[5][10]={
    {'w','b','e','w','l','l','w','L','l','w'},
    {'w','f','f','f','f','f','f','f','f','l'},
    {'D','P','f','f','f','f','f','f','f','w'},
    {'w','f','f','f','f','f','f','f','f','l'},
    {'w','w','w','l','w','w','l','w','w','w'},
  };



// how long the delay between pygame windows/commands are. ABSOLUTELY NEEDED
unsigned int sleepWait=100000;

int main() {
  //each file will be called something different
  cout<<"Call time: "<<time(0)<<endl;
  ofstream createComms("commFile.txt");
  createComms.close();
  fstream commFile("commFile.txt");
  commFile<<"commFile call time: "<<to_string(time(0))<<endl<<endl;
  commFile<<"CPP INIT";
  commFile.close();

  srand(time(0));
  system("start cmd.exe");
  usleep(sleepWait);
  press("cd visuals\rcd screens\rcd menus\rpy mainMenu.py\r");
  if(false){
    MMENU:
    usleep(sleepWait);
    press("py mainMenu.py\r");
  }

  FAIL:
  commFile.close();
  string data;
  do{
    commFile.clear();
    commFile.open("commFile.txt",ios::in);
    if (commFile.is_open()) {
      string line;
      while (getline(commFile, line))
        data=line;
    }
    commFile.close();
  } while(data.substr(0,3).find("CPP")!=string::npos||data.find("PY")==string::npos);
  
  usleep(sleepWait);

  //open credits page
  if(data=="PY Main Menu CREDITS"){
    commFile.open("commFile.txt",ios::app);
    commFile<<endl<<"CPP Call credits.py";
    commFile.close();
    press("py credits.py\r");
    cout<<"credits.py called, waiting for exit."<<endl;
    do{
      commFile.clear();
      commFile.open("commFile.txt",ios::in);
      if (commFile.is_open()) {
        string line;
        while (getline(commFile, line))
          data=line;
      }
      commFile.close();
    } while(data!="PY Cred Manual Exit");
    usleep(sleepWait);
    commFile.close();
    commFile.clear();
    commFile.open("commFile.txt",ios::app);
    commFile<<endl<<"CPP RECALL MMENU";
    commFile.close();
    goto MMENU;
  }
  else if(data=="PY Main Menu QUIT"){
    commFile.clear();
    commFile.open("commFile.txt",ios::app);
    commFile<<endl<<"CPP EXIT from MM CALL"<<endl<<endl<<"CALLING thanks.py";
    commFile.close();
    press("py thanks.py\r");
    do{
      commFile.clear();
      commFile.open("commFile.txt",ios::in);
      if (commFile.is_open()) {
        string line;
        while (getline(commFile, line))
          data=line;
      }
      commFile.close();
    } while(data!="PY Thanks Manual Exit");
    commFile.clear();
    commFile.open("commFile.txt",ios::app);
    commFile<<endl<<endl<<"CPP Program End";
    commFile.close();
    usleep(sleepWait);
    press("exit\r");
    return 0;
  }
  else if(data=="PY Main Menu PLAY"){
    usleep(sleepWait);
    //OPENING CUTSCENE
    press("cd..\rcd cutscenes\rpy openingScene.py\r");
    do{
      commFile.clear();
      commFile.open("commFile.txt",ios::in);
      if (commFile.is_open()) {
        string line;
        while (getline(commFile, line))
          data=line;
      }
      commFile.close();
    } while(data!="PY Opening Scene FINISHED");
  }
  //it worked, then it didn't work, so now this is here and it works again
  else goto FAIL;

  //if we're here, then the opening scene should be done and player should spawn into lvl 1 safe room.
  //create tiles for all
  commFile.close();
  commFile.clear();
  commFile.open("commFile.txt",ios::app);
  commFile<<endl<<endl<<"FLOOR NUMBER: 1 Safe Room"<<endl;
  commFile<<"ROWS, COLS"<<endl<<"100"<<endl<<"100"<<endl;
  commFile<<"Layout Start"<<endl;
  //update max values for a and b if room size changes
  vector<vector<char>> test = mapInt(20, 100);
  for (int y = 0; y<test.size(); y++,cout<<endl,commFile<<endl) {
    for(int x = 0; x<test.size(); x++) {
      cout<<test[y][x]<<" ";
      commFile<<test[y][x];
    }
  }/*
  for(int a=0;a<5;a++,commFile<<endl)
    for(int b=0;b<10;b++)
      commFile<<room[a][b];*/
  commFile<<"Layout End";

  commFile.close();
  commFile.clear();
  usleep(sleepWait);
  press("cd..\rpy labrynth.py\r");
  


 /*
  

  commFile.open("commFile.txt",ios::app);
  commFile<<endl<<endl<<to_string(time(0))<<endl;
  commFile<<"CPP COMPL Map Gen";
  commFile.close();
  cout<<endl<<"c++ map gen complete, waiting for python response"<<endl;
*/
  sleep(20);
  
}