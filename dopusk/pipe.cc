#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc != 2) {
        std::cout << "Bad args" << std::endl;
        return 1;
  }
  int sz_ = std::stod(argv[1]);

  std::cout << "Bytes to send and recieve: " << sz_ << std::endl;

  const int ITERS = 1000;

  int fd0[2], fd1[2], result;
  size_t size;

  char* resstring = (char*) calloc(sz_, sizeof(char));
  char* srcstring = (char*) calloc(sz_, sizeof(char));

  for (int i = 0; i < sz_ - 1; ++i) {
    srcstring[i] = 'q';
  }
  srcstring[0] = 'e';

  if(pipe(fd0) < 0) {
    std::cout << "Can\'t create pipe" << std::endl;
    return 1;
  }

  if(pipe(fd1) < 0) {
    std::cout << "Can\'t create pipe" << std::endl;
    return 1;
  }

  result = fork();

  if(result < 0) {
    std::cout << "Can\'t fork child" << std::endl;
    return 1;
  } 
  else if (result > 0) {
    close(fd0[0]);
    close(fd1[1]);

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < ITERS; ++i) {
      size = write(fd0[1], srcstring, sz_);
      if(size != sz_) {
        std::cout << "Parent: can\'t write all string" << std::endl;
        return 1;
      }
      size = read(fd1[0], resstring, sz_);

      if(size < 0) {
        std::cout << "Parent: can\'t read string" << std::endl;
        return 1;
      }
    }
    
    double tm = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::high_resolution_clock::now() - start).count();

    std::cout << "Parent: " << resstring << std::endl;
    close(fd1[1]);
    close(fd1[0]);
    std::cout << "Parent exit" << std::endl;

    std::cout << "To send and recv " << sz_ << " bytes using pipe taked " << tm / ITERS << " us" << std::endl;
  } 
  else {
    close(fd0[1]);
    close(fd1[0]);

    for (int i = 0; i < ITERS; ++i) {
      size = read(fd0[0], resstring, sz_);
      if(size < 0) {
        std::cout << "Child: can\'t read string" << std::endl;
        return 1;
      }
      size = write(fd1[1], srcstring, sz_);

      if(size != sz_) {
        std::cout << "Child: can\'t write all string" << std::endl;
        return 1;
      }
    }

    std::cout << "Child: " << resstring << std::endl;

    close(fd0[0]);
    close(fd1[1]);
  }

  return 0;
} 