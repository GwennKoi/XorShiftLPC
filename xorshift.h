// xorshift.h
// Pseudo-random repeatable number generator
// Kryssa@SWmud, August 2022
// Taken from "Xorshift RNGs" by George Marsaglia at Florida State University

#ifndef _XORSHIFT_H
#define _XORSHIFT_H

//upper limit of 32-bit int
#define XORSHIFT_32_MAX     0xFFFFFFFF

//to get back the new seed and the result of a procedural function
class seed_result {
  int seed;
  mixed result;
}

/*
//Example code using these functions
int test(int seed) {
  class seed_result result;
  string *arr = explode("abcdefghijklmnopqrstuvwxyz","");
  printf("%d\n",seed);
  result = shuffle_seed(arr, seed);
  if(result) {
    write(implode(result->result,""));
    seed = result->seed;
  }
  printf("%d\n",seed);
  result = element_of_seed(arr, seed);
  if(result) {
    write(result->result);
    seed = result->seed;
  }
  printf("%d\n",seed);
  return seed;
}
//Call:
// ->test(1)
//Should Always Output:
// 1
// vuwkixmspdfclqjyngaerhoztb
// 1562130985
// z
// 2440533925
*/

//make sure the seed is within 1 to 4294967295
private int _sanitize_seed(int seed) {
  return (abs(seed) % XORSHIFT_32_MAX) || XORSHIFT_32_MAX;
}

//pseudo-random number via seed single step.
//Minimum seed 1 enforced by override.
//Maximum seed 4294967295 enforced by override.
private int _step_xorshift32(int seed) {
  //Force seed to stay between 1 and max uint32
  int register = _sanitize_seed(seed);

  //This seems dumb, but we're on 64 bit signed-only.
  //So, we've gotta fake unsigned 32 bit.
  register = (register ^ (register << 13)) & XORSHIFT_32_MAX;
  register = (register ^ (register >> 17)) & XORSHIFT_32_MAX;
  register = (register ^ (register <<  5)) & XORSHIFT_32_MAX;

  return register;
}

//seed generator for within the range of the xorshift32
//Just grab a random within 1 to 4294967295
int seed_generator() {
  return random( XORSHIFT_32_MAX ) + 1;
}

//get a psuedo-random number using a random number seed.
//returns the seed_result class containing the next seed and the result
class seed_result random_seed(int size, int seed) {
  class seed_result result = new(class seed_result);

  seed = _sanitize_seed(seed);
  result->result = seed % size;
  result->seed = _step_xorshift32(seed);

  return result;
}

//shuffle an array using a random number seed.
//returns the seed_result class containing the next seed and the result
class seed_result shuffle_seed(mixed *arr, int seed) {
  class seed_result result = new(class seed_result);
  mixed *new_arr = copy(arr);
  int index = sizeof(new_arr);

  seed = _sanitize_seed(seed);

  //if null or not array return 0;
  if(!arr || !arrayp(arr)) {
    return 0;
  }
  while(index != 0) {
    int random_index = seed % index;
    mixed tmp;
    index--;
    tmp = new_arr[index];
    new_arr[index] = new_arr[random_index];
    new_arr[random_index] = tmp;

    seed = _step_xorshift32(seed);
  }
  result->seed = seed;
  result->result = new_arr;

  return result;
}

//gets an element from an array using a random number seed.
//returns the seed_result class containing the next seed and the result
class seed_result element_of_seed(mixed *arr, int seed) {
  class seed_result result = new(class seed_result);
  seed = _sanitize_seed(seed);

  //if no items or not an array return 0;
  if(!sizeof(arr) || !arrayp(arr)) {
    return 0;
  }
  result->result = arr[seed % sizeof(arr)];
  result->seed = _step_xorshift32(seed);

  return result;
}

#endif
