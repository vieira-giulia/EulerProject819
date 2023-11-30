import random
import threading
import multiprocessing
import time
from collections import defaultdict

class RandomEngine:
    def __init__(self):
        self.mt = random.Random()

    def rand(self):
        return self.mt.uniform(0.0, 1.0)

def all_same(v_cards):
    return all(c == v_cards[0] for c in v_cards)

def run(ncards, niter, e):
    re = RandomEngine()
    e[0] = 0.0
    v_cards_start = list(range(ncards))
    vmap = defaultdict(int)

    seed = int(time.time())
    random.seed(seed)
    n = random.randint(0, 100)
    for _ in range(n):
        re.rand()  # random seed

    v_cards = [0] * ncards
    v_next_cards = [0] * ncards
    cardpos = 0
    step = 0

    for _ in range(niter):
        step = 0
        v_cards = v_cards_start.copy()
        while True:
            if step != 0:
                v_cards = v_next_cards.copy()
            for i in range(ncards):
                cardpos = int(ncards * re.rand())
                v_next_cards[i] = v_cards[cardpos]
            step += 1
            if all_same(v_next_cards):
                vmap[step] += 1
                break

    for nstep, ncnt in vmap.items():
        e[0] += nstep * ncnt / niter

def main():
    niter = 10_000_000
    ncards = 3

    nt = multiprocessing.cpu_count()
    vt = []
    vprob = [0.0]

    for _ in range(nt):
        vt.append(threading.Thread(target=run, args=(ncards, niter // nt, vprob)))

    for thread in vt:
        thread.start()

    for thread in vt:
        thread.join()

    e = vprob[0] / nt
    print(f'{e:.10f}')

if __name__ == "__main__":
    main()
