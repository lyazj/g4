#include "random.hh"
#include <time.h>
#include <unistd.h>

static size_t get_random_seed()
{
  static time_t start_time = time(nullptr);  // Initialized once and only once, synchronized.
  pid_t tid = gettid();
  size_t seed = start_time ^ tid;  // Unique for each thread.
  return seed;
}

thread_local __typeof(thread_random_engine) thread_random_engine(get_random_seed());
