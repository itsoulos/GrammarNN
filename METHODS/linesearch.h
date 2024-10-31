#ifndef LINESEARCH_H
#define LINESEARCH_H
# include <INTERVAL/intervalproblem.h>

/**
 * @brief The LineSearch class defines an abstract class
 * for LineSearch
 */
class LineSearch
{
protected:
    /**
     * @brief myProblem, the objective problem
     */
    IntervalProblem *myProblem;
public:
    /**
     * @brief LineSearch, the constructor of the base class
     * @param p
     */
    LineSearch(IntervalProblem *p);
    /**
     * @brief fl the function to be minimized in the direction of search
     * @param x
     * @param h
     * @return
     */
    virtual double fl(Data &x,double h);
    /**
     * @brief getDirection computes the new search direction
     * @param x
     * @return
     */
    virtual double getDirection(Data &x);
    virtual ~LineSearch();
};

#endif // LINESEARCH_H
