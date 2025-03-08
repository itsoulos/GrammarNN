#ifndef INTERVALANNEAL_H
#define INTERVALANNEAL_H

# include <INTERVAL/intervalproblem.h>
# include <QVariant>
# include <GE/cprogram.h>
# include <CORE/parameterlist.h>
class IntervalAnneal
{
private:
    IntervalProblem     *problem;
    ParameterList plist;
    int neps;
    const int nsamples=100;
    IntervalData bestx;
    Interval besty;
    IntervalData xpoint;
    Interval ypoint;
    double T0;
    int k;
    double alpha;
public:
    IntervalAnneal(IntervalProblem *p);
    void            setProblem(IntervalProblem *p);
    void            setParam(QString name,QString value,QString help="");
    Interval        fitness(IntervalData &x);
    void            Solve();
    void            getBest(IntervalData &x,Interval &y);
    ParameterList   getParameterList() const;
    void            reduceTemp();
    void             randomSample(IntervalData &x);
    ~IntervalAnneal();
};

#endif // INTERVALANNEAL_H
