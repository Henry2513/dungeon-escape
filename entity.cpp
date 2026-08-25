#include "entity.h"
#include "util.h"

using namespace std;

Monster* createMonster(int x, int y, int hp, int atk,
                       int expReward, int goldReward, bool isBoss) {
    Monster *m = new Monster;
    m->x = x;
    m->y = y;
    m->hp = hp;
    m->maxHp = hp;
    m->atk = atk;
    m->expReward = expReward;
    m->goldReward = goldReward;
    m->isBoss = isBoss;
    m->next = NULL;
    return m;
}

void addMonster(Monster* &head, Monster* m) {
    m->next = head;
    head = m;
}

Monster* monsterAt(Monster* head, int x, int y) {
    for (Monster *m = head; m != NULL; m = m->next) {
        if (m->x == x && m->y == y)
            return m;
    }
    return NULL;
}

bool removeMonster(Monster* &head, Monster* m) {
    if (head == NULL || m == NULL)
        return false;
    if (head == m) {
        head = m->next;
        delete m;
        return true;
    }
    for (Monster *cur = head; cur->next != NULL; cur = cur->next) {
        if (cur->next == m) {
            cur->next = m->next;
            delete m;
            return true;
        }
    }
    return false;
}

void freeMonsters(Monster* &head) {
    while (head != NULL) {
        Monster *tmp = head;
        head = head->next;
        delete tmp;
    }
}

int rollDamage(int atk) {
    int dmg = atk + randInt(-1, 2);   // small random spread
    return dmg < 1 ? 1 : dmg;
}

string fight(Player &p, Monster *m, bool &monsterDied) {
    string msg;
    int dmg = rollDamage(p.atk);
    m->hp -= dmg;
    msg = "You hit the " + string(m->isBoss ? "BOSS" : "monster") +
          " for " + to_string(dmg) + " damage.";
    if (m->hp <= 0) {
        monsterDied = true;
        return msg;
    }
    monsterDied = false;
    int mdmg = rollDamage(m->atk);
    p.hp -= mdmg;
    msg += " It strikes back for " + to_string(mdmg) + " damage.";
    return msg;
}
