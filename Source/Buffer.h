#ifndef BUFFER_H
#define BUFFER_H

#include <set>
#include <list>
#include <vector>
#include "Request.h"

#define INF 1e10;

class Buffer
{
public:
  Buffer(size_t N);

  void addRequest(const Request &req);
  Request popBestRequest();
  Request popWorstRequest();

  bool isFull() const;
  bool isEmpty() const;
  size_t size() const;

  std::list<Request>::const_iterator begin() const;
  std::list<Request>::const_iterator end() const;

private:
  size_t N_;
  std::list<Request> req_;

};



#endif