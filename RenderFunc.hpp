#ifndef RENDERFUNC_HPP
#define RENDERFUNC_HPP


class Field;
class Piece;

// Abstract functor
struct IRenderFunc
{
  virtual void operator()(const Field&, const Piece&, const Piece*)=0;
};

// Template for concrete functors using member functions
template <class predicate>
struct RenderFunc : public IRenderFunc
{
  RenderFunc(predicate* pred,
	     void (predicate::*predmember)(const Field&, const Piece&, const Piece*)
	     ):fptr(predmember),dptr(pred){}
  
  virtual void operator()(const Field& f, const Piece& cur, const Piece* hold)
  {
    (*dptr.*fptr)(f,cur,hold);
  }
private:
  void (predicate::*fptr) (const Field&, const Piece&, const Piece*);
  predicate *dptr;
};
  
// Specialization for functors using a simple function pointer
typedef void (*renderfuncptr)(const Field &, const Piece &, const Piece * );
template<>
struct RenderFunc<renderfuncptr> 
  : public IRenderFunc
{
  RenderFunc(renderfuncptr x):fptr(x){}
  
  virtual void operator()(const Field& f, const Piece& cur, const Piece* hold)
  {
    fptr(f,cur,hold);
  }
private:
  renderfuncptr fptr;
};

#endif // RENDERFUNC_HPP
