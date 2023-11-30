#include <iostream>
#include <cmath>
#include <iomanip>
#include <atomic>
#include <vector>
#include <set>
#include <thread>
#include <mutex>
#include <algorithm>
#include <unordered_map>
#include <cstdint>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <limits>
#include <map>
#include <set>
#include <sys/time.h>
#include <sys/resource.h>
#include <future>
#include <chrono>
#include <stdlib.h>

//#include "rational.h"
//#include "main_rational.h"
//#include "prime_square.h"
//#include "set_game.h"
//#include "BigFloat.h"
//#include "uinteger_t.hh"
#include <type_traits>
#include <random>

class random_engine
{
  public:
    std::random_device                      rd;
    std::mt19937                            mt;
    std::uniform_real_distribution<double>  dist;

    random_engine() : rd(), mt(rd()), dist(0.0, 1.0) {}

    double rand() {
      return dist(mt);
    }
};

bool allsame(std::vector<int>& vCards)
{
    for (auto c : vCards)
        if (c != vCards[0]) return false;
    return true;
}

void run(int NCARDS, long NITER, double& e )
{
    random_engine re;
    e = 0;
    std::vector<int> vCardsStart(NCARDS);
    for (long i=0;i<NCARDS;i++)
    {
        vCardsStart[i] = i;
    }
    std::map<int, long> vmap;

    srand (time(NULL));
    int n = rand() % 100;
    for (int i=0;i<n;i++) re.rand(); // random seed

    std::vector<int> vCards(NCARDS);
    std::vector<int> vNextCards(NCARDS);
    int cardpos;
    long step = 0;
    for (long iter=0;iter<NITER;iter++)
    {
        step = 0;
        vCards = vCardsStart;
        while(true)
        {
            if (step!=0) vCards = vNextCards;
            for (long i=0;i<NCARDS;i++)
            {
                cardpos = NCARDS * re.rand();
                vNextCards[i] = vCards[cardpos];
            }
            step++;
            if (allsame(vNextCards))
            {
                vmap[step]++;
                break;
            }
        }
    }

    for(auto& [nstep, ncnt] : vmap)
    {
        //std::cout << nstep << " " << ncnt << std::endl;
        e += nstep * ncnt / ((double)NITER);
    }
}

int main()
{
    long NITER = 10000000;
    int NCARDS = 3;

    int nt = std::thread::hardware_concurrency();
    std::vector<std::thread> vt;
    std::vector<double> vprob(nt, 0);
    for (int j=0;j<nt;j++)
    {
        vt.push_back( std::thread(run, NCARDS , NITER, std::ref(vprob[j]) ) ) ;
    }
    for (int j=0;j<nt;j++)  vt[j].join();

    double e = 0;
    for (int j=0;j<nt;j++) e+=vprob[j];
    e = e/nt;
    std::cout << std::setprecision(10) << e << std::endl;

    return 0;
}

