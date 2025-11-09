# philosophers

An implementation of the classic Dining Philosophers concurrency problem, written in C as part of the 42 curriculum. The goal: coordinate philosophers who alternately think and eat while sharing limited forks, avoiding deadlock, starvation, and data races.

## Problem summary

Each philosopher sits at a round table with a fork between each pair. To eat, a philosopher must hold both the left and right fork. Simulation ends when:
- A philosopher dies (hasn’t eaten within `time_to_die` ms), or
- All philosophers have eaten at least `number_of_times_each_philosopher_must_eat` (if this optional argument is provided).

## Mandatory vs Bonus

| Part | Concurrency Model | Sync Primitives | Executable |
|------|-------------------|-----------------|------------|
| Mandatory | Threads (1 per philosopher) | Mutexes (forks + shared state) | `philo` |
| Bonus | Processes (1 per philosopher) | Semaphores (forks + output control) | `philo_bonus` |

## Build

Assuming a standard 42-style `Makefile` with these targets:

```sh
make          # builds philo
make bonus    # builds philo_bonus
make clean    # removes object files
make fclean   # removes objects + binaries
make re       # full rebuild
```

Dependencies: `gcc`, `make`, and POSIX threads (`-pthread`). Bonus uses POSIX semaphores (`<semaphore.h>`).

## Run

### Mandatory (threads + mutexes)
```sh
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

### Bonus (processes + semaphores)
```sh
./philo_bonus number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

Arguments:
1. `number_of_philosophers` (≥ 1)
2. `time_to_die` (ms)
3. `time_to_eat` (ms)
4. `time_to_sleep` (ms)
5. (optional) `number_of_times_each_philosopher_must_eat` (≥ 1)

Exit codes typically:
- `0` on graceful completion (all ate enough).
- `1` on argument error.
- Non‑zero specific codes if you implement granular error reporting (e.g. allocation / thread creation failures).

## Output (example)

```
0 1 has taken a fork
0 1 has taken a fork
0 1 is eating
201 1 is sleeping
201 2 has taken a fork
220 3 died
```

Format conventions:
- Timestamp in ms since simulation start.
- Ordered logging without overlap (protected by a mutex or semaphore).
- Stop printing after a death event.

## Core design

### Data structures (typical)
- `t_philo`:
  - `id`
  - `last_meal_time`
  - `meals_eaten`
  - Pointers/indexes to left/right fork mutex (mandatory)
  - Thread handle (mandatory) / PID (bonus)
- `t_shared`:
  - Start timestamp
  - Global death flag
  - Array of philosophers
  - Array of fork mutexes (mandatory)
  - Write/output mutex
  - Semaphores (bonus): forks, print, death signaling
  - Configuration parameters

### Initialization steps
1. Parse & validate arguments (non-negative, non-zero where required).
2. Allocate philosopher & fork arrays.
3. Initialize mutexes (mandatory) or semaphores (bonus).
4. Record simulation start time.
5. Spawn threads / fork processes.
6. Launch monitor (watchdog) loop to detect starvation.

### Fork acquisition strategy (deadlock avoidance)
Common approaches:
- Odd philosophers pick up left then right; even pick right then left.
- Or enforce ordering by always locking the lower-addressed fork first.

Either prevents circular wait.

### Timing
- Use `gettimeofday()` or `clock_gettime()` (with `CLOCK_MONOTONIC`) for accurate ms timing.
- Custom `smart_sleep()` that periodically checks for death to avoid oversleeping.

### Monitoring
A single monitor thread/process periodically:
- Locks shared state.
- Checks `current_time - last_meal_time > time_to_die`.
- Sets death flag and prints death event exactly once.

### Stopping conditions
- Death: broadcast flag; threads finish without further logging.
- Optional all-fed condition: when every philosopher’s `meals_eaten >= target`.

## Edge cases handled
- `number_of_philosophers == 1`: Philosopher takes one fork, can’t take second, dies after `time_to_die`.
- Very small `time_to_die` < `time_to_eat`: eat may never complete; monitor still catches death.
- Overflow or invalid numeric input (non-digit chars) rejected.
- Extreme large values: guard against integer overflow in time calculations (cast to `long long`).

## Error handling
Return early with a clear message on:
- Invalid argument count or values.
- `malloc` / thread / semaphore creation failures.
- System call errors (e.g. `gettimeofday` improbable, but you can guard).

Keep error prints on stderr; never interleave with simulation output prints on stdout.

## Bonus notes (process/semaphore model)
- Each philosopher becomes a separate process; parent spawns them and may create a monitor process or implement waiting logic.
- Semaphores:
  - `forks`: counting semaphore initialized to `number_of_philosophers`.
  - `print`: binary semaphore ensuring atomic logging.
  - `meal_check` (optional): protects shared meal counters/time.
- Graceful shutdown: send termination signals or post semaphores so children exit cleanly.

## Testing suggestions

```sh
# Quick death scenario
./philo 5 200 100 100

# Stress: many philosophers
./philo 200 800 200 200

# With required meal count
./philo 5 800 200 200 7
```

Add a script to batch-run parameter combinations and grep for “died” vs completion.

## Performance considerations
- Avoid busy waiting: use short sleeps (`usleep(100)` loops) inside custom sleep.
- Minimize locking granularity (only when printing or changing `last_meal_time`).
- Reuse calculated timestamps; don’t call time functions redundantly.

## Common pitfalls
- Printing after death flag set (should stop).
- Not unlocking mutexes when exiting early (causes deadlocks).
- Race in updating `last_meal_time` vs monitor reading it (protect with mutex).
- Using `printf` without flush or while other threads print (mixes lines).

## Directory layout (assumed)
```
philo/
  main.c
  init.c
  parse_args.c
  time.c
  routine.c
  monitor.c
  utils.c
  philosophers.h
philo_bonus/
  main_bonus.c
  init_bonus.c
  semaphore_setup.c
  routine_bonus.c
  monitor_bonus.c
  utils_bonus.c
  philosophers_bonus.h
Makefile
```
Adjust the file list if names differ.

## Norm & style
- Follow 42 Norm (no forbidden functions).
- Avoid global variables unless mandated; pass shared struct pointers.
- Free all allocated resources (fork mutexes, arrays, semaphores) before exit.

## Extending (optional ideas)
- Add CLI flag for verbose mode.
- Colored output distinguishing states.
- Statistical summary at end (avg time between meals).

No license specified. Consider adding an MIT or BSD license if you want to permit reuse.
