use rand::{Rng, thread_rng};
use std::collections::HashMap;
use std::thread;

struct RandomEngine {
    mt: rand::rngs::ThreadRng,
}

impl RandomEngine {
    fn new() -> RandomEngine {
        RandomEngine {
            mt: thread_rng(),
        }
    }

    fn rand(&mut self) -> f64 {
        self.mt.gen_range(0.0..1.0)
    }
}

fn all_same(v_cards: &Vec<i32>) -> bool {
    v_cards.iter().all(|&c| c == v_cards[0])
}

fn run(ncards: i32, niter: i64, e: &mut f64) {
    let mut re = RandomEngine::new();
    *e = 0.0;
    let v_cards_start: Vec<i32> = (0..ncards).collect();
    let mut vmap: HashMap<i32, i64> = HashMap::new();

    unsafe {
        srand(std::time::SystemTime::now()
            .duration_since(std::time::SystemTime::UNIX_EPOCH)
            .unwrap()
            .as_secs() as u32);
    }

    let n = unsafe { rand() % 100 };
    for _ in 0..n {
        re.rand(); // random seed
    }

    let mut v_cards = vec![0; ncards as usize];
    let mut v_next_cards = vec![0; ncards as usize];
    let mut cardpos;
    let mut step;

    for _ in 0..niter {
        step = 0;
        v_cards.clone_from_slice(&v_cards_start);
        loop {
            if step != 0 {
                v_cards.clone_from_slice(&v_next_cards);
            }
            for i in 0..ncards {
                cardpos = (ncards as f64 * re.rand()) as usize;
                v_next_cards[i as usize] = v_cards[cardpos];
            }
            step += 1;
            if all_same(&v_next_cards) {
                *vmap.entry(step).or_insert(0) += 1;
                break;
            }
        }
    }

    for (nstep, ncnt) in vmap.iter() {
        *e += *nstep as f64 * *ncnt as f64 / niter as f64;
    }
}

extern "C" {
    fn srand(seed: u32);
    fn rand() -> u32;
}

fn main() {
    let niter = 10_000_000;
    let ncards = 3;

    let nt = num_cpus::get();
    let mut handles = vec![];
    let mut vprob = vec![0.0; nt];

    for _ in 0..nt {
        handles.push(thread::spawn(move || {
            let mut e = 0.0;
            run(ncards, niter / nt as i64, &mut e);
            e
        }));
    }

    for handle in handles {
        vprob.push(handle.join().unwrap());
    }

    let e: f64 = vprob.iter().sum::<f64>() / nt as f64;
    println!("{:.10}", e);
}
