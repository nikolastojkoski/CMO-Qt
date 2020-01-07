#include "Buffer.h"
#include <stdexcept>


#include <iostream>

Buffer::Buffer(size_t N)
{
  N_ = N;
}
void Buffer::addRequest(const Request &req)
{
  if (req_.size() == N_)
  {
    throw std::exception("Buffer is full");
  }
  req_.push_back(req);
}
Request Buffer::popBestRequest()
{

  auto best_it = req_.begin();

  for (auto it = std::next(req_.begin()); it != req_.end(); it++)
  {
    if (it->PRIO() > best_it->PRIO())
    {
      best_it = it;
    }
    else if ((it->PRIO() == best_it->PRIO()) && (it->TPOST() > best_it->TPOST()))
    {
      best_it = it;
    }
  }

  Request best_request = *best_it;
  req_.erase(best_it);
  return best_request;

}
Request Buffer::popWorstRequest()
{
  if (req_.empty())
  {
    throw std::exception("Buffer is empty");
  }

  Request worst_request = *req_.begin();
  req_.pop_front();

  return worst_request;
}

bool Buffer::isFull() const
{
  return (req_.size() == N_);
}
bool Buffer::isEmpty() const
{
  return req_.empty();
}
size_t Buffer::size() const
{
  return N_;
}

std::list<Request>::const_iterator Buffer::begin() const
{
  return req_.cbegin();
}

std::list<Request>::const_iterator Buffer::end() const
{
  return req_.cend();
}
